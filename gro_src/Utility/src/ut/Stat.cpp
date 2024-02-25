#include "ut/Stat.hpp"

using namespace ut;


ut::NameMap< StatBase::StatType > StatBase::statTypeNM ( { 
										{ "acc", StatBase::StatType::ACCUMULATION }
                                        , { "sum", StatBase::StatType::SUM }
                                        , { "avg", StatBase::StatType::AVG }
                                        , { "stddev", StatBase::StatType::STDDEV }
                                        , { "min", StatBase::StatType::MIN }
                                        , { "max", StatBase::StatType::MAX }
                                        } );


StatBase::StatBase( StatType type ) 
: type( type ), value( INI_VALUE ), bFirstVal( true ), bVisible( DF_B_VISIBLE ), visibleIdx( DF_VISIBLE_IDX )
{
	inputTypes = StatFactory::extractTypeData< InputTypesExtractor >( type );
}