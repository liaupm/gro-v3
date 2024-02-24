#ifndef UT_ELEMENTINDEX_HPP
#define UT_ELEMENTINDEX_HPP

#include "ut/defines.hpp"

namespace ut
{
	class ElementIndex
	{
		public:
		//---ctor, dtor
			ElementIndex() = default;
			virtual ~ElementIndex() = default;
			ElementIndex( const ElementIndex& rhs ) = default;
			ElementIndex( ElementIndex&& rhs ) noexcept = default;
			ElementIndex& operator=( const ElementIndex& rhs ) = default;
			ElementIndex& operator=( ElementIndex&& rhs ) noexcept = default;

		//---get

		//---set

		//---API

		private:
	};
}

#endif //UT_ELEMENTINDEX_HPP