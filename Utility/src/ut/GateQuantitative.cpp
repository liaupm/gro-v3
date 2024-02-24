#include "ut/GateQuantitative.hpp"

using namespace ut;


//---static
ut::NameMap< GateQuantitative::CompOperator > GateQuantitative::compOperatorNM ( { 
                                          { "<", GateQuantitative::CompOperator::LESS }
                                        , { "<=", GateQuantitative::CompOperator::LESS_EQUAL }
                                        , { ">", GateQuantitative::CompOperator::GREATER }
                                        , { ">=", GateQuantitative::CompOperator::GREATER_EQUAL }
                                        , { "==", GateQuantitative::CompOperator::EQUAL }
                                        , { "!=", GateQuantitative::CompOperator::NOT_EQUAL }
                                        } );


//---------------API
bool GateQuantitative::check( const ut::AgentState::QType& qState ) const
{
    TReal leftVal = qState[ elemIdx ];

    switch( compOperator )
    {
        case CompOperator::LESS:
            return leftVal < value;
        case CompOperator::LESS_EQUAL:
            return leftVal <= value;
        case CompOperator::GREATER:
            return leftVal > value;
        case CompOperator::GREATER_EQUAL:
            return leftVal >= value;
        case CompOperator::EQUAL:
            return areSame( leftVal, value );
            //return leftVal == value;
        case CompOperator::NOT_EQUAL:
            return ! areSame( leftVal, value );
            //return leftVal != value;
        default:
            return false;
    }
}