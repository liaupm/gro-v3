#ifndef UT_THRESHOLDVECTOR_HPP
#define UT_THRESHOLDVECTOR_HPP

#include "ut/defines.hpp"
#include "ut/PairVector.hpp" //base

/*PRECOMPILED
#include <vector> //DSType */


namespace ut
{
    template< typename TFirst, typename TSecond > 
    class ThresholdVector : public PairVector< TFirst, TSecond >
    {
        public:
            using UnitType = std::pair< TFirst, TSecond >;
            using DSType = std::vector< UnitType >;


        //---ctor, dtor
            ThresholdVector() = default;
            virtual ~ThresholdVector() = default;

        //---API
            size_t input2index( TReal input ) const;
            inline const TSecond& retrieveAtIndex( size_t idx ) const { assert( idx < this->size() ); return this->at( idx ).second; }
            inline TSecond& retrieveAtIndexEdit( size_t idx ) { assert( idx < this->size() ); return this->at( idx ).second; }
            inline const TSecond& retrieve( TReal input ) const { return retrieveAtIndex( input2index( input ) ); }
            inline TSecond& retrieveEdit( TReal input ) { return retrieveAtIndexEdit( input2index( input ) ); }
    };




////////////////////////////////////////////// METHOD DEFINITIONS //////////////////////////////////////////////////

//----------ThresholdVector
    template< typename TFirst, typename TSecond > 
    inline size_t ThresholdVector< TFirst, TSecond >::input2index( TReal input ) const
    {
    	if( this->size() > 0 )
    	{
	    	for( int i = this->size() - 1; i >= 0; i-- )
	        {
	            if( input >= this->at( i ).first )
	                return i;
	        }
    	}
        return this->size();
    }
}

#endif //UT_THRESHOLDVECTOR_HPP
