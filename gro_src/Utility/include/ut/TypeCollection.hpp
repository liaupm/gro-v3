#ifndef UT_TYPECOLLECTION_HPP
#define UT_TYPECOLLECTION_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <tuple> //tuple, make_tuple, tuple_element_t, get<> */


namespace ut
{
/* only for static use, no to instantiate objects 
a series of class types can be stored in a tuple and retrieved by an idx (usually an enum)
useful for creating objects of types defined by enums or to cast */

      template< typename T >
      struct TypeHolder
      {
            using Type = T;
      };


      template< typename... Ts > 
      struct TypeCollection
      {
      	using UnderTupleType = std::tuple< TypeHolder<Ts>... >;
      	static constexpr UnderTupleType types = std::make_tuple( TypeHolder<Ts>()... ); //stored types
            static constexpr size_t nSize = sizeof...(Ts ); //number of types

            template< int I >
            using TypeAt = typename std::tuple_element_t< I, UnderTupleType >::Type; //get type by idx
      };
}

#endif //UT_TYPECOLLECTION_HPP