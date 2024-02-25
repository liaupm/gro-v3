#ifndef CG_PILUS_HPP
#define CG_PILUS_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //conjugationOrNotDist, neighbourSelectionUniDist, neighbourSelectionDist, oriTSelectionDist
#include "ut/DistributionScaler.hpp" //ut::DistributionScaler::ParamsType DF_MAX_RATE_PARAM

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <memory> //SP<> maxRateScaler */


namespace cg
{
    class GeneticCollection;
    class QMarker; //neighbourMarker
    class EventMarker; //donor and recipient markers

	class Pilus : public GeneticElement
	/* Transmission system for bacterial conjugation, such as T4TS */
	{
		REGISTER_CLASS( "Pilus", "pil", GeneticElement::ElemTypeIdx::PILUS, false )

		public:
			inline static const GeneticElement* DF_MAX_RATE_RND = nullptr;
			inline static const GeneticElement* DF_NEIGHBOURS_W_RND = nullptr;
			inline static const QMarker* DF_RATE_MARKER = nullptr;

			static constexpr double DF_MAX_RATE_MEAN = 0.001;
			static constexpr double DF_MAX_RATE_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_MAX_RATE_PARAMS = { DF_MAX_RATE_MEAN, DF_MAX_RATE_VAR };
			inline static const ParamDataType DF_MAX_RATE_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_MAX_RATE_PARAMS ); 

			static constexpr double DF_NEIGHBOURS_W_MEAN = 0.75;
			static constexpr double DF_NEIGHBOURS_W_VAR = 0.0;
			inline static const ut::DistributionScaler::ParamsType DF_NEIGHBOURS_W_PARAMS = { DF_NEIGHBOURS_W_MEAN, DF_NEIGHBOURS_W_VAR };
			inline static const ParamDataType DF_NEIGHBOURS_W_DATA = ParamDataType( nullptr, nullptr, nullptr, DF_NEIGHBOURS_W_PARAMS ); 

			static constexpr TReal DF_DISTANCE_THRESHOLD = 100.0;
			static constexpr bool DF_BCOPYDEPENDENT = false;
			static constexpr bool DF_AUTO_VOL_SCALE = true;


		//---ctor, dtor
			Pilus( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* donorGate, const GeneticElement* recipientGate, const GeneticElement* recipientFunction, const QMarker* neighbourMarker
			, const ParamDataType& maxRateData = DF_MAX_RATE_DATA, const ParamDataType& neighboursWData = DF_NEIGHBOURS_W_DATA, const QMarker* rateMarker = DF_RATE_MARKER, bool bAutoVolScale = DF_AUTO_VOL_SCALE );

			virtual ~Pilus() = default;

		//---get
		  //resources
			inline const GeneticElement* getDonorGate() const { return donorGate; }
			inline const GeneticElement* getRecipientGate() const { return recipientGate; }
			inline const GeneticElement* getDonorFunction() const { return donorFunction; }
			inline const GeneticElement* getRecipientFunction() const { return recipientFunction; }
			inline bool getHasRecipientFunction() const { return recipientFunction; }
			inline const GeneticElement* getMaxRateRnd() const { return maxRateRnd; }
			inline const GeneticElement* getNeighboursWRnd() const { return neighboursWRnd; }
			inline const GeneticElement* getMaxRateMarker() const { return maxRateMarker; }
			inline const GeneticElement* getNeighboursWMarker() const { return neighboursWMarker; }
			inline const QMarker* getRateMarker() const { return rateMarker; }

		  //markers
			inline const QMarker* getNeighbourMarker() const { return neighbourMarker; }
			inline bool getHasNeighbourMarker() const { return neighbourMarker; }

		 //options
			//inline TReal getNeighboursW() const { return neighboursW; }
			inline bool getBAutoVolScale() const { return bAutoVolScale; }
			inline bool getBCopyDependent() const { return bCopyDependent; }
			inline bool getIsCopyDependent() const { return bCopyDependent; }
			inline bool getBDistanceCriterium() const { return bDistanceCriterium; }
			inline bool getHasDistanceCriterium() const { return bDistanceCriterium; }

		//---set
			void setAll( const GeneticElement* xDonorGate, const GeneticElement* xRecipientGate, const GeneticElement* xDonorFunction, const GeneticElement* xRecipientFunction, const GeneticElement* xMaxRateRnd, const GeneticElement* xNeighboursWRnd );
			inline void setDonorGate( const GeneticElement* xDonorGate ) { donorGate = xDonorGate; }
			inline void setRecipientGate( const GeneticElement* xRecipientGate ) { recipientGate = xRecipientGate; }
			inline void setDonorFunction( const GeneticElement* xDonorFunction ) { donorFunction = xDonorFunction; }
			inline void setRecipientFunction( const GeneticElement* xRecipientFunction ) { recipientFunction = xRecipientFunction; }
			void setMaxRateRnd( const GeneticElement* xMaxRateRnd );
			void setNeighboursWRnd( const GeneticElement* xNeighboursWRnd );

