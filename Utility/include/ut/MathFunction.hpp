#ifndef UT_MATHFUNCTION_HPP
#define UT_MATHFUNCTION_HPP

#include "ut/defines.hpp"
#include "Metadata.hpp" //REGISTER_SUBTYPE
#include "Factory.hpp" //static DistributionTypeRegister _
#include "Agent.hpp" //AgentState::QType

/*PRECOMPILED
#include <vector> //params, inputs
#include <random> //RandomEngine, all std distributions
#include <math.h> //pow, exp, log, abs
#include <limits> //DF_MIN, DF_MAX
#include <algorithm> //min, max */


namespace ut
{
    template< typename TInput = AgentState::QType >
    class MathFunctionFactory;

    template< typename TInput = AgentState::QType >
	class MathFunction
	{
		public:
			enum Type : int
            {
                CONSTANT, MIN, MAX, ABS, EXP, LOG, SUM, PRODUCT, SIGMOID, HILL, COMBI_SUM, SAT_EXP, EXP_PRODUCT, COUNT
            };

            using SubtypeIdx = Type;
            using ReturnType = TReal;
            using ParamType = TReal; 
			using ParamsType = std::vector<ParamType>; 
			using InputsType = std::vector<size_t>;
			using QType = AgentState::QType;
            using FactoryType = MathFunctionFactory<TInput>;
            
            static constexpr Type DF_TYPE = Type::SUM;
            static constexpr TReal DF_MIN = std::numeric_limits<TReal>::lowest();
            static constexpr TReal DF_MAX = std::numeric_limits<TReal>::max();
            static constexpr int DF_ROUND_PLACES = -1;


		//---ctor, dtor
			virtual ~MathFunction() = default;
			/*MathFunction( const MathFunction& rhs ) = default;
			MathFunction( MathFunction&& rhs ) noexcept = default;
			MathFunction& operator=( const MathFunction& rhs ) = default;
			MathFunction& operator=( MathFunction&& rhs ) noexcept = default;*/

		//---get
            inline Type getType() const { return type; }
            inline const InputsType& getInputs() const { return inputs; }
            inline const ParamsType& getParams() const { return params; }

        //---set
            inline void setInputs( const InputsType& xInputs ) { inputs = xInputs; params = rawParams; completeParams(); } 

		//---API
            virtual TReal calculate( const TInput& inputVals ) const = 0;
            virtual TReal calculate( const TInput& inputVals, TReal scale ) const = 0;
            virtual void completeParams() {;}


            inline TReal evaluate( const TInput& inputVals ) const { return adjust( this->calculate( inputVals ) ); }
            inline TReal evaluateScaled( const TInput& inputVals, TReal scale ) const { return adjust( this->calculate( inputVals, scale ) ); }
            inline TReal evaluate( const TInput& inputVals, TReal noise ) const { return adjust( this->calculate( inputVals ) + noise ); } 
            inline TReal evaluateScaled( const TInput& inputVals, TReal scale, TReal noise ) const { return adjust( this->calculate( inputVals, scale ) + noise ); } 
            
            inline TReal adjustToBounds( TReal val ) const { return fit( val, min, max ); }
            inline TReal trimDecimalPlaces( TReal val ) const { return roundPlaces >= 0 ? ut::trimDecimals( val, roundMultiplier ) : val; }
            inline TReal adjust( TReal val ) const { return trimDecimalPlaces( adjustToBounds( val ) ); }


		protected:
			Type type;
			InputsType inputs;
            ParamsType params;
            ParamsType rawParams;

            TReal min, max;
            int roundPlaces;
            TReal roundMultiplier;

