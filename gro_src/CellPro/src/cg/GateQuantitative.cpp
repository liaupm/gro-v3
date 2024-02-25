#include "cg/GateQuantitative.hpp"

using namespace cg;


uint GateQuantitative::calculateRank()
{
    if( ! bRankCalculated )
    {
        if( input->getElemType() == GeneticElementIdx::GATE ) 
            rank = std::max( const_cast<Gate*>( static_cast<const Gate*>( input ) )->calculateRank() + 1, rank );
     
        bRankCalculated = true;
    }
    return rank;
}