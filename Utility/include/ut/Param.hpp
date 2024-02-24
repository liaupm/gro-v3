#ifndef UT_PARAM_HPP
#define UT_PARAM_HPP

#include "ut/defines.hpp"
#include "ut/DistributionScaler.hpp"

namespace ut
{

	template< typename TBase >
	struct ParamData
	{
		using BaseType = TBase;

	//---ctor, dtor
		ParamData( const BaseType* rnd, const BaseType* fun, const BaseType* marker, ut::DistributionScaler::ParamsType params ) : rnd( rnd ), fun( fun ), marker( marker ), params( params ) {;}
		virtual ~ParamData() = default;

		const BaseType* rnd;
		const BaseType* fun;
		const BaseType* marker;
		ut::DistributionScaler::ParamsType params;

	};


	template< typename TBase >
	class Param
	{
		public:
			enum class Mode
			{
				DETER, RANDOM, CUSTOM, COUNT
			};

			using BaseType = TBase;

		//---ctor, dtor
			Param( const BaseType* rnd, const BaseType* fun, ut::DistributionScaler::Type scalerType, ut::DistributionScaler::ParamsType params )
			: rnd( rnd ), fun( fun ), scaler( scalerType, params ), params( params ) { calculateMode(); }



			virtual ~Param() = default;
			Param( const Param& rhs ) = default;
			Param( Param&& rhs ) noexcept = default;
			Param& operator=( const Param& rhs ) = default;
			Param& operator=( Param&& rhs ) noexcept = default;

		//---get

		//---set

		//---API
			void calculateMode();

		//private:
			Mode mode;
			const BaseType* rnd;
			const BaseType* fun;


			SP<ut::DistributionScaler> scaler;
			ut::DistributionScaler::ParamsType params;
	};



/////////////////////////////// METHOD IMPLEMENTATIONS ///////////////////////////

	template< typename TBase >
	void Param<TBase>::calculateMode() 
	{
		if( fun )
			mode = Mode::CUSTOM;
		else if( rnd )
			mode = Mode::RANDOM;
		else if( params.size() > 0 )
			mode = Mode::DETER;
		else
			mode = Mode::COUNT;
	}

}

#endif //UT_PARAM_HPP