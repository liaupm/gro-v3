#include "cg/Operon.hpp"
#include "cg/Regulation.hpp" //getBDosageEffect()
#include "cg/Molecule.hpp" //need to be included for implicit conversion to GeneticElement in loadDirectElemLink()

using namespace cg;


Operon::Operon( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const Regulation* regulation, const std::vector<const Molecule*>& output, const QMarker* actiMarker, bool bDosageEffect )

: GeneticElement::GeneticElement( id, name, GeneticElement::ElemTypeIdx::OPERON, bDosageEffect, geneticCollection, ElemCategory::DNA_BOTH )
, regulation(regulation), output(output), actiMarker( actiMarker )
, bDosageEffect( bDosageEffect || regulation->getBDosageEffect() ) {;}


//---API-precompute

void Operon::loadDirectElemLink()
{ 
    this->addLink( this, LinkDirection::FORWARD ); 
    this->addContent( regulation, 1 ); 
    this->addLink( regulation, LinkDirection::BACKWARD ); 
    this->addLinks( output, LinkDirection::FORWARD ); 
}