#ifndef UT_PAIRVECTOR_HPP
#define UT_PAIRVECTOR_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <vector> //base
#include <utility> //pair */


namespace ut
{
	template< typename TFirst, typename TSecond >
	class PairVector : public std::vector< std::pair< TFirst, TSecond > >
	{
		public:
			using PairType = std::pair< TFirst, TSecond >;
			using ParentType = std::vector< PairType >;

			using typename ParentType::iterator;
			using typename ParentType::const_iterator;
			using ParentType::size;
			using ParentType::begin;
			using ParentType::end;
			using ParentType::cbegin;
			using ParentType::cend;


		//---ctor, dtor
			PairVector() = default;
			virtual ~PairVector() = default;
			PairVector( const PairVector& rhs ) = default;
			PairVector( PairVector&& rhs ) noexcept = default;
			PairVector& operator=( const PairVector& rhs ) = default;
			PairVector& operator=( PairVector&& rhs ) noexcept = default;

		//---get

		//---set

		//---API
			inline const_iterator cfind( const TFirst& key ) const { for( auto it = cbegin(); it != cend(); it++ ) { if( it->first == key ) return it; } return cend(); }
			inline iterator find( const TFirst& key ) { for( auto it = begin(); it != end(); it++ ) { if( it->first == key ) return it; } return end(); }
			inline const_iterator cfindReverse( const TSecond& val ) const { for( auto it = cbegin(); it != cend(); it++ ) { if( it->second == val ) return it; } return cend(); }
			inline iterator findReverse( const TSecond& val ) { for( auto it = begin(); it != end(); it++ ) { if( it->second == val ) return it; } return end(); }

			inline const TSecond& clookup( const TFirst& key ) const { return cfind( key )->second; }
			inline TSecond& lookup( const TFirst& key ) { return find( key )->second; }
			inline const TFirst& clookupReverse( const TSecond& val ) const { return cfindReverse( val )->first; }
			inline TFirst& lookupReverse( const TSecond& val ) { return findReverse( val )->first; }

			inline bool isFound( const TFirst& key ) const { return cfind( key ) != cend(); }
			inline bool isFoundReverse( const TSecond& val ) const { return cfindReverse( val ) != cend(); }
			inline size_t count( const TFirst& key ) const { return static_cast<size_t>( isFound( key ) ); }
			inline size_t countReverse( const TSecond& val ) const { return static_cast<size_t>( isFoundReverse( val ) ); }

			inline size_t key2idx( const TFirst& key ) const { return cfind( key ) - cbegin(); }
			inline size_t val2idx( const TSecond& val ) const { return cfindReverse( val ) - cbegin(); }
	};
}

#endif //UT_PAIRVECTOR_HPP