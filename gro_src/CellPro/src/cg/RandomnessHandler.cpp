#include "cg/RandomnessHandler.hpp"
#include "cg/Genome.hpp" //owner
#include "cg/Randomness.hpp" //model

using namespace cg;


////////////////////////////////////////////////////////// REPORTS //////////////////////////////////////////////////////////////////////////////

//============================================ RandomnessReport

bool RandomnessReport::init()
{
	if( ! model->getActiveForCellType( ownerHandler->getOwner()->getCellType()->getRelativeId() ) ) //TODO better optimization (requires change at LinkedElement)
		return false;

  //sampling the value
	ownerHandler->getOwner()->justSet( model, model->sampleSimple() );
	//no refresh or changes added as the Randomnesses are the first elements to be init

  //setting the next sampling time and event
	if( model->getBSpontaneousChange() )
	{
		nextSamplingTime = ownerHandler->getOwner()->getTime().time + model->sampleSamplingTime();
		ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, nextSamplingTime );
		return true;
	}
	return false;
}

bool RandomnessReport::divisionOld( RandomnessReport* )
{
  //updating the value and seting changes for dependent elements
	if( model->getBChangeInDivision() )
	{
		ownerHandler->getOwner()->setAndForwardExcludeSelf( model, model->sampleWithInheritance( ownerHandler->getOwner()->checkQState( model ) ) );
		return true;
	}
	return false;
}

bool RandomnessReport::divisionNew( RandomnessReport* )
{
  //creating a new next sampling event only if the waiting time is stochastic; if not, that copied from the mother is kept
	//the event has to be created anyway as they are not copied
	if( model->getBSpontaneousChange() )
	{
		if( model->getBRndSamplingTime() )
			nextSamplingTime = ownerHandler->getOwner()->getTime().time + model->sampleSamplingTime();
		ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, nextSamplingTime );
	}

  //updating the value and seting changes for dependent elements
	if( model->getBChangeInDivision() )
	{
		ownerHandler->getOwner()->setAndForwardExcludeSelf( model, model->sampleWithInheritance( ownerHandler->getOwner()->checkQState( model ) ) );
	 	return true;
	}
	return false;
}

bool RandomnessReport::update( const EventType* event )
//called only on sampling events
// it is always a Ransomness with spontaneous sampling
{
  //only in spontaneous re-sampling events (ignore changes from value changes on cell division )
	if( ! event || event->emitter != model )
	{
		ownerHandler->getOwner()->removeChange( model );
		return false;
	}
  //updating the value and seting changes for dependent elements
	//removing self from the added changes. The condition above prevents the infinite loop anyways, but this way it's more efficient
	ownerHandler->getOwner()->setAndForwardExcludeSelf( model, model->sampleWithInertia( ownerHandler->getOwner()->checkQState( model ) ) ); 

  //setting the next sampling time and event
	nextSamplingTime = ownerHandler->getOwner()->getTime().time + model->sampleSamplingTime();
    ownerHandler->getOwner()->addEvent( Genome::EventType::INTERNAL, model, model, nextSamplingTime );
	return true;
}