#ifndef TIMER_H
#define TIMER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp"  //modeNM
#include "ut/DistributionCombi.hpp" //eventsDist in random Timers
#include "ut/Time.hpp" //StepType, time2step()

#include "Defines.h"
#include "GroElement.h" //base

//PRECOMPILED
/* #include <limits> //DF_END, DF_PERIOD
#include <memory> //SP in timeAdvancer
#include <string> //name
#include <algorithm> //max */


class PopulationGate; //gate  in conditional Timers
class GroCollection; //ctor
class Timer : public GroElement
{
    REGISTER_CLASS( "Timer", "t", ElemTypeIdx::TIMER )
	public:
        using StepType = ut::Time::StepType;
	  
/////////////////////////////////////////////////////////// NESTED TIME ADVANCERS //////////////////////////////////////////////////////
		class NextTimeBase
		/* virtual base class for time advancers */
		{
			public: 
                virtual StepType deltaStep( const ut::Time& t ) = 0;
			
			protected:
				NextTimeBase() = default;
			
		};

		class NextTimeDeterministic : public NextTimeBase
		/* deterministic time advancer: a fixed amount (period) is added every time */
		{
			public: 
				NextTimeDeterministic( StepType period ) : NextTimeBase(), period( period ) {;}
				
                virtual StepType deltaStep( const ut::Time& ) { return period; }
			
			private:
				StepType period; //the period in time step (rounded)
		};

		class NextTimeRnd : public NextTimeBase
		/* stochastic time advancer. The increments are sampled from an exponential distribution and converted to time step by rounding 
		if "bIgnoreStart" = true, the first event will occur at a random time step
		if false, the first one is deterministic and occurst at "start" */
		{
			public: 
				NextTimeRnd( GroCollection* groCollection, TReal period, TReal periodMin );

				virtual StepType deltaStep( const ut::Time& t ) { return std::max<StepType>( t.time2step( periodMin + eventsDist.sample() ), 1 ); }
			
			private:
				TReal periodMin; //minimum period, added to the sampled values
				mutable ut::ConcDistCombi< ut::ExponentialDist > eventsDist; //exponential distribution
		};



/////////////////////////////////////////////////////////// TIMER //////////////////////////////////////////////////////
		enum class Mode
		{
			ALWAYS, FIRST, CHANGE, TRIGGER, COUNT
		};

		static constexpr Mode DF_MODE = Mode::ALWAYS;
		static constexpr TReal DF_START = 0.0;
		static constexpr TReal DF_END = std::numeric_limits<TReal>::max();
		static constexpr TReal DF_PERIOD = 0.1;
		static constexpr TReal DF_PERIOD_MIN = 0.0;
		static constexpr bool DF_B_RND = false;
		static constexpr bool DF_B_IGNORE_START = false;
		
		//str to Mode. Inline to allow for static const initializations in Parser
		inline static const ut::NameMap<Mode> modeNM = ut::NameMap<Mode>( { 
								{ "always", Mode::ALWAYS }
                                ,  { "once", Mode::FIRST }
                                , { "change", Mode::CHANGE }
                                , { "trigger", Mode::TRIGGER } } );

	//---ctor, dtor
	    Timer( GroCollection* groCollection, size_t id, const std::string& name
	    , const PopulationGate* gate, const ut::Time& time, Mode mode
	    , TReal start, TReal end, TReal period, bool bRnd = false, TReal periodMin = DF_PERIOD_MIN, bool bIgnoreStart = DF_B_IGNORE_START );

	    virtual ~Timer() = default;

	//---get
	  //resources
	    inline const PopulationGate* getGate() const { return gate; }
	    inline NextTimeBase* getTimeAdvancer() const { return timeAdvancer.get(); }
	  //options
	    inline Mode getMode() const { return mode; }
	    inline StepType getStart() const { return start; }
	    inline StepType getEnd() const { return end; }
	    inline bool getBIgnoreStart() const { return bIgnoreStart; }

	//---API
	  //precompute
	    void loadDirectElemLink() override;
	    void passUsedForTimers() override;


	private:
	  //resources
		const PopulationGate* gate; //populatio-level condition of conditional Timers
		SP<NextTimeBase> timeAdvancer; //selector of the next Timer event
	  //options
		Mode mode; //pattern of conditional Timers
		StepType start; //first time step when it is evaluated, potential first event
		StepType end; //last time step when it is evaluated, potential last event
		bool bIgnoreStart; //whether to ignore the events that happens in the "start" time step, for random Timers
};

#endif // TIMER_H
