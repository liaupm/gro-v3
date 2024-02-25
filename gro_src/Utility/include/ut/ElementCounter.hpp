#ifndef UT_ELEMENTCOUNTER_HPP
#define UT_ELEMENTCOUNTER_HPP

#include "defines.hpp"
#include "ut/PairVector.hpp" //base
#include "ut/StaticOperators.hpp" //Op


namespace ut
{
	template< typename TElem, typename TVal = int >
	class ElementCounter : public PairVector< TElem, TVal >
	/* a PairVEctor where first are the elements and second the number of them
	when an element is added, its counter is increased if already present
	or a new entry is created otherwise */
	{
		public:
			using KeyType = TElem;
			using ParentType = PairVector< TElem, TVal >;
			using Op = StaticOperators< TVal >;
			
			using typename ParentType::PairType;
			using typename ParentType::iterator;
			using typename ParentType::const_iterator;
		  
		  // methods from std::vector
			using ParentType::size; using ParentType::back; using ParentType::begin; using ParentType::cbegin; using ParentType::end; using ParentType::cend;
			using ParentType::find; using ParentType::cfind; using ParentType::emplace_back;

		//methods from PairVector
			using ParentType::isFound; using ParentType::lookup; using ParentType::clookup; using ParentType::key2idx;
            
			template< typename T >
			using EnableForPairOrCounter = EnableIfAnyIsBase< T, PairType, ElementCounter >; //for individual pairs or whole containers


		//---ctor, dtor
			ElementCounter() = default;
			ElementCounter( const std::vector<KeyType>& elems ) { for( const auto& key : elems ) add( key ); }

			virtual ~ElementCounter() = default;
			ElementCounter( const ElementCounter& rhs ) = default;
			ElementCounter( ElementCounter&& rhs ) noexcept = default;
			ElementCounter& operator=( const ElementCounter& rhs ) = default;
			ElementCounter& operator=( ElementCounter&& rhs ) noexcept = default;

		//---get
			TVal getCount( const TElem& key ) const { return clookup( key ); }

		//---API
		  //check
			inline bool isNull( KeyType key ) const { return clookup( key ) == 0; }
			inline bool isNullIdx( size_t idx ) const { assert( idx < size() ); return ( *this )[ idx ] == 0; }
		  //modofy
			bool add( KeyType key, TVal amount = 1 );
			inline void removeIfNull( KeyType key ) { if( isNull( key ) ) ut::cheapErase( (*this), key2idx( key ) ); }
			inline void removeIfNullIdx( size_t idx ) { if( isNullIdx( idx ) ) ut::cheapErase( (*this), idx ); }


		//---own operators (assigment)
			template< typename TOp >
		    void assigmentOperator( const ElementCounter& rhs );

		    template< typename TOp >
		    void assigmentOperator( const TVal& rhs );

            template< typename T, typename = EnableForPairOrCounter<T> >
            inline void operator+=( const T& rhs ) { assigmentOperator< typename Op::SumAssign >( rhs ); }
            template< typename T, typename = EnableForPairOrCounter<T> >
            inline void operator-=( const T& rhs ) { assigmentOperator< typename Op::DiffAssign >( rhs ); }
            template< typename T, typename = EnableForPairOrCounter<T> >
            inline void operator*=( const T& rhs ) { assigmentOperator< typename Op::MultAssign >( rhs ); }
            template< typename T, typename = EnableForPairOrCounter<T> >
            inline void operator/=( const T& rhs ) { assigmentOperator< typename Op::DivAssign >( rhs ); }

            
          //---friend operators (binary)
            template< typename TOp, typename TRhs >
		    static ElementCounter binaryOperator( const ElementCounter& lhs, const TRhs& rhs );

            inline friend ElementCounter operator-( const ElementCounter& rhs ) { return Op::unaryOperator< Op::Reverse >( rhs ); }
            inline friend ElementCounter operator+( const ElementCounter& lhs, const ElementCounter& rhs ) { return binaryOperator< typename Op::Sum >( lhs, rhs ); }
            inline friend ElementCounter operator-( const ElementCounter& lhs, const ElementCounter& rhs ) { return binaryOperator< typename  Op::Diff >( lhs, rhs ); }
            inline friend ElementCounter operator*( const ElementCounter& lhs, const ElementCounter& rhs ) { return binaryOperator< typename  Op::Mult >( lhs, rhs ); }
            inline friend ElementCounter operator/( const ElementCounter& lhs, const ElementCounter& rhs ) { return binaryOperator< typename  Op::Div >( lhs, rhs ); }

            inline friend ElementCounter operator+( const ElementCounter& lhs, const TVal& rhs ) { return binaryOperator< typename Op::Sum >( lhs, rhs ); }
            inline friend ElementCounter operator-( const ElementCounter& lhs, const TVal& rhs ) { return binaryOperator< typename  Op::Diff >( lhs, rhs ); }
            inline friend ElementCounter operator*( const ElementCounter& lhs, const TVal& rhs ) { return binaryOperator< typename  Op::Mult >( lhs, rhs ); }
            inline friend ElementCounter operator/( const ElementCounter& lhs, const TVal& rhs ) { return binaryOperator< typename  Op::Div >( lhs, rhs ); }
	};





/////////////////////////////////////////////////////////////////////// METHOD DEFINITIONS ////////////////////////////////////////////////////////////
	template< typename TElem, typename TVal >
	inline bool ElementCounter< TElem, TVal >::add( KeyType key, TVal amount ) 
    {
        if( amount == 0 )
            return false;
        
        if( isFound( key ) ) 
        { 
            lookup( key ) += amount;
            removeIfNull( key );
            return true; 
        } 
        else 
        { 
            emplace_back( key, amount ); 
            return false; 
        }
    }


	template< typename TElem, typename TVal >
    template< typename TOp >
    void ElementCounter< TElem, TVal >::assigmentOperator( const ElementCounter< TElem, TVal >& rhs )
    { 
    	assert( size() == rhs.size() );

    	for( const PairType& pair : rhs )
    	{
    		iterator it = find( pair.first );
    		if( it == end() )
    		{
    			emplace_back( pair.first, TOp::nullVal );
    			TOp::call( back().second, pair.second );
    		}
    		else
    			TOp::call( it->second, pair.second );
    	}
    }

    template< typename TElem, typename TVal >
    template< typename TOp >
    void ElementCounter< TElem, TVal >::assigmentOperator( const TVal& rhs )
    { 
    	for( PairType& pair : (*this) )
    		TOp::call( pair.second, rhs );
    }


    template< typename TElem, typename TVal >
    template< typename TOp, typename TRhs >
    ElementCounter< TElem, TVal > ElementCounter< TElem, TVal >::binaryOperator( const ElementCounter< TElem, TVal >& lhs, const TRhs& rhs )
    {
        ElementCounter< TElem, TVal > result = lhs;
        result.assigmentOperator< typename TOp::AssigmentType >( rhs );
        return result;
    }
}

#endif //UT_ELEMENTCOUNTER_HPP