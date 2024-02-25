#include "cg/GateBoolean.hpp"

/*PRECOMPILED
#include <algorithm> //max in calculateRank() */

using namespace cg;


//-----------------------------------------------------------------------------------------------------------------------------------API-precompute

void GateBoolean::calculateMasks( ut::CollectionManagerBase* geneticCollection )
{
    ut::Bitset gateMask = ut::Bitset( geneticCollection->getTotalElementNum() ); //relevant elements
    ut::Bitset presence = ut::Bitset( geneticCollection->getTotalElementNum() ); //present of absent

    for( size_t i = 0; i < input.size(); i++ )
    {
        gateMask.setOn( input[i]->getAbsoluteId() );

        if( inputSigns[i] )
            presence.setOn( input[i]->getAbsoluteId() );
    }
    gate.setMasks( gateMask, presence );
}

uint GateBoolean::calculateRank()
{
    if( ! bRankCalculated )
    {
        for( auto& inElem : input )
        {
            if( inElem->getElemType() == GeneticElementIdx::GATE ) 
                rank = std::max( const_cast<Gate*>( static_cast<const Gate*>(inElem) )->calculateRank() + 1, rank );
        }
        bRankCalculated = true;
    }
    return rank;
}

bool GateBoolean::findElemInLogic( const GeneticElement* elem ) const 
{
	if( this == elem )
		return true;

	for( const GeneticElement* in : input )
	{
		if( in->findElemInLogic( elem ) )
			return true;
	}
	return false;
}