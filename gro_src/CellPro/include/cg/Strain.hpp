#ifndef CG_STRAIN_HPP
#define CG_STRAIN_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //distributions
#include "ut/DistributionScaler.hpp" //baseGrScaler
#include "ut/SeedGenerator.hpp" //nextSeed(), makeDistributions();

#include "cg/defines.hpp" 
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in ctor
#include <memory> //SP<> in baseGrScaler
#include <math.h> //std::log in calculateGenerationTime(), pow */


namespace cg
{
	class GeneticCollection;
	class Strain : public GeneticElement
	/* Basic growth, division and death properties of a cell. Fixed, cannot change during the simulation */
	{
		REGISTER_CLASS( "Strain", "str", GeneticElement::ElemTypeIdx::STRAIN, false )

		public:
			enum DistIdx
            {
                INI_VOL, DIVISION_VOL, DIVISION_FRACTION, DIVISION_ROTATION, COUNT
            };
			
			using DistributionType = ut::DistributionCombi;
			using DistributionsDS = std::array<SP<DistributionType>, DistIdx::COUNT>;
			using DistributionParamsDS = std::array<std::vector<TReal>, DistIdx::COUNT>;
			using DistributionsDataDS = std::array<ut::ContinuousDistData, DistIdx::COUNT>;

		//---static
			static constexpr bool DF_USE_METABOLISM_GROWTH = false;
			static constexpr bool DF_NEGATIVE_GROWTH_ALLOWED = true;
			static constexpr bool DF_GROWTH_CORRECTION = true;

			static constexpr TReal DF_GROWTH_RATE_MEAN = 0.03465735903; // min-1
			static constexpr TReal DF_GROWTH_RATE_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_GROWTH_RATE_PARAMS = { DF_GROWTH_RATE_MEAN, DF_GROWTH_RATE_VAR };
			static constexpr ut::ContinuousDistInterface::DistributionType DF_BASE_GR_DIST_TYPE = ut::ContinuousDistInterface::DistributionType::NORMAL;

			static constexpr TReal DF_INI_VOL_LBOUND = 1.308997;
			static constexpr TReal DF_INI_VOL_UBOUND = 2.879793; 
			static constexpr ut::ContinuousDist::DistributionType DF_INI_VOL_DISTTYPE = ut::ContinuousDist::DistributionType::UNIFORM;
			inline static const std::vector<TReal> DF_INI_VOL_PARAMS = { DF_INI_VOL_LBOUND, DF_INI_VOL_UBOUND };
			inline static const ut::ContinuousDistData DF_INI_VOL_DIST = ut::ContinuousDistData( DF_INI_VOL_DISTTYPE, DF_INI_VOL_PARAMS );

			static constexpr TReal DF_DIV_VOL_MEAN = 2.879793; //um3 = fL, as in the original gro
			static constexpr TReal DF_DIV_VOL_VAR = 0.070711; //as in the original gro
			static constexpr ut::ContinuousDist::DistributionType DF_DIV_VOL_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
			inline static const std::vector<TReal> DF_DIV_VOL_PARAMS = { DF_DIV_VOL_MEAN, DF_DIV_VOL_VAR };
			inline static const ut::ContinuousDistData DF_DIV_VOL_DIST = ut::ContinuousDistData( DF_DIV_VOL_DISTTYPE, DF_DIV_VOL_PARAMS );

			static constexpr TReal DF_DIV_FRACTION_VOL_MEAN = 0.5;
			static constexpr TReal DF_DIV_FRACTION_VOL_VAR = 0.05;
			static constexpr ut::ContinuousDist::DistributionType DF_DIV_FRACTION_VOL_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
			inline static const std::vector<TReal> DF_DIV_FRACTION_VOL_PARAMS = { DF_DIV_FRACTION_VOL_MEAN, DF_DIV_FRACTION_VOL_VAR };
			inline static const ut::ContinuousDistData DF_DIV_FRACTION_VOL_DIST = ut::ContinuousDistData( DF_DIV_FRACTION_VOL_DISTTYPE, DF_DIV_FRACTION_VOL_PARAMS );

			static constexpr TReal DF_DIV_ROTATION_LBOUND = -0.125; 
			static constexpr TReal DF_DIV_ROTATION_UBOUND = 0.125;
			static constexpr TReal DF_DIV_ROTATION_MEAN = ( DF_DIV_ROTATION_LBOUND + DF_DIV_ROTATION_UBOUND ) / 2.0; //0.0
			static constexpr ut::ContinuousDist::DistributionType DF_DIV_ROTATION_DISTTYPE = ut::ContinuousDist::DistributionType::UNIFORM;
			inline static const std::vector<TReal> DF_DIV_ROTATION_PARAMS = { DF_DIV_ROTATION_LBOUND, DF_DIV_ROTATION_UBOUND };
			inline static const ut::ContinuousDistData DF_DIV_ROTATION_DIST = ut::ContinuousDistData( DF_DIV_ROTATION_DISTTYPE, DF_DIV_ROTATION_PARAMS );

			static constexpr TReal DF_DIAMETER = 1.0; //um
			static constexpr TReal DF_RADIUS = DF_DIAMETER / 2.0; //um
			static constexpr TReal DF_DEATH_THRESHOLD = 0.0; //minimum growth rate to be alive

			inline static TReal calculateGenerationTime( TReal iniVol, TReal divisionVol, TReal growthRate ) { return std::log( divisionVol / iniVol ) / growthRate; }
			inline static TReal correctGrowthRate( TReal raw, TReal stepSize ) { return ( std::exp( raw * stepSize ) - 1.0 ) / stepSize; }


		//---ctor, dtor
			Strain( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* divisionGate, const GeneticElement* deathGate, const GeneticElement* grCustomFun, const GeneticElement* baseGrRandomness
			, const ut::DistributionScaler::ParamsType& baseGrParams, const DistributionsDataDS& distributionsData, TReal deathThreshold = DF_DEATH_THRESHOLD, bool bNegativeGrowthAllowed = DF_NEGATIVE_GROWTH_ALLOWED, bool bGrowthCorrection = DF_GROWTH_CORRECTION );

			virtual ~Strain() = default;

		//---get
		  //resources
			inline const GeneticElement* getDivisionGate() const { return divisionGate; } 
			inline const GeneticElement* getDeathGate() const { return deathGate; } 
			inline const GeneticElement* getGrCustomFun() const { return grCustomFun; } 
			inline const GeneticElement* getBaseGrRandomness() const { return baseGrRandomness; }
		  //options
			inline bool getBNegativeGrowthAllowed() const { return bNegativeGrowthAllowed; }
			inline bool getBGrowthCorrection() const { return bGrowthCorrection; }
			inline TReal getDeathThreshold() const { return deathThreshold; }
		  //dists
			inline DistributionType* getDistribution( DistIdx idx ) const { return distributions[idx].get(); }
		  //precomputed
			inline TReal getPrecompCapsVol() const { return precompCapsVol; }

		//---set
			inline void setDivisionGate( const GeneticElement* xDivisionGate ) { divisionGate = xDivisionGate; } 
			inline void setDeathGate( const GeneticElement* xDeathGate ) { deathGate = xDeathGate; } 
			inline void setGrFunction( const GeneticElement* xGrFunction ) { grCustomFun = xGrFunction; } 
			void setBaseGrRandomness( const GeneticElement* xBaseGrRandomness );
			inline void setAll( const GeneticElement* xDivisionGate, const GeneticElement* xDeathGate, const GeneticElement* xGrFunction, const GeneticElement* xBaseGrRandomness )
			{ setDivisionGate( xDivisionGate ); setDeathGate( xDeathGate ); setGrFunction( xGrFunction ); setBaseGrRandomness( xBaseGrRandomness ); }

        //---API
		  //precompute
			void loadDirectElemLink() override;
			void precompute() override { GeneticElement::precompute(); calculateCapsVol(); calculateCilinderVolFactor(); calculateCapsArea(); calculateCilinderAreaFactor(); }
			void makeDistributions( ut::SeedGenerator* seedGen, const DistributionsDataDS& distributionsData );
		  //run
			inline TReal totalLength2length( TReal length ) const { return length - DF_DIAMETER; } //totalLength = whole, including the tips; length = only the cilider (the part that changes)
			inline TReal length2vol( TReal length ) const { return precompCapsVol + precompCilinderVolFactor * length; }
			inline TReal totalLength2vol( TReal length ) const { return length2vol( totalLength2length( length ) ); }
			inline TReal vol2length( TReal vol ) const { return ut::fitL( vol - precompCapsVol ) / precompCilinderVolFactor; }
			inline TReal length2totallength( TReal length ) const { return length + DF_DIAMETER; }
			inline TReal vol2totalLength( TReal vol ) const { return length2totallength( vol2length( vol ) ); }

			inline TReal dlength2dvol( TReal dlength ) const { return precompCilinderVolFactor * dlength; } //only the cilinder (the part that changes) counts
			inline TReal dvol2dlength( TReal dvol ) const { return dvol / precompCilinderVolFactor; }

			inline TReal length2area( TReal length ) const { return precompCapsArea + precompCilinderAreaFactor * length; }
			inline TReal area2length( TReal area ) const { return ( area - precompCapsArea ) / precompCilinderAreaFactor; }

			inline TReal getMinimunVol() const { return precompCapsVol; } //a sphere
			inline TReal getMinimunDivisionVol() const { return precompCapsVol * 2.0; } 

		  //sampling
			inline TReal sampleGrowthRate( TReal rawRndVal, TReal stepSize ) const { return bGrowthCorrection ? correctGrowthRate( justSampleGrowthRate( rawRndVal ), stepSize ) : justSampleGrowthRate( rawRndVal ); }
			inline TReal justSampleGrowthRate( TReal rawRndVal ) const { return bNegativeGrowthAllowed ? baseGrScaler->scale( rawRndVal ) : ut::fitL( baseGrScaler->scale( rawRndVal ) ); } //sample without correction
			inline TReal sampleIniVol() const { return ut::fitL( sampleDistribution( DistIdx::INI_VOL ), getMinimunVol() ); } 
			inline TReal sampleDivisionVol() const { return ut::fitL( sampleDistribution( DistIdx::DIVISION_VOL ), getMinimunDivisionVol() ); }  
			inline TReal sampleDivisionFraction() const { TReal raw = ut::fit( sampleDistribution( DistIdx::DIVISION_FRACTION ) ); return daughterDist.sample() ? 1.0 - raw : raw; } //in [0,1], with random assigment to a daughter cell
			inline TReal sampleDistribution( DistIdx idx ) const { return distributions[idx]->sample(); }

			
		private:
		  //resources
			const GeneticElement* divisionGate; //must be true for the division to take place (AND with division volume)
			const GeneticElement* deathGate; //if true AND growth under deathThreshold, the cell dies
			const GeneticElement* grCustomFun; //custom function for the growth rate. Shadows baseGrRandomness and baseGrScaler
			const GeneticElement* baseGrRandomness; //provides raw random value for baseGrScaler
		  //options
			TReal deathThreshold; //if growth rate under AND deathGate, the cell dies
			bool bNegativeGrowthAllowed; //whether to store negative ggrowth rate values or not. If negative, the dVolume is 0, the cells do not decrease
			bool bGrowthCorrection; //whether to compensate for the discretization error in growth rate
		  //dists
			SP<ut::DistributionScaler> baseGrScaler; //produces potentially stochastic growth rate. Shadowed by grCustomFun
			ut::DistributionScaler::ParamsType baseGrParams;

			DistributionsDS distributions; //potentially stochastic ini volume, division volume, division fraction and division rotation
			mutable ut::ConcDistCombi< ut::DiscreteUniformDist > daughterDist; //which daughter cell receives which part of the cell volume on cell division

		  //precomputed
			TReal precompCapsVol; //volume of the two tips of the capsule
			TReal precompCilinderVolFactor; //area of the central cilinder
			TReal precompCapsArea; //currently not used
			TReal precompCilinderAreaFactor; //currently not used

		  //fun
			//precompute volumes
			inline void calculateCapsVol() { precompCapsVol = 4.0/3.0 * ut::PI * std::pow( DF_RADIUS, 3 ); }
			inline void calculateCilinderVolFactor() { precompCilinderVolFactor = ut::PI * std::pow( DF_RADIUS, 2 ); }
			inline void calculateCapsArea() { precompCapsArea = 4.0 * ut::PI * std::pow( DF_RADIUS, 2 ); }
			inline void calculateCilinderAreaFactor() { precompCilinderAreaFactor = 2.0 * ut::PI * DF_RADIUS; }
	};
}

#endif //CG_STRAIN_HPP