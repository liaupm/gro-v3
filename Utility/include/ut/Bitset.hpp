#ifndef UT_BITSET_HPP
#define UT_BITSET_HPP

#include "ut/defines.hpp"
#include "ut/StaticOperators.hpp" //custom bitwise functions

/*PRECOMPILED
#include <vector> //using UnderlyingDS = std::vector<uint64_t>
#include <string> //std::string toString() const
#include <cstdint> //WordType = uint64_t
#include <algorithm> //std::any_of, std::all_of in get methods
#include <type_traits> // std::enable_if_t, std::is_base_of, std::decay_t for SFINAE in constructor
#include <math.h> //std::log2 */


namespace ut
{
    class Bitset
    {
        public: 
        //---static 
            using WordType = std::uint64_t;
            using UnderDS = std::vector< WordType >; //container of several WordType elements to emulate bit registers bigger than the size of WordType
            using IndexesDS = std::vector< size_t >;
            using Op = StaticOperators< WordType>;

            template< typename T >
            using EnableIfUnderDS = EnableForDerived< UnderDS, T >; //for SFINAE

          //consts
            static constexpr short WORD_SIZE = sizeof( WordType ) * CHAR_BIT; //size in bits of the type used as basic bit register
            static constexpr uint SHIFT_SIZE = std::log2( WORD_SIZE ); //number of bits shifted to efficiently multiply (divide) by the word size

          //bitwise functions from StaticOperators
            static constexpr auto allZeros = Op::BitwiseOp::allZeros;
            static constexpr auto allOnes = Op::BitwiseOp::allOnes;
            static constexpr auto before = Op::BitwiseOp::before;
            static constexpr auto after = Op::BitwiseOp::after;
            static constexpr auto between = Op::BitwiseOp::between;
            static constexpr auto index2flag = Op::BitwiseOp::index2flag;
            static constexpr auto index2ReverseFlag = Op::BitwiseOp::index2ReverseFlag;

          //checks
            inline static constexpr size_t bit2word( size_t bit ) noexcept { return bit / WORD_SIZE; }
            inline static constexpr size_t bit2offset( size_t bit ) noexcept { return bit % WORD_SIZE; }
            inline static constexpr size_t bitNum2wordNum( size_t bitNum ) noexcept { return bitNum == 0 ? 0 : ( bitNum - 1 )/ WORD_SIZE + 1; }
            inline static constexpr size_t wordNum2bitNum( size_t wordNum ) noexcept { return wordNum * WORD_SIZE; }
            inline static constexpr bool sameWord( size_t bit1, size_t bit2 ) noexcept { return bit2word( bit1 ) == bit2word( bit2 ); }

          //make Bitset
            inline static Bitset zeros( size_t bitNum ) noexcept { return Bitset( bitNum ); }
            inline static Bitset ones( size_t bitNum ) noexcept { return ~Bitset( bitNum ); }
            template< typename TVal >
            static Bitset makeFromVals( const std::vector< TVal >& vals ) noexcept { Bitset result( vals.size() ); result.setAll< typename Op::BitAssign >( vals ); return result; }
            inline static Bitset makeFromIds( const IndexesDS& ids ) noexcept { Bitset result( ids.size() ); result.setIndexes< typename Op::BitTrueAssign >( ids ); return result; }
            template< typename TElem >
            static Bitset makeFromElems( const std::vector< const TElem* >& elems, size_t bitNum ) noexcept { Bitset result( bitNum ); result.setElems< typename Op::BitTrueAssign >( elems ); return result; }
            inline static Bitset rangeMask( size_t bitNum, size_t start, size_t end ) noexcept { Bitset result( bitNum ); result.setRangeCheap< typename Op::BitTrueAssign >( start, end ); return result; }
            inline static Bitset outRangeMask( size_t bitNum, size_t start, size_t end ) noexcept { return ~rangeMask( bitNum, start, end ); }

          //static generic operators  
            template< typename TOp >
            static Bitset unaryOperator( const Bitset& obj ) { return Bitset( Op::unaryOperator< TOp >( obj.getWords() ), obj.getBitNum() ); }

            template< typename TOp >
            static Bitset binaryOperator( const Bitset& obj1, const Bitset& obj2 ) { return Bitset( Op::binaryOperator<TOp>( obj1.getWords(), obj2.getWords() ), obj1.getBitNum() ); }


        //----nested classes
            struct BitRef //inspired in STL reference form std::vector<bool>
            {
                friend Bitset;

                WordType& word;
                short offset;
                WordType offsetMask;

                ~BitRef() = default;

