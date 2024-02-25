#ifndef UT_SORTEDVECTOR_HPP
#define UT_SORTEDVECTOR_HPP

#include "ut/defines.hpp"
#include "ut/ExtendedVector.hpp" //base

/*PRECOMPILED
#include <vector> //ParentType
#include <memory> //PtrSortedVector
#include <algorithm> //std::lower_bound
#include <utility> //forward */


namespace ut
{
	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti = true >
	class SortedVector : public ExtendedVector< TElem >
	/* For objects with a data member to sort by
	  TElem: custom class of the stored elements
	  TMember : type of Member 
	  Member : data member of TElem used to sort
	  BMulti : wheter multiple identical entries are allowed */
	{
		public:
			using ElemType = TElem;
			using ParentType = std::vector< TElem >;
			using MemberType = TMember;
			using CmpFun = bool(*)( const TElem& elem, const TElem val ); //type of the comparison function

			using typename ParentType::iterator; using typename ParentType::const_iterator;
			using ParentType::size; using ParentType::begin; using ParentType::end; using ParentType::cbegin; using ParentType::cend;

			inline static bool lessThan( const TElem& lhs, const TElem& rhs ) { return lhs.*Member < rhs.*Member; }


		//---ctor, dtor
			SortedVector() = default;
			virtual ~SortedVector() = default;
			SortedVector( const SortedVector& rhs ) = default;
			SortedVector( SortedVector&& rhs ) noexcept = default;
			SortedVector& operator=( const SortedVector& rhs ) = default;
			SortedVector& operator=( SortedVector&& rhs ) noexcept = default;

		//---API
			const_iterator find( const TElem& elem ) const;
			void add( const TElem& newElem );
	};


	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti = true >
	class PtrSortedVector : public ExtendedVector< CSP<TElem> >
	/* For storing pointers intead of objects */
	{
		public:
			using ElemType = TElem;
			using PtrType = CSP<ElemType>;
			using ParentType = std::vector< PtrType >;
			using MemberType = TMember;
			using CmpFun = bool(*)( const TElem* elem, const TElem* val ); //type of the comparison function

			using typename ParentType::iterator; using typename ParentType::const_iterator;
			using ParentType::size; using ParentType::begin; using ParentType::end; using ParentType::cbegin; using ParentType::cend;

			inline static bool lessThan( PtrType lhs, PtrType rhs ) { return lhs.get()->*Member < rhs.get()->*Member; }


		//---ctor, dtor
			PtrSortedVector() = default;
			virtual ~PtrSortedVector() = default;
			PtrSortedVector( const PtrSortedVector& rhs ) = default;
			PtrSortedVector( PtrSortedVector&& rhs ) noexcept = default;
			PtrSortedVector& operator=( const PtrSortedVector& rhs ) = default;
			PtrSortedVector& operator=( PtrSortedVector&& rhs ) noexcept = default;

		//---API
			const_iterator find( PtrType elem ) const;
			void add( PtrType newElem );
	};


	template< typename TElem, bool BMulti = true >
	class SimpleSortedVector : public ExtendedVector< TElem >
	/* for elements that are just numbers */
	{
		public:
			using ElemType = TElem;
			using ParentType = std::vector< TElem >;
			
			using typename ParentType::iterator; using typename ParentType::const_iterator;
			using ParentType::size; using ParentType::begin; using ParentType::end; using ParentType::cbegin; using ParentType::cend;

			inline static bool lessThan( const TElem& lhs, const TElem& rhs ) { return lhs < rhs; }


		//---ctor, dtor
			SimpleSortedVector() = default;
			virtual ~SimpleSortedVector() = default;
			SimpleSortedVector( const SimpleSortedVector& rhs ) = default;
			SimpleSortedVector( SimpleSortedVector&& rhs ) noexcept = default;
			SimpleSortedVector& operator=( const SimpleSortedVector& rhs ) = default;
			SimpleSortedVector& operator=( SimpleSortedVector&& rhs ) noexcept = default;

		//---API
			const_iterator find( const TElem& elem ) const;
			void add( const TElem& newElem );
	};




///////////////////////////////////// METHOD DEFINITION /////////////////////////////////////////////////////////////////////////

//-----------------------------SortedVector

	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti >
	inline typename SortedVector< TElem, TMember, Member, BMulti >::const_iterator SortedVector< TElem, TMember, Member, BMulti >::find( const TElem& elem ) const
	{ 
		const_iterator it = std::lower_bound( begin(), end(), elem, lessThan ); 
		return it == end() || lessThan( elem, *it ) ? end() : it; 
	}

	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti >
	inline void SortedVector< TElem, TMember, Member, BMulti >::add( const TElem& newElem )
	{ 
		iterator it = std::lower_bound( begin(), end(), newElem, lessThan );
		
		if( it == end() || lessThan( newElem, *it ) || BMulti  )
			this->insert( it, newElem );
	}


//-----------------------------PtrSortedVector

	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti >
	inline typename PtrSortedVector< TElem, TMember, Member, BMulti >::const_iterator PtrSortedVector< TElem, TMember, Member, BMulti >::find( PtrType elem ) const
	{ 
		const_iterator it = std::lower_bound( begin(), end(), elem, lessThan ); 
		return it == end() || lessThan( elem, *it ) ? end() : it; 
	}

	template< typename TElem, typename TMember, TMember TElem::* Member, bool BMulti >
	inline void PtrSortedVector< TElem, TMember, Member, BMulti >::add( PtrType newElem )
	{ 
		iterator it = std::lower_bound( begin(), end(), newElem, lessThan );
		
		if( it == end() || lessThan( newElem, *it ) || BMulti  )
			this->insert( it, newElem );
	}


//-----------------------------SimpleSortedVector

	template< typename TElem, bool BMulti >
	inline typename SimpleSortedVector< TElem, BMulti >::const_iterator SimpleSortedVector< TElem, BMulti >::find( const TElem& elem ) const
	{ 
		const_iterator it = std::lower_bound( begin(), end(), elem, lessThan ); 
		return it == end() || lessThan( elem, *it ) ? end() : it; 
	}

	template< typename TElem, bool BMulti >
	inline void SimpleSortedVector< TElem, BMulti >::add( const TElem& newElem )
	{ 
		iterator it = std::lower_bound( begin(), end(), newElem, lessThan );
		
		if( it == end() || lessThan( newElem, *it ) || BMulti  )
			this->insert( it, newElem );
	}
}

#endif //UT_SORTEDVECTOR_HPP