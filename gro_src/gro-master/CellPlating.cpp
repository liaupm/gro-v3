#include "CellPlating.h"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "Timer.h" //loadDirectElemLink()
#include "GroCollection.h" ////ctor, for conversion to base


CellPlating::CellPlating( GroCollection* groCollection, size_t id, const std::string& name
, const cg::GeneticElement* gate, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, const CoordsCircular& spatialFilter
, const ut::ContinuousDistData& takeFractionDist, const ut::ContinuousDistData& putFractionDist )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::CELL_PLATING )
, gate( gate ), timer( timer ), spatialFilter( std::make_shared<CoordsCircular>( spatialFilter ) ), coordsSelector( CoordsSelectorFactory::create( coordsMode, groCollection, coords ) )
, bPut( ut::anyNotNull( putFractionDist.params ) )
, takeDist( takeFractionDist, groCollection->getSeedGenerator()->nextSeed() )
, putDist( putFractionDist, groCollection->getSeedGenerator()->nextSeed() )
, rotationDist( { 0.0, DF_MAX_ROTATION }, groCollection->getSeedGenerator()->nextSeed() )
, shuffleRndEngine( groCollection->getSeedGenerator()->nextSeed() ) {;}


void CellPlating::loadDirectElemLink() { this->addLink( timer, LinkDirection::BACKWARD ); }