			MathFunction( Type type, const InputsType& inputs, const ParamsType& params, TReal min = DF_MIN, TReal max = DF_MAX, int roundPlaces = DF_ROUND_PLACES ) 
			: type( type ), inputs( inputs ), params( params ), rawParams( params )
            , min( min ), max( max ), roundPlaces( roundPlaces ), roundMultiplier( std::pow( 10.0, roundPlaces ) ) {;}
	};
    using MathFun = MathFunction<>;



////////////////////////////////////////////////////////// SIMPLE FUNCTIONS ////////////////////////////////////////////////////////////////
    template< typename TInput = AgentState::QType >
    class ConstantFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "const", "const", MathFunction<TInput>::Type::CONSTANT )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;

            inline static const ParamsType DF_PARAMS = { 0.0 };


        //---cotr, dtor
            explicit ConstantFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::CONSTANT, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~ConstantFunction() = default;

        //---API
            TReal calculate( const TInput& ) const override { return params[0]; }
            TReal calculate( const TInput&, TReal scale ) const override { return params[0] * scale; }
    };


    template< typename TInput = AgentState::QType >
    class MinFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "min", "min", MathFunction<TInput>::Type::MIN )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = { std::numeric_limits<TReal>::max() };


        //---cotr, dtor
            explicit MinFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::MIN, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~MinFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                TReal min = inputVals[ inputs[0] ];
                for( size_t i = 1; i < inputs.size(); i++ )
                    min = std::min( min, inputVals[ inputs[i] ] );
                return params.size() > 0 ? std::min( min, params[0] ) : min;
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override { return calculate( inputVals ) * scale; }    
    };


    template< typename TInput = AgentState::QType >
    class MaxFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "max", "max", MathFunction<TInput>::Type::MAX )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = { std::numeric_limits<TReal>::lowest() };


        //---cotr, dtor
            explicit MaxFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::MAX, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~MaxFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 

                TReal max = inputVals[ inputs[0] ];
                for( size_t i = 1; i < inputs.size(); i++ )
                    max = std::max( max, inputVals[ inputs[i] ] );
                return params.size() > 0 ? std::max( max, params[0] ) : max;
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override { return calculate( inputVals ) * scale; }        
    };


    template< typename TInput = AgentState::QType >
    class AbsFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "abs", "abs", MathFunction<TInput>::Type::ABS )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = {};


        //---cotr, dtor
            explicit AbsFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::ABS, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~AbsFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override { return std::abs( inputVals[ inputs[0] ] ); }
            TReal calculate( const TInput& inputVals, TReal scale ) const override { return std::abs( inputVals[ inputs[0] ] ) * scale; } 
    };


    template< typename TInput = AgentState::QType >
    class ExponentialFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "exp", "exp", MathFunction<TInput>::Type::EXP )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = {};


        //---cotr, dtor
            explicit ExponentialFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::EXP, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~ExponentialFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                if( inputs.size() >= 2 )
                    return std::pow( inputVals[ inputs[0] ], inputVals[ inputs[1] ] );
                if( params.size() > 0 )
                    return std::pow( params[0], inputVals[ inputs[0] ] );
                return std::exp( inputVals[ inputs[0] ] );
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                if( inputs.size() >= 2 )
                    return std::pow( inputVals[ inputs[0] ] * scale, inputVals[ inputs[1] ] * scale );
                if( params.size() > 0 )
                    return std::pow( params[0], inputVals[ inputs[0] ] * scale );
                return std::exp( inputVals[ inputs[0] ] * scale );
            }    
    };


    template< typename TInput = AgentState::QType >
    class LogFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "log", "log", MathFunction<TInput>::Type::LOG )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = {};


        //---cotr, dtor
            explicit LogFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::EXP, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~LogFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                if( inputs.size() > 1 )
                    return std::log2( inputVals[ inputs[0] ] ) / std::log2( inputVals[ inputs[1] ] );
                if( params.size() > 0 )
                    return std::log2( inputVals[ inputs[0] ] ) / std::log2( params[0] );
                return std::log( inputVals[ inputs[0] ] );
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                if( inputs.size() > 1 )
                    return std::log2( inputVals[ inputs[0] ] * scale ) / std::log2( inputVals[ inputs[1] ] * scale );
                if( params.size() > 0 )
                    return std::log2( inputVals[ inputs[0] ] * scale ) / std::log2( params[0] );
                return std::log( inputVals[ inputs[0] ] * scale );
            }  
    };





