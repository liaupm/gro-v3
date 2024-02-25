#ifndef UT_DISTRIBUTION_INTERFACE_HPP
#define UT_DISTRIBUTION_INTERFACE_HPP

#include "defines.hpp"
#include "ut/Metadata.hpp" //REGISTER_SUBTYPE
#include "ut/Factory.hpp" //ContinuousDistFactory, DiscreteDistFactory
#include "ut/DistributionScaler.hpp" //DistributionScaler::Type

/*PRECOMPILED
#include <vector> //params
#include <map> //SCALER_2_DIST
#include <random> //Base std distributions, RandomEngine in all std distributions, when sampling
#include <math.h> //log in Exponential dist
#include <numeric> //std::accumulate in DiscreteDistribution::createAccumulatedProbs */


namespace ut
{
    template< typename TRandomEngine, typename TRealNumber >
    class ContinuousDistributionFactory;

    template< typename TRandomEngine, typename TIntNumber, typename TRealNumber >
    class DiscreteDistributionFactory;


///////////////////////////////////////////////////////////* CONTINUOUS DISTRIBUTIONS *////////////////////////////////////////////////////////////////////////
	///ABSTRACT common interface for continuous distributions. Classes derived from STL distributions mustn't add var members cause STL destructors are not virtual
    class ContinuousDistributionInterfaceBase
    {
	    public: 
	        enum DistributionType : int
	        {
	            UNIFORM, NORMAL, EXPONENTIAL, COUNT
	        };
	        using DistType = DistributionType;
    };

    template< typename TRandomEngine = RandomEngine, typename TRealNumber = TReal >
    class ContinuousDistributionInterface : public ContinuousDistributionInterfaceBase
    {
    	REGISTER_CLASS( "ContinuousDistributionInterface", "cdist", DistributionType::COUNT, false )

        public: 
            using RandomEngineType = TRandomEngine;
            using SubtypeIdx = DistributionType;
            using RealNumberType = TRealNumber;
            using ReturnType = RealNumberType;
            using ParamType = RealNumberType; 
            using ParamsType = std::vector<ParamType>; 
            using FactoryType = ContinuousDistributionFactory< RandomEngineType, RealNumberType >;
            
            static constexpr DistributionType DF_DISTRIBUTION_TYPE = DistributionType::NORMAL;

            inline static const std::map< DistributionScaler::Type, DistributionType > SCALER_2_DIST = { { DistributionScaler::Type::UNIFORM, DistributionType::UNIFORM }
                                                                                                        , { DistributionScaler::Type::NORMAL, DistributionType::NORMAL } };

        //---ctor, dtor
            virtual ~ContinuousDistributionInterface() = default;

        //---get
            inline DistributionType getDistributionType() const { return distributionType; }
            inline const ParamsType& getParams() const { return params; }
            inline ParamType getParam( size_t idx = 0 ) const { return params[ idx ]; }

            virtual DistributionScaler::Type getScalerType() const { return DistributionScaler::Type::COUNT; }

        //---set
            inline void setParams( const ParamsType& xParams ) { params = xParams; }
            inline void setParam( ParamType xParam, size_t idx = 0 ) { params[ idx ] = xParam; }

        //---API
            virtual void updateParams( const ParamsType& xParams ) = 0;
            virtual void updateParam( ParamType xParam ) = 0;
            inline bool checkParams( const ParamsType& xParams ) const { return params == xParams; }
            inline bool checkParam( ParamType xParam, size_t idx = 0 ) const { return params[idx] == xParam; }

            virtual TRealNumber sample( RandomEngineType& randomEngine ) = 0; //sample once the distribution with the current params
            inline TRealNumber sample( RandomEngineType& randomEngine, ParamType xParam ) { updateParam( xParam ); return sample( randomEngine ); } //changing one param
            inline TRealNumber sample( RandomEngineType& randomEngine, ParamsType xParams ) { updateParams( xParams ); return sample( randomEngine ); } //changing all the params
            inline TRealNumber operator() ( RandomEngineType& randomEngine ) { return this->sample( randomEngine ); }
            
