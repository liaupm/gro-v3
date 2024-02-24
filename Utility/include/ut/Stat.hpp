#ifndef UT_STAT_HPP
#define UT_STAT_HPP

#include "ut/defines.hpp"
#include "ut/Metadata.hpp" //REGISTER_SUBTYPE
#include "ut/NameMap.hpp" //statTypeNM
#include "ut/Factory.hpp" //StatFactory

/*PRECOMPILED
#include <vector>  //inputTypes, inputStats
#include <string> //getTypeStr()
#include <limits>  //quiet_NaN
#include <math.h>  //std::pow */


namespace ut
{
	class StatBase
	{
		public:
			friend class StatPack;

			enum StatType : int
            {
                ACCUMULATION, SUM, AVG, STDDEV, MIN, MAX, COUNT
            };

            using SubtypeIdx = StatType;

			static constexpr TReal INI_VALUE = 0.0;
			static constexpr bool DF_B_VISIBLE = false;
			static constexpr size_t DF_VISIBLE_IDX = 0;

			static ut::NameMap<StatType> statTypeNM;

		//---ctor, dtor
			virtual ~StatBase() = default;
			/*Stat( const Stat& rhs ) = default;
			Stat( Stat&& rhs ) noexcept = default;
			Stat& operator=( const Stat& rhs ) = default;
			Stat& operator=( Stat&& rhs ) noexcept = default;*/

		//---get
			inline StatType getType() const { return type; }
			inline std::vector< StatType > getInputType() const { return inputTypes; }
			inline std::string getTypeStr() const { return statTypeNM.subtype2Name( type ); }
			inline TReal getValue() const { return value; }
			inline bool getBVisible() const { return bVisible; }

		//---set
			inline void setVisibility( bool val ) { bVisible = val; }

		//---API
			virtual void firstPass( TReal newValue ) = 0;
			virtual void secondPass( TReal newValue ) = 0;
			virtual void final( TReal total ) = 0;
			virtual void commit() {;}
			virtual void reset() { value = INI_VALUE; bFirstVal = true; }


		protected:
			StatType type; //statistic operator to calculate
			std::vector< StatType > inputTypes; //which other stats are required as input. E.g. average for stddev
			std::vector< const StatBase* > inputStats; //other stats are required as input, just pointers, no ownership
			
			TReal value; //current calculated value
			bool bFirstVal; //wheter the element is just reset and waiting for the first value or there is a calculation already started. Required by min and max
			
			bool bVisible; //wheter this stat is externally used or just an automatically created input for another stat
			size_t visibleIdx; //the idx within the visible stats of a StatPack, only used if bVisible
			

			StatBase( StatType type );
	};



////////////////////////////////////// DERIVED STATS /////////////////////////////////////////

	class AccumulationStat : public StatBase
	{
		REGISTER_SUBTYPE( "acc", "acc", StatBase::SubtypeIdx::ACCUMULATION )

		public:
			inline static const std::vector<StatType> INPUT_TYPES = {};

			AccumulationStat() : StatBase( StatBase::SubtypeIdx::ACCUMULATION ), lastVal( 0.0 ) {;}
			virtual ~AccumulationStat() = default;

			TReal getLastVal() const { return lastVal; }
			void setLastVal( TReal xLastVal ) { lastVal = xLastVal; } 
			
			void firstPass( TReal newValue ) override { value += newValue; bFirstVal = false; }
			void secondPass( TReal ) override {;}
			void final( TReal ) override {;}
			void commit() override { lastVal = value; } 
			void reset() override { value = lastVal; bFirstVal = true; }


		private:
			TReal lastVal;
	};

	class SumStat : public StatBase
	{
		REGISTER_SUBTYPE( "sum", "sum", StatBase::SubtypeIdx::SUM )

		public:
			inline static const std::vector<StatType> INPUT_TYPES = {};

