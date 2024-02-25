#include "CellPlacer.h"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "Timer.h" //loadDirectElemLink()
#include "GroCollection.h" //ctor, for conversion to base, getSeedGenerator()


CellPlacer::CellPlacer( GroCollection* groCollection, size_t id, const std::string& name
, const std::vector< const cg::CellType* > cellTypes, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, bool bMixed
, const std::vector<TReal>& cellProbs, const ut::ContinuousDistData& amountDist )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::CELL_PLACER )
, cellTypes( cellTypes ), timer( timer ), coordsSelector( CoordsSelectorFactory::create( coordsMode, groCollection, coords ) )
, bMixed( bMixed && cellTypes.size() > 1 )
, cellSelectDist( cellProbs, {}, groCollection->getSeedGenerator()->nextSeed() )
, amountDist( amountDist, groCollection->getSeedGenerator()->nextSeed() ) 
, rotationDist( { 0.0, DF_MAX_ROTATION }, groCollection->getSeedGenerator()->nextSeed() ){;}


void CellPlacer::loadDirectElemLink() { this->addLink( timer, LinkDirection::BACKWARD ); }