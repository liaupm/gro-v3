#include "cg/CellColour.hpp"
#include "cg/Randomness.hpp" //Randomness::getScalerType()

using namespace cg;


ut::NameMap<CellColour::ColourMode> CellColour::colourModeNM ( { 
                                          { "absolute", CellColour::ColourMode::ABSOLUTE }
                                        , { "delta", CellColour::ColourMode::DELTA } } );



CellColour::CellColour( const GeneticCollection* geneticCollection, size_t id, const std::string& name, const GeneticElement* gate, ColourMode mode, const ut::RgbColour& rgbColour
, const GeneticElement* maxValueRandomness, const ut::DistributionScaler::ParamsType& maxValueParams, const GeneticElement* deltaRandomness, const ut::DistributionScaler::ParamsType& deltaParams
, const GeneticElement* target, TReal scale )

: GeneticElement(id, name, GeneticElement::ElemTypeIdx::CELL_COLOUR, false, geneticCollection, ElemCategory::FUNCTIONAL )
, gate( gate ), target( target ), maxValueRandomness( maxValueRandomness ), deltaRandomness( deltaRandomness )
, mode( mode ), rgbColour( rgbColour ), scale( scale )
, maxValueScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( maxValueRandomness ), maxValueParams ) ), maxValueParams( maxValueParams )
, deltaScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( deltaRandomness ), deltaParams ) ), deltaParams( deltaParams ) {;}


void CellColour::setAll( const GeneticElement* xGate, const GeneticElement* xMaxValueRandomness, const GeneticElement* xDeltaRandomness, const GeneticElement* xTarget ) 
{ 
	setGate( xGate ); 
	setMaxValueRandomness( xMaxValueRandomness ); 
	seDeltaRandomness( xDeltaRandomness ); 
	setTarget( xTarget ); 
}

void CellColour::setMaxValueRandomness( const GeneticElement* xMaxValueRandomness ) 
{ 
	maxValueRandomness = xMaxValueRandomness; 
	maxValueScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( maxValueRandomness ), maxValueParams ); 
}

void CellColour::seDeltaRandomness( const GeneticElement* xDeltaRandomness ) 
{ 
	deltaRandomness = xDeltaRandomness; 
	deltaScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( deltaRandomness ), deltaParams );
}


//------------------------------------------ API-precompute
void CellColour::loadDirectElemLink()
{ 
	this->addLink( gate, LinkDirection::BACKWARD ); 
	this->addLink( maxValueRandomness, LinkDirection::BACKWARD ); 
	this->addLink( deltaRandomness, LinkDirection::BACKWARD ); 
	this->addLink( target, LinkDirection::BACKWARD );
}