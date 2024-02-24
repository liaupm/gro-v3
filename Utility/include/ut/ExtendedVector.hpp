#ifndef UT_EXTENDEDVECTOR_HPP
#define UT_EXTENDEDVECTOR_HPP

#include "ut/defines.hpp"
#include "ut/StaticOperators.hpp" //Op

/*PRECOMPILED
#include <vector> //base
#include <utility> //forward */


namespace ut
{
	template< typename TElem >
	class ExtendedVector : public std::vector< TElem >
	/* extends the std::vector with arithmetic operators and fitting to bounds */
	{
		public:
			using ParentType = std::vector< TElem >;
			using ElemType = TElem;
			using Op = StaticOperators< TElem >;

			using ParentType::size; using ParentType::begin; using ParentType::cbegin; using ParentType::end; using ParentType::cend; 
			using ParentType::clear; using ParentType::erase;
			
			template< typename T >
			using EnableForElemOrVec = EnableIfAnyIsBase< T, TElem, std::vector<TElem> >;


		//---ctor, dtor
			ExtendedVector() = default;
			template< typename... Ts>
			ExtendedVector( Ts&&... args ) : std::vector< TElem >( ( std::forward<Ts>(args) )... ) {;}

			virtual ~ExtendedVector() = default; 
			ExtendedVector( const ExtendedVector& rhs ) = default;
			ExtendedVector( ExtendedVector&& rhs ) noexcept = default;
			ExtendedVector& operator=( const ExtendedVector& rhs ) = default;
			ExtendedVector& operator=( ExtendedVector&& rhs ) noexcept = default;

		//---API
			ExtendedVector& fitL( const TElem& val ) { Op::template assigmentOperator< typename Op::MaxAssign >( (*this), val ); return *this; }
			ExtendedVector& fitU( const TElem& val ) {Op::template assigmentOperator< typename Op::MinAssign >( (*this), val ); return *this; }

		//---own operators
			template< typename T, typename = EnableForElemOrVec<T> >
			inline ExtendedVector& operator+=( const T& rhs ) { Op::template assigmentOperator< typename Op::SumAssign >( (*this), rhs ); return *this; }
			template< typename T, typename = EnableForElemOrVec<T> >
			inline ExtendedVector& operator-=( const T& rhs ) { Op::template assigmentOperator< typename Op::DiffAssign >( (*this), rhs ); return *this; }
			template< typename T, typename = EnableForElemOrVec<T> >
			inline ExtendedVector& operator*=( const T& rhs ) { Op::template assigmentOperator< typename Op::MultAssign >( (*this), rhs ); return *this; }
			template< typename T, typename = EnableForElemOrVec<T> >
			inline ExtendedVector& operator/=( const T& rhs ) { Op::template assigmentOperator< typename Op::DivAssign >( (*this), rhs ); return *this; }

	    //---friend operators
			inline friend ExtendedVector operator-( const ExtendedVector& rhs ) { return Op::template unaryOperator< Op::Reverse >( rhs ); }
			inline friend ExtendedVector operator+( const ExtendedVector& lhs, const ExtendedVector& rhs ) { return Op::template binaryOperator< typename Op::Sum >( lhs, rhs ); }
			inline friend ExtendedVector operator-( const ExtendedVector& lhs, const ExtendedVector& rhs ) { return Op::template binaryOperator< typename Op::Diff >( lhs, rhs ); }
			inline friend ExtendedVector operator*( const ExtendedVector& lhs, const ExtendedVector& rhs ) { return Op::template binaryOperator< typename Op::Mult >( lhs, rhs ); }
			inline friend ExtendedVector operator/( const ExtendedVector& lhs, const ExtendedVector& rhs ) { return Op::template binaryOperator< typename Op::Div >( lhs, rhs ); }

			inline friend ExtendedVector operator*( const ExtendedVector& lhs, const TElem& val ) { return Op::template binaryOperator< typename Op::Mult >( lhs, val ); }
	};
}

#endif //UT_EXTENDEDVECTOR_HPP