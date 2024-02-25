#include "cg/Molecule.hpp"
#include "cg/Randomness.hpp" //Randomness::getScalerType()

using namespace cg;

///////////////////////////////////////////// NESTED CLASSES (TIME DISTRIBUTION ) /////////////////////////////////////////////////////////////

Molecule::TimeDistribution::TimeDistribution( const GeneticElement* fun, const GeneticElement* rnd, const ut::ContinuousDistData& distData, ExpressionDirection direction ) 
: fun( fun ), scaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( rnd ), distData.params ) )
, direction( direction ) {;} 




////////////////////////////////////////////////////////////////// MOLECULE /////////////////////////////////////////////////////////////

Molecule::Molecule( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* randomness, const DistVectorDS& timeDistributions, const GeneticElement* customRndSymmetry, bool bSymmetricRnd )
: GeneticElement( id, name, GeneticElement::ElementIndex::MOLECULE, false, geneticCollection, ElemCategory::FREE_ELEM )
, randomness( randomness ), customRndSymmetry( customRndSymmetry )
, timeDistributions( timeDistributions ), bSymmetricRndParam( bSymmetricRnd ) { calculateBSymmetricRnd(); calculateBCustomFunctions(); }


void Molecule::setAll( const GeneticElement* xRandomness, const GeneticElement* xCustomRndSymmetry, const DistVectorDS& xTimeDistributions ) 
{ 
	setRandomness( xRandomness ); 
	setCustomRndSymmetry( xCustomRndSymmetry ); 
	setTimeDistributions( xTimeDistributions );
}

void Molecule::calculateBCustomFunctions()
{
	bCustomFunctions = false;
	for( size_t d = 0; d < timeDistributions.size(); d++ )
	{
		if( timeDistributions.retrieveAtIndexEdit( d ).fun )
		{
			bCustomFunctions = true;
			break;
		}
	}
}


//------------------API-precompute

void Molecule::loadDirectElemLink() 
{ 
	this->addLink( this, LinkDirection::FORWARD ); 
	this->addLink( randomness, LinkDirection::BACKWARD ); 

	for( size_t d = 0; d < timeDistributions.size(); d++ )
		this->addLink( timeDistributions.retrieveAtIndex( d ).fun, LinkDirection::BACKWARD ); 
}


//------------------API-run

size_t Molecule::inputIndex( ExpressionDirection& direction, TReal activation ) const
{
    size_t idx = timeDistributions.input2index( activation );
    direction = timeDistributions.retrieveAtIndex( idx ).direction;
    return idx;
}


TReal Molecule::sampleExpressionTimeAtIndex( size_t idx, TReal funVal, TReal rawRndVal, TReal symmetricValue ) const
{
    const TimeDistribution& dist = timeDistributions.retrieveAtIndex( idx );
  //custom function
    if( dist.fun )
    	return ut::Time::applyEpsilon( ut::fitL( funVal ) );
    
  //built-in calculation
    //make symmetric of rawRndVal if DIR_OFF and required
    TReal rndVal = rawRndVal;
    if( dist.direction == ExpressionDirection::DIR_OFF )
    {
    	if( bSymmetricRnd ) //cannonical
    		rndVal = static_cast<const Randomness*>( randomness )->getDistribution().calculateSymmetric( rawRndVal );
    	else if( customRndSymmetry ) //custom
    		rndVal = symmetricValue;
    }
    return ut::Time::applyEpsilon( dist.scaler->scale( rndVal ) ); //scale and apply minimum
}