            virtual void reset() = 0; 
            virtual TReal calculateSymmetric( TReal val ) = 0; 


        protected:
            DistributionType distributionType;
            ParamsType params;

            ContinuousDistributionInterface( DistributionType distributionType, ParamsType params ) : distributionType( distributionType ), params( params ) {;}
    };
    using ContinuousDistInterface = ContinuousDistributionInterface<>;
    using ContinuousDist = ContinuousDistInterface;

    struct ContinuousDistData
    /* used to create eny ContinuousDistribution from it
    passed as parameter to constructors */
    {
        static constexpr ContinuousDist::DistributionType DF_DIST_TYPE = ContinuousDist::DF_DISTRIBUTION_TYPE;
        inline static const ContinuousDist::ParamsType DF_PARAMS = {}; //ÑAPA

        ContinuousDist::DistributionType type;
        ContinuousDist::ParamsType params;

        ContinuousDistData() 
        : type( ContinuousDist::DF_DISTRIBUTION_TYPE ), params( DF_PARAMS ) {;}

        ContinuousDistData( ContinuousDist::DistributionType type, const ContinuousDist::ParamsType& params ) 
        : type( type ), params( params ) {;}
    };



////////////////////////////////////////////////////////// CONCRETE CONTINUOUS DISTRIBUTIONS ////////////////////////////////////////////////////////////////////////
    template< typename TRandomEngine = RandomEngine, typename TRealNumber = TReal >
    class UniformDistribution : public ContinuousDistributionInterface<TRandomEngine, TRealNumber>
    {

        REGISTER_SUBTYPE( "uniform", "uni", ContinuousDistributionInterface<TRealNumber>::SubtypeIdx::UNIFORM )
        
        public:
            enum ParamsIdx : size_t
            {
                LBOUND, UBOUND, COUNT
            };

            using UnderType = std::uniform_real_distribution<TRealNumber>;
            using InterfaceType = ContinuousDistributionInterface< TRandomEngine, TRealNumber >;
            using typename InterfaceType::RandomEngineType; using typename InterfaceType::ParamsType; using typename InterfaceType::ParamType; using InterfaceType::sample;

            inline static const ParamsType DF_PARAMS = { 0.0, 1.0 };
            static constexpr DistributionScaler::Type SCALER_TYPE = DistributionScaler::Type::UNIFORM;


        //---ctor, dtor
        	explicit UniformDistribution( const ParamsType& params = DF_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::UNIFORM, params.size() > 1 ? params : DF_PARAMS )
            , dist( params.size() > 1 ? params[ ParamsIdx::LBOUND ] : DF_PARAMS[ ParamsIdx::LBOUND ],  params.size() > ParamsIdx::UBOUND ? params[ ParamsIdx::UBOUND ] : DF_PARAMS[ ParamsIdx::UBOUND ] ) {;}

            virtual ~UniformDistribution() = default;

        //---get
            DistributionScaler::Type getScalerType() const override { return SCALER_TYPE; }

        //---API
            void updateParams( const ParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( xParams[ ParamsIdx::LBOUND ], xParams[ ParamsIdx::UBOUND ] ); } }
            void updateParam( ParamType xParam ) override { if( ! InterfaceType::checkParam( xParam, ParamsIdx::UBOUND ) ) { InterfaceType::setParam( xParam, ParamsIdx::UBOUND ); dist = UnderType( InterfaceType::getParam( ParamsIdx::LBOUND ), xParam ); } }
            
            TRealNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); }
            
            void reset() override { dist.reset(); }
            TReal calculateSymmetric( TReal val ) override { return InterfaceType::getParam( ParamsIdx::LBOUND ) + InterfaceType::getParam( ParamsIdx::UBOUND ) - val; }


        private:
            UnderType dist;
    };
    using UniformDist = UniformDistribution<>;


    template< typename TRandomEngine = RandomEngine, typename TRealNumber = TReal >
    class NormalDistribution : public ContinuousDistributionInterface<TRandomEngine, TRealNumber>
    {
        REGISTER_SUBTYPE( "normal", "norm", ContinuousDistributionInterface<TRealNumber>::SubtypeIdx::NORMAL )

        public:
            enum ParamsIdx : size_t
            {
                MEAN, STDDEV, COUNT
            };

            using UnderType = std::normal_distribution<TRealNumber>;
            using InterfaceType = ContinuousDistributionInterface<TRandomEngine, TRealNumber>;
            using typename InterfaceType::RandomEngineType; using typename InterfaceType::ParamsType; using typename InterfaceType::ParamType; using InterfaceType::sample;

            inline static const ParamsType DF_PARAMS = { 0.0, 1.0 };
            static constexpr DistributionScaler::Type SCALER_TYPE = DistributionScaler::Type::NORMAL;


        //---ctor, dtor
            explicit NormalDistribution( const ParamsType& params = DF_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::NORMAL, params.size() > 1 ? params : DF_PARAMS )
            , dist( params.size() > 1 ? params[ ParamsIdx::MEAN ] : DF_PARAMS[ ParamsIdx::MEAN ],  params.size() > 1 ? params[ ParamsIdx::STDDEV ] : DF_PARAMS[ ParamsIdx::STDDEV ] ) {;}

            virtual ~NormalDistribution() = default;

        //---get
            DistributionScaler::Type getScalerType() const override { return SCALER_TYPE; }

        //---API
            void updateParams( const ParamsType& xParams ) override 
            { 
                if( ! InterfaceType::checkParams( xParams ) ) 
                { 
                    InterfaceType::setParams( xParams ); 
                    dist = UnderType( xParams[ ParamsIdx::MEAN ], xParams[ ParamsIdx::STDDEV ] ); 
                } 
            }

            void updateParam( ParamType xParam ) override
            { 
                if( ! InterfaceType::checkParam( xParam, ParamsIdx::MEAN ) ) 
                { 
                    InterfaceType::setParam( xParam, ParamsIdx::MEAN ); 
                    dist = UnderType( xParam, InterfaceType::getParam( ParamsIdx::STDDEV ) ); 
                } 
            }

            TRealNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); }

            void reset() override { dist.reset(); }
            TReal calculateSymmetric( TReal val ) override { return 2.0 * InterfaceType::getParam( ParamsIdx::MEAN ) - val; }

        private:
            UnderType dist;
    };
    using NormalDist = NormalDistribution<>;


    template< typename TRandomEngine = RandomEngine, typename TRealNumber = TReal >
    class ExponentialDistribution : public ContinuousDistributionInterface<TRandomEngine, TRealNumber>
    {
        REGISTER_SUBTYPE( "exponential", "exp", ContinuousDistributionInterface<TRealNumber>::SubtypeIdx::EXPONENTIAL )

        public:
            enum ParamsIdx : size_t
            {
                LAMBDA, COUNT
            };

            using UnderType = std::exponential_distribution<TRealNumber>;
            using InterfaceType = ContinuousDistributionInterface<TRandomEngine, TRealNumber>;
            using typename InterfaceType::RandomEngineType; using typename InterfaceType::ParamsType; using typename InterfaceType::ParamType; using InterfaceType::sample;

            inline static const ParamsType DF_PARAMS = { 1.0 };
            static constexpr DistributionScaler::Type SCALER_TYPE = DistributionScaler::Type::EXPONENTIAL;
       

        //---ctor, dtor
            explicit ExponentialDistribution( const ParamsType& params = DF_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::EXPONENTIAL, params.size() > 0 ? params : DF_PARAMS )
            , dist( params.size() > 0 ? params[ ParamsIdx::LAMBDA ] : DF_PARAMS[ ParamsIdx::LAMBDA ] ) {;}

            virtual ~ExponentialDistribution() = default;

        //---get
            DistributionScaler::Type getScalerType() const override { return SCALER_TYPE; }

        //---API
            void updateParams( const ParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( xParams[ ParamsIdx::LAMBDA ] ); } }
            void updateParam( ParamType xParam ) override { if( ! InterfaceType::checkParam( xParam, ParamsIdx::LAMBDA ) ) { InterfaceType::setParam( xParam, ParamsIdx::LAMBDA ); dist = UnderType( xParam ); } }
            
            TRealNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); }

            void reset() override { dist.reset(); }
            TReal calculateSymmetric( TReal val ) override { return - std::log( 1.0 - std::exp( - InterfaceType::getParam( ParamsIdx::LAMBDA ) * val ) ) / InterfaceType::getParam( ParamsIdx::LAMBDA ); }

        private:
            UnderType dist;
    };
    using ExponentialDist = ExponentialDistribution<>;








