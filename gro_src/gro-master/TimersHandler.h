#ifndef TIMERSHANDLER_H
#define TIMERSHANDLER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "Defines.h"
#include "GroHandler.h" //base
#include "GroReport.h" //base

//PRECOMPILED
/* #include <tuple> //std::get<>
#include <memory> //SP, static_pointer_cast in cpp */


struct TimersHReportIdx
{
	enum ReportTypeIdx : size_t { TIMER, COUNT };
};

namespace ut { class Bitset; class MultiBitset; } //update() params
class Timer; //model
class Checkpoint; //model
class GroCollection; //ctor
class TimersHandler;
class TimerReport : public GroReport< Timer, TimersHandler >
{
    public:
        //friend class LogicHandler;
        using StepType = Timer::StepType;

    ////////////////////////////////////////// NESTED CONDITIONAL BEHAVIOUR //////////////////////////////////////////////////////////////////////
        class Condition
        /* virtual base class */
        {
            public:
                Condition() = default;
                virtual ~Condition() = default;
                
                virtual bool check( bool newState ) = 0;
        };

        class ConditionFirst : public Condition
        /* only once, the first time the condition holds */
        {
            public:
                ConditionFirst() : bDone( false ) {;}
                virtual ~ConditionFirst() = default;
                
                bool check( bool newState ) override { if( newState && ! bDone ) { bDone = true; return true; } return false; }
            
            private:
                bool bDone; //whether it has already been triggered, so it never will
        };

        class ConditionChange : public Condition
        /* when the condition switches to true */
        {
            public:
                ConditionChange() : lastState( false ) {;}
                virtual ~ConditionChange() = default;

                bool check( bool newState ) override { if( newState != lastState ) { lastState = newState; return newState; } return false; }
            
            private:
                bool lastState; //the previous state to check if it changes
        };

        class ConditionTrigger : public Condition
        /* every period once the condition holds once */
        {
            public:
                ConditionTrigger() : bTriggered( false ) {;}
                virtual ~ConditionTrigger() = default;

                inline bool getBTriggered() const { return bTriggered; } 
                bool check( bool newState ) override { if( newState && ! bTriggered ) bTriggered = true; return bTriggered; }

            
            private:
                bool bTriggered; //whether it is active. Once activated, it stays active forever
        };

        class ConditionAlways : public Condition
        /* every time it is resampled (period) and the condition holds */
        {
            public:
                ConditionAlways() {;}
                virtual ~ConditionAlways() = default;

                bool check( bool newState ) override { return newState; }
        };

    
////////////////////////////////////////// TIMER REPORT  //////////////////////////////////////////////////////////////////////

	//---ctor, dtor
        TimerReport( const Timer* model, TimersHandler* ownerHandler );
        virtual ~TimerReport() = default;

    //---get
        inline bool getState() const { return state; }

    //---API
        void link() {;}
        bool init() { return true; }
        bool secondInit() { return true; }
        void reset() {;}
        bool divisionOld( TimerReport* ) { return true; }
        bool divisionNew( TimerReport* ) { return true; }
        bool update( const EventType* );
        bool tick() { return true; }


    private:
        SP<Condition> condition; //conditional bahaviour
    	bool bActive; //whether the timer is active. Once inactive (after stop time or type First), it is not evaluated any more. 
        bool bDeactivate; //whether the timer has to be inactivated in the next update
		bool state; //whether a timer event is triggered the current time step
		StepType nextEvent; //step of the next timer event
};


////////////////////////////////////////// HANDLER //////////////////////////////////////////////////////////////////////

class TimersHandler : public GroHandler< ut::ReportsDS<TimerReport> >
{
	REGISTER_CLASS( "TimersHandler", "timersH", GroHandler<>::HandlerTypeIdx::H_TIMERS )
	
	public:
    //---ctor, dtor
    	TimersHandler( PetriDish* owner, const GroCollection* collection ) : GroHandler( owner, collection ) {;}
    	virtual ~TimersHandler() = default;
    //---get
        inline const std::vector<TimerReport>& getTimerReports() const { return std::get< TimersHReportIdx::TIMER >( this->reports ); }
        inline std::vector<TimerReport>& getTimerReportsEdit() { return std::get< TimersHReportIdx::TIMER >( this->reports ); }
    //---API
        bool init() override { update( nullptr, nullptr, nullptr ); return true; }
        bool update( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask, const GroHandler<>::EventType* event ) override;
};

#endif // TIMERSHANDLER_H
