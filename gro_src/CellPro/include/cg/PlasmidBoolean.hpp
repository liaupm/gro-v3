#ifndef CG_PLASMIDCACA_HPP
#define CG_PLASMIDCACA_HPP

#include "ut/AgentState.hpp" //sampleLoss()
#include "ut/DistributionScaler.hpp" //lossDist

#include "cg/defines.hpp"
#include "cg/PlasmidBase.hpp" //base class

/*PRECOMPILED
#include <string> //name */


namespace cg
{
	class GeneticCollection;
	class Operon;
    class OriT;

	class PlasmidBoolean : public PlasmidBase
	/* A plasmid or DNA molecule with digital representation (no explicit replication and split on cell division) */
	{
		public:
			inline static const GeneticElement* DF_LOSS_FUN = nullptr;
			inline static const GeneticElement* DF_LOSS_RND = nullptr;
			static constexpr TReal DF_LOSS_MEAN = 0.0;
			static constexpr TReal DF_LOSS_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_LOSS_PARAMS = { DF_LOSS_MEAN, DF_LOSS_VAR };
			inline static const ParamDataType DF_LOSS_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_LOSS_PARAMS ); 


		//---ctor, dtor
			PlasmidBoolean( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const MutationProcess*>& repliMutations, const std::vector<size_t>& repliMutationKeys = PlasmidBase::DF_REPLI_MUT_KEYS
            , const ParamDataType& lossData = DF_LOSS_DATA );

			virtual ~PlasmidBoolean() = default;

		//---get
			inline const GeneticElement* getLossFunction() const { return lossFunction; }
			inline const GeneticElement* getLossRandomness() const { return lossRandomness; }
			inline const GeneticElement* getLossMarker() const { return lossMarker; }

		//---set
			inline void setAll( const GeneticElement* xLossFunction, const GeneticElement* xLossRandomness ) { setLossFunction( xLossFunction ); setLossRandomness( xLossRandomness ); } 
			inline void setLossFunction( const GeneticElement* xLossFunction ) { lossFunction = xLossFunction; }
			void setLossRandomness( const GeneticElement* xLossRandomness );
			
		//---API
		  //run
			//sample whether the plasmid is loss (1 or 2, the daguhter cell that losses it) or not (0), considering the potential custom function and randomness
			uint sampleLoss( const ut::AgentState::QType& qState ) const; 
			uint sampleLoss( TReal rawRndVal, TReal funValue ) const;
            
            inline TReal sampleLossProb( TReal rawRndVal ) const { return ut::fit( lossScaler->scale( rawRndVal ) ); }
            inline uint sampleDaughter() const { return daughterDist.sample(); }


		private:
		  //resources
			const GeneticElement* lossFunction; //custom function for the loss probability
			const GeneticElement* lossRandomness; //raw random values for lossScaler
			const GeneticElement* lossMarker;
		  //dist
			SP<ut::DistributionScaler> lossScaler; //potentially stochastic loss prob
			ut::DistributionScaler::ParamsType lossParams;
			mutable ut::DiscreteDistCombi< ut::DiscreteDist > lossDist; //to sample whether there is plasmid loss (using the prob from lossScaler)
			mutable ut::ConcDistCombi< ut::DiscreteUniformDist > daughterDist; //to sample which daughter cell losses the plasmid
	};

	using BPlasmid = PlasmidBoolean;
}

#endif //CG_PLASMIDCACA_HPP