///////////////////////////////////////////////////////////* DISCRETE DISTRIBUTIONS *////////////////////////////////////////////////////////////////////////
    ///ABSTRACT common interface for discrete distributions
    class DiscreteDistributionInterfaceBase
    {
	    public: 
	        enum DistributionType : int
	        {
	            UNIFORM, DISCRETE, BINOMIAL, COUNT
	        };
    };

    template< typename TRandomEngine = RandomEngine, typename TIntNumber = uint, typename TRealNumber = TReal >
    class DiscreteDistributionInterface : public DiscreteDistributionInterfaceBase
    {
        public:
            using RandomEngineType = TRandomEngine;
            using SubtypeIdx = DistributionType;
            using IntNumberType = TIntNumber;
            using RealNumberType = TRealNumber;
            using ReturnType = IntNumberType;
            using RealParamType = RealNumberType;
            using RealParamsType = std::vector<RealParamType>; 
            using IntParamType = IntNumberType;
            using IntParamsType = std::vector<IntParamType>; 
            using FactoryType = DiscreteDistributionFactory< RandomEngineType, IntNumberType, RealNumberType >;

            static constexpr DistributionType DF_DISTRIBUTION_TYPE = DistributionType::DISCRETE;


        //---ctor, dtor
            virtual ~DiscreteDistributionInterface() = default;

        //---get
            inline DistributionType getDistributionType() const { return distributionType; }
            inline const RealParamsType& getRealParams() const { return realParams; }
            inline const IntParamsType& getIntParams() const { return intParams; }
            inline RealParamType getRealParam( size_t idx = 0 ) const { return realParams[idx]; }
            inline IntParamType getIntParam( size_t idx = 0 ) const { return intParams[idx]; }

        //---set
            inline void setParams( const RealParamsType& xRealParams ) { realParams = xRealParams; }
            inline void setParam( RealParamType xRealParam, size_t idx = 0 ) { realParams[ idx ] = xRealParam; }
            inline void setParams( const IntParamsType& xIntParams ) { intParams = xIntParams; }
            inline void setParam( IntParamType xIntParam, size_t idx = 0 ) { intParams[ idx ] = xIntParam; }
            inline void setParams( const RealParamsType& xRealParams, const IntParamsType& xIntParams ) { setParams( xRealParams ); setParams( xIntParams ); }
            inline void setParam( RealParamType xRealParam, IntParamType xIntParam, size_t realIdx = 0, size_t intIdx = 0 ) { setParam( xRealParam, realIdx ); setParam( xIntParam, intIdx ); }

        //---API
            virtual void updateParams( const RealParamsType& xParams ) = 0;
            virtual void updateParam( RealParamType xParam ) = 0;
            virtual void updateParams( const IntParamsType& xParams ) = 0;
            virtual void updateParam( IntParamType xParam ) = 0;
            virtual void updateParams( const RealParamsType& xRealParams, const IntParamsType& xIntParams ) = 0;
            virtual void updateParam( RealParamType xRealParam, IntParamType xIntParam ) = 0;

            inline bool checkParams( const RealParamsType& xRealParams ) const { return realParams == xRealParams; }
            inline bool checkParam( RealParamType xRealParam, size_t idx = 0 ) const { return realParams[idx] == xRealParam; }
            inline bool checkParams( const IntParamsType& xIntParams ) const { return intParams == xIntParams; }
            inline bool checkParam( IntParamType xIntParam, size_t idx = 0 ) const { return intParams[idx] == xIntParam; }
            inline bool checkParams( const RealParamsType& xRealParams, const IntParamsType& xIntParams ) const { return checkParams( xRealParams) && checkParams( xIntParams); }
            inline bool checkParam( RealParamType xRealParam, IntParamType xIntParam, size_t realIdx = 0, size_t intIdx = 0  ) const { return checkParam( xRealParam, realIdx ) && checkParam( xIntParam, intIdx ); }

            virtual TIntNumber sample( RandomEngineType& randomEngine ) = 0;
            inline IntNumberType sample( RandomEngineType& randomEngine, const RealParamsType& xParams ) { updateParams( xParams ); return sample( randomEngine ); }
            inline IntNumberType sample( RandomEngineType& randomEngine, RealParamType xParam ) { updateParam( xParam ); return sample( randomEngine ); }
            inline IntNumberType sample( RandomEngineType& randomEngine, const IntParamsType& xParams ) { updateParams( xParams ); return sample( randomEngine ); }
            inline IntNumberType sample( RandomEngineType& randomEngine, IntParamType xParam ) { updateParam( xParam ); return sample( randomEngine ); }
            inline IntNumberType sample( RandomEngineType& randomEngine, const RealParamsType& xRealParams, const IntParamsType& xIntParams ) { updateParams( xRealParams, xIntParams ); return sample( randomEngine ); }
            inline IntNumberType sample( RandomEngineType& randomEngine, RealParamType xRealParam, IntParamType xIntParam ) { updateParam( xRealParam, xIntParam ); return sample( randomEngine ); }
            template< typename TParam >
            inline IntNumberType sample( RandomEngineType& randomEngine, const std::vector<TParam>& xParams ) { updateParams( xParams ); return sample( randomEngine ); } 
            inline IntNumberType operator() ( RandomEngineType& randomEngine ) { return this->sample( randomEngine ); }
            
            virtual void reset() = 0;

 
        protected: 
            DistributionType distributionType;
            RealParamsType realParams;
            IntParamsType intParams;

            DiscreteDistributionInterface( DistributionType distributionType, const RealParamsType& realParams ) 
            : distributionType(distributionType), realParams( realParams ), intParams( {} ) {;}

            DiscreteDistributionInterface( DistributionType distributionType, const IntParamsType& intParams ) 
            : distributionType(distributionType), realParams( {} ), intParams( intParams ) {;}

            DiscreteDistributionInterface( DistributionType distributionType, const RealParamsType& realParams, const IntParamsType& intParams ) 
            : distributionType(distributionType), realParams( realParams ), intParams( intParams ) {;}
    };

    using DiscreteDistInterface = DiscreteDistributionInterface<>;