                operator bool() const noexcept { return word & offsetMask; } //implicit conversion to bool
                BitRef& operator=( const BitRef& rhs ) noexcept { return operator=( static_cast<bool>( rhs ) ); }; // assign from another bitref

                template< typename TOp >
                bool unaryOperator( bool val = true ) { return static_cast<bool>( TOp::callSingle( word, offset, val ) & offsetMask ); }

                template< typename TOp >
                BitRef& assigmentOperator( bool val = true ) { TOp::callSingle( word, offset, val ); return *this; }

                inline BitRef& operator=( bool val ) { return assigmentOperator< typename Op::BitAssign >( val ); }
                inline BitRef& operator|=( bool val ) { return assigmentOperator< typename Op::BitOrAssign >( val ); }
                inline BitRef& operator&=( bool val ) { return assigmentOperator< typename Op::BitAndAssign >( val ); }
                inline BitRef& operator^=( bool val ) { return assigmentOperator< typename Op::BitXorAssign >( val ); }

                inline BitRef& flip() { return assigmentOperator<typename Op::BitNegAssign>(); }
                inline bool operator~() { return unaryOperator<typename Op::BitNeg>(); }

                private: 
                    BitRef( Bitset::WordType& word, short offset ) noexcept // no public constructor (only Bitset can create BitRef )
                    : word( word ), offset( offset ), offsetMask( Bitset::index2flag( offset, true ) ) {;}

                    void operator&(); // not defined
            };


        //---ctor, dtor
            inline Bitset( const UnderDS& words, size_t bitNum ) : words( words ), bitNum( bitNum ), wordNum( words.size() ) {;} //intentionally not explicit
            inline explicit Bitset( size_t bitNum = 0 ) : words( bitNum2wordNum( bitNum ), 0 ), bitNum( bitNum ), wordNum( words.size() ) {;}

            inline explicit Bitset( size_t bitNum, size_t bitOn ) 
            : words( bitNum2wordNum( bitNum ), 0 ), bitNum( bitNum ), wordNum( words.size() ) { setOn( bitOn ); }      
            
            inline explicit Bitset( size_t bitSize, const IndexesDS& indexesOn ) 
            : words( bitNum2wordNum( bitSize ), 0 ), bitNum( words.size() * WORD_SIZE ), wordNum( words.size() ) { setIndexes< typename Op::BitTrueAssign >( indexesOn ); }

            virtual ~Bitset() = default;
            Bitset( const Bitset& rhs ) = default;
            Bitset( Bitset&& rhs ) noexcept = default;
            Bitset& operator=( const Bitset& rhs ) = default;
            Bitset& operator=( Bitset&& rhs ) noexcept = default;
      
        //---get
            inline const UnderDS& getWords() const { return words; } //this must keep encapsulated
            inline size_t getWordNum() const { return words.size(); }
          //global  
            inline size_t getBitNum() const { return bitNum; }
            inline size_t bitSize() const { return getBitNum(); } //an alias
            inline size_t npos() const { return getBitNum(); } //another alias
          //individual bits  
            inline bool checkBit( size_t bit ) const { assertBit( bit ); return getWordOfBit( bit ) & index2flag( bit2offset( bit ), true ); } //const checking (read only)
            inline bool operator[]( size_t bit ) const { return checkBit( bit ); } //operator alias
            inline BitRef getBit( size_t bit ) { assertBit( bit ); return BitRef( getWordOfBitEditable( bit ), bit2offset( bit ) ); } //non-const checking (read and write)
            inline BitRef operator[]( size_t bit ) { return getBit( bit ); } //operator alias
          //all the bits
          inline bool isEmpty() const { return words.size() == 0; }  
            inline bool none() const { return std::all_of( words.begin(), words.end(), []( const WordType& word ){ return word == allZeros(); } ); }
            inline bool all() const { return std::all_of( words.begin(), words.end(), []( const WordType& word ){ return word == allOnes(); } ); }
            inline bool any() const { return ! none(); }
            inline IndexesDS getAllIndexes( bool val = true ) const { IndexesDS result{}; for( size_t b = 0; b < npos(); b++ ) { if( checkBit( b ) == val ) result.push_back( b ); } return result; }
          //range
            inline IndexesDS getIndexesInRange( size_t start, size_t end, bool val = true ) const { assertRange( end ); IndexesDS result{}; for( size_t b = start; b < end; b++ ) { if( checkBit( b ) == val ) result.push_back( b ); } return result; }
            inline Bitset getRange( size_t start, size_t end, bool val = false ) const { Bitset result( *this ); result.setComplementaryRange< typename Op::BitAssign >( start, end, val ); return result; }
            bool anyInRange( size_t start, size_t end ) const;
            bool allInRange( size_t start, size_t end ) const;
            inline bool noneInRange( size_t start, size_t end ) const { return ! allInRange( start, end ); }