////////////////////////////////////////////////////////// COMPOUND FUNCTIONS ////////////////////////////////////////////////////////////////

    template< typename TInput = AgentState::QType >
    class SumFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "sum", "+", MathFunction<TInput>::Type::SUM )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamType DF_PARAM = 1.0;
            inline static const ParamsType DF_PARAMS = { 1.0 };


        //---cotr, dtor
        	explicit SumFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::SUM, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) { completeParams(); }

            virtual ~SumFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                TReal result = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                    result += inputVals[ inputs[i] ] * params[i];
                return result; 
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                TReal result = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                    result += inputVals[ inputs[i] ] * scale * params[i];
                return result; 
            }

            void completeParams() override
            {
                while( params.size() < inputs.size() )
                    params.push_back( DF_PARAM );
                if( params.size() == inputs.size() )
                    params.push_back( 0.0 ); //bias
            }
    };


    template< typename TInput = AgentState::QType >
    class ProductFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "product", "*", MathFunction<TInput>::Type::PRODUCT )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamType DF_PARAM = 1.0;
            inline static const ParamsType DF_PARAMS = { 1.0 };


        //---cotr, dtor
        	explicit ProductFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::PRODUCT, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) { completeParams(); }

            virtual ~ProductFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
            	TReal result = params.back();
            	for( size_t i = 0; i < inputs.size(); i++ )
                {
					if( inputVals[ inputs[i] ] == 0.0 && params[i] < 0.0 )
						return 0.0;
            		result *= std::pow( inputVals[ inputs[i] ], params[i] );
                }
            	return result; 
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                TReal result = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                {
                    if( inputVals[ inputs[i] ] * scale == 0.0 && params[i] < 0.0 )
                        return 0.0;
                    result *= std::pow( inputVals[ inputs[i] ] * scale, params[i] );
                }
                return result; 
            }

            void completeParams() override
            {
                while( params.size() < inputs.size() )
                    params.push_back( DF_PARAM );
                if( params.size() == inputs.size() )
                    params.push_back( 1.0 );
            }     
    };


    template< typename TInput = AgentState::QType >
    class SigmoidFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "sigmoid", "sig", MathFunction<TInput>::Type::SIGMOID )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = {};


        //---cotr, dtor
        	explicit SigmoidFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::EXP, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~SigmoidFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override { return 1.0 / ( 1.0 + std::exp( - inputVals[ inputs[0] ] ) ); }
            TReal calculate( const TInput& inputVals, TReal scale ) const override { return 1.0 / ( 1.0 + std::exp( - inputVals[ inputs[0] ] * scale ) ); }  
    };


    template< typename TInput = AgentState::QType >
    class HillFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "hill", "hill", MathFunction<TInput>::Type::HILL )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = { 1.0, 1.0, 1.0 };


        //---cotr, dtor
        	explicit HillFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::EXP, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) {;}

            virtual ~HillFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
            	if( params.size() > 2 )
            		return params[0] * std::pow( inputVals[ inputs[0] ], params[2] ) / ( std::pow( params[1], params[2] ) + std::pow( inputVals[ inputs[0] ], params[2] ) );
            	return params[0] * inputVals[ inputs[0] ] / ( params[1] + inputVals[ inputs[0] ] );
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                if( params.size() > 2 )
                    return params[0] * std::pow( inputVals[ inputs[0] ] * scale, params[2] ) / ( std::pow( params[1], params[2] ) + std::pow( inputVals[ inputs[0] ] * scale, params[2] ) );
                return params[0] * inputVals[ inputs[0] ] * scale / ( params[1] + inputVals[ inputs[0] ] * scale );
            }
    };


    template< typename TInput = AgentState::QType >
    class CombiSumFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "combi_sum", "combi", MathFunction<TInput>::Type::COMBI_SUM )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamType DF_PARAM = 1.0;
            inline static const ParamsType DF_PARAMS = { 1.0 };


        //---cotr, dtor
            explicit CombiSumFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::COMBI_SUM, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) { completeParams(); }

            virtual ~CombiSumFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                TReal result = params.back();
                for( size_t o = 1; o < inputs.size(); o++ )
                {
                    for( size_t i = 0; i < inputs.size(); i++ )
                    {
                        TReal term = inputVals[ inputs[i] ] * params[i];
                        for( size_t j = i + 1; j < i + o; j++ )
                            term *= inputVals[ inputs[j] ] * params[j];

                        result += term;
                    }
                }
                return result;
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                TReal result = params.back();
                for( size_t o = 1; o < inputs.size(); o++ )
                {
                    for( size_t i = 0; i < inputs.size(); i++ )
                    {
                        TReal term = inputVals[ inputs[i] ] * params[i] * scale;
                        for( size_t j = i + 1; j < i + o; j++ )
                            term *= inputVals[ inputs[j] ] * params[j] * scale;

                        result += term;
                    }
                }
                return result;
            }

            void completeParams() override
            {
                while( params.size() <= inputs.size() )
                    params.push_back( DF_PARAM );
            }   
    };


    template< typename TInput = AgentState::QType >
    class SatExponentialFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "sat_exp", "sexp", MathFunction<TInput>::Type::SAT_EXP )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = { 1.0 };


        //---cotr, dtor
            explicit SatExponentialFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::SAT_EXP, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) { completeParams(); }

            virtual ~SatExponentialFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                TReal exponent = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                    exponent += inputVals[ inputs[i] ] * params[i+1];
                
                return params[0] * ( 1.0 - std::exp( - exponent ) );
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                TReal exponent = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                    exponent += inputVals[ inputs[i] ] * params[i+1] * scale;
                
                return params[0] * ( 1.0 - std::exp( - exponent ) );
            }

            void completeParams() override
            {
                while( params.size() <= inputs.size() )
                    params.push_back( 1.0 );
                if( params.size() <= inputs.size() + 1 )
                    params.push_back( 0.0 );
            }
    };


    template< typename TInput = AgentState::QType >
    class ExponentialProductFunction : public MathFunction<TInput>
    {
        REGISTER_SUBTYPE( "exp_product", "exp_prod", MathFunction<TInput>::Type::EXP_PRODUCT )
        
        public:
            using InterfaceType = MathFunction<TInput>;
            using typename InterfaceType::InputsType; using typename InterfaceType::ParamType; using typename InterfaceType::ParamsType;
            using typename InterfaceType::Type; using typename InterfaceType::QType; using InterfaceType::inputs; using InterfaceType::params;
            
            inline static const ParamsType DF_PARAMS = { 1.0 };


        //---cotr, dtor
            explicit ExponentialProductFunction( const InputsType& inputs, const ParamsType& params = DF_PARAMS, TReal min = InterfaceType::DF_MIN, TReal max = InterfaceType::DF_MAX, int roundPlaces = InterfaceType::DF_ROUND_PLACES ) 
            : InterfaceType( Type::EXP_PRODUCT, inputs, params.size() > 0 ? params : DF_PARAMS, min, max, roundPlaces ) { completeParams(); }

            virtual ~ExponentialProductFunction() = default;

        //---API
            TReal calculate( const TInput& inputVals ) const override
            { 
                TReal exponent1 = params[ params.size() - 2 ];
                TReal exponent2 = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                {
                    if( params[i+1] >= 0.0 )
                        exponent1 += inputVals[ inputs[i] ] * params[i+1];
                    else
                        exponent2 -= inputVals[ inputs[i] ] * params[i+1];
                }
                
                return params[0] * ( 1.0 - std::exp( - exponent1 ) ) * std::exp( - exponent2 );
            }

            TReal calculate( const TInput& inputVals, TReal scale ) const override
            { 
                TReal exponent1 = params[ params.size() - 2 ];
                TReal exponent2 = params.back();
                for( size_t i = 0; i < inputs.size(); i++ )
                {
                    if( params[i+1] >= 0.0 )
                        exponent1 += inputVals[ inputs[i] ] * params[i+1] * scale;
                    else
                        exponent2 -= inputVals[ inputs[i] ] * params[i+1] * scale;
                }
                
                return params[0] * ( 1.0 - std::exp( - exponent1 ) ) * std::exp( - exponent2 );
            }

            void completeParams() override
            {
                while( params.size() <= inputs.size() )
                    params.push_back( 1.0 );
                while( params.size() <= inputs.size() + 2 )
                    params.push_back( 0.0 );
            }      
    };


////////////////////////////////////////////////////////// FACTORY ////////////////////////////////////////////////////////////////
    template< typename TInput >
    class MathFunctionFactory : public Factory< MathFunction<TInput>, ConstantFunction<TInput>, MinFunction<TInput>, MaxFunction<TInput>, AbsFunction<TInput>, ExponentialFunction<TInput>, LogFunction<TInput>
    , SumFunction<TInput>, ProductFunction<TInput>, SigmoidFunction<TInput>, HillFunction<TInput>, CombiSumFunction<TInput>, SatExponentialFunction<TInput>, ExponentialProductFunction<TInput> > {};
}

#endif //UT_MATHFUNCTION_HPP
