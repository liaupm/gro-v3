#include "cg/Randomness.hpp"
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "ut/SeedGenerator.hpp" //nextSeed()

using namespace cg;

//================================================ STATIC ==========================================================

ut::DistributionScaler::Type Randomness::getScalerType( const GeneticElement* rnd )
/* returns the appropriate DistScaler type for the given Randomness element */
{
	if( rnd )
	{
		if( rnd->getElemType() == ElemTypeIdx::RANDOMNESS )
			return static_cast< const Randomness* >( rnd )->getScalerType(); //the type that matches its probability distribution
		else
			return ut::DistributionScaler::Type::NONE; //if custom function instead of Randomness element, no scaling, keep the original raw value
	}
	else
		return ut::DistributionScaler::Type::UNIT; //if null (deterministic), return a fixed deterministic value
}


//================================================ RANDOMNESS ==========================================================

Randomness::Randomness( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, DistributionType distributionType, const ut::ContinuousDist::ParamsType& params, TReal inheritance, TReal inertia
, TReal samplingPeriod, bool bRndSamplingTime, bool bCorrection )

: GeneticElement( id, name, GeneticElement::ElemTypeIdx::RANDOMNESS, false, geneticCollection, ElemCategory::LOGIC )
, inheritance( inheritance ), inertia( inertia )
, samplingPeriod( samplingPeriod ), bResample( inertia < 1.0 && samplingPeriod > 0.0 ), bRndSamplingTime( bRndSamplingTime )
, bCorrection( bCorrection && distributionType == DistributionType::NORMAL ) 
, inheritanceCorrection( distributionType == DistributionType::NORMAL ? calculateCorrection( inheritance ) : 1.0 )
, inertiaCorrection( distributionType == DistributionType::NORMAL ? calculateCorrection( inertia ) : 1.0 )
, distribution( distributionType, std::vector<TReal>( params ), geneticCollection->getSeedGenerator()->nextSeed() )
, samplingTimeDistribution( { 1.0 / samplingPeriod }, geneticCollection->getSeedGenerator()->nextSeed() ) {;}