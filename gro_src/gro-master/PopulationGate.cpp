#include "PopulationGate.h"
#include "ut/Bitset.hpp" //gateMask, presence of PopulationGateBoolean
#include "GroCollection.h" //ctor


//PRECOMPILED
//#include <algorithm> //max in calculateRank()


/////////////////////////////////////////////////////////////////  VIRTUAL POPUATION GATE ////////////////////////////////////////////////

PopulationGate::PopulationGate( const GroCollection* groCollection, size_t id, const std::string& name, GateType gateType )
: GroElement( groCollection, id, name, GroElement::ElemTypeIdx::POP_GATE )
, gateType( gateType ), rank( INI_RANK ), bRankCalculated( false ) {;}



///////////////////////////////////////////////////////////////// BOOLEAN POPUATION GATE ////////////////////////////////////////////////

//---------------API precompute
void PopulationGateBoolean::calculateMasks( ut::CollectionManagerBase* groCollection )
{
    ut::Bitset gateMask = ut::Bitset( groCollection->getTotalElementNum() );
    ut::Bitset presence = ut::Bitset( groCollection->getTotalElementNum() );

    for( size_t i = 0; i < input.size(); i++ )
    {
        gateMask.setOn( input[i]->getAbsoluteId() );

        if( inputSigns[i] )
            presence.setOn( input[i]->getAbsoluteId() );
    }
    gate.setMasks( gateMask, presence );
}

uint PopulationGateBoolean::calculateRank()
{
    if( ! bRankCalculated )
    {
        for( auto& inElem : input )
        {
            if( inElem->getElemType() == GroElementIdx::POP_GATE ) 
                rank = std::max( const_cast<PopulationGate*>( static_cast<const PopulationGate*>( inElem ) )->calculateRank() + 1, rank );
        }
        bRankCalculated = true;
    }
    return rank;
}