#ifndef CG_RANDOMNESS_HPP
#define CG_RANDOMNESS_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionScaler.hpp" //getScalerType()
#include "ut/DistributionCombi.hpp" //distribution, samplingTimeDistribution

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <cmath> //pow */


namespace cg
{
    class GeneticCollection;
    class Randomness : public GeneticElement
    /* Generator of random samples from a probability distribution that follows a re-sampling schedule with custom inertia between consecutive samples */
    {
        REGISTER_CLASS( "Randomness", "rnd", GeneticElement::ElemTypeIdx::RANDOMNESS, false )

        public:
            using DistributionType = ut::ContinuousDistInterface::DistributionType;

        //---static
            inline static const DistributionType DF_DIST_TYPE = DistributionType::NORMAL;
            inline static const ut::ContinuousDist::ParamsType DF_PARAMS = {};
            static constexpr double DF_INHERITANCE = 0.9;
            static constexpr double DF_INERTIA = 0.9;
            static constexpr double DF_SAMPLING_PERIOD = 20.0; //min
            static constexpr bool DF_B_RND_SAMPLING_TIME = true;
            static constexpr bool DF_B_CORRECTION = true;
            
            static ut::DistributionScaler::Type getScalerType( const GeneticElement* rnd );
            static constexpr TReal calculateCorrection( TReal inertia ) { return std::sqrt( ( 1.0 - std::pow( inertia, 2 ) ) / std::pow( 1.0 - inertia, 2 ) ); }


        //---ctor, dtor
            Randomness( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , DistributionType distributionType = DF_DIST_TYPE, const ut::ContinuousDist::ParamsType& params = DF_PARAMS, TReal inheritance = DF_INHERITANCE, TReal inertia = DF_INERTIA
            , TReal samplingPeriod = DF_SAMPLING_PERIOD, bool bRndSamplingTime = DF_B_RND_SAMPLING_TIME, bool bCorrection = DF_B_CORRECTION );

            virtual ~Randomness() = default;

        //---get
           //options
            inline TReal getInheritance() const { return inheritance; }
            inline TReal getInertia() const { return inertia; }
            inline TReal getSamplingPeriod() const { return samplingPeriod; }
            inline bool getBChangeInDivision() const { return inheritance < 1.0; }
            inline bool getBSpontaneousChange() const { return bResample; }
            inline bool getBRndSamplingTime() const { return bRndSamplingTime; }
          //--
            inline bool getBCorrection() const { return bCorrection; }
            inline TReal getInheritanceCorrection() const { return inheritanceCorrection; }
            inline TReal getInertiaCorrection() const { return inertiaCorrection; }
          //dists
            inline ut::ContinuousDistCombi& getDistribution() const { return distribution; }
            inline ut::DistributionScaler::Type getScalerType() const { return distribution.getScalerType(); }

        //---API
          //precompute
            void loadDirectElemLink() override { this->addLink( this, LinkDirection::FORWARD ); }
          //run
            inline TReal sampleWithInheritance( TReal previousVal ) const { return ut::weightedSum( previousVal, sampleOnDivision(), inheritance ); } //called on cell division
            inline TReal sampleWithInertia( TReal previousVal ) const { return ut::weightedSum( previousVal, sampleSpontaneous(), inertia ); } //called on spontaneous re-sampling
            inline TReal sampleSimple() const { return distribution.sample(); } //without correction
            inline TReal sampleSamplingTime() const { return bRndSamplingTime ? sampleSamplingTimeRnd() : samplingPeriod; } //called both for deterministic and stochastic re-sampling times
            

        private:
          //options
            TReal inheritance; //on cell division
            TReal inertia; //on optional spontaneous re-sampling
            TReal samplingPeriod; //in min
            bool bResample; //whether spontaneous re-sampling 
            bool bRndSamplingTime; //whether exponential (waiting time) between re-sampling events or deterministic
          //--
            bool bCorrection; //whether to use constant stddev correction (only for normal distributions)
            TReal inheritanceCorrection; //used on cell division if bCorrection
            TReal inertiaCorrection; //used on spontaneous re-sampling if bCorrection
          //dists
            mutable ut::ContinuousDistCombi distribution;
            mutable ut::ConcDistCombi<ut::ExponentialDist> samplingTimeDistribution;

          //fun
            inline TReal sampleOnDivision() const { return bCorrection ? sampleCorrectedOnDivision() : sampleSimple(); }
            inline TReal sampleSpontaneous() const { return bCorrection ? sampleCorrectedSpontaneous() : sampleSimple(); }
            inline TReal sampleCorrectedOnDivision() const { return distribution.sample() * inheritanceCorrection; }
            inline TReal sampleCorrectedSpontaneous() const { return distribution.sample() * inertiaCorrection; }
            inline TReal sampleSamplingTimeRnd() const { return samplingTimeDistribution.sample(); }
    };
}

#endif //CG_RANDOMNESS_HPP
