#include "cg/LogicHandler.hpp"
#include "ut/Time.hpp" //time.time

#include "cg/Function.hpp" //model
#include "cg/Ode.hpp" //model
#include "cg/Historic.hpp" //model
#include "cg/GateBase.hpp" //model base
#include "cg/Genome.hpp" //owner

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////////// REPORTS ///////////////////////////////////////////////////////////////////////////////////

//=======================================================================================================================================================Historic

//-----------------------------------common
bool HistoricReport::init()
{ 
	ownerHandler->getOwner()->removeChange( model );
	if( model->getHasCustomIniValue() )
		value = ownerHandler->getOwner()->checkQState( model->getIniValueElem() ); //custom initial value
	else
		value = ownerHandler->getOwner()->checkQState( model->getTarget() ); //initial value = that of the target (assumed that it did not change during the last model->delay min)
	ownerHandler->getOwner()->setAndForward( model, value );
	return true;
}

bool HistoricReport::secondInit() 
{ 
	ownerHandler->getOwner()->removeChange( model );
	if( model->getHasCustomIniValue() && value != ownerHandler->getOwner()->checkQState( model->getIniValueElem() ) )
	{
		value = ownerHandler->getOwner()->checkQState( model->getIniValueElem() ); //custom initial value
		ownerHandler->getOwner()->setAndForward( model, value );
	}
	else if( value != ownerHandler->getOwner()->checkQState( model->getTarget() ) )
	{
		value = ownerHandler->getOwner()->checkQState( model->getTarget() ); //initial value = that of the target (assumed that it did not change during the last model->delay min)
		ownerHandler->getOwner()->setAndForward( model, value );
	}
	return update( nullptr ); 
}

bool HistoricReport::update( const EventType* event )
/* two cases:
  a) internal event: update the value to the next stored future value
  b) change in the target: store the future value 
    b1) first value. Different from value. Create the event
    b2) not the first value. Different from the last stored one. Just store */
{
	ownerHandler->getOwner()->removeChange( model );

	if( event && event->emitter == model ) // a - /update value to the first future value -> set changes and reset
	{
		value = futureValues[0].value;
		ownerHandler->getOwner()->setAndForward( model, value );
		futureValues.erase( futureValues.begin() );
		if( futureValues.size() > 0 )
			ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, futureValues[0].time );
		return true;
	}
	else if( futureValues.size() == 0 && ownerHandler->getOwner()->checkQState( model->getTarget() ) != value ) //b1
	{
		storeValue();
		ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, futureValues[0].time );
		return true;
	}
	else if( futureValues.size() > 0 && ownerHandler->getOwner()->checkQState( model->getTarget() ) != futureValues.back().value  ) //b2
	{
		storeValue();
		return true;
	}
	return false;
}

bool HistoricReport::divisionNew( HistoricReport* )
{
	if( futureValues.size() > 0 )
		ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, futureValues[0].time );
	return true;
}


//-----------------------------------private
void HistoricReport::storeValue()
{
	TReal eventTime;
	eventTime = ownerHandler->getOwner()->getTime().time + model->getDelay();
	futureValues.push_back( FutureValue( ownerHandler->getOwner()->checkQState( model->getTarget() ), eventTime ) );
}



//=======================================================================================================================================================Ode

OdeReport::OdeReport( const Ode* model, LogicHandler* ownerHandler )
: GeneticReport( model, ownerHandler ), delta(INI_DELTA), lastVal( INI_LAST_VAL ), lastUpdateTime( ownerHandler->getOwner()->getTime().time ) {;}

//---------------------------------API common
bool OdeReport::init()
{ 
	ownerHandler->getOwner()->removeChange( model );
  //sample and set initial value
	TReal iniVal;
	if( model->getIniValueFunction() )
		iniVal = ut::fitL( ownerHandler->getOwner()->checkQState( model->getIniValueFunction() ) );
	else
		iniVal = ut::fitL( model->sampleIniValue() );
	ownerHandler->getOwner()->setAndForward( model, iniVal );

  //store the derivative
	lastVal = ownerHandler->getOwner()->checkBState( model->getGate() ) ? ownerHandler->getOwner()->checkQState( model->getDeltaElem() ) : 0.0;

	exposeParams( true );
	return true;
}

