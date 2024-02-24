#ifndef UT_STATICOPERATORS_HPP
#define UT_STATICOPERATORS_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <vector> //apply operator to
#include <cmath> //log2
#include <limits> //infinite null values
#include <algorithm> //min, max */


namespace ut
{
	template< typename TVal >
	class StaticOperators
    /* just used as a namespace */
	{
		public:
			using ValType = TVal;


	///////////////////////////////////////////////////////////// OPERATOR BASES  /////////////////////////////////////////////////////////////
			struct OpBase {};
			struct AssigmentOp : public OpBase 
			{
				inline static constexpr bool B_MODIFIES_ARG = true;
				inline static constexpr short ARG_NUM = 2;
			};

			struct UnaryOp : public OpBase
			{
				inline static constexpr bool B_MODIFIES_ARG = false;
				inline static constexpr short ARG_NUM = 1;

			};
			struct BinaryOp : public OpBase
			{
				inline static constexpr bool B_MODIFIES_ARG = false;
				inline static constexpr short ARG_NUM = 2;
			};

			struct BitwiseOp : public OpBase 
			{

	            static constexpr short WORD_SIZE = sizeof( TVal ) * CHAR_BIT;
	            static constexpr uint SHIFT_SIZE = std::log2( WORD_SIZE );

				static inline constexpr TVal allZeros() noexcept { return static_cast<TVal>( 0 ); }
	            static inline constexpr TVal allOnes() noexcept { return ~allZeros(); }

	            static inline constexpr TVal before( short bit ) noexcept { return bit < WORD_SIZE ? ( static_cast<TVal>( 1 ) << bit ) - 1 : allOnes(); }
	            static inline constexpr TVal after( short bit ) noexcept { return ~before( bit ); }
	            static inline constexpr TVal between( short start, short end ) noexcept { return after( start ) & before( end ); }

	            static inline constexpr TVal index2flag( short bit, bool val = true ) noexcept { assert( bit < WORD_SIZE ); return static_cast<TVal>( val ) << bit; }
	            static inline constexpr TVal index2ReverseFlag( short bit, bool val = false ) noexcept { return ~index2flag( bit, ! val ); }
			};
			struct ArithmeticOp : public OpBase {};


			struct Assign; struct ReverseAssign; struct SumAssign; struct DiffAssign; struct MultAssign; struct DivAssign; struct MinAssign; struct MaxAssign;
			struct Sum; struct Diff; struct Mult; struct Div; struct Min; struct Max;


			struct BitTrueAssign; struct BitFalseAssign; struct BitAndAssign; struct BitOrAssign; struct BitXorAssign; struct BitNegAssign;
			struct BitAnd; struct BitOr; struct BitXor; struct BitNeg;




		///////////////////////////////////////////////////////////// BITWISE ASSIGN /////////////////////////////////////////////////////////////

            struct BitAssign : public AssigmentOp, public BitwiseOp
            { 
                static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs = rhs; } 
                static constexpr TVal& callMasked( TVal& lhs, TVal rhs, TVal mask ) { return lhs = ( lhs & ~mask ) | ( rhs & mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool val ) { return lhs = ( lhs & ~BitwiseOp::index2flag( bit ) ) | BitwiseOp::index2flag( bit, val ); }
            };

            struct BitTrueAssign : public AssigmentOp, public BitwiseOp
            { 
                static constexpr TVal& call( TVal& lhs, TVal = BitwiseOp::allOnes() ) { return lhs = BitwiseOp::allOnes(); }
                static constexpr TVal& callMasked( TVal& lhs, TVal mask ) { return lhs |= mask; } 
                static constexpr TVal& callMasked( TVal& lhs, TVal, TVal mask ) { return callMasked( lhs, mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool = true ) { return lhs |= BitwiseOp::index2flag( bit ); }
            };

            struct BitFalseAssign : public AssigmentOp, public BitwiseOp
            { 
                static constexpr TVal& call( TVal& lhs, TVal = BitwiseOp::allZeros() ) { return lhs = BitwiseOp::allZeros(); } 
                static constexpr TVal& callMasked( TVal& lhs, TVal mask ) { return lhs &= ~mask; }
                static constexpr TVal& callMasked( TVal& lhs, TVal, TVal mask ) { return callMasked( lhs, mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool = false ) { return lhs &= BitwiseOp::index2ReverseFlag( bit ); }
            };

            struct BitAndAssign : public AssigmentOp, public BitwiseOp
            { 
            	using FreeType = BitAnd;

                static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs &= rhs; }
                static constexpr TVal& callMasked( TVal& lhs, TVal rhs, TVal mask ) { return lhs &= ( rhs | ~mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool val ) { return lhs &= ~BitwiseOp::index2flag( bit, val ); }
            };

            struct BitOrAssign : public AssigmentOp, public BitwiseOp
            { 
            	using FreeType = BitOr;

                static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs |= rhs; } 
                static constexpr TVal& callMasked( TVal& lhs, TVal rhs, TVal mask ) { return lhs |= ( rhs & mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool val ) { return lhs |= BitwiseOp::index2flag( bit, val ); }
            };

            struct BitXorAssign : public AssigmentOp, public BitwiseOp
            { 
            	using FreeType = BitXor;

                static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs ^= rhs; } 
                static constexpr TVal& callMasked( TVal& lhs, TVal rhs, TVal mask ) { return lhs ^= ( rhs & mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool val ) { return lhs ^= BitwiseOp::index2flag( bit, val ); }
            };

            struct BitNegAssign : public AssigmentOp, public BitwiseOp
            { 
            	using FreeType = BitNeg;

                static constexpr TVal& call( TVal& lhs, TVal = BitwiseOp::allZeros() ) { return lhs = ~lhs; } 
                static constexpr TVal& callMasked( TVal& lhs, TVal mask ) { return lhs ^= mask; }
                static constexpr TVal& callMasked( TVal& lhs, TVal, TVal mask ) { return callMasked( lhs, mask ); }
                static constexpr TVal& callSingle( TVal& lhs, short bit, bool = false ) { return lhs ^= BitwiseOp::index2flag( bit ); }
            };



        ///////////////////////////////////////////////////////////// BITWISE BINARY /////////////////////////////////////////////////////////////

			struct BitAnd : public BinaryOp, public BitwiseOp
			{ 
				using AssigmentType = BitAndAssign;

				static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs & rhs; } 
			};

            struct BitOr : public BinaryOp, public BitwiseOp
            { 
            	using AssigmentType = BitOrAssign;

            	static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs | rhs; } 
            };

            struct BitXor : public BinaryOp, public BitwiseOp
            { 
            	using AssigmentType = BitXorAssign;

            	static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs ^ rhs; } 
            };

