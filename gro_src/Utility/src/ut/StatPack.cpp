#include "ut/StatPack.hpp"

/*PRECOMPILED
#include <set>//createStats() */

using namespace ut;



ut::NameMap< StatPack::Type > StatPack::typeNM ( { 
                                          { "none", StatPack::Type::NONE }
                                        , { "avg", StatPack::Type::AVG }
                                        , { "classic", StatPack::Type::CLASSIC }
                                        , { "range", StatPack::Type::RANGE }
                                        , { "all", StatPack::Type::ALL }
                                        } );


std::vector< std::vector< StatPack::StatType > > StatPack::STATS_BY_TYPE = {
	{}
	, { StatType::AVG }
	, { StatType::SUM, StatType::AVG, StatType::STDDEV }
	, { StatType::MIN, StatType::MAX }
	, { StatType::SUM, StatType::AVG, StatType::STDDEV, StatType::MIN, StatType::MAX }
	, {}
};


//-----------------------------------------------------------------------------------preprocess
void StatPack::createStats( const std::vector< StatBase::StatType >& statTypes )
{
	createFullPack();
	std::set< StatBase::StatType > visibleStatTypes;
	std::set< StatBase::StatType > auxStatTypes;
	for( const auto& statType : statTypes )
	{
		visibleStatTypes.insert( statType );
		const std::vector< StatBase::StatType >& inputStats = StatFactory::extractTypeData< InputTypesExtractor >( statType );
		for( const auto& inStat : inputStats )
		{
			auxStatTypes.insert( inStat );
			const std::vector< StatBase::StatType >& inputStats2 = StatFactory::extractTypeData< InputTypesExtractor >( inStat );
			for( const auto& inStat2 : inputStats2 )
				auxStatTypes.insert( inStat2 );
		}
	}

	for( auto is = stats.begin(); is != stats.end(); )
	{
		if( visibleStatTypes.count( (*is)->getType() ) == 0 && auxStatTypes.count( (*is)->getType() ) == 0 )
        	stats.erase( is );
        else
        {
        	if( visibleStatTypes.count( (*is)->getType() ) != 0 )
        	{
        		(*is)->bVisible = true;
        		(*is)->visibleIdx = visibleStats.size();
        		visibleStats.push_back( is->get() );
        	}
            is++;
        }
	}
	linkStats();
}

void StatPack::createFullPack()
{
	stats.clear();
	stats.push_back( std::make_unique< AccumulationStat >() );
	stats.push_back( std::make_unique< SumStat >() );
	stats.push_back( std::make_unique< AverageStat >() );
	stats.push_back( std::make_unique< StddevStat >() );
	stats.push_back( std::make_unique< MinStat >() );
	stats.push_back( std::make_unique< MaxStat >() );
}

void StatPack::linkStats()
{
	for( size_t s = 0; s < stats.size(); s++ )
	{
		stats[s]->inputStats.clear();
		for( const auto& inStatType : stats[s]->inputTypes )
		{
			for( size_t s2 = 0; s2 < s; s2++ )
			{
				if( stats[s2]->type == inStatType )
					stats[s]->inputStats.push_back( stats[s2].get() );
			}
		}
	}
}


//----------------------------------------------run
void StatPack::firstPass( TReal newValue )
{ 
	count++; 
	for( auto& stat : stats ) 
		stat->firstPass( newValue ); 
}