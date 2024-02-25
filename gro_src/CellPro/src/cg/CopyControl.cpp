#include "cg/CopyControl.hpp"
#include "cg/Randomness.hpp" //::getScalerType()

using namespace cg;


CopyControl::CopyControl( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const ParamDataType& wData, const QMarker* activeMarker )

: GeneticElement( id, name, GeneticElement::ElementIndex::COPY_CONTROL, true, geneticCollection, ElemCategory::DNA_CONTENT ) 
, gate(gate), activeMarker( activeMarker ), wRandomness( wData.rnd ), wMarker( wData.marker )
, wScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( wData.rnd ), wData.params ) ), wParams( wData.params ) {;}


void CopyControl::setWRandomness( const GeneticElement* xWRandomness )
{
	wRandomness = xWRandomness;
	wScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( wRandomness ), wParams );
}


//--------------------------------------API-precompute

void CopyControl::loadDirectElemLink()
{ 
	this->addLink( this, LinkDirection::FORWARD ); 
	this->addLink( gate, LinkDirection::BACKWARD ); 
	this->addLink( wRandomness, LinkDirection::BACKWARD ); 
}


//------------------------------------API-run

TReal CopyControl::calculate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const 
{ 
	if( bState.checkBit( gate->getAbsoluteId() ) && qState[ getAbsoluteId() ] ) //active and present 
		return sampleW( wRandomness ? qState[ wRandomness->getAbsoluteId() ] : 0.0 ) * qState[ getAbsoluteId() ]; //weight multiplied by copy number
	return 0.0; //inactive or not present
}