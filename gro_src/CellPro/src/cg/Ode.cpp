#include "cg/Ode.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/DivisionSplit.hpp" //common split functions
#include "cg/Randomness.hpp"

using namespace cg;


///////////////////////////////////////////////////////////// NESTED CLASSES (PARTITION) ////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------PartitionBase

Ode::PartitionBase::PartitionBase( const GeneticCollection* geneticCollection, const ParamDataType& paramData, bool bVolumeScaling ) 
: bVolumeScaling( bVolumeScaling ) 
, paramFunction( paramData.fun ), paramRandomness( paramData.rnd )
, paramScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( paramData.rnd ), paramData.params ) ), paramParams( paramData.params )
, daughterDist( { 0, 1 }, geneticCollection->getSeedGenerator()->nextSeed() ) {;}

void Ode::PartitionBase::setParamRandomness( const GeneticElement* xParamRandomness ) 
{ 
	paramRandomness = xParamRandomness; 
	paramScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( paramRandomness ), paramParams );
}


//--------------------------------------------------------------------------------PartitionContinuous

Ode::PartitionContinuous::PartitionContinuous( const GeneticCollection* geneticCollection, const ParamDataType& paramData, bool bVolumeScaling )
: PartitionBase( geneticCollection, paramData, bVolumeScaling ) {;}


uint Ode::PartitionContinuous::sample( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const
{ 
	TReal exactFraction;
	if( paramFunction )
		exactFraction = ut::fit( funVal );
	else
		exactFraction = sampleFraction( rawRndVal );  //potentially stochastic exact fraction (noisy)

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


//--------------------------------------------------------------------------------PartitionBinomial

Ode::PartitionBinomial::PartitionBinomial( const GeneticCollection* geneticCollection, const ParamDataType& paramData, bool bVolumeScaling )
: PartitionBase( geneticCollection, paramData, bVolumeScaling )
, binomialDist( { paramData.params.size() > 0 ? paramData.params[0] : DF_P }, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}

uint Ode::PartitionBinomial::sample( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const
{ 
	TReal p;
	if( paramFunction )
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
		uint raw = binomialDist.sample( p, ut::round<uint>( copyNum ) ); //binomial sample
		return daughter ? copyNum - raw : raw; //random assigment to one daughter cell
	}

  //affected by volume
	TReal ratio = DivisionSplit::scaleSplitFraction( p, volFraction, daughter ); //combine p and volFraction
	return ut::fitL( binomialDist.sample( ratio, ut::round<uint>( copyNum ) ) ); //binomial sample
}



///////////////////////////////////////////////////////////// ODE ////////////////////////////////////////////////////////////////////////

Ode::Ode( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const GeneticElement* deltaElem, const GeneticElement* iniValueFunction, const ut::ContinuousDistData& iniValueDist, TReal scale
, const ParamDataType& partitionParamData, PartitionType partitionType, TReal partitionScale, bool bVolumeScalingPartition )
: GeneticElement( id, name, GeneticElement::ElemTypeIdx::ODE, true, geneticCollection, ElemCategory::LOGIC )
, gate( gate ), deltaElem( deltaElem ), iniValueFunction( iniValueFunction )
, partition( nullptr ), splitMarker( partitionParamData.marker )
, scale( scale ), partitionScale( partitionScale )
, iniValueDist( iniValueDist, geneticCollection->getSeedGenerator()->nextSeed() ) 
{
	if( partitionType == PartitionType::CONTINUOUS )
		partition = std::make_shared<PartitionContinuous>( geneticCollection, partitionParamData, bVolumeScalingPartition );
	else if( partitionType == PartitionType::BINOMIAL )
		partition = std::make_shared<PartitionBinomial>( geneticCollection, partitionParamData, bVolumeScalingPartition );
}

void Ode::setAll( const GeneticElement* xGate, const GeneticElement* xDeltaElem, const GeneticElement* xIniValueFunction, const GeneticElement* xPartitionFunction, const GeneticElement* xPartitionRandomness ) 
{ 
	setGate( xGate ); 
	setDeltaElem( xDeltaElem );
	setIniValueFunction( xIniValueFunction ); 
	setPartitionFunction( xPartitionFunction );
	setPartitionRandomness( xPartitionRandomness );
}