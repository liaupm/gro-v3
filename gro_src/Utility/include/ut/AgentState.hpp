#ifndef UT_AGENT_STATE_HPP
#define UT_AGENT_STATE_HPP

#include "ut/defines.hpp"
#include "ut/ExtendedVector.hpp" //QType
#include "ut/MultiBitset.hpp" //BType, BInitializerType
#include "ut/Bitset.hpp" //BTypeSimple


/*PRECOMPILED
#include <vector> //multiple get for states and changes
#include <tuple> //handlers
#include <utility> //std::move */


namespace ut
{
	class AgentState
	{
		public:
		    using BType = MultiBitset;
		    using BTypeSimple = Bitset;
		    using QType = ExtendedVector<TReal>;
	};
}

#endif //UT_AGENT_STATE_HPP