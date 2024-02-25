#ifndef UT_DISTRIBUTIONSCALER_HPP
#define UT_DISTRIBUTIONSCALER_HPP

#include "ut/defines.hpp"
#include "ut/Metadata.hpp" //REGISTER_SUBTYPE
#include "ut/Factory.hpp" //DistributionScalerFactory


namespace ut
{
    class DistributionScalerFactory;
	class DistributionScaler
	{
		public:
			enum Type : int
            {
                NONE, UNIT, NORMAL, UNIFORM, EXPONENTIAL, COUNT
            };

            using SubtypeIdx = Type;
            using RealNumType = TReal;
            using ParamType = RealNumType;
            using ParamsType = std::vector<ParamType>;
			using FactoryType = DistributionScalerFactory;


		//---ctor, dtor
			virtual ~DistributionScaler() = default;
			DistributionScaler( const DistributionScaler& rhs ) = default;
			DistributionScaler( DistributionScaler&& rhs ) noexcept = default;
			DistributionScaler& operator=( const DistributionScaler& rhs ) = default;
			DistributionScaler& operator=( DistributionScaler&& rhs ) noexcept = default;

			virtual ParamsType getParams() const { return ParamsType( {} ); }

		//---API
			virtual RealNumType scale( RealNumType raw ) const = 0;

		

		protected:
			DistributionScaler() = default;
	};


	class NoneScaler : public DistributionScaler
	{
		REGISTER_SUBTYPE( "none", "none", DistributionScaler::Type::NONE )

		public: 
			NoneScaler( const std::vector<RealNumType>& ) {;}
			NoneScaler() = default;
			virtual ~NoneScaler() = default;

		//---API
			RealNumType scale( RealNumType raw ) const override { return raw; } 
	};


	class UnitScaler : public DistributionScaler
	{
		REGISTER_SUBTYPE( "unit", "unit", DistributionScaler::Type::UNIT )

		static constexpr RealNumType DF_PARAM = 0.0;

		public: 
			UnitScaler( const std::vector<RealNumType>& params ) : val( params.size() > 0 ? params[0] : DF_PARAM ) {;}
			UnitScaler( RealNumType param ) : val( param ) {;}
			virtual ~UnitScaler() = default;

			ParamsType getParams() const override { return ParamsType( { val } ); }

		//---API
			RealNumType scale( RealNumType ) const override { return val; } 


		private:
			RealNumType val;
	};


	class NormalScaler : public DistributionScaler
	{
		REGISTER_SUBTYPE( "normal", "norm", DistributionScaler::Type::NORMAL )

		public: 
			enum ParamIdx
			{
				P_MEAN, P_STDDV, P_COUNT
			};

		//---ctor, dtor
			NormalScaler( const std::vector<RealNumType>& params ) : mean( params[ParamIdx::P_MEAN] ), stddv( params[ParamIdx::P_STDDV] ) {;}
			NormalScaler( RealNumType mean, RealNumType stddv ) : mean( mean ), stddv( stddv ) {;}
			virtual ~NormalScaler() = default;

		//---get
			RealNumType getMean() const { return mean; }
			RealNumType getStddv() const { return stddv; }
			ParamsType getParams() const override { return ParamsType( { mean, stddv } ); }
			
		//---API
			RealNumType scale( RealNumType raw ) const override { return raw * stddv + mean; } 


		private:
			RealNumType mean;
			RealNumType stddv;
	};


	class UniformScaler : public DistributionScaler
	{
		REGISTER_SUBTYPE( "uniform", "uni", DistributionScaler::Type::UNIFORM )
		
		public: 
			enum ParamIdx
			{
				P_LBOUND, P_UBOUND, P_COUNT
			};

		//---ctor, dtor
			UniformScaler( const std::vector<RealNumType>& params ) : lbound( params[ParamIdx::P_LBOUND] ), ubound( params[ParamIdx::P_UBOUND] ), dist( ubound - lbound ) {;}
			UniformScaler( RealNumType lbound, RealNumType ubound ) : lbound( lbound ), ubound( ubound ), dist( ubound - lbound ) {;}
			virtual ~UniformScaler() = default;

			ParamsType getParams() const override { return ParamsType( { lbound, ubound } ); }
		//---API
			RealNumType scale( RealNumType raw ) const override { return raw * dist + lbound; } 


		private:
			RealNumType lbound;
			RealNumType ubound;
			RealNumType dist;
	};


	class ExponentialScaler : public DistributionScaler
	{
		REGISTER_SUBTYPE( "exponential", "exp", DistributionScaler::Type::EXPONENTIAL )
		
		public: 
			enum ParamIdx
			{
				P_LAMBDA, P_COUNT
			};

		//---ctor, dtor
			ExponentialScaler( const std::vector<RealNumType>& params ) : lambda( params[ParamIdx::P_LAMBDA] ), reverseLambda( isPositive( lambda ) ? 1.0 / lambda : std::numeric_limits<RealNumType>::max() ) {;}
			ExponentialScaler( RealNumType lambda ) : lambda( lambda ), reverseLambda( isPositive( lambda ) ? 1.0 / lambda : std::numeric_limits<RealNumType>::max() ) {;}
			virtual ~ExponentialScaler() = default;

			ParamsType getParams() const override { return ParamsType( { lambda } ); }
		//---API
			RealNumType scale( RealNumType raw ) const override { return raw * reverseLambda; } 


		private:
			RealNumType lambda;
			RealNumType reverseLambda;
	};


    class DistributionScalerFactory : public Factory< DistributionScaler, NoneScaler, UnitScaler, NormalScaler, UniformScaler, ExponentialScaler > {};
}

#endif //UT_DISTRIBUTIONSCALER_HPP