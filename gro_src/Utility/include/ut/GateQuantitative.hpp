#ifndef UT_GATEQUANTITATIVE_HPP
#define UT_GATEQUANTITATIVE_HPP

#include "ut/defines.hpp"
#include "ut/NameMap.hpp" //compOperatorNM
#include "ut/AgentState.hpp" //AgentState::QType in check()


namespace ut
{
	class GateQuantitative
	{
		public:
			enum class CompOperator : uint
            {
                LESS, LESS_EQUAL, GREATER, GREATER_EQUAL, EQUAL, NOT_EQUAL, COUNT
            };

            static constexpr size_t DF_ELEM_IDX = 0;
            static constexpr TReal DF_VALUE = 1.0;
            static constexpr CompOperator DF_COMP_OPERATOR = CompOperator::GREATER_EQUAL;
            
            static ut::NameMap<CompOperator> compOperatorNM; //string to CompOperator


		//---ctor, dtor
			GateQuantitative( size_t elemIdx, TReal value = DF_VALUE, CompOperator compOperator = DF_COMP_OPERATOR )
            : elemIdx( elemIdx ), value(value), compOperator( compOperator ) {;}

            explicit GateQuantitative( TReal value = DF_VALUE, CompOperator compOperator = DF_COMP_OPERATOR )
            : elemIdx( DF_ELEM_IDX ), value( value ), compOperator( compOperator ) {;}

			virtual ~GateQuantitative() = default;
			/*GateQuantitative( const GateQuantitative& rhs ) = default;
			GateQuantitative( GateQuantitative&& rhs ) noexcept = default;
			GateQuantitative& operator=( const GateQuantitative& rhs ) = default;
			GateQuantitative& operator=( GateQuantitative&& rhs ) noexcept = default;*/

		//---get
			inline size_t getElemIdx() const { return elemIdx; }
            inline TReal getValue() const { return value; }
            inline CompOperator getCompOperator() const { return compOperator; }

		//---set
            inline void setElemIdx( size_t xElemIdx ) { elemIdx = xElemIdx; }

		//---API
            bool check( const AgentState::QType& qState ) const; //whether the equation or inequation holds


		private:
			size_t elemIdx; //absolute idx of the input element, left side
			TReal value; //value on the right side
            CompOperator compOperator; //equation or inequation operator
	};
}

#endif //UT_GATEQUANTITATIVE_HPP