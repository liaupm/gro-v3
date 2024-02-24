#ifndef UT_DEFINES_HPP
#define UT_DEFINES_HPP

//PRECOMPILED
/*#include <memory> //smart pointers
#include <random> //RandomEngine
#include <assert.h> //exceptions
#include <iostream> //debugging messages

#include <set>
#include <vector>
#include <array>
#include <map>

#include <cmath>
#include <algorithm>
#include <utility>
#include <type_traits>
#include <sstream>
#include <stdexcept> */


//types
using RealNumType = double;
using TReal = RealNumType;
using uint = unsigned int;
using ulong = unsigned long;
using ulong2 = unsigned long long;
using RandomEngine = std::minstd_rand;
using RandomEngine2 = std::mt19937_64;

//ptrs
template<typename T> 
using UP = std::unique_ptr<T>;

template<typename T> 
using CUP = std::unique_ptr< const T>;

template<typename T> 
using SP = std::shared_ptr<T>;

template<class T>
using CSP = std::shared_ptr<const T>;


//macros and operators
constexpr size_t operator "" _st ( ulong2 val ) { return val; } //cast to size_t by adding the "_st" sufix to a number (like 1_st)

#define utQuote(x) #x

#define _utUnused(x) ( (void) (x) )

namespace ut
{
/////////////////////////////////////// CONSTANTS /////////////////////////////////////////////////////
  	
  	constexpr double THOUSAND = 1'000;
  	constexpr double MILLION = 1'000'000;
	constexpr double BILLION = 1'000'000'000;

	constexpr double VERY_LITTLE_1 = 0.0000001;
	constexpr double VERY_LITTLE_2 = 0.0000002;
	constexpr double VERY_LITTLE = VERY_LITTLE_1;
	
	constexpr double PI = 3.14159265358979323846;



/////////////////////////////////////// SFINAE /////////////////////////////////////////////////////
	
	template< typename T, typename ... TBases > 
	struct AnyIsBase
	{ 
	  inline static constexpr bool value = ( std::is_base_of_v< std::decay_t< TBases >, std::decay_t< T > > || ... );
	  using type = typename std::enable_if_t< value, T >;
	};

	template< typename T, typename ... TBases > 
	struct NoneIsBase
	{ 
	  inline static constexpr bool value = ( ( ! std::is_base_of_v< std::decay_t< TBases >, std::decay_t< T > > ) && ... );
	  using type = typename std::enable_if_t< value, T >;
	};

	template< typename T, typename ... TBases > 
	using EnableIfAnyIsBase = typename AnyIsBase< T, TBases... >::type;
	
	template< typename T, typename ... TBases > 
	using EnableIfNoneIsBase = typename NoneIsBase< T, TBases... >::type;

	template< typename TBase, typename TDerived >
	using EnableForDerived = std::enable_if_t< std::is_base_of_v< std::decay_t< TBase >, std::decay_t< TDerived > >, TDerived >;
	
	template< typename TBase, typename TDerived >
	using EnableForNonDerived = std::enable_if_t< ! std::is_base_of_v< std::decay_t< TBase >, std::decay_t< TDerived > >, TDerived >;



//////////////////////////////// EXCEPTIONS /////////////////////////////////////////////////////////////
	inline void throwMessage( const std::string& msg ) 
	{
		std::stringstream strm;
		strm << msg;
		throw ( strm.str() );
	}
		    
    

    

//////////////////////////////// FUNCTIONS FOR SIMPLE VALUES ////////////////////////////////////////////

	constexpr bool isZero( TReal val ) { return std::abs( val ) < VERY_LITTLE; }
	constexpr bool notZero( TReal val ) { return std::abs( val ) > VERY_LITTLE; }
	constexpr bool isPositive( TReal val ) { return val > VERY_LITTLE; }
	constexpr bool isNegative( TReal val ) { return val < -VERY_LITTLE; }
	constexpr bool areSame( TReal val1, TReal val2 ) { return std::abs( val1 - val2 ) < VERY_LITTLE; }
	constexpr bool areSame( TReal val1, TReal val2, TReal sensitivity ) { return std::abs( val1 - val2 ) < sensitivity; }
	constexpr bool notSame( TReal val1, TReal val2 ) { return ! areSame( val1, val2 ); }
	constexpr bool notSame( TReal val1, TReal val2, TReal sensitivity ) { return ! areSame( val1, val2, sensitivity ); }

