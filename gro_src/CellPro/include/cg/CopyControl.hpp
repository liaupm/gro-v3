#ifndef CG_COPYCONTROL_HPP
#define CG_COPYCONTROL_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/AgentState.hpp" //ut::AgentState in calculate()
#include "ut/DistributionScaler.hpp" //eventTimeDist, plasmidSelectDist

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
    class GeneticCollection;
    class QMarker; //activeMarker

	class CopyControl : public GeneticElement
	/*A sequence to control of copy number of qplasmids using the canonical simplified method (no custom functions), in combination with OriV */
	{
		REGISTER_CLASS( "CopyControl", "cc", GeneticElement::ElemTypeIdx::COPY_CONTROL, true )

		public:
			inline static const GeneticElement* DF_W_RND = nullptr;
			inline static const QMarker* DF_ACTIVE_MARKER = nullptr;
			static constexpr TReal DF_W_MEAN = 1.0;
			static constexpr TReal DF_W_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_W_PARAMS = { DF_W_MEAN, DF_W_VAR };
			inline static const ParamDataType DF_W_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_W_PARAMS ); 

		//---ctor, dtor
			CopyControl( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* gate, const ParamDataType& wData = DF_W_DATA, const QMarker* activeMarker = DF_ACTIVE_MARKER );

			virtual ~CopyControl() = default;

		//---get
			inline const GeneticElement* getGate() const { return gate; }
			inline const GeneticElement* getWRandomness() const { return wRandomness; }
			inline const QMarker* getActiveMarker() const { return activeMarker; }
			inline const GeneticElement* getWMarker() const { return wMarker; }
			inline bool getHasActiveMarker() const { return activeMarker; }

		//---set
			inline void setAll( const GeneticElement* xGate, const GeneticElement* xWRandomness ) { setGate( xGate ); setWRandomness( xWRandomness ); }
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			void setWRandomness( const GeneticElement* xWRandomness );
			
		//---API
		  //preprocess
            void loadDirectElemLink() override;
          //run
			TReal calculate( const ut::AgentState::BType& bState, const ut::AgentState::QType& qState ) const; //get the total control effect, considering the state, the copy number and the unit weight, wchich may be stochastic, using the state of wRandomness
			inline TReal calculate( bool gateVal, TReal copyNum, TReal rawRndVal ) const { return gateVal ? sampleW( rawRndVal ) * copyNum : 0.0; }
		  //sampling
			inline TReal sampleW( TReal rawRndVal ) const { return ut::fitL( wScaler->scale( rawRndVal ) ); } //always positive


		private:
		  //resources
			const GeneticElement* gate; //active only if this evalautes to true
			const QMarker* activeMarker; //optional, to expose the number of active elements (copy number )
			const GeneticElement* wRandomness; //provides raw random numbers to generate stochastic control effects
			const GeneticElement* wMarker;
		  //dists
			SP<ut::DistributionScaler> wScaler; //produces deterministic or stochastic weights 
			ut::DistributionScaler::ParamsType wParams;
	};
}

#endif //CG_COPYCONTROL_HPP