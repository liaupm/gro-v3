#ifndef CG_COPYPARTITION_HPP
#define CG_COPYPARTITION_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp"

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <algorithm> //max */


namespace cg
{
    class GeneticCollection;
    class QMarker; //eclipseMarker
	class PartitionSystem : public GeneticElement
	/* DNA component of QPlasmids that rules the split of copies to daughter cells on cell division. 
	The split can be regulated (exact or somehow noisy) or random (binomial). A gate is used to select which of the two to use */
	{
		REGISTER_CLASS( "PartitionSystem", "part", GeneticElement::ElemTypeIdx::PARTITION_SYSTEM, false )

		public:
			inline static const GeneticElement* DF_FRACTION_FUN = nullptr;
			inline static const GeneticElement* DF_FRACTION_RND = nullptr;
			static constexpr TReal DF_FRACTION_MEAN = 0.5;
			static constexpr TReal DF_FRACTION_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_FRACTION_PARAMS = { DF_FRACTION_MEAN, DF_FRACTION_VAR };
			inline static const ParamDataType DF_FRACTION_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_FRACTION_PARAMS ); 

			inline static const GeneticElement* DF_P_FUN = nullptr;
			inline static const GeneticElement* DF_P_RND = nullptr;
			static constexpr TReal DF_P_MEAN = 0.5;
			static constexpr TReal DF_P_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_P_PARAMS = { DF_P_MEAN, DF_P_VAR };
			inline static const ParamDataType DF_P_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_P_PARAMS ); 

			static constexpr bool DF_B_VOLUME_SCALING = true;

		
		//---ctor, dtor
			PartitionSystem( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* gate, const QMarker* eclipseMarker, const ParamDataType& fractionData = DF_FRACTION_DATA, const ParamDataType& pData = DF_P_DATA
			, bool bVolumeScaling = DF_B_VOLUME_SCALING );

			virtual ~PartitionSystem() = default;

		//---get
			inline const GeneticElement* getGate() const { return gate; }
			inline const QMarker* getEclipseMarker() const { return eclipseMarker; }
			inline const GeneticElement* getFractionFunction() const { return fractionFunction; }
			inline const GeneticElement* getPFunction() const { return pFunction; }
			inline const GeneticElement* getFractionRandomness() const { return fractionRandomness; }
			inline const GeneticElement* getPRandomness() const { return pRandomness; }
			inline const GeneticElement* getFractionMarker() const { return fractionMarker; }
			inline const GeneticElement* getPMarker() const { return pMarker; }


			inline bool getBVolumeScaling() const { return bVolumeScaling; } 
			inline ut::DistributionCombiBase<>::RandomEngineType& getShuffleRndEngine() const { return shuffleRnEngine; }

		//---set
			void setAll( const GeneticElement* xGate, const GeneticElement* xFractionFunction, const GeneticElement* xPFunction, const GeneticElement* xFractionRandomness, const GeneticElement* xPRandomness );
			inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
			inline void setFractionFunction( const GeneticElement* xFractionFunction ) { fractionFunction = xFractionFunction; }
			inline void setPFunction( const GeneticElement* xPFunction ) { pFunction = xPFunction; }
			void setFractionRandomness( const GeneticElement* xFractionRandomness );
			void setPRandomness( const GeneticElement* xPRandomness );
			
		//---API
		  //run, sampling
			uint sampleExact( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const; //perfect split. Returns the copies of one of the daughter cells
			uint sampleRandom( TReal copyNum, TReal volFraction, TReal funVal, TReal rawRndVal ) const; //random binomial split. Returns the copies of one of the daughter cells

			inline TReal sampleExactFraction( TReal rawRndVal ) const { return ut::fit( fractionScaler->scale( rawRndVal ) ); } //as a fraction, in [0,1]
			inline TReal sampleP( TReal rawRndVal ) const { return ut::fit( pScaler->scale( rawRndVal ) ); } //in [0,1]
			inline bool sampleDaughter() const { return daughterDist.sample(); }

			
		private:
		  //resources
			const GeneticElement* gate; //exact split if true, binomial if false
			const QMarker* eclipseMarker; //exposes the number of plasmids with this element in eclipse period, to consider them too in the split
			const GeneticElement* fractionFunction; //optional custom function for fraction
			const GeneticElement* pFunction; //optional custom function for p param
			const GeneticElement* fractionRandomness; //raw random values for fractionScaler
			const GeneticElement* pRandomness; //raw random values for pScaler
			const GeneticElement* fractionMarker;
			const GeneticElement* pMarker;
		  //options
			bool bVolumeScaling; //whether the volume ratio between the daughter cells is considered 
		  //dists
			SP<ut::DistributionScaler> fractionScaler; //to get stochastic exact fraction values
			ut::DistributionScaler::ParamsType fractionParams;
			SP<ut::DistributionScaler> pScaler; //to get stochastic values for the p param of the binomial
			ut::DistributionScaler::ParamsType pParams;

			mutable ut::DiscreteDistCombi< ut::BinomialDist > randomDist; //binomial distribution for random split, uses param from pScaler
			mutable ut::ConcDistCombi< ut::DiscreteUniformDist > daughterDist; //to select which daughter cell receives which plasmids
			mutable ut::DistributionCombiBase<>::RandomEngineType shuffleRnEngine; //to aplit the concrete copies of the plasmid	
	};
}

#endif //CG_COPYPARTITION_HPP