		//---API
		  //precompute
            void loadDirectElemLink() override;
            void precompute() override;
          //run
            inline TReal calculateConjugationProb( TReal maxRate, size_t neighbourNum, TReal rawRndVal ) const { return maxRate * ( 1.0 - std::exp( - sampleNeighboursW( rawRndVal ) * neighbourNum ) ); } //for homogeneous neighbours
            inline TReal calculateConjugationProb( TReal maxRate, const ut::DiscreteDist::ParamsType& params, TReal rawRndVal ) const 
            { TReal total = std::accumulate( params.begin(), params.end(), 0.0 ); return maxRate * ( 1.0 - std::exp( - sampleNeighboursW( rawRndVal ) * total ) ); } //for neighbours with different probs
          //sampling
            inline TReal sampleMaxRate( TReal rawRndVal ) const { return ut::fitL( maxRateScaler->scale( rawRndVal ) ); } //non-negative
			inline TReal sampleNeighboursW( TReal rawRndVal ) const { return ut::fitL( neighboursWScaler->scale( rawRndVal ) ); } //non-negative

            inline bool sampleConjugationOrNot( TReal maxRate, size_t neighbourNum, TReal rawRndVal ) const { return conjugationOrNotDist.sample() < calculateConjugationProb( maxRate, neighbourNum, rawRndVal ); } //for homogeneous neighbours
            inline bool sampleConjugationOrNot( TReal maxRate, const ut::DiscreteDist::ParamsType& params, TReal rawRndVal ) const { return conjugationOrNotDist.sample() < calculateConjugationProb( maxRate, params, rawRndVal ); } //for neighbours with different probs
            inline bool sampleConjugationOrNot( TReal conjugationProb ) const { return conjugationOrNotDist.sample() < conjugationProb; }

            inline size_t sampleNeighbour( size_t neighbourNum ) const { return neighbourSelectionUniDist.sample( static_cast< typename ut::DiscreteUniformDist::IntParamType >( neighbourNum ) ); } //homogeneous neighbours
            inline size_t sampleNeighbour( const ut::DiscreteDist::ParamsType& params ) const { return neighbourSelectionDist.sample( params ); } //neighbours with different probs
            inline size_t sampleOriT( const ut::DiscreteDist::ParamsType& params ) const { return oriTSelectionDist.sample( params ); }

            
		private:
		  //resources
			const GeneticElement* donorGate; //must be true for the donor
			const GeneticElement* recipientGate; //must be true for the recipient. Surface exclusion (true entry exclusion at OriT level)
			const GeneticElement* donorFunction; //custom function for the base conjugation rate that only depends on the donor. May then be scaled by bAutoVolScale and the neighbours (potential recipients)
			const GeneticElement* recipientFunction; //custom function for the weight or probability of selecting a particular neighbour as a recipient
			const QMarker* neighbourMarker; // !!
			const QMarker* rateMarker;
			const GeneticElement* maxRateRnd; //raw random values for maxRateScaler
			const GeneticElement* neighboursWRnd; //raw random values for neighboursWScaler
			const GeneticElement* maxRateMarker;
			const GeneticElement* neighboursWMarker;
		  //options
			bool bAutoVolScale; //whether to represent the conjugation rate as relative and multiply by the cell volume to get the absolute rate
			bool bCopyDependent; //whether the probability of selecting a particular compatible OriT depends on their copy number. Must be true for all the compatible OriTs to be set to true
			bool bDistanceCriterium; //whether the distance between donor and neighbour is used in the calculations
		  //dists
			SP<ut::DistributionScaler> maxRateScaler; //to sample potentially stochasttic maxRate
			ut::DistributionScaler::ParamsType maxRateParams;
			SP<ut::DistributionScaler> neighboursWScaler; //to sample potentially stochasttic neighboursW
			ut::DistributionScaler::ParamsType neighboursWParams;

			mutable ut::ConcDistCombi< ut::UniformDist > conjugationOrNotDist; // to sample whether conjugation takes place each time step
			mutable ut::DiscreteDistCombi< ut::DiscreteUniformDist > neighbourSelectionUniDist; //to select a recipient when they are homogeneous (same prob)
			mutable ut::DiscreteDistCombi< ut::DiscreteDist > neighbourSelectionDist; //to select a recipient when each hae a different probability
			mutable ut::DiscreteDistCombi< ut::DiscreteDist > oriTSelectionDist; //to select one OriT from the available compatible ones
	};
}

#endif //CG_PILUS_HPP