#include "cg/OriV.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/CopyControl.hpp" //cast in addLink()

using namespace cg;


OriV::OriV( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const std::vector<const CopyControl*>& copyControls, const GeneticElement* customFunction
, const ParamDataType& vmaxData, const ParamDataType& wData, const QMarker* eclipseMarker, const QMarker* repliEventMarker
, bool bEclipse, const std::vector<size_t>& mutationKeys )

: GeneticElement( id, name, GeneticElement::ElementIndex::ORI_V, true, geneticCollection, ElemCategory::DNA_CONTENT ) 
, gate( gate ), copyControls( copyControls ), customFunction( customFunction )
, eclipseMarker( eclipseMarker ), repliEventMarker( repliEventMarker ), vmaxRandomness( vmaxData.rnd ), wRandomness( wData.rnd ), vmaxMarker( vmaxData.marker ), wMarker( wData.marker )
, bEclipse( bEclipse ), mutationKeys( mutationKeys )

, vmaxScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( vmaxData.rnd ), vmaxData.params ) ), vmaxParams( vmaxData.params )
, wScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( wData.rnd ), wData.params ) ), wParams( wData.params )
, plasmidSelectDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void OriV::setAll( const GeneticElement* xGate, const GeneticElement* xCustomFunction, const GeneticElement* xVmaxRandomness, const GeneticElement* xWRandomness ) 
{ 
	setGate( xGate ); 
	setCustomFunction( xCustomFunction ); 
	setVmaxRandomness( xVmaxRandomness ); 
	setWRandomness( xWRandomness ); 
}

void OriV::setVmaxRandomness( const GeneticElement* xVmaxRandomness )
{
	vmaxRandomness = xVmaxRandomness;
	vmaxScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( vmaxRandomness ), vmaxParams );
}

void OriV::setWRandomness( const GeneticElement* xWRandomness )
{
	wRandomness = xWRandomness;
	wScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( wRandomness ), wParams );
}


//---------------------------------------- API-preprocess

void OriV::loadDirectElemLink()
{ 
	this->addLink( this, LinkDirection::FORWARD );
	this->addLink( gate, LinkDirection::BACKWARD ); 
	this->addLink( customFunction, LinkDirection::BACKWARD );
	this->addLinks( copyControls, LinkDirection::BACKWARD ); 
	this->addLink( vmaxRandomness, LinkDirection::BACKWARD ); 
	this->addLink( wRandomness, LinkDirection::BACKWARD ); 

	if( ! customFunction )
        this->addLink( static_cast<const GeneticCollection*>( collection )->getVolMarker(), LinkDirection::BACKWARD ); 
}


//---------------------------------------- API-run

TReal OriV::calculateRate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState, TReal vol ) const
{ 
	if( bState.checkBit( gate->getAbsoluteId() ) && qState[ getAbsoluteId() ] ) //active and present
	{
	  //custom function way
		if( customFunction )
			return qState[ customFunction->getAbsoluteId() ];
	  //canonic simplified way
		TReal vmax = sampleVmax( vmaxRandomness ? qState[ vmaxRandomness->getAbsoluteId() ] : 0.0 );
		TReal w = sampleW( wRandomness ? qState[ wRandomness->getAbsoluteId() ] : 0.0 );
		return vmax * ( 1.0 - std::exp( -w * qState[ getAbsoluteId() ] / vol ) ) * std::exp( - calculateCopyControlRaw( bState, qState ) / vol );
	}
	return 0.0; //inactive or not present
}


TReal OriV::calculateCopyControlRaw( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const
{
	TReal copyControlVal = 0.0;
	for( const CopyControl* cc : copyControls )
		copyControlVal += cc->calculate( bState, qState );
	return copyControlVal;
}