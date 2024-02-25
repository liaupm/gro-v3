#include "cg/ExpressionHandler.hpp"
#include "cg/Genome.hpp" //owner
#include "cg/Regulation.hpp" //model
#include "cg/Operon.hpp" //model
#include "cg/Molecule.hpp" //model
#include "cg/Flux.hpp" //collectActivation() --> getActivation()
#include "cg/MetabolismHandler.hpp"//std::vector<const FluxReport*> metSourceReports of MoleculeReport

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////

//======================================================================================================================================================================Regulation
bool RegulationReport::init()
{
	ownerHandler->getOwner()->removeChange( model );
	if( ownerHandler->getOwner()->checkBState( model ) )
	{
	  //calculate activation per copy, considering presence of the element, gate state, copy number (dosage effects) and randomness (potentially noisy)
		activationPerCopy = model->sampleActivation( ownerHandler->getOwner()->checkQState( model ), ownerHandler->getOwner()->checkQState( model->getOnFunction() ), ownerHandler->getOwner()->checkQState( model->getOffFunction() )
		, ownerHandler->getOwner()->checkQState( model->getRandomness() ) , ownerHandler->getOwner()->checkBState( model->getGate() )
		, model->getBDosageEffect() ? ownerHandler->getOwner()->checkQState( model->getDosageFun() ) : 1.0 );
	  //expose the activation if marker exists
		if( model->getActiMarker() )
			ownerHandler->getOwner()->setAndForward( model->getActiMarker(), activationPerCopy ); 
		
		//changes not added as the only affected element is the Operon and Operons are initialized after Regulations
		//logic refresh not required as the activation cannot be an input
		return true;
	}
	//if inactive, the activation remains as initialized in the ctor (0.0)
	return false;
}

bool RegulationReport::update( const EventType* )
{
	ownerHandler->getOwner()->removeChange( model );
	TReal oldActivationPerCopy = activationPerCopy;

  //update activation per copy
	if( ownerHandler->getOwner()->checkBState( model ) )
	{
		activationPerCopy = model->sampleActivation( ownerHandler->getOwner()->checkQState( model ), ownerHandler->getOwner()->checkQState( model->getOnFunction() ), ownerHandler->getOwner()->checkQState( model->getOffFunction() )
		, ownerHandler->getOwner()->checkQState( model->getRandomness() ) , ownerHandler->getOwner()->checkBState( model->getGate() )
		, model->getBDosageEffect() ? ownerHandler->getOwner()->checkQState( model->getDosageFun() ) : 1.0 );
	}
	else
		activationPerCopy = 0.0;

  //if it changes
	if( ut::notSame( activationPerCopy, oldActivationPerCopy ) )
	{
	  //changes in the activation can only affect Operons if not exposed
		ownerHandler->getOwner()->addChangesFrom( model, GeneticCollection::ElemTypeIdx::OPERON );
	  //expose if marker
		if( model->getActiMarker() )
			ownerHandler->getOwner()->setAndForward(  model->getActiMarker(), activationPerCopy ); 
		//logic refresh not required as the activation cannot be an input
		return true;
	}	
	return false;
}


//======================================================================================================================================================================Operon
void OperonReport::link()
{ 
	//an Operon always has a Regulation
	regReport = &ownerHandler->getRegulationReports()[ model->getRegulation()->getRelativeId() ]; 
}

bool OperonReport::init()
{	
	ownerHandler->getOwner()->removeChange( model );
	if( ownerHandler->getOwner()->checkBState( model ) )
	{
	  //calculate total activation considering the activation of its regulation and its copy number (dosage effects)
		activation = model->scaleActivation( regReport->activationPerCopy, ownerHandler->getOwner()->checkQState( model ) );
	  //expose activation if marker
		if( model->getActiMarker() )
			ownerHandler->getOwner()->setAndForward( model->getActiMarker(), activation ); 
		//changes not added as the only affected element is the Molecule and Molecules are initialized after Operons
		//logic refresh not required as the activation cannot be an input
		return true;
	}
	//if inactive, the activation remains as initialized in the ctor (0.0)
	return false;
}

bool OperonReport::update( const EventType* )
{	
	ownerHandler->getOwner()->removeChange( model );
	TReal oldActivation = activation;
  //update activation
	if( ownerHandler->getOwner()->checkBState( model ) )
		activation = model->scaleActivation( regReport->activationPerCopy, ownerHandler->getOwner()->checkQState( model ) );
	else
		activation = 0.0;
  //if it changes
	if( ut::notSame( activation, oldActivation ) )
	{
	  //changes in the activation can only affect Molecules if not exposed
		ownerHandler->getOwner()->addChangesFrom( model, GeneticCollection::ElemTypeIdx::MOLECULE );
	  //expose if marker
		if( model->getActiMarker() )
			ownerHandler->getOwner()->setAndForward( model->getActiMarker(), activation ); 
		//logic refresh not required as the activation cannot be an input
		return true;
	}
	return false;
}


//======================================================================================================================================================================Molecule

