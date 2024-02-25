#ifndef CG_ORIV_HPP
#define CG_ORIV_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //plasmidSelectDist
#include "ut/DistributionScaler.hpp" //plasmidSelectDist
#include "ut/AgentState.hpp" //AgentState in calculate() methods

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <vector> //copyControls */


namespace cg
{
    class GeneticCollection;
    class CopyControl; //copyControls
    class QMarker; //eclipseMarker, repliEventMarker

	class OriV : public GeneticElement
	/* A DNA component of QPlasmids that allow for replication via Gillespie algorithm, potentially followed by an exlipse period.
	Used to simulate true OriVs or others, like phage replication. In the canonic way, works in combination with CopyControl */
	{
		REGISTER_CLASS( "OriV", "oV", GeneticElement::ElemTypeIdx::ORI_V, true )

		public:
			inline static const GeneticElement* DF_CUSTOM_FUN = nullptr;
			inline static const QMarker* DF_ECLIPSE_MARKER = nullptr;
			inline static const QMarker* DF_REPLI_EVENT_MARKER = nullptr;
			inline static const GeneticElement* DF_VMAX_RND = nullptr;
			inline static const GeneticElement* DF_W_RND = nullptr;

			static constexpr TReal DF_VMAX_MEAN = 100.0;
			static constexpr TReal DF_VMAX_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_VMAX_PARAMS = { DF_VMAX_MEAN, DF_VMAX_VAR };
			inline static const ParamDataType DF_VMAX_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_VMAX_PARAMS ); 

			static constexpr TReal DF_W_MEAN = 1.0;
			static constexpr TReal DF_W_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_W_PARAMS = { DF_W_MEAN, DF_W_VAR };
			inline static const ParamDataType DF_W_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_W_PARAMS ); 

			static constexpr bool DF_B_ECLIPSE = true;
			inline static const std::vector<size_t> DF_MUTATION_KEYS = {1};


		//---ctor, dtor
			OriV( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* gate, const std::vector<const CopyControl*>& copyControls, const GeneticElement* customFunction = DF_CUSTOM_FUN
			, const ParamDataType& vmaxData = DF_VMAX_DATA, const ParamDataType& wData = DF_W_DATA, const QMarker* eclipseMarker = DF_ECLIPSE_MARKER, const QMarker* repliEventMarker = DF_REPLI_EVENT_MARKER
			, bool bEclipse = DF_B_ECLIPSE, const std::vector<size_t>& mutationKeys = DF_MUTATION_KEYS );

			virtual ~OriV() = default;

		//---get
		  //resources
			inline const GeneticElement* getGate() const { return gate; }
			inline const std::vector<const CopyControl*>& getCopyControls() const { return copyControls; }
			inline const GeneticElement* getCustomFunction() const { return customFunction; }
			inline const QMarker* getEclipseMarker() const { return eclipseMarker; }
			inline const QMarker* getRepliEventMarker() const { return repliEventMarker; }

			inline const GeneticElement* getVmaxRandomness() const { return vmaxRandomness; }
			inline const GeneticElement* getWRandomness() const { return wRandomness; }
			inline const GeneticElement* getVmaxMarker() const { return vmaxMarker; }
			inline const GeneticElement* getWMarker() const { return wMarker; }

		  //options
			inline bool getBEclipse() const { return bEclipse; }
			inline bool getHasEclipse() const { return bEclipse; }
			inline const std::vector<size_t>& getMutationKeys() const { return mutationKeys; }

		//---set
			void setAll( const GeneticElement* xGate, const GeneticElement* xCustomFunction, const GeneticElement* xVmaxRandomness, const GeneticElement* xWRandomness );
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			inline void setCustomFunction( const GeneticElement* xCustomFunction ) { customFunction = xCustomFunction; }
			void setVmaxRandomness( const GeneticElement* xVmaxRandomness );
			void setWRandomness( const GeneticElement* xWRandomness );

		//---API
		  //precompute
            void loadDirectElemLink() override;
		  //run
			TReal calculateRate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState, TReal vol ) const; //get rate from gate state, copy number, potential custom function value and potential random values
			TReal calculateCopyControlRaw( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const;
		  //sampling
			inline TReal sampleVmax( TReal rawRndVal ) const { return ut::fitL( vmaxScaler->scale( rawRndVal ) ); } //non-negative
			inline TReal sampleW( TReal rawRndVal ) const { return ut::fitL( wScaler->scale( rawRndVal ) ); } //non-negative
			inline size_t samplePlasmid( const ut::DiscreteDistribution<>::RealParamsType& params ) const { return plasmidSelectDist.sample( params ); } //return idx of concrete plasmid to replicate
            

		private:
		  //resources
			const GeneticElement* gate; //active only if this condition holds; if not, no replication
			std::vector<const CopyControl*> copyControls; //the CopyControl sequences that influence the rate in the canonical calculation
			const GeneticElement* customFunction; //custom function for the rate. Shadows the canonical way (vmaxRandomness, wRandomness, vmaxScaler, vmaxParams)
			const QMarker* eclipseMarker;
			const QMarker* repliEventMarker;
			const GeneticElement* vmaxRandomness; //raw random values for the stochastic vmax in canonic calculation
			const GeneticElement* wRandomness; //raw random values for the w param in canonic calculation
			const GeneticElement* vmaxMarker;
			const GeneticElement* wMarker;
		  //options
			bool bEclipse; //whether eclipse period (time in each QPlasmid) for both copies
			std::vector<size_t> mutationKeys; //mutation groups that are possible on replication from this oriV
		  //dists
			SP<ut::DistributionScaler> vmaxScaler; //deterministic or stochastic vmax param for canonic calculation
			ut::DistributionScaler::ParamsType vmaxParams;
			SP<ut::DistributionScaler> wScaler; //deterministic or stochasticw param for canonic calculation
			ut::DistributionScaler::ParamsType wParams;

			mutable ut::DiscreteDistCombi< ut::DiscreteDist > plasmidSelectDist; //to select one concrete plasmid from all the plasmids that carry this oriV
	};
}

#endif //CG_ORIV_HPP