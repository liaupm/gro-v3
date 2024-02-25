#ifndef CG_PLASMID_HPP
#define CG_PLASMID_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/AgentState.hpp" //AgentState
#include "ut/ElementCounter.hpp" //OritCount
#include "ut/DistributionCombi.hpp" //mutationSelectionDist

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <vector> //operons, oriTs
#include <string> //name */


namespace cg
{
    class GeneticCollection;
    class Operon;
    class OriT;
    class MutationProcess; //repliMutations
    class PlasmidBase : public GeneticElement
    /* Virtual base class for plasmids with digital (PlasmidBoolean, bplasmid) and quantitative (PlasmidQuantitative, qplasmid) representation 
    A true bacterial plasmid or any DNA molecule that has components and can replicate or be transmitted or undergo reactions, like a virus or synthetic vectors like phagemids */
    {
        REGISTER_CLASS( "Plasmid", "pla", GeneticElement::ElemTypeIdx::PLASMID, true )

        public:
            enum class PlasmidType
            {
                BPLASMID, QPLASMID, COUNT
            };

            static constexpr PlasmidType DF_PLASMID_TYPE = PlasmidType::BPLASMID;
            inline static const std::vector<size_t> DF_REPLI_MUT_KEYS = {1};


        //---ctor, dtor
            virtual ~PlasmidBase() = default;
			
        //---get
          //resources
            PlasmidType getPlasmidType() const { return plasmidType; }
            inline const std::vector<const Operon*>& getOperons() const { return operons; }
			inline const Operon* getOperonByIndex( size_t idx ) const { return operons[idx]; }
            inline const std::vector<const OriT*>& getOriTs() const { return oriTs; }
            inline const std::vector<const MutationProcess*>& getRepliMutations() const { return repliMutations; }
          //options
            inline const std::vector<size_t>& getRepliMutationKeys() const { return repliMutationKeys; }

        //---set
            inline void setAll( const std::vector<const MutationProcess*>& xRepliMutations ) { setRepliMutations( xRepliMutations ); }
            inline void setRepliMutations( const std::vector<const MutationProcess*>& xRepliMutations ) { repliMutations = xRepliMutations; lockRepliMutations(); }

        //---API
          //precompute
            void loadDirectElemLink() override;
          //run
            //select a replication mutation. On conjugation for both types of plasmid; or on replication for Qplasmids only
            const MutationProcess* selectMutation( const std::vector<size_t>& keys, const ut::AgentState::BType& bState, const ut::AgentState::QType& qState, ut::DiscreteDist::ParamsType& params ) const; 
            inline const MutationProcess* sampleMutation( const ut::DiscreteDist::ParamsType& params ) const { size_t mutIdx = sampleMutationIdx( params ); return mutIdx < repliMutations.size() ? repliMutations[mutIdx] : nullptr; } 
            inline size_t sampleMutationIdx( const ut::DiscreteDist::ParamsType& params ) const { return mutationSelectionDist.sample( params ); } 


        protected:
            PlasmidType plasmidType;
          //resources
            std::vector<const Operon*> operons;
            std::vector<const OriT*> oriTs;
            std::vector<const MutationProcess*> repliMutations; //mutations that can take place on replication, from OriV (QPlasmids only) or OriT
          //options
            std::vector<size_t> repliMutationKeys; //assigns a numeric key to each of the repliMutations to allow different subsets of them depending on the concrete replication element (OriV or OriT)
          //dists
            mutable ut::DiscreteDistCombi< ut::DiscreteDist > mutationSelectionDist; //to select a replication mutation (or none)


            PlasmidBase( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const MutationProcess*>& repliMutations
            , const std::vector<size_t>& repliMutationKeys = DF_REPLI_MUT_KEYS, PlasmidType plasmidType = DF_PLASMID_TYPE );

            void lockRepliMutations();
    };

    using Plasmid = PlasmidBase;
}

#endif //CG_PLASMID_HPP