	constexpr bool relativeChange( TReal oldVal, TReal newVal, TReal sensitivity )
	{ 
		if( oldVal == 0.0 )
		{
			if( newVal == 0.0 )
				return false;
			return std::abs( oldVal - newVal ) / newVal >= sensitivity;
		}
		return std::abs( oldVal - newVal ) / oldVal >= sensitivity; 
	}

	template< typename T >
	constexpr bool isEven( T val ) { return val % 2 == 0; }
	template< typename T >
	constexpr int sign( const T& val ) { return ( val >= T( 0 ) ) - ( val < T( 0 ) ); }
	template< typename T >
	constexpr bool sign2bool( T val ) { return val > 0; }

	template< typename T >
	constexpr T fit( T val, T lbound = 0.0, T ubound = 1.0 ) { return std::min( std::max( val, lbound ), ubound ); }
	template< typename T >
	constexpr T fitL( T val, T lbound = 0.0 ) { return std::max( val, lbound ); }
	template< typename T >
	constexpr T fitU( T val, T ubound = 1.0 ) { return std::min( val, ubound ); }

	template<typename TOut, typename TIn>
	constexpr TOut round( TIn val ) 
	{ 
		if( val > std::numeric_limits<TIn>::max() - 0.5 || val > std::numeric_limits<TOut>::max() ) //overflow control
			return std::numeric_limits<TOut>::max();
		if( val < std::numeric_limits<TOut>::lowest() )
			return std::numeric_limits<TOut>::lowest();
		return static_cast<TOut>( val + 0.5 ); 
	}

	template< typename T >
	constexpr T trimDecimals( T inVal, T multiplier ) { return static_cast<T>( round<long long int>( inVal * multiplier ) ) / multiplier; }

	template< typename T1, typename T2, typename TWeight >
	constexpr auto weightedSum( T1 val1, T2 val2, TWeight weight ) { return weight * val1 + ( 1.0 - weight ) * val2; }




//////////////////////////////// FUNCTIONS FOR CONTAINERS ////////////////////////////////////////////

//=============================================== set
	template< typename T >
	inline bool isAnyOf( T val, std::set<T> cases ) { return cases.count( val ); }



//=============================================== vector
	template< typename T >
	inline bool isInVector( const std::vector<T>& vect, const T& item ) { return std::find( vect.begin(), vect.end(), item ) != vect.end(); }

	template< typename T >
	inline size_t elem2idx( const std::vector<T>& vect, const T& elem ) { return std::find( vect.begin(), vect.end(), elem ) - vect.begin(); }

//---asserts
	template< typename T1, typename T2 >
	inline void assertEqualSize( const std::vector<T1>& vect1, const std::vector<T2>& vect2 ) { assert( vect1.size() == vect2.size() ); }

	template< typename T >
	inline void assertRange( size_t start, size_t end, const std::vector<T>& vect ) { assert( end - start <= vect.size() ); }

	template< typename T >
	inline bool anyNotNull( const std::vector<T>& container )
	{
		for( size_t i = 0; i < container.size(); i++ )
		{
			if( container[i] != T(0.0) )
				return true;
		}
		return false;
	}

//---
	template< typename T > 
	inline T cheapPop( std::vector<T>& container, std::size_t index ) 
	{ 
		T element( std::move( container[index] ) );
		container[index] = container.back();
		container.pop_back();
		return element;
	}