bool OdeReport::secondInit() 
{ 
	ownerHandler->getOwner()->removeChange( model );

	if( model->getIniValueFunction() && ut::isPositive( ownerHandler->getOwner()->checkQState( model->getIniValueFunction() ) ) && ! ut::areSame( ownerHandler->getOwner()->checkQState( model->getIniValueFunction() ), ownerHandler->getOwner()->checkQState( model ) )  )
		ownerHandler->getOwner()->setAndForward( model, ownerHandler->getOwner()->checkQState( model->getIniValueFunction() ) );
	return update( nullptr ); 
}

void OdeReport::reset()
{ 
	delta = INI_DELTA; 
	lastVal = INI_LAST_VAL; 
	lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

void OdeReport::partialReset()
{ 
	delta = INI_DELTA; 
	lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

bool OdeReport::update( const EventType* )
{
	ownerHandler->getOwner()->removeChange( model );
	TReal newVal = ownerHandler->getOwner()->checkBState( model->getGate() ) ? ownerHandler->getOwner()->checkQState( model->getDeltaElem() ) : 0.0;

	if( ! ut::areSame( newVal, lastVal ) )
	{
		delta += lastVal * ( ownerHandler->getOwner()->getTime().time - lastUpdateTime );
		lastVal = newVal;
		lastUpdateTime = ownerHandler->getOwner()->getTime().time;
		return true;
	}
	return false;
}

bool OdeReport::tick()
{
  //accumulate the last value of the derivative
	delta += lastVal * ( ownerHandler->getOwner()->getTime().time - lastUpdateTime );
  //integrate
	if( ut::notZero( delta ) )
	{
		TReal newVal = ut::fitL( ownerHandler->getOwner()->checkQState( model ) + delta * model->getScale() );
		ownerHandler->getOwner()->setAndForward( model, newVal );
	}
	partialReset();
	return true;
}

bool OdeReport::divisionOld( OdeReport* daughter )
/* the value is considered to be a concentration, so it is converted into an amount by multiplying by the volume
after the halves are sampled, they are converted back to concentrations by the opposite operation */
{
	TReal factor = ownerHandler->getOwner()->getVolume() * model->getPartitionScale();
	TReal divisionFraction = ownerHandler->getOwner()->getGrowthHandler().getLastDivisionFraction();

	uint totalAmount = ut::round<uint>( ownerHandler->getOwner()->checkQState( model ) * factor );
	uint halfAmount = model->samplePartition( totalAmount, divisionFraction, ownerHandler->getOwner()->checkQState( model->getPartitionFunction() ), ownerHandler->getOwner()->checkQState( model->getPartitionRandomness() ) );

	ownerHandler->getOwner()->setAndForward( model, halfAmount / ( factor * divisionFraction ) );
	daughter->ownerHandler->getOwner()->setAndForward( model, ( totalAmount - halfAmount ) / ( factor * ( 1.0 - divisionFraction ) ) );
	partialReset();
	daughter->partialReset();

	exposeParams( false );
	return true;
}


void OdeReport::exposeParams( bool bInit )
{
	if( model->getSplitMarker() && ! model->getPartitionFunction() && ( model->getPartitionRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getSplitMarker(), model->samplePartitionParam( ownerHandler->getOwner()->checkQState( model->getPartitionRandomness() ) ) );
}


/////////////////////////////////////////////////////////////////////////////////////// HANDLER ///////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------API common
bool LogicHandler::init()
{
  //Functions
	for( size_t fun = 0; fun < owner->getCollection()->getElementNum<Function>(); fun++ )
    {
    	const Function* function = owner->getCollection()->getById<Function>( fun );
    	owner->removeChange( function );
    	TReal val = function->evaluate( owner->getQState(), owner->getVolume(), owner->checkQState( function->getRandomness() ) );
    	owner->setAndForward( function, val );
    }
  //Gates
	for( size_t ga = 0; ga < owner->getCollection()->getElementNum<GateBase>(); ga++ )
    {
    	const GateBase* gate = owner->getCollection()->getById<GateBase>( ga );
    	owner->removeChange( gate );
    	owner->setAndForward( gate, gate->check( owner->getBState(), owner->getQState() ) );
    }
  //Odes and Historics
    return HandlerBaseType::init(); 
}

bool LogicHandler::secondInit( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask )
{
	return HandlerBaseType::secondInit( mask, typeSummaryMask ); //only Odes and Historics. Gates and functions are update online by refresh() calls from their inputs
}


//------------------------------------------------------------------- API special
TReal LogicHandler::check( const Function* function )
{
	TReal newState = function->evaluate( owner->getQState(), owner->getVolume(), owner->checkQState( function->getRandomness() ) );
	if( newState != owner->checkQState( function ) )
		owner->setAndForward( function, newState );
	return newState;
}

bool LogicHandler::check( const GateBase* gate )
{
	bool newState = gate->check( owner->getBState(), owner->getQState() );
	if( newState != owner->checkBState( gate ) )
		owner->setAndForward( gate, newState );
	return newState;
}

TReal LogicHandler::deepCheck( const Function* function )
{
	for( const auto& in : function->getInput() )
	{
		if( in->getElemType() == GeneticElement::ElemTypeIdx::FUNCTION )
			deepCheck( static_cast<const Function*>( in ) );
		else if( in->getElemType() == GeneticElement::ElemTypeIdx::GATE )
			deepCheck( static_cast<const GateBase*>( in ) );
	}	
	return check( function );
}	

bool LogicHandler::deepCheck( const GateBase* gate )
{
  //if QGate
	if( gate->getGateType() == GateBase::GateType::QGATE && static_cast<const GateQuantitative*>( gate )->getInput()->getElemType() == GeneticElement::ElemTypeIdx::FUNCTION )
		deepCheck( static_cast<const Function*>( static_cast<const GateQuantitative*>( gate )->getInput() ) );
  //if BGate
	else if( gate->getGateType() == GateBase::GateType::BGATE )
	{
		for( const auto& in : static_cast<const GateBoolean*>( gate )->getInput() )
		{
			if( in->getElemType() == GeneticElement::ElemTypeIdx::FUNCTION )
				deepCheck( static_cast<const Function*>( in ) );
			else if( in->getElemType() == GeneticElement::ElemTypeIdx::GATE )
				deepCheck( static_cast<const GateBase*>( in ) );
		}	
	}
	return check( gate );
}

bool LogicHandler::deepCheck( const GeneticElement* elem )
{
	if( elem->getElemType() == ElemTypeIdx::FUNCTION )
		return deepCheck( static_cast< const Function* >( elem ) );
	else if( elem->getElemType() == ElemTypeIdx::GATE )
		return deepCheck( static_cast< const GateBase* >( elem ) );
	else
		return owner->checkBState( elem );
}


bool LogicHandler::refresh()
{
  //Functions
	for( size_t fun = owner->getChanges().findFirstRel( ElemTypeIdx::FUNCTION ); fun < owner->getChanges().npos(); fun = owner->getChanges().findNextRel( ElemTypeIdx::FUNCTION, fun + 1 ) )
	{
		const Function* function = owner->getCollection()->getById<Function>( fun );
		owner->removeChange( function );
		check( function );
	}
  //Gates
    for( size_t ga = owner->getChanges().findFirstRel( ElemTypeIdx::GATE ); ga < owner->getChanges().npos(); ga = owner->getChanges().findNextRel( ElemTypeIdx::GATE, ga + 1 ) )
	{
		const GateBase* gate = owner->getCollection()->getById<GateBase>( ga );
		owner->removeChange( gate );
		check( gate );
	}
	return true;
}