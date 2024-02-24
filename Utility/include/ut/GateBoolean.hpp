#ifndef UT_GATEBOOLEAN_HPP
#define UT_GATEBOOLEAN_HPP

#include "ut/defines.hpp"
#include "ut/NameMap.hpp" //functionNM
#include "ut/Bitset.hpp" //masks
#include "ut/AgentState.hpp" //AgentState::BTypeSimple in check()


namespace ut
{
	class GateBoolean
	{
		public:
			enum class GateFunction
            {
                OPEN, CLOSED, AND, NAND, OR, NOR, XOR, XNOR, COUNT
            };

            static constexpr GateFunction DF_FUNCTION = GateFunction::AND;

            static ut::NameMap<GateFunction> functionNM;


		//---ctor, dtor
            GateBoolean( const ut::Bitset& gateMask, const ut::Bitset& presence, GateFunction gateFunction = DF_FUNCTION )
            : gateFunction( gateFunction ), gateMask( gateMask ), presence( presence ) {;}

            explicit GateBoolean( GateFunction gateFunction = DF_FUNCTION )
            : gateFunction( gateFunction ), gateMask(), presence() {;}

			virtual ~GateBoolean() = default;
			/*GateBoolean( const GateBoolean& rhs ) = default;
			GateBoolean( GateBoolean&& rhs ) noexcept = default;
			GateBoolean& operator=( const GateBoolean& rhs ) = default;
			GateBoolean& operator=( GateBoolean&& rhs ) noexcept = default;*/

		//---get
			inline GateFunction getGateFunction() const { return gateFunction; }
            inline const ut::Bitset& getGateMask() const { return gateMask; }
            inline const ut::Bitset& getPresence() const { return presence; }

		//---set
            inline void setMasks( const ut::Bitset& xGateMask, const ut::Bitset& xPresence ) { gateMask = xGateMask; presence = xPresence; }

		//---API
            bool check( const AgentState::BTypeSimple& rawInput ) const;


		protected:
			GateFunction gateFunction; //logic function to evaluate
            ut::Bitset gateMask; //which elements are relevant
            ut::Bitset presence; //the required state of the relevant elements (present or absent)
            //the two masks together are used to store 3 states: on, off and irrelevant
	};
}

#endif //UT_GATEBOOLEAN_HPP