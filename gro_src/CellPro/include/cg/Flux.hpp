#ifndef CG_FLUX_HPP
#define CG_FLUX_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp" //timingTypeNM
#include "ut/DistributionScaler.hpp" //amountScaler, thresholdScaler

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <memory> */


namespace cg
{
    class GeneticCollection;
    class Molecule; //output
    class Metabolite; //metabolite
	class Flux : public GeneticElement
	/* A metabolic flux used for exchange with the medium (absorption and emission) or to affect the growth rate 
	Can also be used to sense medium Signals and transmit that to gene expression (QS) without altering the medium */
	{
		REGISTER_CLASS( "Flux", "flux", GeneticElement::ElemTypeIdx::FLUX, false )

		public:
			enum class TimingType
			{
				LYSIS, CONTINUOUS, COUNT
			};

			enum class FluxDirection : int
			{
				ABSORPTION = -1, EMISSION = 1, BOTH = 0, COUNT = 2
			};

			static constexpr TimingType DF_TIMING_TYPE = TimingType::CONTINUOUS;
			static constexpr FluxDirection DF_DIRECTION = FluxDirection::BOTH;
			static constexpr TReal DF_ACTIVATION = 1.0;
			static constexpr bool DF_BRELATIVE = true;
			static constexpr bool DF_B_INVERTED = false;

			static constexpr TReal DF_AMOUNT_MEAN = 0.0;
			static constexpr TReal DF_AMOUNT_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_AMOUNT_PARAMS = { DF_AMOUNT_MEAN, DF_AMOUNT_VAR };
			inline static const ParamDataType DF_AMOUNT_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_AMOUNT_PARAMS ); 

			static constexpr TReal DF_THRESHOLD_MEAN = -1.0; //no threshold
			static constexpr TReal DF_THRESHOLD_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_THRESHOLD_PARAMS = { DF_THRESHOLD_MEAN, DF_THRESHOLD_VAR };
			inline static const ParamDataType DF_THRESHOLD_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_THRESHOLD_PARAMS ); 

		
			static constexpr size_t INI_RANK = 0;

			static ut::NameMap<TimingType> timingTypeNM;
			static ut::NameMap<FluxDirection> directionNM;

		//---ctor, dtor
			Flux( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const Metabolite* metabolite, const GeneticElement* internalGate, const GeneticElement* signalsGate, const std::vector<const Molecule*>& output
			, const ParamDataType& amountData, const ParamDataType& thresholdData
			, TimingType timingType, TReal activation = DF_ACTIVATION, FluxDirection direction = DF_DIRECTION, bool bRelative = DF_BRELATIVE, bool bInverted = DF_B_INVERTED );

			virtual ~Flux() = default;

		//---get
		  //resources
			inline const Metabolite* getMetabolite() const { return metabolite; }
			inline const GeneticElement* getInternalGate() const { return internalGate; }
			inline const GeneticElement* getSignalsGate() const { return signalsGate; }
			inline const GeneticElement* getAmountFunction() const { return amountFunction; }
			inline const std::vector<const Molecule*>& getOutput() const { return output; }
			inline const GeneticElement* getAmountRandomness() const { return amountRandomness; }
			inline const GeneticElement* getThresholdRandomness() const { return thresholdRandomness; }
			inline const GeneticElement* getThresholdMarker() const { return thresholdMarker; }
		  //options
			inline TimingType getTimingType() const { return timingType; }
			inline TReal getActivation() const { return activation; }
			inline FluxDirection getFluxDirection() const { return direction; }
			inline bool getBRelative() const { return bRelative; }
			inline bool getIsRelative() const { return bRelative; }
			inline bool getBInverted() const { return bInverted; }
			inline bool getIsInverted() const { return bInverted; }
			inline bool getHasThreshold() const { return bThreshold; }
		  //state
			inline size_t getRank() const { return rank; }

		//---set
			void setAll( const GeneticElement* xInternalGate, const GeneticElement* xSignalsGate, const GeneticElement* xAmountFunction, const GeneticElement* xAmountRandomness, const GeneticElement* xThresholdRandomness );
			inline void setInternalGate( const GeneticElement* xInternalGate ) { internalGate = xInternalGate; } 
			inline void setSignalsGate( const GeneticElement* xSignalsGate ) { signalsGate = xSignalsGate; } 
			inline void setAmountFunction( const GeneticElement* xAmountFunction ) { amountFunction = xAmountFunction; } 
			void setAmountRandomness( const GeneticElement* xAmountRandomness );
			void setThresholdRandomness( const GeneticElement* xThresholdRandomness );

		//---API
		  //precompute
            void loadDirectElemLink() override;
			size_t calculateRank();
		  //run
			inline TReal calculate( TReal rndNum, bool gateState = true ) const { return gateState ? scaleAmount( rndNum ) : 0.0; } //returns the exchanged amount if the flux is active
		  //sampling
			inline TReal scaleAmount( TReal rndNum ) const { return amountScaler->scale( rndNum ); } //get a stochastic exchanged amount from a raw random value from amountRandomness
			inline TReal scaleThreshold( TReal rndNum ) const { return ut::fitL( thresholdScaler->scale( rndNum ) ); } //get a stochastic threshold value from a sample of thresholdRandomness. Only non-negative thresholds allowed
		

		private:
		  //resources
			const Metabolite* metabolite; //the exchanged Metabolite (Signal or biomass)
			const GeneticElement* internalGate; //condition on the internal state for the flux to be active
			const GeneticElement* signalsGate; //condition on the medium concentrations for the flux to be active
			const GeneticElement* amountFunction; //optional custom function for the exchanged amount. If null, amountRandomness and amountScaler are used instead
			std::vector<const Molecule*> output; //Molecules that are affected when the gates hold (QS)
			const GeneticElement* amountRandomness; //produces the random samples for stochastic exchanged amount. Used only if amountFunction is null
			const GeneticElement* thresholdRandomness; //produces the random samples for the stochastic medium threshold
			const GeneticElement* thresholdMarker;
		  //options
			TimingType timingType; //whether the flux is evaluated and applied during the whole life of the cell or just once on cell death (lysis)
			TReal activation; //activation amount that is transmitted to output Molecules when on
			FluxDirection direction; //to restrict the direction (absorption / emission) or allow both
			bool bRelative; //whether the amount is relative to the cell volume and has to be multiplied by this to get the absolute exchanged amount
			bool bInverted; //whether to invert the direction of amountFunction (absorption = negative, emission = positive). Only used if amountFunction not null
			bool bThreshold; //whether to use a simple medium condition as a threshold on the concentration of metabolite. AND wiith signalsGate
		  //dists
			SP<ut::DistributionScaler> amountScaler; //produces deterministic or stochastic raw exchanged amounts
			ut::DistributionScaler::ParamsType amountParams; //stored to pass to amountScaler
			SP<ut::DistributionScaler> thresholdScaler; //produces deterministic or stochastic threshold in case it is used
			ut::DistributionScaler::ParamsType thresholdParams; //stored to pass to thresholdScaler
		  //state
			size_t rank; //TODO
            bool bRankCalculated;
	};
}

#endif //CG_FLUX_HPP