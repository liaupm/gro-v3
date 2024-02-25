#ifndef CG_EVENT_SCHEDULER_HPP
#define CG_EVENT_SCHEDULER_HPP

#include "ut/SortedVector.hpp" //ut::PtrSortedVector UnderDS
#include "ut/Event.hpp" //EventType

#include "cg/defines.hpp"


namespace cg
{
    class GeneticElement; //elem in addEvent() and removeEvent()
    class GeneticCollection; //ctor
    class EventScheduler
    {
        public:
            using EventType = ut::Event<GeneticElement>;
            using EventMode = EventType::EventMode;
            using UnderDS = ut::PtrSortedVector< EventType, TReal, &EventType::time, true >;
            static constexpr size_t INI_IDX = 0;


        //---ctor, dtor
            EventScheduler() : currentIdx( INI_IDX ), firstIdx( INI_IDX ), lastIdx( INI_IDX ) {;}

            EventScheduler( const EventScheduler& ) //custom copy constructor to avoid copying the events DS
            : currentIdx( INI_IDX ), firstIdx( INI_IDX ), lastIdx( INI_IDX ) {;}

            virtual ~EventScheduler() = default;
            EventScheduler& operator=( const EventScheduler& rhs ) = default;
            EventScheduler( EventScheduler&& rhs ) noexcept = default;
            EventScheduler& operator=( EventScheduler&& rhs ) noexcept = default;

        //---get
            inline const UnderDS& getEvents() const { return events; }
            inline UnderDS::const_iterator getCurrentIt() const { return events.cbegin() + currentIdx; }
            inline UnderDS::const_iterator getLastIt() const { return events.cbegin() + lastIdx; }
            inline const EventType* getCurrent() const { return events[ currentIdx ].get(); }
            inline TReal getTime() const { return events[ currentIdx ]->time; }

        //---API
            void addEvent( EventMode eventMode, const GeneticElement* emitter, const GeneticElement* elem, TReal eventTime, int quantity = 0 );
            void removeEvent( EventMode eventMode, const GeneticElement* emitter, const GeneticElement* elem, TReal eventTime, int quantity = 0 );

            const EventType* nextEvent( TReal simulationTime ); //return the next event, eadvancing the time if required. nullptr if no more events
            UnderDS::const_iterator nextTime( TReal simulationTime ); //advances the time and set the first, current and last event for that time. cend if no more events
            const EventType* nextEventSameTime(); //return the next event for the curent time or nullptr if no more (signal for finding the next time). nullptr if no more events
            inline const EventType* backToFirst() { currentIdx = firstIdx; return ( events.cbegin() + currentIdx )->get(); } //move currentIdx to the first event for the current time
            inline bool findNextTime( TReal simulationTime ) { return nextTime( simulationTime ) != events.end(); } //calls nextTime() and returns whether there are more events
            inline void reset() { currentIdx = firstIdx = lastIdx= INI_IDX; } //set the idxs to the initial value
            inline void commit() { events.erase( events.begin(), events.begin() + lastIdx ); reset(); } //removes the events for the current time and resets the idxs

            
        private:
            UnderDS events;
            size_t currentIdx; //current event
            size_t firstIdx; //first event for the current time
            size_t lastIdx; //last event for the current time + 1
    };
}

#endif //CG_EVENT_SCHEDULER_HPP