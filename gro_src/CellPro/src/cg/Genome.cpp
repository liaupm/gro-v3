#include "cg/Genome.hpp"
#include "ut/Time.hpp" //.time
#include "cg/CellType.hpp" //getStrain(), add initial QPlasmids

using namespace cg;


//================================================================================================= ctor, dtor
Genome::Genome( GeneticCollection* collection, const CellType* cellType, const ut::Time& time, const std::vector<TReal>& mediumConcs )
: GenomeMembersWrapper( cellType )
, AgentBaseType( collection, time )
{ 
  //initial volume, required by Functions and other calculations in concentrations, including the initial copy number of QPlasmids
	getHandlerEdit<HandlerIdx::H_GROWTH>().preInitVol();
  //initial contents
	justAddContent( cellType, 1 ); //Molecules and BPlasmids, deterministic
	justAddContent( cellType->getStrain(), 1 ); //just the Strain 
	addQPlasmids(); //the copy number may be stochastic or dependent on the volume
  //init all the handlers, including logic. Tracking the changes and refreshing before is not neccesary as all the Functions and Gates are checked here
	initMetExternal( mediumConcs );
	initHandlers(); 
	initCycle(); 
}

Genome::Genome( Genome* mother )
: GenomeMembersWrapper( mother->cellType )
, AgentBaseType( mother )
{ 
	mother->divisionOldHandlers( this );
	divisionNewHandlers( mother ); 
  //update after the changes introduced on division (resampling, partition of q elements...)
	mother->updateCycle( nullptr );
	updateCycle( nullptr );
}


//======================================================================== API
bool Genome::asynchronousUpdate( double simTime )
{
	SchedulerType& scheduler = getSchedulerEdit();
	while( scheduler.findNextTime( simTime ) ) //get next time with events
	{
	  //advance the time
		time.time = scheduler.getTime();
	  //make the element changes for all the events for the same time. This way, when there are several events for the same time, the order do not affect, there is no need for random ordering
		for( const EventType* ev = scheduler.getCurrent(); ev != nullptr; ev = scheduler.nextEventSameTime() )
		{
			if( ev->eventMode == EventType::EventMode::ELEM_CHANGE )
			{
				justAddContent( ev->element, ev->quantity );
				refreshLogicFromContent( ev->element );
				//addAndForwardContent( ev->element, ev->quantity );
				//refreshLogic();
			}
		}
	  //iterate the events for the time again, now attending the changes and the INTERNAL events too and passing each event  to the handlers and reports
		scheduler.backToFirst();
		for( const EventType* ev = scheduler.getCurrent(); ev != nullptr; ev = scheduler.nextEventSameTime() )
		{
			if( ev->eventMode == EventType::EventMode::ELEM_CHANGE  )
				forwardFromContent( ev->element );
			else if( ev->element != nullptr )
				addChange( ev->element );
				//forwardFrom( ev->element );
				//addChangesFrom( ev->element );
            if( ev->emitter != nullptr && ev->element != ev->emitter  )
            	addChange( ev->emitter );
                //forwardFrom( ev->emitter );
                //addChangesFrom( ev->emitter );

			updateCycle( ev );
		}
	}
	scheduler.commit(); //remove the applied events
	time.setFromTime( simTime );
	//time.time = simTime; //advance the time to the end of the time step
	return true;
}


//---private
void Genome::addQPlasmids()
/* add the initial copy numbers of QPlasmids, determined by the CellType
may be stochastic and may depend on the initial volume 
used only for newly created cells (no daughter cells) */
{
	for( size_t p = 0; p < cellType->getQPlasmidNum(); p++ )
		justAddContent( cellType->getQPlasmids()[p], cellType->sampleAmount( p, getVolume() ) );
}

void Genome::setEventMarkersOff()
{
	for( size_t em = getBState().findFirstRel( ElemTypeIdx::EVENT_MARKER ); em < getBState().npos(); em = getBState().findNextRel( ElemTypeIdx::EVENT_MARKER, em + 1 ) )
		setAndForward( collection->getById<EventMarker>( em ), false );
}