			SumStat() : StatBase( StatType::SUM ) {;}
			virtual ~SumStat() = default;

			void firstPass( TReal newValue ) override { value += newValue; }
			void secondPass( TReal ) override {;}
			void final( TReal ) override { /*if( total == 0.0 ) value = std::numeric_limits<double>::quiet_NaN()*/; }
	};

	class AverageStat : public StatBase
	{
		REGISTER_SUBTYPE( "average", "avg", StatBase::SubtypeIdx::AVG )

		public:
			inline static const std::vector<StatType> INPUT_TYPES = { StatType::SUM };

			AverageStat() : StatBase( StatType::AVG ), bConst( true ), constVal( INI_VALUE ) {;}
			virtual ~AverageStat() = default;

			void firstPass( TReal newValue ) override 
			{ 
				if( bFirstVal )
				{
					constVal = newValue;
					bFirstVal = false;
				}
				else if( newValue != constVal )
					bConst = false;
			}

			void secondPass( TReal ) override {;}
			void final( TReal total ) override 
			{ 
				if( bConst )
					value = constVal;
				else
				{
					if( isPositive ( total ) )
						value = inputStats[0]->getValue() / total;
					else
						value = 0.0;
						//value = std::numeric_limits<double>::quiet_NaN();	
				}
			}

			void reset() override { bConst = true; constVal = INI_VALUE; StatBase::reset(); } 

		private:
			bool bConst;
			TReal constVal;
	};

	class StddevStat : public StatBase
	{
		REGISTER_SUBTYPE( "standard_deviation", "stddev", StatBase::SubtypeIdx::STDDEV )
		public:
			inline static const std::vector<StatType> INPUT_TYPES = { StatType::AVG };

			StddevStat () : StatBase( StatType::STDDEV ) {;}
			virtual ~StddevStat () = default;

			void firstPass( TReal ) override {;}
			void secondPass( TReal newValue ) override { value += std::pow( newValue - inputStats[0]->getValue(), 2 ) ; }
			void final( TReal total ) override
			{ 
				if( isPositive ( total ) )
					value = std::sqrt( value / ( total - 1.0 ) ); 
				else
					value = 0.0;
					//value = std::numeric_limits<double>::quiet_NaN();
			}
	};

	class MinStat : public StatBase
	{
		REGISTER_SUBTYPE( "minimum", "min", StatBase::SubtypeIdx::MIN )

		public:
			inline static const std::vector<StatType> INPUT_TYPES = {};

			MinStat () : StatBase( StatType::MIN ) {;}
			virtual ~MinStat () = default;

			void firstPass( TReal newValue ) override { value = bFirstVal ? newValue : std::min( value, newValue ); bFirstVal = false; }
			void secondPass( TReal ) override {;}
			void final( TReal ) override { /*if( total == 0.0 ) value = std::numeric_limits<double>::quiet_NaN()*/; }
	};

	class MaxStat : public StatBase
	{
		REGISTER_SUBTYPE( "maximum", "max", StatBase::SubtypeIdx::MAX )
		
		public:
			inline static const std::vector<StatType> INPUT_TYPES = {};

			MaxStat () : StatBase( StatType::MAX ) {;}
			virtual ~MaxStat () = default;

			void firstPass( TReal newValue ) override { value = bFirstVal ? newValue : std::max( value, newValue ); bFirstVal = false; }
			void secondPass( TReal ) override {;}
			void final( TReal ) override { /*if( total == 0.0 ) value = std::numeric_limits<double>::quiet_NaN()*/; }
	};


///////////////////////////////////////////////// FACTORY /////////////////////////////////////////////////////
    using StatFactory = Factory< StatBase, SumStat, AverageStat, StddevStat, MaxStat, MinStat >;

   	struct InputTypesExtractor
    {
    	template< typename T >
    	static const std::vector< StatBase::StatType >& call() { return T::INPUT_TYPES; }
    };
}

#endif //UT_STAT_HPP