        //---set
          //individual bits
            template< typename TOp = typename Op::BitAssign, typename TVal = bool >
            inline void setSingle( size_t bit, TVal val = true ) { assertBit( bit ); TOp::callSingle( getWordOfBitEditable( bit ), bit2offset( bit ), static_cast<bool>( val ) ); }

            inline void setBit( size_t bit, bool val ) { setSingle< typename Op::BitAssign >( bit, val ); } 
            inline void setOn( size_t bit ) { setSingle< typename Op::BitTrueAssign >( bit ); }
            inline void setOff( size_t bit ) { setSingle< typename Op::BitFalseAssign >( bit ); }
            inline void setAnd( size_t bit, bool val ) { setSingle< typename Op::BitAndAssign >( bit, val ); }
            inline void setOr( size_t bit, bool val ) { setSingle< typename Op::BitOrAssign >( bit, val ); }
            inline void flipBit( size_t bit ) { setSingle< typename Op::BitNegAssign >( bit ); }
            inline bool getAndFlipBit( size_t bit ) { bool val = checkBit( bit ); flipBit( bit ); return val; }
          //all the bits
            template< typename TOp = typename Op::BitAssign >
            inline void setAll( WordType val = allZeros() ) { for( auto& w : words ) TOp::call( w, val ); }
            template< typename TOp = typename Op::BitAssign, typename T >
            inline void setAll( const std::vector<T>& vals ) { assert( vals.size() == bitNum ); for( size_t i = 0; i < npos(); i++ ) setSingle( i, vals[i] ); }
            inline void setAllOn() { setAll< typename Op::BitTrueAssign >(); }
            inline void reset() { if( words.size() > 0 ) words[0] = 0; setAll< typename Op::BitFalseAssign >(); }
            //functions for setAnd, setOr and flip are not required as there are operators for those
          //indexes
            template< typename TOp = typename Op::BitAssign >
            inline void setIndexes( const IndexesDS& bits, bool val = true ) { for( auto& bit : bits ) setSingle( bit, val ); }
            template< typename TOp = typename Op::BitAssign >
            inline void setIndexes( const IndexesDS& bits, const std::vector<bool>& vals ) { for( size_t i = 0; i < bits.size(); i++ ) setSingle( i, vals[i] ); }
            inline void flipIndexes( const IndexesDS& bits ) { setIndexes<typename Op::BitNegAssign>( bits ); }

            template< typename TOp = typename Op::BitAssign, typename TElem, typename TVal = bool >
            inline void setElems( const std::vector<const TElem*>& elems, TVal val = true ) { for( const TElem* elem : elems ) setSingle( elem->getAbsoluteId(), val ); }
            template< typename TOp = typename Op::BitAssign, typename TElem, typename TVal = bool >
            inline void setElems( const std::vector<const TElem*>& elems, const std::vector<TVal>& vals ) { assert( elems.size() == vals.size() ); for( size_t e = 0; e < elems.size(); e++ ) setSingle( elems[e]->getAbsoluteId(), static_cast<bool>( vals[e] ) ); }

          //range
            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setRange( size_t start, size_t end, bool val = true ) { assertRange( end ); for( size_t i = start; i < end; i++ ) TOp::callSingle( getWordOfBitEditable( i ), bit2offset( i ), val ); }
            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setRange( size_t start, size_t end, const std::vector<bool>& vals ) 
            { assertRange( end ); ut::assertRange( start, end, vals ); for( size_t i = start; i < end; i++ ) TOp::callSingle( getWordOfBitEditable( i ), bit2offset( i ), vals[i] ); }
            inline void resetRange( size_t start, size_t end ) { setRange<Op::BitFalseAssign>( start, end ); }

            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setRangeWholeWords( size_t start, size_t end, WordType val = allOnes() ) { assert( end <= wordNum ); for( size_t w = start; w < end; w++ ) TOp::call( words[w], val ); }
            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setRangeWholeWords( size_t start, size_t end, const std::vector<WordType>& vals ) { assertRange( end <= wordNum ); ut::assertRange( start, end, vals ); for( size_t w = start; w < end; w++ ) TOp::call( words[w], vals[w] ); }
            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setRangePartialWord( WordType& word, short start, short end, WordType val = allOnes() ) { assert( end <= WORD_SIZE ); TOp::callMasked( word, val, between( start, end ) ); }

