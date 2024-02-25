#include "cg/Strain.hpp"
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/Randomness.hpp" //getScalerType()


using namespace cg;


Strain::Strain( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* divisionGate, const GeneticElement* deathGate, const GeneticElement* grCustomFun, const GeneticElement* baseGrRandomness
, const ut::DistributionScaler::ParamsType& baseGrParams, const DistributionsDataDS& distributionsData, TReal deathThreshold, bool bNegativeGrowthAllowed, bool bGrowthCorrection )

: GeneticElement::GeneticElement( id, name, GeneticElement::ElemTypeIdx::STRAIN, false, geneticCollection, ElemCategory::FREE_ELEM )
, divisionGate( divisionGate ), deathGate( deathGate ), grCustomFun( grCustomFun ), baseGrRandomness(baseGrRandomness)
, deathThreshold( deathThreshold ), bNegativeGrowthAllowed( bNegativeGrowthAllowed ), bGrowthCorrection( bGrowthCorrection )

, baseGrScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( baseGrRandomness ), baseGrParams ) ), baseGrParams( baseGrParams )
, daughterDist( { 0, 1 }, geneticCollection->getSeedGenerator()->nextSeed() )
{
	makeDistributions( geneticCollection->getSeedGenerator(), distributionsData );
}

void Strain::setBaseGrRandomness( const GeneticElement* xBaseGrRandomness ) 
{ 
	baseGrRandomness = xBaseGrRandomness; 
	baseGrScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( baseGrRandomness ), baseGrParams );
} 

void Strain::makeDistributions( ut::SeedGenerator* seedGen, const DistributionsDataDS& distributionsData )
{
	distributions[INI_VOL] = std::make_shared<ut::DistributionCombi>( distributionsData[INI_VOL], seedGen->nextSeed() );
	distributions[DIVISION_VOL] = std::make_shared<ut::DistributionCombi>( distributionsData[DIVISION_VOL], seedGen->nextSeed() );
	distributions[DIVISION_FRACTION] = std::make_shared<ut::DistributionCombi>( distributionsData[DIVISION_FRACTION], seedGen->nextSeed() );
	distributions[DIVISION_ROTATION] = std::make_shared<ut::DistributionCombi>( distributionsData[DIVISION_ROTATION], seedGen->nextSeed() );
}


//----------------------------------------API-precompute

void Strain::loadDirectElemLink()
{ 
	this->addLink( baseGrRandomness, LinkDirection::BACKWARD );
	this->addLink( deathGate, LinkDirection::BACKWARD );
	if( grCustomFun )
		this->addLink( grCustomFun, LinkDirection::BACKWARD );
	else
		this->addLink( static_cast<const GeneticCollection*>( collection )->getBiomass(), LinkDirection::BACKWARD );
}