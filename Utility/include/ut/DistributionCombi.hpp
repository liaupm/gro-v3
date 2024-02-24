#ifndef UT_DISTRIBUTION_COMBI_HPP
#define UT_DISTRIBUTION_COMBI_HPP

#include "ut/defines.hpp"
#include "ut/DistributionInterface.hpp" //DistributionInterfaceSP distribution
#include "ut/DistributionScaler.hpp"

/*PRECOMPILED
#include <vector> //distributionParams
#include <memory> //make_shared RandomEngine
#include <random> //std::minstd_rand */


namespace ut
{
    template< typename TRandomEngine = std::minstd_rand >
    class DistributionCombiBase
    /* combination of a (STL) distribution and a STL RandomEngine
    virtual base */
    {
        public:
            using RandomEngineType = TRandomEngine;
            static constexpr uint DF_SEED = 1;

        //---ctor, dtor
            virtual ~DistributionCombiBase() = default;

        //---get
            inline SP<RandomEngineType> getRandomEngineSP() { return randomEngine; }
            inline RandomEngineType* getRandomEngine() { return randomEngine.get(); }
        
        //---set
            inline void setRandomEngine( SP<RandomEngineType> xRandomEngine ) { randomEngine = xRandomEngine; }
            virtual void setSeed( uint xSeed ) { randomEngine->seed( xSeed );  }


        protected:
            SP<RandomEngineType> randomEngine;

            inline DistributionCombiBase( SP<RandomEngineType> randomEngine ) : randomEngine(randomEngine) {;}
            explicit inline DistributionCombiBase( uint seed = DF_SEED ) : randomEngine( std::make_shared<RandomEngineType>( seed ) ) {;}
    };


    template< typename TRandomEngine = std::minstd_rand >
    class ContinuousDistributionCombi : public DistributionCombiBase<TRandomEngine>
    {
        public:
            using RandomEngineType = TRandomEngine;
            using InterfaceType = ContinuousDistributionInterface< RandomEngineType >;
            using DistributionType = typename InterfaceType::DistributionType;
            using RealNumType = typename InterfaceType::RealNumberType;
            using ParamsType = typename InterfaceType::ParamsType;
            using ParamType = typename InterfaceType::ParamType;
            using DistributionCombiBase<TRandomEngine>::DF_SEED;


        //---ctor, dtor
            inline ContinuousDistributionCombi( SP<RandomEngineType> randomEngine, SP<InterfaceType> distribution ) : DistributionCombiBase<TRandomEngine>( randomEngine ), distribution( distribution ) {;}
            
            template< typename TParam >
            explicit inline ContinuousDistributionCombi( DistributionType distributionType = InterfaceType::DF_DISTRIBUTION_TYPE, const std::vector<TParam>& distributionParams = {}, uint seed = DF_SEED )
            : DistributionCombiBase<TRandomEngine>( seed )
            , distribution ( InterfaceType::FactoryType::create( distributionType, distributionParams ) ) {;}

            explicit inline ContinuousDistributionCombi( const ContinuousDistData& distData, uint seed = DF_SEED )
            : DistributionCombiBase<TRandomEngine>( seed )
            , distribution( InterfaceType::FactoryType::create( distData.type, distData.params ) ) {;}

            virtual ~ContinuousDistributionCombi() = default;

        //---get
            inline SP<InterfaceType> getDistributionSP() { return distribution; }
            inline InterfaceType* getDistribution() { return distribution.get(); }
            inline DistributionScaler::Type getScalerType() const { return distribution->getScalerType(); }

        //---set
            inline void setDistribution( SP<InterfaceType> xDistribution ) { distribution = xDistribution; }
            void setSeed( uint xSeed ) override { DistributionCombiBase<TRandomEngine>::setSeed( xSeed ); distribution->reset(); }

        //---API
            inline TReal sample() { return distribution->sample( *this->randomEngine ); }
            template< typename TParams >
            inline TReal sample( TParams xParams ) { return distribution->sample( *this->randomEngine, xParams ); }
            inline TReal operator() () { return this->sample(); }
            inline TReal sampleScaled( TReal lBound, TReal uBound ) { return lBound + ( uBound - lBound ) * distribution->sample( *this->randomEngine ); } 
            inline TReal calculateSymmetric( TReal val ) { return distribution->calculateSymmetric( val ); }


