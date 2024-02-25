#include "cg/Regulation.hpp"
#include "cg/Randomness.hpp" //in NoisyRegulation, Randomness::getScalerType()

using namespace cg;


/////////////////////////////////////////////////////////////////// *REGULATION BASE* ////////////////////////////////////////////////////////////////////////////////

Regulation::Regulation( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const GeneticElement* randomness, const ut::DistributionScaler::ParamsType& onParams, const ut::DistributionScaler::ParamsType& offParams
, const GeneticElement* dosageFun, const GeneticElement* onFunction, const GeneticElement* offFunction, const QMarker* actiMarker, bool bAutoDivision ) 

: GeneticElement( id, name, GeneticElement::ElemTypeIdx::REGULATION, true, geneticCollection, ElemCategory::DNA_CONTENT )
, gate( gate ), dosageFun( dosageFun ), onFunction( onFunction ), offFunction( offFunction ), actiMarker( actiMarker ), randomness( randomness ), bAutoDivision( bAutoDivision )
, onScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( randomness ), onParams ) ), onParams( onParams )
, offScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( randomness ), offParams ) ), offParams( offParams ) {;}


void Regulation::setAll( const GeneticElement* xGate, const GeneticElement* xDosageFun, const GeneticElement* xOnFunction, const GeneticElement* xOffFunction, const GeneticElement* xRandomness )
{ 
	setGate( xGate ); 
	setDosageFun( xDosageFun ); 
	setOnFunction( xOnFunction ); 
	setOffFunction( xOffFunction ); 
	setRandomness( xRandomness ); 
} 

void Regulation::setRandomness( const GeneticElement* xRandomness ) 
{ 
	randomness = xRandomness; 
	onScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( randomness ), onParams );
	offScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( randomness ), offParams );
} 

//----------------------------API-precompute

void Regulation::loadDirectElemLink()
{ 
	this->addLink( this, LinkDirection::FORWARD ); 
	this->addLink( gate, LinkDirection::BACKWARD ); 
	this->addLink( dosageFun, LinkDirection::BACKWARD );
	this->addLink( onFunction, LinkDirection::BACKWARD );
	this->addLink( offFunction, LinkDirection::BACKWARD );
	this->addLink( randomness, LinkDirection::BACKWARD );  
}


//---------------------API-run

TReal Regulation::sampleActivation( TReal copyNumber, TReal onFunVal, TReal offFunVal, TReal rawRndVal, bool gateState, TReal dosageFunValue ) const
{
    TReal output;
    if( gateState )
    {
    	if( onFunction )
    		output = onFunVal;
    	else
        	output = onScaler->scale( rawRndVal );
    }
    else
    {
    	if( offFunction )
    		output = offFunVal;
    	else
        	output = offScaler->scale( rawRndVal );
    }
    return applyDosageEffect( output, copyNumber, dosageFunValue  );
}

TReal Regulation::applyDosageEffect( TReal input, TReal copyNumber, TReal dosageFunValue ) const
{ 
    TReal raw = dosageFun ? input * dosageFunValue : input; 
    return bAutoDivision ? raw / copyNumber : raw;
}