            struct BitNeg : public UnaryOp, public BitwiseOp
            { 
            	using AssigmentType = BitNegAssign;

                static constexpr TVal call( TVal rhs ) { return ~rhs; } 
                static constexpr TVal callMasked( TVal rhs, TVal mask ) { return ~( rhs ^ mask ); }
                static constexpr TVal callSingle( TVal rhs, short bit, bool = false ) { return ~( rhs ^ BitwiseOp::index2flag( bit ) ); }
            };


            struct Any : public UnaryOp, public BitwiseOp
            { 
                static constexpr bool call( TVal rhs ) { return rhs; } 
                static constexpr bool callMasked( TVal rhs, TVal mask ) { return rhs ^ mask; }
                static constexpr bool callSingle( TVal rhs, short bit, bool = false ) { return rhs ^ BitwiseOp::index2flag( bit ); }
            };

            struct All : public UnaryOp, public BitwiseOp
            { 
                static constexpr bool call( TVal rhs ) { return !~rhs; } 
                static constexpr bool callMasked( TVal rhs, TVal mask ) { return !~( rhs ^ mask ); }
                static constexpr bool callSingle( TVal rhs, short bit, bool = false ) { return !~( rhs ^ BitwiseOp::index2flag( bit ) ); }
            };

            struct None : public UnaryOp, public BitwiseOp
            { 
                static constexpr bool call( TVal rhs ) { return !rhs; } 
                static constexpr bool callMasked( TVal rhs, TVal mask ) { return !( rhs ^ mask ); }
                static constexpr bool callSingle( TVal rhs, short bit, bool = false ) { return !( rhs ^ BitwiseOp::index2flag( bit ) ); }
            };