        private:
            SP<InterfaceType> distribution;
    };
    using ContinuousDistCombi = ContinuousDistributionCombi<>;
    using DistributionCombi = ContinuousDistributionCombi<>;
    using DistCombi = ContinuousDistributionCombi<>;


    template< typename TDist, typename TRandomEngine = std::minstd_rand >
    class ConcreteDistributionCombi : public DistributionCombiBase<TRandomEngine>
    {
        public:
            using DistType = TDist;
            using RandomEngineType = TRandomEngine;
            using ParamsType = typename DistType::ParamsType;
            using ParamType = typename DistType::ParamType;
            using DistributionCombiBase<TRandomEngine>::DF_SEED;
        

        //---ctor, dtor
            explicit inline ConcreteDistributionCombi( const ParamsType& params = {}, uint seed = DF_SEED )
            : DistributionCombiBase<TRandomEngine>( seed ), dist( params ) {;}

            virtual ~ConcreteDistributionCombi() = default;

        //---get
            inline DistType& getDistribution() { return dist; }

        //---set
            void setSeed( uint xSeed ) override { DistributionCombiBase<TRandomEngine>::setSeed( xSeed ); dist.reset(); }

        //---API
            inline TReal sample() { return dist.sample( *this->randomEngine ); }
            inline typename DistType::ReturnType sample( ParamType xParam ) { return dist.sample( *this->randomEngine, xParam ); } 
            inline typename DistType::ReturnType sample( ParamsType xParams ) { return dist.sample( *this->randomEngine, xParams ); }
            inline TReal operator() () { return this->sample(); }


        private:
            DistType dist;
    };
    template< typename TDist, typename TRandomEngine = std::minstd_rand >
    using ConcDistCombi = ConcreteDistributionCombi< TDist, TRandomEngine >;


    template< typename TDist, typename TRandomEngine = std::minstd_rand >
    class DiscreteDistributionCombi : public DistributionCombiBase<TRandomEngine>
    {
        public:
            using DistType = TDist;
            using RandomEngineType = TRandomEngine;
            using ParamsType = typename DistType::ParamsType;
            using ParamType = typename DistType::ParamType;
            using RealParamsType = typename DistType::RealParamsType;
            using RealParamType = typename DistType::RealParamType;
            using IntParamsType = typename DistType::IntParamsType;
            using IntParamType = typename DistType::IntParamType;
            using DistributionCombiBase<TRandomEngine>::DF_SEED;
        

        //---ctor, dtor
            explicit inline DiscreteDistributionCombi( const RealParamsType& realParams = {}, const IntParamsType& intParams = {}, uint seed = DF_SEED )
            : DistributionCombiBase<TRandomEngine>( seed )
            , dist( realParams, intParams ) {;}

            virtual ~DiscreteDistributionCombi() = default;

        //---get
            inline DistType& getDistribution() { return dist; }

        //---set
            void setSeed( uint xSeed ) override { DistributionCombiBase<TRandomEngine>::setSeed( xSeed ); dist.reset(); }

        //---API
            inline TReal sample() { return dist.sample( *this->randomEngine ); }
            inline IntParamType sample( RealParamType xParam ) { return dist.sample( *this->randomEngine, xParam ); } 
            inline IntParamType sample( IntParamType xParam ) { return dist.sample( *this->randomEngine, xParam ); } 
            inline IntParamType sample( RealParamType xRealParam, IntParamType xIntParam ) { return dist.sample( *this->randomEngine, xRealParam, xIntParam ); } 

            inline IntParamType sample( const RealParamsType& xParams ) { return dist.sample( *this->randomEngine, xParams ); }
            inline IntParamType sample( const IntParamsType& xParams ) { return dist.sample( *this->randomEngine, xParams ); }
            inline IntParamType sample( const RealParamsType& xRealParams, const IntParamsType& xIntParams ) { return dist.sample( *this->randomEngine, xRealParams, xIntParams ); }
            inline TReal operator() () { return this->sample(); }


        private:
            DistType dist;
    };
    template< typename TDist, typename TRandomEngine = std::minstd_rand >
    using DiscreteDistCombi = DiscreteDistributionCombi< TDist, TRandomEngine >;
}

#endif //UT_DISTRIBUTION_COMBI_HPP