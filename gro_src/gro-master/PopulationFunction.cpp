#include "PopulationFunction.h"
#include "PopulationStat.h" //cast, setBUsedForTimers() and setBUsedForOutput() in passUsedForTimers() and passUsedForOutput()
#include "GroCollection.h" //ctor

//PRECOMPILED
//#include <algorithm> //max in calculateRank()


PopulationFunction::PopulationFunction( const GroCollection* groCollection, size_t id, const std::string& name
, const std::vector<const GroElement*>& input, const std::vector<TReal>& params, MathFunctionType::Type funType, TReal min, TReal max, int roundPlaces )
: GroElement( groCollection, id, name, GroElement::ElemTypeIdx::POP_FUNCTION )
, mathFun( MathFunctionType::FactoryType::create( funType, MathFunctionType::InputsType( {} ), params, min, max, roundPlaces ) )
, input( input )
, rank( INI_RANK ), bRankCalculated( false ) {;}


uint PopulationFunction::calculateRank()
{
    if( ! bRankCalculated )
    {
        for( auto& inElem : input )
        {
            if( inElem->getElemType() == GroElementIdx::POP_FUNCTION ) 
                rank = std::max<uint>( const_cast<PopulationFunction*>( static_cast<const PopulationFunction*>( inElem ) )->calculateRank() + 1, rank );
        }
        bRankCalculated = true;
    }
    return rank;
}

//----private

void PopulationFunction::createMathFunction()
{
 //translate inputs into their absolute ids
    std::vector<size_t> inputIdxs;
    for( size_t i = 0; i < input.size(); i++ )
        inputIdxs.push_back( input[i]->getAbsoluteId() );

    mathFun->setInputs( inputIdxs );
}