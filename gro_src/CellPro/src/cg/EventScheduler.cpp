#include "cg/EventScheduler.hpp"

/*PRECOMPILED
#include <memory> //std::make_shared in createEvent()
#include <algorithm> //std::find_if in removeEvent() */

using namespace cg;


//================================================ API =================================================

void EventScheduler::addEvent( EventMode eventMode, const GeneticElement* emitter, const GeneticElement* elem, TReal eventTime, int quantity ) 
{ 
	events.add( std::make_shared<EventType>( eventMode, emitter, elem, eventTime, quantity ) );
}

void EventScheduler::removeEvent( EventMode eventMode, const GeneticElement* emitter, const GeneticElement* element, TReal eventTime, int quantity )
{
	auto compEvents = [eventMode, emitter, element, eventTime, quantity ]( CSP<EventType> event ){ return event->compare( eventMode, emitter, element, eventTime, quantity ); };
	auto it = std::find_if( events.begin(), events.end(), compEvents );
	if( it != events.end() )
		events.erase( it );
}


const EventScheduler::EventType* EventScheduler::nextEvent( TReal simulationTime )
{
	currentIdx++;
	if( currentIdx < lastIdx ) //same time
		return events[currentIdx].get();

	auto it = nextTime( simulationTime ); //next time
	return it != events.cend() ? it->get() : nullptr;
}

EventScheduler::UnderDS::const_iterator EventScheduler::nextTime( TReal simulationTime ) 
{ 
	if( events.size() <= lastIdx || events[ lastIdx ]->time > simulationTime ) //no pending events for this time step
		return events.cend();
 
	currentIdx = firstIdx = lastIdx; //advance to next events block
    while( lastIdx < events.size() && events[lastIdx]->time == events[firstIdx]->time ) //find the last + 1
    	lastIdx++;

    return events.cbegin() + firstIdx; //return next event
}

const EventScheduler::EventType* EventScheduler::nextEventSameTime()
{
	currentIdx++;
	if( currentIdx < lastIdx )
		return events[ currentIdx ].get();
	return nullptr;
}