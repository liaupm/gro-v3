#include "cg/PlasmidQuantitative.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()

#include "cg/GeneticCollection.hpp"
#include "cg/CopyControl.hpp" //loadDirectElemLink()
#include "cg/OriV.hpp" //loadDirectElemLink()
#include "cg/PartitionSystem.hpp" //loadDirectElemLink()

using namespace cg;


PlasmidQuantitative::PlasmidQuantitative( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const OriV*>& oriVs, const std::vector<const CopyControl*>& copyControls, const PartitionSystem* partitionSystem, const std::vector<const MutationProcess*>& repliMutations
, const GeneticElement* eclipseFunction, const QMarker* eclipseMarker, const ut::ContinuousDistData& copyNumber, const ut::ContinuousDistData& eclipse, const std::vector<size_t>& repliMutationKeys )

: PlasmidBase( geneticCollection, id, name, operons, oriTs, repliMutations, repliMutationKeys, PlasmidType::QPLASMID )
, oriVs( oriVs ), copyControls( copyControls ), partitionSystem( partitionSystem ), eclipseFunction( eclipseFunction ), eclipseMarker( eclipseMarker )
, bHasEclipse( eclipseFunction || ut::anyNotNull( eclipse.params ) )
, copyNumber( copyNumber )
, eclipseDist( eclipse, geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void PlasmidQuantitative::setEclipseFunction( const GeneticElement* xEclipseFunction ) 
{ 
	eclipseFunction = xEclipseFunction; 
	bHasEclipse = eclipseFunction || ut::anyNotNull( eclipseDist.getDistribution()->getParams() ) ;
}

//----------------------------------API-precompute

void PlasmidQuantitative::loadDirectElemLink()
{ 
	PlasmidBase::loadDirectElemLink();
	this->addContents( oriVs );
	this->addContents( copyControls );
	this->addContent( partitionSystem );
	if( bHasEclipse )
		this->addLink( this, LinkDirection::FORWARD );
}