		///////////////////////////////////////////////////////////// ARITHMETIC ASSIGN /////////////////////////////////////////////////////////////
			struct Assign : public AssigmentOp, public ArithmeticOp
            { 
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs = rhs; } 
            };

			struct ReverseAssign : public AssigmentOp, public ArithmeticOp
            { 
                inline static constexpr TVal& call( TVal& lhs, TVal = TVal{} ) { return lhs = -lhs; } 
            };

			struct SumAssign : public AssigmentOp, public ArithmeticOp
            { 
            	using FreeType = Sum;
            	inline static constexpr TVal nullVal = 0;
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs += rhs; } 
            };

			struct DiffAssign : public AssigmentOp, public ArithmeticOp
            { 
            	using FreeType = Diff;
            	inline static constexpr TVal nullVal = 0;
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs -= rhs; } 
            };

            struct MultAssign : public AssigmentOp, public ArithmeticOp
            { 
            	using FreeType = Mult;
            	inline static constexpr TVal nullVal = 1;
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs *= rhs; } 
            };

            struct DivAssign : public AssigmentOp, public ArithmeticOp
            { 
            	using FreeType = Div;
            	inline static constexpr TVal nullVal = 1;
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs /= rhs; } 
            };

            struct MinAssign : public AssigmentOp, public ArithmeticOp
            { 
                using FreeType = Min;
                inline static constexpr TVal nullVal = std::numeric_limits<TVal>::lowest();
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs = std::min( lhs, rhs ); } 
            };

            struct MaxAssign : public AssigmentOp, public ArithmeticOp
            { 
                using FreeType = Max;
                inline static constexpr TVal nullVal = std::numeric_limits<TVal>::max();
                inline static constexpr TVal& call( TVal& lhs, TVal rhs ) { return lhs = std::max( lhs, rhs ); } 
            };

        

        ///////////////////////////////////////////////////////////// ARITHMETIC BINARY /////////////////////////////////////////////////////////////

			struct Sum : public BinaryOp, public ArithmeticOp
            { 
            	using AssigmentType = SumAssign;
            	inline static constexpr TVal nullVal = 0;
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs + rhs; } 
            };

			struct Diff : public BinaryOp, public ArithmeticOp
            { 
            	using AssigmentType = DiffAssign;
            	inline static constexpr TVal nullVal = 0;
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs - rhs; } 
            };

            struct Mult : public BinaryOp, public ArithmeticOp
            { 
            	using AssigmentType = MultAssign;
            	inline static constexpr TVal nullVal = 1;
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs * rhs; } 
            };

            struct Div : public BinaryOp, public ArithmeticOp
            { 
            	using AssigmentType = DivAssign;
            	inline static constexpr TVal nullVal = 1;
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return lhs / rhs; } 
            };

            struct Reverse : public UnaryOp, public ArithmeticOp
            { 
                inline static constexpr TVal call( TVal& lhs, TVal = TVal{} ) { return -lhs; } 
            };

            struct Min : public BinaryOp, public ArithmeticOp
            { 
                using AssigmentType = MinAssign;
                inline static constexpr TVal nullVal = std::numeric_limits<TVal>::lowest();
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return std::min( lhs, rhs ); } 
            };

            struct Max : public BinaryOp, public ArithmeticOp
            { 
                using AssigmentType = MaxAssign;
                inline static constexpr TVal nullVal = std::numeric_limits<TVal>::max();
                inline static constexpr TVal call( TVal lhs, TVal rhs ) { return std::max( lhs, rhs ); } 
            };



    ///////////////////////////////////////////////////////////// API /////////////////////////////////////////////////////////////
            template< typename T >
            using EnableForNonVectors = EnableForNonDerived< std::vector< TVal >, T >;

            template< typename T >
            using EnableForNonVal = EnableForNonDerived< TVal, T >;

        //---assigment
            template< typename TOp, typename TCont, typename = EnableForNonVectors< TCont >, typename = EnableForNonVal< TCont > >
            static void assigmentOperator( TCont& lhs, const TCont& rhs );

            template< typename TOp >
            static void assigmentOperator( std::vector< TVal >& lhs, const std::vector< TVal >& rhs );

            template< typename TOp, typename TCont >
            static void assigmentOperator( TCont& lhs, const TVal& rhs ) { for( TVal& elem : lhs  ) TOp::call( elem, rhs ); }

        //---unary
            template< typename TOp, typename TCont, typename = EnableForNonVectors< TCont > >
            static TCont unaryOperator( const TCont& rhs );

		    template< typename TOp >
		    static std::vector< TVal > unaryOperator( const std::vector< TVal >& rhs );

        //---binary
            template< typename TOp, typename TCont, typename = EnableForNonVectors< TCont >, typename = EnableForNonVal< TCont > >
            static TCont binaryOperator( const TCont& lhs, const TCont& rhs );

            template< typename TOp >
            static std::vector< TVal > binaryOperator( const std::vector< TVal >& lhs, const std::vector< TVal >& rhs );

            template< typename TOp, typename TCont >
            static TCont binaryOperator( const TCont& lhs, const TVal& rhs ) { TCont result = lhs; assigmentOperator<typename TOp::AssigmentType>( result, rhs ); return result; }
	};







