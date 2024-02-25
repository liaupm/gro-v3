#include "ut/Bitset.hpp"

using namespace ut;


bool Bitset::anyInRange( size_t start, size_t end ) const
{
    for( size_t i = start; i < end; i++ )
    {
        if( checkBit( i ) )
            return true;
    }
    return false;
}

bool Bitset::allInRange( size_t start, size_t end ) const 
{
    for( size_t i = start; i < end; i++ )
    {
        if( ! checkBit( i ) )
            return false;
    }
    return true;
}