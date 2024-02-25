#include "cg/Flux.hpp"
#include "cg/GeneticCollection.hpp"
#include "cg/Randomness.hpp" // Randomness::getScalerType()

using namespace cg;


ut::NameMap<Flux::TimingType> Flux::timingTypeNM ( { 
                                          { "lysis", Flux::TimingType::LYSIS }
                                        , { "continuous", Flux::TimingType::CONTINUOUS } } );

ut::NameMap<Flux::FluxDirection> Flux::directionNM ( { 
                                          { "emission", Flux::FluxDirection::EMISSION }
                                        , { "both", Flux::FluxDirection::BOTH }
                                        , { "absorption", Flux::FluxDirection::ABSORPTION } } );



Flux::Flux( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const Metabolite* metabolite, const GeneticElement* internalGate, const GeneticElement* signalsGate, const std::vector<const Molecule*>& output
, const ParamDataType& amountData, const ParamDataType& thresholdData
, TimingType timingType, TReal activation, FluxDirection direction, bool bRelative, bool bInverted )

: GeneticElement( id, name, GeneticElement::ElementIndex::FLUX, false, geneticCollection, ElemCategory::FUNCTIONAL )
, metabolite( metabolite ), internalGate( internalGate ), signalsGate( signalsGate ), amountFunction( amountData.fun ), output( output ), amountRandomness( amountData.rnd ), thresholdRandomness( thresholdData.rnd ), thresholdMarker( thresholdData.marker )
, timingType( timingType ), activation( activation ), direction( direction ), bRelative( metabolite->getIsBiomass() ? false : bRelative ), bInverted( bInverted ), bThreshold( ut::anyNotNull( thresholdData.params ) )

, amountScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( amountData.rnd ), amountData.params ) ), amountParams( amountData.params )
, thresholdScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( thresholdData.rnd ), thresholdData.params ) ), thresholdParams( thresholdData.params )
, rank( 0 ), bRankCalculated( false ) {;}


void Flux::setAll( const GeneticElement* xInternalGate, const GeneticElement* xSignalsGate, const GeneticElement* xAmountFunction, const GeneticElement* xAmountRandomness, const GeneticElement* xThresholdRandomness )
{ 
	setInternalGate( xInternalGate ); 
	setSignalsGate( xSignalsGate ); 
	setAmountFunction( xAmountFunction ); 
	setAmountRandomness( xAmountRandomness ); 
	setThresholdRandomness( xThresholdRandomness ); 
} 

void Flux::setAmountRandomness( const GeneticElement* xAmountRandomness )
{ 
	amountRandomness = xAmountRandomness; 
	amountScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( amountRandomness ), amountParams );
} 

void Flux::setThresholdRandomness( const GeneticElement* xThresholdRandomness ) 
{ 
	thresholdRandomness = xThresholdRandomness; 
	thresholdScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( thresholdRandomness ), thresholdParams );
} 


//------------------------------------ API-precompute
void Flux::loadDirectElemLink()
{ 
	this->addLink( metabolite, LinkDirection::FORWARD ); 
	this->addLink( internalGate, LinkDirection::BACKWARD ); 
	this->addLink( signalsGate, LinkDirection::BACKWARD ); 
	this->addLink( amountFunction, LinkDirection::BACKWARD ); 
	this->addLink( amountRandomness, LinkDirection::BACKWARD );
	this->addLink( thresholdRandomness, LinkDirection::BACKWARD ); 
	this->addLinks( output, LinkDirection::FORWARD ); 

	if( bThreshold )
		this->addLink( metabolite->getSignalMarker(), LinkDirection::BACKWARD ); 
}

size_t Flux::calculateRank()
{
	/*if( ! bRankCalculated )
    {
    	const std::vector<const GeneticElement*>* input;
    	if( signalsGate->getGateType() == GateBase::GateType::BGATE )
    		input = & static_cast<const GateBoolean*>( signalsGate )->getInput();
    	else
        {
            const GeneticElement* qElem = static_cast<const GateQuantitative*>( signalsGate )->getElem();
            if( qElem->getElemType() == GeneticElement::ElemTypeIdx::FUNCTION )
            {
                input = & static_cast<const Function*>( qElem )->getInput();
                for( auto& inElem : *input )
                {
                    if( inElem->getElemType() == GeneticElementIdx::FLUX ) 
                        rank = std::max( const_cast<Flux*>( static_cast<const Flux*>(inElem) )->calculateRank() + 1, rank );
                }
            }
            else if( qElem->getElemType() == GeneticElementIdx::FLUX )
                rank = std::max( const_cast<Flux*>( static_cast<const Flux*>( qElem ) )->calculateRank() + 1, rank );
        }

        if( amountFunction )
        {
	        for( auto& inElem : amountFunction->getInput() )
	        {
	            if( inElem->getElemType() == GeneticElementIdx::FLUX ) 
	                rank = std::max( const_cast<Flux*>( static_cast<const Flux*>(inElem) )->calculateRank() + 1, rank );
	        }
        }

        bRankCalculated = true;
    }*/
    return rank;
}

