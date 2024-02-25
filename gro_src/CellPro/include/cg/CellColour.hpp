#ifndef CG_CELLCOLOUR_HPP
#define CG_CELLCOLOUR_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/RgbColour.hpp" //rgbColour
#include "ut/NameMap.hpp" //ut::NameMap colourModeNM
#include "ut/DistributionScaler.hpp" //maxValueScaler, deltaScaler

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class and target

/*PRECOMPILED
#include <memory> //SP<> */


namespace cg
{
	class GeneticCollection;
	class CellColour : public GeneticElement
	/*Colour of a cell, used to report cell types and internal states */
	{
		REGISTER_CLASS( "CellColour", "ccol", GeneticElement::ElemTypeIdx::CELL_COLOUR, false )
		
		public:
            enum class ColourMode
			{
                ABSOLUTE, DELTA, COUNT
			};

			inline static const GeneticElement* DF_TARGET = nullptr;
            static constexpr ColourMode DF_MODE = ColourMode::ABSOLUTE;
            static constexpr TReal DF_SCALE = 1.0;

			static constexpr double DF_MAX_VALUE_MEAN = 1.0;
			static constexpr double DF_MAX_VALUE_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_MAX_VALUE_PARAMS = { DF_MAX_VALUE_MEAN, DF_MAX_VALUE_VAR };

			inline static const GeneticElement* DF_DELTA_RND = nullptr;
			static constexpr double DF_DELTA_MEAN = 0.01;
			static constexpr double DF_DELTA_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_DELTA_PARAMS = { DF_DELTA_MEAN, DF_DELTA_VAR };

			static constexpr TReal RESET_VAL = 0.0; //initial value in newly created cells
			static constexpr TReal MAX_VAL = 1.0; //maximum intensity value
			
			static ut::NameMap<ColourMode> colourModeNM;


		//---ctor, dtor
            CellColour( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const GeneticElement* gate, ColourMode mode, const ut::RgbColour& rgbColour
    		, const GeneticElement* maxValueRandomness, const ut::DistributionScaler::ParamsType& maxValueParams, const GeneticElement* deltaRandomness, const ut::DistributionScaler::ParamsType& deltaParams
    		, const GeneticElement* target = DF_TARGET, TReal scale = DF_SCALE );

			virtual ~CellColour() = default;

		//---get
			inline const GeneticElement* getGate() const { return gate; }
            inline ColourMode getMode() const { return mode; }
			inline const ut::RgbColour& getRgbColour() const { return rgbColour; }
			inline const GeneticElement* getMaxValueRandomness() const { return maxValueRandomness; }
			inline const GeneticElement* getDeltaRandomness() const { return deltaRandomness; }

			inline const GeneticElement* getTarget() const { return target; }
			inline bool getHasTarget() const { return target; }
			inline TReal getScale() const { return scale; }

		//---set
			void setAll( const GeneticElement* xGate, const GeneticElement* xMaxValueRandomness, const GeneticElement* xDeltaRandomness, const GeneticElement* xTarget );
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			void setMaxValueRandomness( const GeneticElement* xMaxValueRandomness );
			void seDeltaRandomness( const GeneticElement* xDeltaRandomness );
			inline void setTarget( const GeneticElement* xTarget ) { target = xTarget; }
			
		//---API
		  //precompute
			void loadDirectElemLink() override;
		  //run
			inline TReal calculateMaxValue( TReal funVal, TReal rawRndVal ) const { return target ? ut::fit( funVal * scale ) : sampleMaxValue( rawRndVal ); } //selects the maxValue depending on the existence of target
            inline TReal updateValue( TReal maxValue, TReal delta, TReal value, bool gateState ) const { return mode == ColourMode::DELTA ? updateDelta( maxValue, delta, value, gateState ) : updateAbsolute( maxValue, gateState ); } //method selector by mode
			inline TReal updateAbsolute( TReal maxValue, bool gateState ) const { return gateState ? maxValue : RESET_VAL; } 
			inline TReal updateDelta( TReal maxValue, TReal delta, TReal value, bool gateState ) const { return gateState ? ut::fitU( value + delta, maxValue ) : ut::fitL( value - delta, RESET_VAL ); }

			inline TReal sampleMaxValue( TReal rawRndVal ) const { return ut::fit( maxValueScaler->scale( rawRndVal ) ); } //always in [0,1]
			inline TReal sampleDelta( TReal rawRndVal ) const { return ut::fitL( deltaScaler->scale( rawRndVal ) ); } //always positive. May be greater than 1 depending on the time step size


		private:
		  //resources
			const GeneticElement* gate; //the colour is only applied if this condition evaluates to true
			const GeneticElement* target; //if not null, the maximum value matches the value of this element, to report it. Shadows maxValueRandomness and maxValueScaler
			const GeneticElement* maxValueRandomness; //generates the raw random values for stochastic maximum value. Used only if null target
			const GeneticElement* deltaRandomness; //generates the raw random values for the delta. Used only if mode = DELTA
		  //options
            ColourMode mode; //whther instant (absolute) or gradual (delta)
			ut::RgbColour rgbColour; //rgb channels in [0,1]
			TReal scale; //automatically multiplies the value of target. Used only if target not null
		  //dists
			SP<ut::DistributionScaler> maxValueScaler; //produces deterministic or stochastic maximum values (intensity of colour)
			ut::DistributionScaler::ParamsType maxValueParams; 
			SP<ut::DistributionScaler> deltaScaler; //produces deterministic or stochastic delta (intensity increases). In intensity (in [0,1]) / min
			ut::DistributionScaler::ParamsType deltaParams;
	};
}

#endif //CG_CELLCOLOUR_HPP