//////////////////////////////////////////////////////////// METHOD DEFINITIONS //////////////////////////////////////////////////
	
//---assigment
    template< typename TVal >
    template< typename TOp, typename TCont, typename, typename >
    inline void StaticOperators< TVal >::assigmentOperator( TCont& lhs, const TCont& rhs )
    { 
    	assert( lhs.size() == rhs.size() );

    	typename TCont::iterator itL = lhs.begin();
    	typename TCont::const_iterator itR = rhs.cbegin();

    	for( ; itL != lhs.end(); itL++, itR++ )
    		TOp::call( *itL, *itR );
    }

    template< typename TVal >
    template< typename TOp>
    inline void StaticOperators< TVal >::assigmentOperator( std::vector< TVal >& lhs, const std::vector< TVal >& rhs )
    { 
    	assert( lhs.size() == rhs.size() );

    	for( size_t i = 0; i < lhs.size(); i++ ) 
    		TOp::call( lhs[i], rhs[i] );
    }

//---unary
    template< typename TVal >
    template< typename TOp, typename TCont, typename >
    inline TCont  StaticOperators< TVal >::unaryOperator( const TCont& rhs )
    {
        TCont result = rhs;
        for( const auto& elem : result )
        	TOp::AssigmentType::call( elem );
        return result;
    }

    template< typename TVal >
    template< typename TOp >
    inline std::vector< TVal > StaticOperators< TVal >::unaryOperator( const std::vector< TVal >& rhs )
    {
        std::vector< TVal > result;
        result.reserve( rhs.size() );

        for( TVal elem : rhs )
            result.push_back( TOp::call( elem ) );
        return result;
    }

//---binary
   	template< typename TVal >
    template< typename TOp, typename TCont, typename, typename >
    inline TCont  StaticOperators< TVal >::binaryOperator( const TCont& lhs, const TCont& rhs )
    {
        TCont result = lhs;
        assigmentOperator< typename TOp::AssigmentType >( result, rhs );
        return result;
    }

    template< typename TVal >
    template< typename TOp >
    inline std::vector< TVal > StaticOperators< TVal >::binaryOperator( const std::vector< TVal >& lhs, const std::vector< TVal >& rhs )
    {
        assert( lhs.size() == rhs.size() );
        std::vector< TVal > result;
        result.reserve( lhs.size() );

        for( size_t i = 0; i < lhs.size(); i++ )
            result.push_back( TOp::call( lhs[i], rhs[i] ) );
        return result;
    }
}

#endif //UT_STATICOPERATORS_HPP