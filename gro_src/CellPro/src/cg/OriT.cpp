#include "cg/OriT.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/Randomness.hpp" //Randomness::getScalerType()
#include "cg/Pilus.hpp" //loadDirectElemLink()

using namespace cg;


OriT::OriT( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const Pilus* pilus, const GeneticElement* donorGate, const GeneticElement* recipientGate, const ParamDataType& rateWData, const Markers& markers
, bool bCopyDependent, bool bEclipse, const std::vector<size_t>& mutationKeys )

: GeneticElement( id, name, GeneticElement::ElementIndex::ORI_T, true, geneticCollection, ElemCategory::DNA_CONTENT )
, pilus( pilus ), donorGate( donorGate ), recipientGate( recipientGate ), rateWFunction( rateWData.fun ), rateWRandomness( rateWData.rnd ), rateWMarker( rateWData.marker ), markers( markers )
, bCopyDependent( bCopyDependent ), bEclipse( bEclipse ), mutationKeys( mutationKeys )
, rateWScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( rateWData.rnd ), rateWData.params ) ), rateWParams( rateWData.params )
, plasmidSelectionDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void OriT::setAll( const GeneticElement* xDonorGate, const GeneticElement* xRecipientGate, const GeneticElement* xRateWFunction, const GeneticElement* xRateWRandomness ) 
{ 
	setDonorGate( xDonorGate ); 
	setRecipientGate( xRecipientGate );
	setRateWFunction( xRateWFunction );
	setRateWRandomness( xRateWRandomness ); 
}

void OriT::setRateWRandomness( const GeneticElement* xRateWRandomness )
{
	rateWRandomness = xRateWRandomness;
	rateWScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( rateWRandomness ), rateWParams );
}


//--------------------------------API-precompute

void OriT::loadDirectElemLink()
{ 
	this->addLink( this, LinkDirection::FORWARD ); 
	this->addLink( pilus, LinkDirection::FORWARD ); 
	this->addLink( donorGate, LinkDirection::BACKWARD ); 
	this->addLink( recipientGate, LinkDirection::BACKWARD ); 
}