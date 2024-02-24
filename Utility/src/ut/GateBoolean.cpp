#include "ut/GateBoolean.hpp"

using namespace ut;


//---static
ut::NameMap<GateBoolean::GateFunction> GateBoolean::functionNM ( { 
                                          { "TRUE", GateBoolean::GateFunction::OPEN }
                                        , { "OPEN", GateBoolean::GateFunction::OPEN }
                                        , { "FALSE", GateBoolean::GateFunction::CLOSED }
                                        , { "CLOSED", GateBoolean::GateFunction::CLOSED }
                                        , { "YES", GateBoolean::GateFunction::AND }
                                        , { "NOT", GateBoolean::GateFunction::NAND }
                                        , { "AND", GateBoolean::GateFunction::AND }
                                        , { "OR", GateBoolean::GateFunction::OR }
                                        , { "XOR", GateBoolean::GateFunction::XOR }
                                        , { "NAND", GateBoolean::GateFunction::NAND }
                                        , { "NOR", GateBoolean::GateFunction::NOR }
                                        , { "XNOR", GateBoolean::GateFunction::XNOR }
                                        } );


bool GateBoolean::check( const ut::AgentState::BTypeSimple& rawInput ) const
{
    if( gateFunction == GateFunction::OPEN )
        return true;
    if( gateFunction == GateFunction::CLOSED )
        return false;

    ut::Bitset gateValue = gateMask & ( ~rawInput ^ presence );

    switch( gateFunction )
    {
        case GateFunction::AND:
            return gateValue == gateMask;
        case GateFunction::NAND:
            return gateValue != gateMask;
        case GateFunction::OR:
            return gateValue.any();
        case GateFunction::NOR:
            return gateValue.none();
        case GateFunction::XOR:
            return gateValue.any() && gateValue != gateMask;
        case GateFunction::XNOR:
            return ! ( gateValue.any() && gateValue != gateMask );
        default:
            return false;
    }
}