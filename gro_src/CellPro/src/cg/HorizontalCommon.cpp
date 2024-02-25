#include "cg/HorizontalCommon.hpp"
#include "cg/GeneticCollection.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()

using namespace cg;


HorizontalCommon::HorizontalCommon( const GeneticCollection* geneticCollection, size_t id, const std::string& name )
: GeneticElement( id, name, GeneticElement::ElementIndex::HORIZONTAL_COMMON, false, geneticCollection, ElemCategory::HANDLER_COMMON )
, shuffleRnEngine( geneticCollection->getSeedGenerator()->nextSeed() ) {;}