////////////////////////////////////////////////////////// CONCRETE DISCRETE DISTRIBUTIONS ////////////////////////////////////////////////////////////////////////

    template< typename TRandomEngine = RandomEngine, typename TIntNumber = uint, typename TRealNumber = TReal >
    class DiscreteUniformDistribution : public DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber > //this implmentation guarantees the independence of the samples
    {
        REGISTER_SUBTYPE( "d_uniform", "duni", DiscreteDistributionInterface<>::SubtypeIdx::UNIFORM )

        public:
            using UnderType = std::uniform_int_distribution<TIntNumber>;
            using InterfaceType = DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber >;
            
            using typename InterfaceType::IntNumberType; using typename InterfaceType::RealNumberType;
            using typename InterfaceType::IntParamsType; using typename InterfaceType::IntParamType;
            using typename InterfaceType::RealParamsType; using typename InterfaceType::RealParamType;
            using typename InterfaceType::RandomEngineType; using InterfaceType::sample;
            using ParamsType = IntParamsType;
            using ParamType = IntParamType;

            inline static IntParamType DF_INT_PARAM = 1;
            inline static IntParamsType DF_INT_PARAMS = { 0, 1 };


        //---ctor, dtor
            explicit DiscreteUniformDistribution( const IntParamsType& intParams = DF_INT_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::UNIFORM, intParams.size() > 0 ? intParams : DF_INT_PARAMS )
            , dist( intParams.size() > 0 ? intParams[0] : DF_INT_PARAMS[0], intParams.size() > 1 ? intParams[1] : DF_INT_PARAMS[1] ) {;}

            explicit DiscreteUniformDistribution( const RealParamsType& = {}, const IntParamsType& intParams = DF_INT_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::UNIFORM, intParams.size() > 0 ? intParams : DF_INT_PARAMS )
            , dist( intParams.size() > 0 ? intParams[0] : DF_INT_PARAMS[0], intParams.size() > 1 ? intParams[1] : DF_INT_PARAMS[1] ) {;}

            virtual ~DiscreteUniformDistribution() = default;


        //---API
            void updateParams( const IntParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( xParams[0], xParams[1] ); } }
            void updateParam( IntParamType xParam ) override { if( ! InterfaceType::checkParam( xParam, 1_st ) ) { InterfaceType::setParam( xParam, 1_st ); dist = UnderType( InterfaceType::getIntParam( 0 ), xParam ); } }
            void updateParams( const RealParamsType& ) override {;}
            void updateParam( RealParamType ) override {;}
            void updateParams( const RealParamsType&, const IntParamsType& ) override {;}
            void updateParam( RealParamType, IntParamType ) override {;}

            TIntNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); }
            void reset() override { dist.reset(); }


        private:
            std::uniform_int_distribution<TIntNumber> dist;
    };
    using DiscreteUniformDist = DiscreteUniformDistribution<>;


    template< typename TRandomEngine = RandomEngine, typename TIntNumber = uint, typename TRealNumber = TReal >
    class DiscreteDistribution : public DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber> //this implmentation guarantees the independence of the samples
    {
        REGISTER_SUBTYPE( "discrete", "disc", DiscreteDistributionInterface<>::SubtypeIdx::DISCRETE )

        public:
        	using UnderType = std::discrete_distribution<TIntNumber>;
            using InterfaceType = DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber >;

            using typename InterfaceType::IntNumberType; using typename InterfaceType::RealNumberType;
            using typename InterfaceType::IntParamsType; using typename InterfaceType::IntParamType;
            using typename InterfaceType::RealParamsType; using typename InterfaceType::RealParamType;
            using typename InterfaceType::RandomEngineType; using InterfaceType::sample;
            using ParamsType = RealParamsType;
            using ParamType = RealParamType;

            inline static RealParamType DF_REAL_PARAM = 1.0;
            inline static RealParamsType DF_REAL_PARAMS = { 1.0 };


        //---ctor, dtor
            explicit DiscreteDistribution( const RealParamsType& realParams = DF_REAL_PARAMS ) 
            : InterfaceType( InterfaceType::SubtypeIdx::DISCRETE, realParams.size() > 0 ? realParams : DF_REAL_PARAMS )
            , dist( realParams.begin(), realParams.end() ) {;}
            /*, uniform( 0.0, 1.0 ) 
            { createAccumulatedProbs( realParams.size() > 0 ? realParams : DF_REAL_PARAMS ); }*/
            
            explicit DiscreteDistribution( const RealParamsType& realParams = DF_REAL_PARAMS, const IntParamsType& = {} ) 
            : InterfaceType( InterfaceType::SubtypeIdx::DISCRETE, realParams.size() > 0 ? realParams : DF_REAL_PARAMS )
            , dist( realParams.begin(), realParams.end() ) {;}
            /*, uniform( 0.0, 1.0 ) 
            { createAccumulatedProbs( realParams.size() > 0 ? realParams : DF_REAL_PARAMS ); } */

            virtual ~DiscreteDistribution() = default;
            
        //---get
            //inline size_t getOutputNum() const { return accumulatedProbs.size(); }
        
        //---API
            //void updateParams( const RealParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); createAccumulatedProbs( xParams ); } }
            //void updateParam( RealParamType xParam ) override { if( ! InterfaceType::checkParams( RealParamsType( { xParam } ) ) ) { InterfaceType::setParams( RealParamsType( { xParam } ) ); createAccumulatedProbs( RealParamsType( { xParam } ) ); } }
            void updateParams( const RealParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( xParams.begin(), xParams.end() ); } }
            void updateParam( RealParamType xParam ) override { if( ! InterfaceType::checkParams( RealParamsType( { xParam } ) ) ) { InterfaceType::setParams( RealParamsType( { xParam } ) ); dist = UnderType( { xParam } ); } }
            void updateParams( const IntParamsType& ) override {;}
            void updateParam( IntParamType ) override {;}
            void updateParams( const RealParamsType&, const IntParamsType& ) override {;}
            void updateParam( RealParamType, IntParamType ) override {;}

            TIntNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); } 
            void reset() override { dist.reset(); }


        private:
        	UnderType dist;
            //std::vector<TRealNumber> accumulatedProbs;
            //std::uniform_real_distribution<TRealNumber> uniform;

            //void createAccumulatedProbs( const RealParamsType& params );
    };
    using DiscreteDist = DiscreteDistribution<>;


    template< typename TRandomEngine = RandomEngine, typename TIntNumber = uint, typename TRealNumber = TReal >
    class BinomialDistribution : public DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber > //this implmentation guarantees the independence of the samples
    {
        REGISTER_SUBTYPE( "binomial", "bin", DiscreteDistributionInterface<TIntNumber>::SubtypeIdx::BINOMIAL )

        public:
            using UnderType = std::binomial_distribution<TIntNumber>;
            using InterfaceType = DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber >;

            using typename InterfaceType::IntNumberType; using typename InterfaceType::RealNumberType;
            using typename InterfaceType::IntParamsType; using typename InterfaceType::IntParamType;
            using typename InterfaceType::RealParamsType; using typename InterfaceType::RealParamType;
            using typename InterfaceType::RandomEngineType; using InterfaceType::sample;
            using ParamsType = RealParamsType;
            using ParamType = RealParamType;

            inline static RealParamType DF_REAL_PARAM = 0.5;
            inline static RealParamsType DF_REAL_PARAMS = { 0.5 };
            inline static IntParamType DF_INT_PARAM = 1;
            inline static IntParamsType DF_INT_PARAMS = { 1 };


        //---ctor, dtor
            explicit BinomialDistribution(  const RealParamsType& realParams = DF_REAL_PARAMS, const IntParamsType& intParams = DF_INT_PARAMS  ) 
            : InterfaceType( InterfaceType::SubtypeIdx::BINOMIAL, realParams.size() > 0 ? realParams : DF_REAL_PARAMS, intParams.size() > 0 ? intParams : DF_INT_PARAMS )
            , dist( intParams.size() > 0 ? intParams[0] : DF_INT_PARAM, realParams.size() > 0 ? realParams[0] : DF_REAL_PARAM ) {;}
            
            virtual ~BinomialDistribution() = default;
            
        //---API
            void updateParams( const RealParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( InterfaceType::getIntParam( 0 ), xParams[0] ); } }
            void updateParam( RealParamType xParam ) override { if( ! InterfaceType::checkParam( xParam, 0_st ) ) { InterfaceType::setParam( xParam, 0_st ); dist = UnderType( InterfaceType::getIntParam( 0 ), xParam ); } }
            void updateParams( const IntParamsType& xParams ) override { if( ! InterfaceType::checkParams( xParams ) ) { InterfaceType::setParams( xParams ); dist = UnderType( xParams[0], InterfaceType::getRealParam( 0 ) ); } }
            void updateParam( IntParamType xParam ) override { if( ! InterfaceType::checkParam( xParam, 0_st ) ) { InterfaceType::setParam( xParam, 0_st ); dist = UnderType( xParam, InterfaceType::getRealParam( 0 ) ); } }
            void updateParams( const RealParamsType& xRealParams, const IntParamsType& xIntParams ) override { updateParam( xRealParams[0], xIntParams[0] ); }
            void updateParam( RealParamType xRealParam, IntParamType xIntParam ) override 
            { if( ! InterfaceType::checkParam( xRealParam, 0_st ) || ! InterfaceType::checkParam( xIntParam, 0_st ) ) { InterfaceType::setParam( xRealParam, 0_st ); InterfaceType::setParam( xIntParam, 0_st ); dist = UnderType( xIntParam, xRealParam ); } }
            
            TIntNumber sample( RandomEngineType& randomEngine ) override { return dist( randomEngine ); }
            void reset() override { dist.reset(); }


        private:
            UnderType dist;
    };
    using BinomialDist = BinomialDistribution<>;


    