            template< typename TOp = typename Op::BitTrueAssign > 
            void setRangeCheap( size_t start, size_t end, WordType val = allOnes() );

            inline void flipRange( size_t start, size_t end ) { setRange< typename Op::BitNegAssign >( start, end ); }
            template< typename TOp = typename Op::BitTrueAssign > 
            inline void setComplementaryRange( size_t start, size_t end, bool val = true ) { assertRange( end ); setRange<TOp>( 0, start, val ); setRange<TOp>( end, npos(), val ); }
            
        //---API
          //misc
            inline size_t next( size_t start, size_t end, bool val = true ) const { for( size_t i = start; i < end; i++ ) { if( checkBit( i ) == val ) return i; } return npos(); }
            inline size_t next( size_t start, bool val = true ) const { return next( start, npos(), val ); }
            inline size_t first( size_t end, bool val = true ) const { return next( 0, end, val); }
            inline size_t first( bool val = true ) const { return next( 0, npos(), val); }

            inline std::string toString( const std::string& separator = "." ) const { return ut::toString( words, separator ); }

        //---own operators
            template< typename TOp >
            void assigmentOperator( const Bitset& obj ) { assert( wordNum == obj.wordNum ); for( size_t i = 0; i < wordNum; i++ ) TOp::call( words[i], obj.words[i] ); }
    
            inline void operator&=( const Bitset& obj ) { assigmentOperator< typename Op::BitAndAssign >( obj ); }
            inline void operator|=( const Bitset& obj ) { assigmentOperator< typename Op::BitOrAssign >( obj ); }
            inline void operator^=( const Bitset& obj ) { assigmentOperator< typename Op::BitXorAssign >( obj ); }

            inline friend Bitset operator~( const Bitset& rhs ) { return unaryOperator< typename Op::BitNeg >( rhs ); }
            inline bool operator!() const { return none(); }

        //---friend operators (use static functions )
            inline friend Bitset operator&( const Bitset& obj1, const Bitset& obj2 ) { return Bitset::binaryOperator< typename Op::BitAnd >( obj1, obj2 ); }
            inline friend Bitset operator|( const Bitset& obj1, const Bitset& obj2 ) { return Bitset::binaryOperator< typename Op::BitOr >( obj1, obj2 ); }
            inline friend Bitset operator^( const Bitset& obj1, const Bitset& obj2 ) { return Bitset::binaryOperator< typename Op::BitXor >( obj1, obj2 ); }

            inline friend bool operator==( const Bitset& obj1, const Bitset& obj2 ) { return obj1.words == obj2.words; }
            inline friend bool operator!=( const Bitset& obj1, const Bitset& obj2 ) { return obj1.words != obj2.words; }
            inline friend bool operator<( const Bitset& obj1, const Bitset& obj2 ) { return obj1.words < obj2.words; }


        protected:
            UnderDS words;
            size_t bitNum;
            size_t wordNum;

            inline const WordType& getWordOfBit( size_t bit ) const { return words[ bit2word( bit ) ]; } 
            inline WordType& getWordOfBitEditable( size_t bit ) { return words[ bit2word( bit ) ]; } 

            inline void assertBit( size_t bit ) const { assert( bit < npos() ); _utUnused( bit ); }
            //inline void assertBit( size_t bit ) const { if( bit < npos() ) controlTrue(); else controlFalse(); }

            template< typename T >
            inline void assertSize( const std::vector<T>& vect ) const { assert( vect.size() == bitNum ); }
            inline void assertRange( size_t end ) const { assert( end <= npos() ); _utUnused( end ); }


            //inline void controlTrue() const { ut::print( "good size "); }
            //inline void controlFalse() const { ut::print( "bad size "); }
    };




//////////////////////////////////////////////////// METHOD DEFINITIONS //////////////////////////////////////////////////////////////////
  //---set
    template< typename TOp > 
    void Bitset::setRangeCheap( size_t start, size_t end, WordType val )
    { 
        assertRange( end );  
        if( sameWord( start, end ) )
            setRangePartialWord<TOp>( getWordOfBitEditable( start ), bit2offset( start ), bit2offset( end ), val );
        else
        {
            setRangeWholeWords<TOp>( bit2word( start ) + 1, bit2word( end ), val );
            setRangePartialWord<TOp>( getWordOfBitEditable( start ), start, WORD_SIZE, val );
            setRangePartialWord<TOp>( getWordOfBitEditable( end ), 0, end, val );
        }
    }
}

#endif //UT_BITSET_HPP
