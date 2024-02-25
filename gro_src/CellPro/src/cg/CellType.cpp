#include "cg/CellType.hpp"
#include "cg/GeneticCollection.hpp"
#include "cg/Strain.hpp" //strain loadDirectElemLink()
#include "cg/PlasmidBase.hpp" //bPlasmids, qPlasmids loadDirectElemLink()
#include "cg/Molecule.hpp" //molecules loadDirectElemLink()

using namespace cg;


CellType::CellType( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const Strain* strain, const std::vector<const PlasmidBase*>& bPlasmids, const std::vector<const Molecule*>& molecules, const std::vector<const PlasmidBase*>& qPlasmids, const std::vector< ut::ContinuousDistData >& qPlasmidAmountsData, bool bQPlasmidsAsConc )
: GeneticElement( id, name, GeneticElement::ElemTypeIdx::CELL_TYPE, true, geneticCollection, ElemCategory::INITIALIZER )
, strain(strain)
, bPlasmids(bPlasmids), molecules(molecules), qPlasmids(qPlasmids)
, bQPlasmidsAsConc( bQPlasmidsAsConc )
{
    for( const auto& distData : qPlasmidAmountsData )
		qPlasmidAmounts.push_back( std::make_shared<ut::ContinuousDistCombi>( distData.type, distData.params, geneticCollection->getSeedGenerator()->nextSeed() ) );
}


//----------------------------------- API-precompute

void CellType::loadDirectElemLink()
{ 
	this->addLink( strain, LinkDirection::FORWARD ); 
	this->addContents( bPlasmids ); 
	this->addContents( molecules );  
	//qPlasmids not included as the number has to be sampled
}

//-------------------------------API-run

uint CellType::sampleAmount( size_t idx, TReal vol ) const 
{ 
	if( bQPlasmidsAsConc )
		return ut::round<uint>( ut::fitL( qPlasmidAmounts[ idx ]->sample() * vol ) );
	else
		return ut::round<uint>( ut::fitL( qPlasmidAmounts[ idx ]->sample() ) ); 
}

std::vector<uint> CellType::sampleAmounts( TReal vol ) const
{ 
	std::vector<uint> result; 
	for( size_t p = 0; p < qPlasmidAmounts.size(); p++ )
		result.push_back( sampleAmount( p, vol ) ); 
	return result; 
}