	template< typename T > 
	inline void cheapErase( std::vector<T>& container, size_t index ) 
	{ 
		container[index] = container.back();
		container.pop_back();
	}

//---
	template< typename TOut, typename TIn >
	inline std::vector<TOut> castVector( const std::vector<TIn>& inVector )
	{
		std::vector<TOut> outVector;
		for( const auto& elem : inVector )
			outVector.push_back( static_cast<TOut>( elem ) );
		return outVector;
	}

	template< typename T > //a real number
	inline std::vector<T> normalizeVector( const std::vector<T>& inVector )
	{
		T total = 0.0;
		for( const auto& elem : inVector )
			total += elem;

        std::vector<T> outVector;
		for( const auto& elem : inVector )
			outVector.push_back( elem / total );
		
		return outVector;
	}

	template< typename TElem >
	inline std::vector<TElem> vectorUnion( const std::vector<TElem>& vect1, const std::vector<TElem>& vect2 )
	{
		std::vector<TElem> outVector = vect1;
		for( const auto& elem : vect2 )
		{
			if( ! isInVector( outVector, elem ) )
				outVector.push_back( elem );
		}
		return outVector;
	}

	template< typename TElem >
	inline void unionToVector( std::vector<TElem>& vect1, const std::vector<TElem>& vect2 )
	{
		for( const auto& elem : vect2 )
		{
			if( ! isInVector( vect1, elem ) )
				vect1.push_back( elem );
		}
	}

	template< typename T > 
	inline void appendVector( std::vector<T>& a, const std::vector<T>& b ) { a.insert( a.end(), b.begin(), b.end() ); }

//---
	template< typename T >
	inline std::string toString( const std::vector<T>& vect, const std::string& separator = "-" )
	{
		if( vect.size() == 0 )
			return "";
	    std::string key = std::to_string( vect[0] );
	    for( size_t i = 1; i < vect.size(); i++ )
	        key += separator + std::to_string( vect[i] );
	    return key;
	}

	inline std::string toString( const std::vector<std::string>& vect, const std::string& separator = "-" )
	{
		if( vect.size() == 0 )
			return "";
	    std::string key = vect[0];
	    for( size_t i = 1; i < vect.size(); i++ )
	        key += separator + vect[i];
	    return key;
	}



//=============================================== map
	template< typename TKey, typename TValue >
	inline std::vector<TKey> getMapKeys( const std::map<TKey, TValue>& map )
	{
		std::vector<TKey> keys;
		for( const auto& pair : map )
			keys.push_back( pair.first );
		return keys;
	}

	template< typename TKey, typename TValue >
	inline std::vector<TKey> getMapValues( const std::map<TKey, TValue>& map )
	{
		std::vector<TKey> values;
		for( const auto& pair : map )
			values.push_back( pair.second );
		return values;
	}




//////////////////////////////// STRINGS AND PRINT ////////////////////////////////////////////

	inline void toLower( std::string& str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::tolower );
	}

	inline std::string copyToLower( const std::string& str )
	{
		std::string result = str;
		std::transform( result.begin(), result.end(), result.begin(), ::tolower );
		return result;
	}

	inline void toUpper( std::string& str )
	{
		std::transform( str.begin(), str.end(), str.begin(), ::toupper );
	}

	inline std::string copyToUpper( const std::string& str )
	{
		std::string result = str;
		std::transform( result.begin(), result.end(), result.begin(), ::toupper );
		return result;
	}

	inline void print() { std::cout << "\n"; }

	template< typename T, typename... Ts >
	inline void print( T firstArg, Ts... args )
	{
		std::cout << firstArg << " ";
		print( args... );
	}

	inline void errorPrint() { std::cerr << "\n"; }

	template< typename T, typename... Ts >
	inline void errorPrint( T firstArg, Ts... args )
	{
		std::cerr << firstArg << " ";
		errorPrint( args... );
	}


	template< typename T >
	inline void printVector( const std::vector<T>& container )
	{
		ut::print( "vector of size ", container.size(), " elements: " );
		for( size_t i = 0; i < container.size(); i++ )
			ut::print( "-", i, ": ", container[i] );
	}
}

#endif //UT_DEFINES_HPP