//------------------------------------ common
void MoleculeReport::link()
{ 
  //operons
	sourceReports.clear();
	for ( const auto& op : model->getBackwardLink().elemsByType[ GeneticCollection::ElemTypeIdx::OPERON ] )
		sourceReports.push_back( &ownerHandler->getOperonReports()[ op->getRelativeId() ] );
  //fluxes
	metSourceReports.clear();
	for ( const auto& flux : model->getBackwardLink().elemsByType[ GeneticCollection::ElemTypeIdx::FLUX ] )
		metSourceReports.push_back( &ownerHandler->getOwner()->getHandler<OwnerType::HandlerIdx::H_METABOLISM>().getFluxReports()[ flux->getRelativeId() ] );
}

bool MoleculeReport::init()
{ 
	ownerHandler->getOwner()->removeChange( model );
  //init activation, index and direction
	collectActivation();
	expressionIdx = model->inputIndex( direction, activation );

	if( checkDirection( ownerHandler->getOwner()->checkBState( model ), direction ) ) //compare the direction to the current molecule state to check if there is a change process
	{
	  //sample delay (wholeTime) and create event
		wholeTime = model->sampleExpressionTimeAtIndex( expressionIdx, ownerHandler->getOwner()->checkQState( model->getFunctionAtIndex( expressionIdx ) )
		, ownerHandler->getOwner()->checkQState( model->getRandomness() ), ownerHandler->getOwner()->checkQState( model->getCustomRndSymmetry() ) );

		eventTime = ownerHandler->getOwner()->getTime().time + wholeTime;
		ownerHandler->getOwner()->addEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
		//logic refresh not required as the expression time cannot be an input
		return true;
	}
	return false;
}

bool MoleculeReport::divisionNew( MoleculeReport* )
{ 
  //copy ongoing event to the new cell. If the event time changes (re-sampling its Randomness), it will be updated in the updateCycle() after division
	if( eventTime >= ownerHandler->getOwner()->getTime().time )
	{
		ownerHandler->getOwner()->addEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
		return true;
	}
	return false;
}

bool MoleculeReport::update( const EventType* )
{ 
	ownerHandler->getOwner()->removeChange( model );
  //upddate activation and index
	collectActivation();
	Molecule::ExpressionDirection newDirection;
	expressionIdx = model->inputIndex( newDirection, activation );

	if( newDirection != direction )
	{
		if( checkDirection( ownerHandler->getOwner()->checkBState( model ), newDirection ) ) //case 1: from off to on
			updateOff2On( newDirection ); // a process starts (create event)
		else if( eventTime != INI_EVENT_TIME ) //case 2: from on to off
			updateOn2Off( newDirection ); // a process ends unfinished (remove event)
		return true;
	}
	//a change that does not affect the direction is only relevant if there is an ongoing event
	//if so, the change may be in expressionIdx (due to a change in the activation) or just in the sampled value (due to the re-sampling of rnd)
	else if( eventTime != INI_EVENT_TIME ) //case 3: from on to on, the ongoing process changes its time
		return updateOn2on(); //update the current event
	return false;
	//logic refresh not required as the expression time cannot be an input
}


//--------------------------------------------------------------------------------- special
void MoleculeReport::collectActivation()
{ 
	activation = 0.0;
  //operons
	for( const auto& s : sourceReports )
		activation += s->activation;
  //fluxes
	for( const auto& f : metSourceReports )
	{
		if( f->getBActive() )
			activation += f->getModel()->getActivation();
	}
}

bool MoleculeReport::updateOn2on()
/* samples a new time and scales the remaining time accordingly */
{
	//the whole delay if the current conditions had been the same during the whole process
	TReal newWholeTime = model->sampleExpressionTimeAtIndex( expressionIdx, ownerHandler->getOwner()->checkQState( model->getFunctionAtIndex( expressionIdx ) )
	, ownerHandler->getOwner()->checkQState( model->getRandomness() ), ownerHandler->getOwner()->checkQState( model->getCustomRndSymmetry() ) );

	if( ut::notSame( newWholeTime, wholeTime ) ) //if there is an actual change
	{
	  //remove old event
		ownerHandler->getOwner()->removeEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
	  //scale the remaining time
		TReal remainingTime = eventTime - ownerHandler->getOwner()->getTime().time;
		TReal newRemainingTime = newWholeTime / wholeTime * remainingTime;
		wholeTime = newWholeTime;
	  //create the new event
		eventTime = ownerHandler->getOwner()->getTime().time + ut::Time::applyEpsilon( newRemainingTime );
		ownerHandler->getOwner()->addEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
		return true;
	}
	return false;
}

bool MoleculeReport::updateOn2Off( Molecule::ExpressionDirection newDirection )
{
  //remove event, update direction and set the eventTime to null value (no ongoing event)
	ownerHandler->getOwner()->removeEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
	direction = newDirection;
	eventTime = INI_EVENT_TIME;
	return true;
}

bool MoleculeReport::updateOff2On( Molecule::ExpressionDirection newDirection )
{
	direction = newDirection; 
  //sample a new event time and create the event
	wholeTime = model->sampleExpressionTimeAtIndex( expressionIdx, ownerHandler->getOwner()->checkQState( model->getFunctionAtIndex( expressionIdx ) )
	, ownerHandler->getOwner()->checkQState( model->getRandomness() ), ownerHandler->getOwner()->checkQState( model->getCustomRndSymmetry() ) );

	eventTime = ownerHandler->getOwner()->getTime().time + wholeTime;
	ownerHandler->getOwner()->addEvent( Genome::EventType::ELEM_CHANGE, model, model, eventTime, direction );
	return true;
}