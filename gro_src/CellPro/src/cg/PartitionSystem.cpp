#include "cg/PartitionSystem.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/DivisionSplit.hpp" //base function for sample() methods

using namespace cg;


PartitionSystem::PartitionSystem( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const QMarker* eclipseMarker, const ParamDataType& fractionData, const ParamDataType& pData
, bool bVolumeScaling )

: GeneticElement(id, name, GeneticElement::ElemTypeIdx::PARTITION_SYSTEM, false, geneticCollection, ElemCategory::DNA_CONTENT )
, gate( gate ), eclipseMarker( eclipseMarker ), fractionFunction( fractionData.fun ), pFunction( pData.fun ), fractionRandomness( fractionData.rnd ), pRandomness( pData.rnd ), fractionMarker( fractionData.marker ), pMarker( pData.marker )
, bVolumeScaling( bVolumeScaling )

, fractionScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( fractionData.rnd ), fractionData.params ) ), fractionParams( fractionData.params )
, pScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( pData.rnd ), pData.params ) ), pParams( pData.params )
, randomDist( { pData.params.size() > 0 ? pData.params[0] : 0.5 }, {}, geneticCollection->getSeedGenerator()->nextSeed() ) 
, daughterDist( { 0, 1 }, geneticCollection->getSeedGenerator()->nextSeed() )
, shuffleRnEngine( geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void PartitionSystem::setAll( const GeneticElement* xGate, const GeneticElement* xFractionFunction, const GeneticElement* xPFunction, const GeneticElement* xFractionRandomness, const GeneticElement* xPRandomness )
{ 
	setGate( xGate ); 
	setFractionFunction( xFractionFunction );
	setPFunction( xPFunction );
	setFractionRandomness( xFractionRandomness ); 
	setPRandomness( xPRandomness ); 
}

void PartitionSystem::setFractionRandomness( const GeneticElement* xFractionRandomness ) 
{ 
	fractionRandomness = xFractionRandomness; 
	fractionScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( fractionRandomness ), fractionParams );
}

void PartitionSystem::setPRandomness( const GeneticElement* xPRandomness ) 
{ 
	pRandomness = xPRandomness; 
	pScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( pRandomness ), pParams );
}



//-------------------------------------- API-run

uint PartitionSystem::sampleExact( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const
{ 
	TReal exactFraction;
	if( fractionFunction )
		exactFraction = ut::fit( funVal );
	else
		exactFraction = sampleExactFraction( rawRndVal );  //potentially stochastic exact fraction (noisy)

	bool daughter = sampleDaughter();

  //extreme cases
	int result = DivisionSplit::splitExtremeCases( exactFraction, copyNum, daughter ); //when exactFraction is 0 or 1
	if( result >= 0 ) //negative return value if not a extreme case
		return result;

  //volume independent
	if( ! bVolumeScaling || volFraction == 0.5 )
	{
		uint raw = ut::round<uint>( copyNum * exactFraction ); //new copy number of one of the cells
		return daughter ? copyNum - raw : raw; //select daughter 
	}
  //affected by volume
	TReal ratio = DivisionSplit::scaleSplitFraction( exactFraction, volFraction, daughter ); //combine exactFraction and volFraction
	return ut::round<uint>( copyNum * ratio ); 
}


uint PartitionSystem::sampleRandom( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const
{ 
	TReal p;
	if( pFunction )
		p = ut::fit( funVal );
	else
		p = sampleP( rawRndVal );  //potentially stochastic p param

	uint daughter = sampleDaughter();

  //extreme cases
	int result = DivisionSplit::splitExtremeCases( p, copyNum, daughter ); //when exactFraction is 0 or 1
	if( result >= 0 ) //negative return value if not a extreme case
		return result; 

  //volume independent
	if( ! bVolumeScaling )
	{
		uint raw = randomDist.sample( p, ut::round<uint>( copyNum ) ); //binomial sample
		return daughter ? copyNum - raw : raw; //random assigment to one daughter cell
	}

  //affected by volume
	TReal ratio = DivisionSplit::scaleSplitFraction( p, volFraction, daughter ); //combine p and volFraction
	return ut::fitL( randomDist.sample( ratio, ut::round<uint>( copyNum ) ) ); //binomial sample
}