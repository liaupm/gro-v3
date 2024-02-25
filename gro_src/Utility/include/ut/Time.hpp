#ifndef UT_TIME_HPP
#define UT_TIME_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <algorithm> //min, max
#include <cmath> //std::floor */


namespace ut
{
	class Time
	{
		public:
		//--static
			using TimeType = TReal;
			using StepType = ulong2;
			
			static constexpr TimeType DF_STEP_SIZE = 0.1;
			static constexpr TimeType DF_EPSILON = VERY_LITTLE * DF_STEP_SIZE;

			static constexpr uint DF_ROUNDER = 1.0 / DF_EPSILON;

			static constexpr bool checkEpsilon( TimeType value ) { return value >= DF_EPSILON; }
			static constexpr TimeType applyEpsilon( TimeType value = 0.0 ) { return ut::fitL( value, DF_EPSILON ); }

		//---ctor, dtor
			Time( TimeType stepSize = DF_STEP_SIZE, TimeType iniTime = 0.0 ) :
			stepSize( stepSize ), inverseStepSize( 1.0 / stepSize ), time( iniTime ), iniTime( iniTime ), step( 0 ) {;}

			virtual ~Time() = default;
			operator TimeType() { return time; } //implicit conversion to .time
			//Time( const Time& rhs ) = default;
			//Time( Time&& rhs ) noexcept = default;
			//Time& operator=( const Time& rhs ) = default;
			//Time& operator=( Time&& rhs ) noexcept = default;

		//---get

		//---set
			inline void setFromTime( TimeType xTime ) { time = xTime; recalculateStep(); } 
			inline void setFromStep( StepType xStep ) { step = xStep; recalculateTime(); } 

		//---API
		  //time-step conversions
			inline StepType time2step( TimeType time ) const { return ut::round<StepType>( time / stepSize ); }
        	inline TimeType step2time( StepType step ) const { return static_cast<TimeType>( static_cast<TimeType>( step ) * stepSize ); }
        	inline StepType getStepFromTime() const { return time2step( time ); }
			inline TimeType getTimeFromStep() const { return step2time( step ); }
        	inline void recalculateTime() { time = step2time( step ); }
        	inline void recalculateStep() { step = time2step( time ); }
          //run
        	inline void setIniTime() { iniTime = time; }
        	inline StepType justAdvanceStep() { return ++step; } 
			inline StepType advance() { step++; recalculateTime(); roundTime(); return step; } 
			inline TimeType getNextTime() const { return step2time( step + 1 ); }
			inline void reset( StepType xStep = 0 ) { step = xStep; recalculateTime(); roundTime(); setIniTime(); }
			inline void reset( TimeType xTime ) { time = iniTime = xTime; recalculateStep(); }
		  //misc
        	inline void roundTime() { time = std::floor( time * DF_ROUNDER ) / DF_ROUNDER; }


		//private:
			TimeType stepSize;
			TimeType inverseStepSize;
			TimeType time;
			TimeType iniTime;
			StepType step;
	};
}

#endif //UT_TIME_HPP