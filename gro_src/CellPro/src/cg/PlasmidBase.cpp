#include "cg/PlasmidBase.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/MutationProcess.hpp" //calculateRate() in selectMutation()
#include "cg/Operon.hpp" //need to be included for implicit conversion to GeneticElement in loadDirectElemLink()
#include "cg/OriT.hpp" //need to be included for implicit conversion to GeneticElement in loadDirectElemLink()

using namespace cg;


PlasmidBase::PlasmidBase( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const MutationProcess*>& repliMutations
, const std::vector<size_t>& repliMutationKeys, PlasmidType plasmidType )

: GeneticElement(id, name, ElemTypeIdx::PLASMID, plasmidType == PlasmidType::QPLASMID, geneticCollection, ElemCategory::DNA_CONTAINER )
, plasmidType( plasmidType ), operons( operons ), oriTs( oriTs ), repliMutations( repliMutations ), repliMutationKeys( repliMutationKeys )
, mutationSelectionDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() ) { lockRepliMutations(); }


void PlasmidBase::lockRepliMutations()
{
	for( const MutationProcess* mutP : repliMutations )
		const_cast<MutationProcess*>( mutP )->makeRepliDependent();
}


//----------------------------------------------- API-precmpute

void PlasmidBase::loadDirectElemLink() 
{ 
	this->addContents( operons ); 
	this->addContents( oriTs ); 
}


//----------------------------------------------- API-run

const MutationProcess* PlasmidBase::selectMutation( const std::vector<size_t>& keys, const ut::AgentState::BType& bState, const ut::AgentState::QType& qState, ut::DiscreteDist::ParamsType& params ) const
/* samples a replication mutation from those that the QPlasmid can undergo from the keys of the emmiter (OriV or OriT) */
{
    if( repliMutations.size() > 0 && keys.size() > 0 )
    {
        //ut::DiscreteDist::ParamsType params; //probability of each possible MutationProcess
        params.reserve( repliMutations.size() ); //fixed size, if not in the emitter key or inactive, a 0 is pushed

        TReal total = 0.0;
      //calculate the probability of each potential mutation
        for( size_t m = 0; m < repliMutations.size(); m++ )
        {
            if( std::count( keys.begin(), keys.end(), repliMutationKeys[m] ) && bState.checkBit( repliMutations[m]->getGate()->getAbsoluteId() ) ) //if in emitter keys and active
            {
                TReal prob = repliMutations[m]->calculateRate( bState, qState );
                params.push_back( prob );
                total += prob;
            }
            else
                params.push_back( 0.0 );
        }
      //add the probability of no mutation
        if( total < 1.0 )
            params.push_back( 1.0 - total );

      //sample the mutation
        return sampleMutation( params );
    }
    return nullptr;
}


