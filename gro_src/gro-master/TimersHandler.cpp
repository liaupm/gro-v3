#include "TimersHandler.h"
#include "ut/Routine.hpp" //update() override
#include "ut/Time.hpp" //.step

#include "Timer.h" //model
#include "Checkpoint.h" //model
#include "PetriDish.h" //owner, setState()
#include "World.h" //getTime()
#include "GroThread.h" //addCheckpoint()


//////////////////////////////////////////////////// TIMER REPORT ////////////////////////////////////////////////////////////////

TimerReport::TimerReport( const Timer* model, TimersHandler* ownerHandler ) 
: GroReport( model, ownerHandler )
, bActive( model->getEnd() >= model->getStart() ), bDeactivate( false ), state( false ), nextEvent( model->getStart() )
{
    switch( model->getMode() )
    {
        case Timer::Mode::FIRST:
            condition = std::make_shared<ConditionFirst>();
            break;
        case Timer::Mode::CHANGE:
            condition = std::make_shared<ConditionChange>();
            break;
        case Timer::Mode::TRIGGER:
            condition = std::make_shared<ConditionTrigger>();
            break;
        default:
            condition = std::make_shared<ConditionAlways>();
    }
}


bool TimerReport::update( const EventType* )
{ 
	if( ! bActive )
		return false;

  //set to off, done for all the active Timers every tome step, as the events do not last 
    //bool oldState = state; //used to know if it was previously on, to inactivate those of First mode
	state = false;
	ownerHandler->getOwner()->setState( model, state );

  //deactivate
	if( bDeactivate )
	{
		bActive = false;
		return false;
	}

  //get state
	bool bFirstTrigger = false;
	if( model->getMode() == Timer::Mode::TRIGGER && ! std::static_pointer_cast<ConditionTrigger>( condition )->getBTriggered() )
		bFirstTrigger = true;
	bool newState = condition->check( ownerHandler->getOwner()->checkBState( model->getGate() ) ); //state of the condition, polymorphism
	
  //update if nedeed
	//if this is the time of the current event or when a Trigget type is true for the first time
	if( nextEvent == ownerHandler->getOwner()->getWorld()->getTime().step || bFirstTrigger )
	{
		newState &= ( nextEvent != model->getStart() || ! model->getBIgnoreStart() ); //false if start and start must be ignored
	  //calculate next event
		nextEvent = ownerHandler->getOwner()->getWorld()->getTime().step + model->getTimeAdvancer()->deltaStep( ownerHandler->getOwner()->getWorld()->getTime() ); //get next event time
	  //check for deactivation
		if( nextEvent > model->getEnd() || ( model->getMode() == Timer::Mode::FIRST && newState ) ) //must be deactivated if the next event is not before the stop time or it is First (once) mode
			bDeactivate = true;
	  //update the state if true (already updated to false)
		if( newState )
		{
			state = newState;
			ownerHandler->getOwner()->setState( model, state );
		}
	}
	return state;
}


/////////////////////////////////////////////// HANDLER /////////////////////////////////////////////
bool TimersHandler::update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* event )
{ 
  //timers
	ut::applyRoutine<ut::UpdateRoutine>( reports, event ); 

  //checkpoints
	for( size_t ch = 0; ch < owner->getCollection()->getElementNum<Checkpoint>(); ch++ )
    {
    	const Checkpoint* checkpoint = owner->getCollection()->getById<Checkpoint>( ch );
    	if( owner->checkBState( checkpoint->getTimer() ) )
    	{
			owner->getWorld()->getCallingThread()->addCheckpoint( checkpoint );
			owner->getWorld()->getCallingThread()->setBUpdateGraphics( true );
    	}
    }
    return true;
}
