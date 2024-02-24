#ifndef UT_STATPACK_HPP
#define UT_STATPACK_HPP

#include "ut/defines.hpp"
#include "ut/NameMap.hpp" //typeNM
#include "ut/Stat.hpp"

/*PRECOMPILED
#include <vector> //stats, visibleStats
#include <string> //name
#include <memory> //std::vector< UP<StatBase> > stats */


namespace ut
{
    class StatPack
    {
    	public:
    		enum class Type : size_t
    		{
    			NONE, AVG, CLASSIC, RANGE, ALL, CUSTOM, COUNT
    		};

    		using StatType = StatBase::StatType;
    		static constexpr Type DF_TYPE = Type::AVG;

    		static ut::NameMap<Type> typeNM;
    		static std::vector< std::vector< StatType > > STATS_BY_TYPE;

    		static const std::vector< StatType >& pack2stats( Type type ) { return STATS_BY_TYPE[ static_cast<size_t>( type ) ]; } //convers the type of a pack to the vector of invidiual stats


    	//---ctor, dtor
    		StatPack( Type type ) : type( type ), count( 0 ) { createStats( STATS_BY_TYPE[ static_cast<size_t>( type ) ] ); }
    		StatPack( const std::vector< StatBase::StatType >& statTypes ) : type( Type::CUSTOM ), count( 0 ) { createStats( statTypes ); }
    		StatPack( const StatPack& ) = delete;
    		~StatPack() = default;

    	//---get
    	  //all the stats
    		inline size_t getStatNum() const { return stats.size(); }
    		inline const std::vector< UP<StatBase> >& getStats() const { return stats; }
    		inline TReal getStat( size_t idx ) const { return stats[ idx ]->getValue(); }
    		inline TReal getStatType( size_t idx ) const { return stats[ idx ]->getType(); }
    		inline std::string getStatName( size_t idx ) const { return stats[ idx ]->getTypeStr(); }
    	  //visible stats
    		inline size_t getVisibleStatNum() const { return visibleStats.size(); }
    		inline const std::vector< StatBase* >& getVisibleStats() const { return visibleStats; }
    		inline TReal getVisibleStat( size_t idx ) const { return visibleStats[ idx ]->getValue(); }
    		inline TReal getVisibleStatType( size_t idx ) const { return visibleStats[ idx ]->getType(); }
    		inline std::string getVisibleStatName( size_t idx ) const { return visibleStats[ idx ]->getTypeStr(); }
    		inline bool getStatVisibility( size_t idx ) const { return stats[idx]->getBVisible(); }
    	  //count
    		inline uint getCount() const { return count; }

    	//---API
    	  //precompute
    		void createStats( const std::vector< StatBase::StatType >& statTypes );
    	  //run
    		void firstPass( TReal newValue );
			inline void secondPass( TReal newValue, size_t idx ) { stats[ idx ]->secondPass( newValue ); }
			inline void final( size_t idx ) { stats[ idx ]->final( count ); }
			inline void commit() { for( auto& stat : stats ) stat->commit(); }
			inline void reset() { count = 0; for( auto& stat : stats ) stat->reset(); }
		  //aux
			inline size_t idxToVisibleIdx( size_t idx ) const { return stats[ idx ]->visibleIdx; }

    		
    	private:
    		Type type; //type of stat
    		std::vector< UP<StatBase> > stats; //individual stats that make the stack. Ownership
    		std::vector< StatBase* > visibleStats; //pointers to just the visible stats of "stats"
    		uint count; //total number of elements used to compute the stats


    		void createFullPack(); //populate with al the types of Stat
    		void linkStats(); //create pointers form stats to their required input stats
    };
}

#endif //UT_STATPACK_HPP