///////////////////////////////////////////////////////////// FACTORY ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template< typename TRandomEngine = RandomEngine, typename TRealNumber = TReal >
    class ContinuousDistributionFactory : public Factory< ContinuousDistributionInterface<TRandomEngine, TRealNumber>
    , UniformDistribution<TRandomEngine, TRealNumber>, NormalDistribution<TRandomEngine, TRealNumber>, ExponentialDistribution<TRandomEngine, TRealNumber> > {};
    using ContinuousDistFactory = ContinuousDistributionFactory<>;

    template< typename TRandomEngine = RandomEngine, typename TIntNumber = uint, typename TRealNumber = TReal >
    class DiscreteDistributionFactory : public Factory< DiscreteDistributionInterface< TRandomEngine, TIntNumber, TRealNumber >
    , DiscreteUniformDistribution< TRandomEngine, TIntNumber, TRealNumber >, DiscreteDistribution< TRandomEngine, TIntNumber, TRealNumber >, BinomialDistribution< TRandomEngine, TIntNumber, TRealNumber > > {};
    using DiscreteDistFactory = DiscreteDistributionFactory<>;






////////////////////////////////////////////////////////////////////// METHOD DEFINITIONS /////////////////////////////////////////////////////////////////////
    /*template< typename TRandomEngine, typename TIntNumber, typename TRealNumber >
    TIntNumber DiscreteDistribution< TRandomEngine, TIntNumber, TRealNumber >::sample( RandomEngineType& randomEngine )
    {
        TReal uniSample = uniform( randomEngine );
        for( uint p = 0; p < accumulatedProbs.size(); p++ )
        {
            if( uniSample < accumulatedProbs[p] )
                return p;
        }
        return accumulatedProbs.size() -1;
    }

    template< typename TRandomEngine, typename TIntNumber, typename TRealNumber >
    void DiscreteDistribution< TRandomEngine, TIntNumber, TRealNumber >::createAccumulatedProbs( const RealParamsType& params )
    {
        accumulatedProbs = std::vector< TRealNumber >( params.size() );
        auto multiplier = 1.0 / std::accumulate( params.begin(), params.end(), 0.0 );

        accumulatedProbs.back() = 1.0;
        for( size_t p = accumulatedProbs.size() -1; p > 0; p-- ) //works due to p > 0. If changed to >= 0, potential infinite loop as the type is unsigned (-1 = big pos num)
            accumulatedProbs[ p - 1 ] = accumulatedProbs[p] - params[p] * multiplier;
    }*/
}

#endif //UT_DISTRIBUTION_INTERFACE_HPP
