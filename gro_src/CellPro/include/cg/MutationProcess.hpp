#ifndef CG_MUTATIONPROCESS_HPP
#define CG_MUTATIONPROCESS_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp" //typeNM
#include "ut/DistributionCombi.hpp" //emutationSelectDist
#include "ut/AgentState.hpp" //ut::AgentState in claculateRate()

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <vector> //mutations
#include <string> //name */


namespace cg
{
	class GeneticCollection;
	class QMarker; //eventMarker
	class Mutation; //mutations
	class MutationProcess : public GeneticElement
	/* A reaction proccess that modifies plasmids. Used together with Mutation
	Can be used to simulate any reaction involving small amounts of reactants (with the Gillespie algorithm) */
	{
		REGISTER_CLASS( "MutationProcess", "mutp", GeneticElement::ElemTypeIdx::MUTATION_PROCESS, true )
		
		public:
			enum class Type
			{
				REPLICATION, SPONTANEOUS, COUNT
			};

			inline static const QMarker* DF_EVENT_MARKER = nullptr;
			inline static const GeneticElement* DF_RATE_RND = nullptr;
			static constexpr Type DF_TYPE = Type::SPONTANEOUS;
			static constexpr bool DF_B_ECLIPSE = false;
			inline static const std::vector<TReal>& DF_MUTATION_PROBS = { 1.0 };

			static constexpr TReal DF_RATE_MEAN = 1.0;
			static constexpr TReal DF_RATE_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_RATE_PARAMS = { DF_RATE_MEAN, DF_RATE_VAR };
			inline static const ParamDataType DF_RATE_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_RATE_PARAMS ); 

			static ut::NameMap<Type> typeNM;
			

		//---ctor, dtor
			MutationProcess( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const GeneticElement* gate, const std::vector<const Mutation*>& mutations, const ParamDataType& rateData = DF_RATE_DATA, const QMarker* eventMarker = DF_EVENT_MARKER
            , bool bEclipse = DF_B_ECLIPSE, const std::vector<TReal>& mutationProbs = DF_MUTATION_PROBS );

			virtual ~MutationProcess() = default;

		//---get
		  //resource
			inline const GeneticElement* getGate() const { return gate; }
			inline const GeneticElement* getRateFunction() const { return rateFunction; }
			inline const std::vector<const Mutation*>& getMutations() const { return mutations; }
			inline const QMarker* getEventMarker() const { return eventMarker; }
			inline const GeneticElement* getRateRandomness() const { return rateRandomness; }
			inline const GeneticElement* getRateMarker() const { return rateMarker; }
		  //options
			inline Type getType() const { return type; }
			inline const std::vector<TReal>& getMutationProbs() const { return mutationProbs; }
			inline bool getBEclipse() const { return bEclipse; }
			inline bool getHasEclipse() const { return bEclipse; }

		//---set
			void setAll( const GeneticElement* xGate, const GeneticElement* xRateFunction, const GeneticElement* xRateRandomness );
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			inline void setRateFunction( const GeneticElement* xRateFunction ) { rateFunction = xRateFunction; }
			void setRateRandomness( const GeneticElement* xRateRandomness );

			inline void setType( Type xType ) { type = xType; }
			inline void makeRepliDependent() { type = Type::REPLICATION; }
			
		//---API
		  //precompute
			void loadDirectElemLink() override;
		  //run
			TReal calculateRate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const; //calculates the rate considering the gate state, the optional custom function and the optional randomness
			inline TReal calculateRate( TReal functionVal, bool gateVal, TReal rawRndVal ) const { return gateVal ? ( rateFunction ? functionVal : sampleRate( rawRndVal ) ) : 0.0; }
		  //sampling
			inline TReal sampleRate( TReal rawRndVal ) const { return ut::fitL( rateScaler->scale( rawRndVal ) ); } //always non-negative
			inline const Mutation* sampleMutation() const { return mutations.size() > 0 ? mutations[ sampleMutationIdx() ] : nullptr; } //returns the mutation; nullptr if empty
			inline size_t sampleMutationIdx() const { return mutationSelectDist.sample(); } //returns the index in mutations


		private:
		  //resources
			const GeneticElement* gate; //the process is active when it evaluates to true
			const GeneticElement* rateFunction; //rate in reactions /min, used as the lambda parameter of an exponential distribution (Poisson process)
			std::vector<const Mutation*> mutations; //description of the possible reactions with their stoichiometry
			const QMarker* eventMarker; 
			const GeneticElement* rateRandomness; 
			const GeneticElement* rateMarker;
		  //options
			Type type; //either evaluated all the time (spontaneous) or linked to replication (from OriV or OriT)
			bool bEclipse; //whether the qplasmids created by the reaction undergo an eclipse period or appear instantaneously. Uses the eclipse times of each qplasmid
			std::vector<TReal> mutationProbs; //probability of each mutation
		  //dists
			SP<ut::DistributionScaler> rateScaler; //to produce deterministic or stochastic rates. Only used if rateFunction is null
			ut::DistributionScaler::ParamsType rateParams;
			mutable ut::DiscreteDistCombi< ut::DiscreteDist > mutationSelectDist; //uses mutationProbs to select from mutations
	};
}

#endif //CG_MUTATIONPROCESS_HPP