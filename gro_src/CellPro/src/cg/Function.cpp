#include "cg/Function.hpp"
#include "cg/GeneticCollection.hpp"
#include "cg/Randomness.hpp" //Randomness::getScalerType()

using namespace cg;


ut::NameMap<Function::AutoVolMode> Function::autoVolModeNM( { 
                                          { "none", Function::AutoVolMode::NONE }
                                        , { "division", Function::AutoVolMode::DIVISION }
                                        , { "product", Function::AutoVolMode::PRODUCT } } );


//-----------------------------------------------------------------------------------------------------------------------------------ctor

Function::Function( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const std::vector<const GeneticElement*>& input, const std::vector<TReal>& params, MathFunctionType::Type funType, TReal min, TReal max
, const GeneticElement* randomness, const typename ut::DistributionScaler::ParamsType& rndScalerParams, AutoVolMode autoVolMode, int roundPlaces )

: GeneticElement( id, name, GeneticElement::ElemTypeIdx::FUNCTION, true, geneticCollection, ElemCategory::LOGIC )
, mathFun( MathFunctionType::FactoryType::create( funType, MathFunctionType::InputsType( {} ), params, min, max, roundPlaces ) ), randomness( randomness ), input( input )
, autoVolMode( autoVolMode )
, rndScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( randomness ), rndScalerParams ) ) 
, rank( INI_RANK ), bRankCalculated( false ) {;}



//-----------------------------------------------------------------------------------------------------------------------------------API-precompute

void Function::loadDirectElemLink() 
{ 
    this->addLinks( input, LinkDirection::BACKWARD ); 
    this->addLink( randomness, LinkDirection::BACKWARD ); 

    if( autoVolMode != AutoVolMode::NONE )
        this->addLink( static_cast<const GeneticCollection*>( collection )->getVolMarker(), LinkDirection::BACKWARD ); 
}

void Function::precompute()
{
    std::vector<size_t> inputIdxs;
    for( size_t i = 0; i < input.size(); i++ )
        inputIdxs.push_back( input[i]->getAbsoluteId() );

    mathFun->setInputs( inputIdxs );
}

uint Function::calculateRank()
{
    if( ! bRankCalculated )
    {
        for( auto& inElem : input )
        {
            if( inElem->getElemType() == GeneticElementIdx::FUNCTION ) 
                rank = std::max<uint>( const_cast<Function*>( static_cast<const Function*>(inElem) )->calculateRank() + 1, rank );
        }
        bRankCalculated = true;
    }
    return rank;
}

bool Function::findElemInLogic( const GeneticElement* elem ) const 
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


//-----------------------------------------------------------------------------------------------------------------------------------API-run

TReal Function::evaluate( const ut::AgentState::QType& qState, TReal vol, TReal rawRndVal ) const
{
	if( autoVolMode != AutoVolMode::NONE ) //if the volume is automatically included in the formula
	{
	  //get the volume factor
		TReal volScale;
		if( autoVolMode == AutoVolMode::DIVISION )
			volScale = 1.0 / vol;
		else //product
			volScale = vol;

		return randomness ? mathFun->evaluateScaled( qState, volScale, scaleRnd( rawRndVal ) ) : mathFun->evaluateScaled( qState, volScale );
	}
	else //no volume in the formula
		return randomness ? mathFun->evaluate( qState, scaleRnd( rawRndVal ) ) : mathFun->evaluate( qState );
}