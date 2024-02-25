#include "CoordsSelector.h"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "GroCollection.h" //getSeedGenerator()

//PRECOMPILED
//#include <cmath> //sqrt, cos, sin


/////////////////////////////////////////////////////// RECTANGULAR ////////////////////////////////////////////////////////////

CoordsSelectorRectangular::CoordsSelectorRectangular( GroCollection* groCollection, const ut::Rectangle& coords ) 
: CoordsSelectorBase::CoordsSelectorBase( CoordsMode::RECTANGULAR ) 
, coords( coords )
, xDist( { coords.x1, coords.x2 }, groCollection->getSeedGenerator()->nextSeed() )
, yDist( { coords.y1, coords.y2 }, groCollection->getSeedGenerator()->nextSeed() ) {;}


/////////////////////////////////////////////////////// CARTESIAN ////////////////////////////////////////////////////////////

CoordsSelectorCartesian::CoordsSelectorCartesian( GroCollection* groCollection, const ut::Circle& coords ) 
: CoordsSelectorBase::CoordsSelectorBase( CoordsMode::CARTESIAN ) 
, coords( coords )
, radiusDist( { 0.0, 1.0 }, groCollection->getSeedGenerator()->nextSeed() )
, angleDist( { 0.0, 360.0 }, groCollection->getSeedGenerator()->nextSeed() ) {;}


void CoordsSelectorCartesian::sampleCoords( TReal& x, TReal& y ) const
{
    TReal rndRadius = coords.r * std::sqrt( radiusDist.sample() ); //r * U(0,1), this transformation makes the samples uniformly distributed
    TReal rndAngle = angleDist.sample();

    x = coords.xCenter + rndRadius * std::cos( rndAngle );
    y = coords.yCenter + rndRadius * std::sin( rndAngle );
}


/////////////////////////////////////////////////////// POLAR ////////////////////////////////////////////////////////////

CoordsSelectorPolar::CoordsSelectorPolar( GroCollection* groCollection, const ut::Circle& coords )
: CoordsSelectorBase::CoordsSelectorBase( CoordsMode::POLAR )
, coords( coords )
, radiusDist( { 0.0, coords.r }, groCollection->getSeedGenerator()->nextSeed() )
, angleDist( { 0.0, 360.0 }, groCollection->getSeedGenerator()->nextSeed() ) {;}


void CoordsSelectorPolar::sampleCoords( TReal& x, TReal& y ) const
{
    TReal rndRadius = radiusDist.sample();
    TReal rndAngle = angleDist.sample();

    x = coords.xCenter + rndRadius * std::cos( rndAngle );
    y = coords.yCenter + rndRadius * std::sin( rndAngle );
}