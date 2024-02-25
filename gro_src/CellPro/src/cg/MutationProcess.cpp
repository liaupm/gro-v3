#include "cg/MutationProcess.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()

using namespace cg;


ut::NameMap<MutationProcess::Type> MutationProcess::typeNM ( { 
                                          { "spontaneous", MutationProcess::Type::SPONTANEOUS }
                                        , { "replication", MutationProcess::Type::REPLICATION } } );


MutationProcess::MutationProcess( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* gate, const std::vector<const Mutation*>& mutations, const ParamDataType& rateData, const QMarker* eventMarker
, bool bEclipse, const std::vector<TReal>& mutationProbs )

: GeneticElement( id, name, ElemTypeIdx::MUTATION_PROCESS, true, geneticCollection, ElemCategory::FUNCTIONAL )
, gate( gate ), rateFunction( rateData.fun ), mutations( mutations ), eventMarker( eventMarker ), rateRandomness( rateData.rnd ), rateMarker( rateData.marker )
, type( Type::SPONTANEOUS ), bEclipse( bEclipse ), mutationProbs( mutationProbs )

, rateScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( rateData.rnd ), rateData.params ) ), rateParams( rateData.params )
, mutationSelectDist( mutationProbs, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}


void MutationProcess::setAll( const GeneticElement* xGate, const GeneticElement* xRateFunction, const GeneticElement* xRateRandomness ) 
{ 
	setGate( xGate ); 
	setRateFunction( xRateFunction ); 
	setRateRandomness( xRateRandomness ); 
}

void MutationProcess::setRateRandomness( const GeneticElement* xRateRandomness )
{
	rateRandomness = xRateRandomness;
	rateScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( rateRandomness ), rateParams );
}


//-------------------------------------------------API-precompute

void MutationProcess::loadDirectElemLink()
{ 
	this->addLink( gate, LinkDirection::BACKWARD );
	this->addLink( rateFunction, LinkDirection::BACKWARD );
	this->addLinks( mutations, LinkDirection::BACKWARD );
	this->addLink( rateRandomness, LinkDirection::BACKWARD );
}


//-------------------------------------------------API-run

TReal MutationProcess::calculateRate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const 
{ 
	if( bState.checkBit( gate->getAbsoluteId() ) ) //active
	{
		if( rateFunction )
			return qState[ rateFunction->getAbsoluteId() ] ; //custom rate function
		
		return sampleRate( rateRandomness ? qState[ rateRandomness->getAbsoluteId() ] : 0.0 ); //simple deterministic or stochastic value, depending on scaler type
	}
	return 0.0; //inactive
}