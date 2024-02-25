#include "SignalPlacer.h"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "Timer.h" //loadDirectElemLink()
#include "GroCollection.h" //ctor, for conversion to base, getSeedGenerator()


SignalPlacer::SignalPlacer( GroCollection* groCollection, size_t id, const std::string& name
, const std::vector<const mg::Signal*>& msignals, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, Mode mode, bool bMixed, const std::vector<TReal>& signalProbs
, const ut::ContinuousDistData& amountDist, const ut::ContinuousDistData& sourceNumDist, const ut::ContinuousDistData& sizeDist, const ut::Rectangle& constArea )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::SIGNAL_PLACER )
, msignals( msignals ), timer( timer ), coordsSelector( CoordsSelectorFactory::create( coordsMode, groCollection, coords ) )
, mode( mode ), bMixed( bMixed && msignals.size() > 1 )
, signalSelectDist( signalProbs, {}, groCollection->getSeedGenerator()->nextSeed() )
, sourceNumDist( sourceNumDist, groCollection->getSeedGenerator()->nextSeed() )
, amountDist( amountDist, groCollection->getSeedGenerator()->nextSeed() ) 
, sizeDist( sizeDist, groCollection->getSeedGenerator()->nextSeed() ) 
, constArea( constArea ) {;}


void SignalPlacer::loadDirectElemLink() { this->addLink( timer, LinkDirection::BACKWARD ); }