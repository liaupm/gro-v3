#include "cg/PlasmidBoolean.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()

using namespace cg;


PlasmidBoolean::PlasmidBoolean( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const MutationProcess*>& repliMutations, const std::vector<size_t>& repliMutationKeys
, const ParamDataType& lossData )

: PlasmidBase( geneticCollection, id, name, operons, oriTs, repliMutations, repliMutationKeys, PlasmidType::BPLASMID )
, lossFunction( lossData.fun ), lossRandomness( lossData.rnd ), lossMarker( lossData.marker )
, lossScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( lossData.rnd ), lossData.params ) ), lossParams( lossData.params )
, lossDist( { lossData.params.size() > 0 ? 1.0 - lossData.params[0] : 1.0, lossData.params.size() > 0 ? lossData.params[0] : 0.0 }, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void PlasmidBoolean::setLossRandomness( const GeneticElement* xLossRandomness )
{
	lossRandomness = xLossRandomness;
	lossScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( lossRandomness ), lossParams );
}


//-----------------------------------------API-run

uint PlasmidBoolean::sampleLoss( const ut::AgentState::QType& qState ) const
{
	TReal lossProb;
	if( lossFunction )
		lossProb = ut::fit( qState[ lossFunction->getAbsoluteId() ] );
	else
		lossProb = sampleLossProb( lossRandomness ? qState[ lossRandomness->getAbsoluteId() ] : 0.0 );

	if( lossDist.sample( ut::DiscreteDist::RealParamsType( { 1.0 - lossProb, lossProb } ) ) )
		return sampleDaughter() + 1;
	return 0;
}

uint PlasmidBoolean::sampleLoss( TReal rawRndVal, TReal funValue ) const
{
	TReal lossProb;
	if( lossFunction )
		lossProb = ut::fit( funValue );
	else
		lossProb = sampleLossProb( rawRndVal );

	if( lossDist.sample( ut::DiscreteDist::RealParamsType( { 1.0 - lossProb, lossProb } ) ) )
		return sampleDaughter() + 1;
	return 0;
}