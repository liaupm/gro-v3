#ifndef UT_SEEDGENERATOR_HPP
#define UT_SEEDGENERATOR_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <random> */


namespace ut
{
	class SeedGeneratorBase
	{
		public:
			static constexpr uint DF_SEED = 1;

		//---ctor, dtor
			virtual ~SeedGeneratorBase() = default;
			//SeedGeneratorBase( const SeedGeneratoBaser& rhs ) = default;
			//SeedGeneratorBase( SeedGeneratorBase&& rhs ) noexcept = default;
			//SeedGeneratorBase& operator=( const SeedGeneratorBase& rhs ) = default;
			//SeedGeneratorBase& operator=( SeedGeneratorBase&& rhs ) noexcept = default;

		//---get
			inline uint getSeed() const { return seed; }

		//---API
			virtual void reset() = 0; //set to the initial value, using the current seed
			virtual void reseed( uint newSeed ) = 0; //reset with a different seed
			virtual uint nextSeed() = 0; //provide a new value


		protected:
			uint seed; //the stating base seed

			SeedGeneratorBase( uint seed = DF_SEED  ) : seed( seed ) {;}
	};
	using SeedGenerator = SeedGeneratorBase;


	class SeedGeneratorArithmetic : public SeedGeneratorBase
	/* deterministic, justs adds an offset to  previous values */
	{
		public:
			static constexpr uint DF_OFFSET = 1;

		//---cotr, dtor
			SeedGeneratorArithmetic( uint offset = DF_OFFSET, uint seed = DF_SEED )
			: SeedGeneratorBase::SeedGeneratorBase( seed ), offset( offset ), currentVal( seed ) {;}

			virtual ~SeedGeneratorArithmetic() = default;

		//---API
			void reset() override { currentVal = seed; }
			void reseed( uint newSeed ) override { seed = newSeed; reset(); }
			uint nextSeed() override { currentVal += offset; return currentVal; }


		private:
			uint offset; //edded to the current value to get the next one 
			uint currentVal; //current value
	};


	template<typename TGenerator = std::minstd_rand0>
	class SeedGeneratorRnd : public SeedGeneratorBase
	/* stochastic, a random generator is used to seed others 
	a wrapper for std:: generators */
	{
		public:
			using GeneratorType = TGenerator;

			SeedGeneratorRnd( uint seed = 1 ): SeedGeneratorBase::SeedGeneratorBase( seed ), generator(seed) {;}
			virtual ~SeedGeneratorRnd() = default;

			void reset() override { generator.seed( seed ); }
			void reseed( uint newSeed ) override { seed = newSeed; reset(); }
			uint nextSeed() override { return generator(); }


		private:
			GeneratorType generator; //a random numbers generator from std::
	};
}

#endif //UT_SEEDGENERATOR_HPP