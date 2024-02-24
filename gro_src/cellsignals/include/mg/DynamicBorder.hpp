#ifndef MG_DYNAMICBORDER_HPP
#define MG_DYNAMICBORDER_HPP

#include "mg/defines.hpp"
#include "mg/Signal.hpp"
#include "mg/GridReport.hpp"

#include <limits> //RESET_VALUE
#include <algorithm> //max, min
#include <array>

namespace mg
{
	class DynamicBorder
	{
		public:
	//============================================== *nested classes* ==============================================================
            enum CoordIdx : uint
            {
            	COORD_X1, COORD_X2, COORD_Y1, COORD_Y2, COORD_COUNT
            };

            using GlobalBorderDS = std::array< int, CoordIdx::COORD_COUNT >;

            struct DimX
            {
    			using OtherType = class DimY;
    			static constexpr int getRow( int, int in ) { return in; }
    			static constexpr int getCol( int out, int ) { return out; }
            };

            struct DimY
            {
				using OtherType = DimX;
				static constexpr int getRow( int out, int ) { return out; }
    			static constexpr int getCol( int, int in ) { return in; }
            };

            struct SideL
            {
            		using OtherType = class SideR;
            		static constexpr int RESET_VALUE = std::numeric_limits<int>::max();

            		static constexpr int signedPad( int pad ) { return -pad; }
			        static constexpr void applyPad( int& current, int pad ) { current -= pad; } 
			        static constexpr void fitToLimits( int& current, int = 0 ) { current = std::max( current, 0 ); }
			        static constexpr int next( int current ) { return current + 1; } 
			        static constexpr int compare( int current, int newVal ) { return std::min( current, newVal ); }
			        static constexpr void applyCompare( int& current, int newVal ) { current = std::min( current, newVal ); }
			        static constexpr int plus() { return -1; }
                    static constexpr bool loopCheck( int current, int loopVal ) { return loopVal >= current; }
            };

            class SideR
            {
            	public:
					using OtherType = SideL;
					static constexpr int RESET_VALUE = -1;

					static constexpr int signedPad( int pad ) { return pad; }
			        static constexpr void applyPad( int& current, int pad ) { current += pad; } 
			        static constexpr void fitToLimits( int& current, int gridSize ) { current = std::min( current, gridSize - 1 ); }
			        static constexpr int next( int current ) { return current - 1; } 
					static constexpr int compare( int current, int newVal ) { return std::max( current, newVal ); }
			        static constexpr void applyCompare( int& current, int newVal ) { current = std::max( current, newVal ); }
			        static constexpr int plus() { return 1; }
                    static constexpr bool loopCheck( int current, int loopVal ) { return loopVal <= current; }
			};


            template< typename TDim, typename TSide, typename TMate, typename TOther1, typename TOther2, CoordIdx VIdx >
            struct BorderCoord
            {
            	CoordIdx idx = VIdx;

            	using DimType = TDim;
            	using SideType = TSide;

            	using MateType = TMate;
            	using OtherType1 = TOther1;
            	using OtherType2 = TOther2;

            	MateType* mate;
            	OtherType1* other1;
            	OtherType2* other2;

            	int value;
            	int newValue;

            //---ctor, dtor
				inline BorderCoord() : value( TSide::RESET_VALUE ), newValue( TSide::RESET_VALUE ) {;}
				inline ~BorderCoord() = default;
				BorderCoord( const BorderCoord& rhs ) = delete;
				BorderCoord& operator=( const BorderCoord& rhs ) = delete;
			//---get
				inline int getValue() const { return value; } 

			//---set
				inline void setFriendCoords( MateType* xMate, OtherType1* xOther1, OtherType2* xOther2 ) { mate = xMate; other1 = xOther1; other2 = xOther2; }

			//---API
				inline void offset( int val ) { newValue = value += val; } 
				inline void reset() { newValue = TSide::RESET_VALUE; }
				inline void applyPad( int pad ) { SideType::applyPad( newValue, pad ); } 
			    inline void fitToLimits( int gridSize ) { SideType::fitToLimits( newValue, gridSize ); }
			    inline void applyCompare( int candidate ) { SideType::applyCompare( newValue, candidate ); }

			    inline void commit() { value = newValue; } 
			    inline void resizeTo( GridReport* gridReport, int externalPoint, int pad ) { SideType::applyPad( externalPoint, pad ); applyCompare( externalPoint ); fitToLimits( gridReport->getGridSize() ); commit(); } 
			    
			    void clean( GridReport* gridReport, size_t id, bool bPresent );
			    bool update( GridReport* gridReport, const Signal* signal );

			    inline void registerToGlobal( GlobalBorderDS& globalBorder ) const { globalBorder[idx] = SideType::compare( value, globalBorder[idx] ); } 

                inline bool loopCheck( int loopVal ) { return SideType::loopCheck( value, loopVal ); }
            };

            class X2; class Y1; class Y2; 
            class X1 : public BorderCoord< DimX, SideL, X2, Y1, Y2, CoordIdx::COORD_X1 > {};
            class X2 : public BorderCoord< DimX, SideR, X1, Y1, Y2, CoordIdx::COORD_X2 > {};
            class Y1 : public BorderCoord< DimY, SideL, Y2, X1, X2, CoordIdx::COORD_Y1 > {};
            class Y2 : public BorderCoord< DimY, SideR, Y1, X1, X2, CoordIdx::COORD_Y2 > {};




	//============================================== *dynamic border* ==============================================================
		//---ctor, dtor
			DynamicBorder()
			: x1(), x2(), y1(), y2(), bPresent(false)
			{
				x1.setFriendCoords( &x2, &y1, &y2 );
				x2.setFriendCoords( &x1, &y1, &y2 );
				y1.setFriendCoords( &y2, &x1, &x2 );
				y2.setFriendCoords( &y1, &x1, &x2 );
			}

			virtual ~DynamicBorder() = default;
			DynamicBorder( const DynamicBorder& rhs ) = delete;
			DynamicBorder& operator=( const DynamicBorder& rhs ) = delete;

		//---get
			inline int getX1() const { return x1.value; } 
			inline int getX2() const { return x2.value; } 
			inline int getY1() const { return y1.value; } 
			inline int getY2() const { return y2.value; }

			inline bool getBPresent() const { return bPresent; } 

		//---set

		//---API
		    void update( GridReport* gridReport, const Signal* signal );
		    inline void reset() { resetThese( x1, x2, y1, y2 ); }
		    inline void commit() { commitThese<>( x1, x2, y1, y2 ); } 
		    inline void clean( GridReport* gridReport, size_t id ) { cleanThese( gridReport, id, x1, x2, y1, y2 ); } 
		    inline void resizeTo( GridReport* gridReport, int row, int col, int pad ) { resizeToThese( gridReport, col, pad, x1, x2 ); resizeToThese( gridReport, row, pad, y1, y2 ); bPresent = true; } 
		    inline void registerToGlobal( GlobalBorderDS& globalBorder ) const { registerToGlobalThese( globalBorder, x1, x2, y1, y2 ); } 
		    inline void offset( int offsetVal ) { x1.offset( offsetVal ); x2.offset( offsetVal ); y1.offset( offsetVal ); y2.offset( offsetVal ); }

		    template< typename TFirst, typename... Ts>
		    void updateThese( GridReport* gridReport, const Signal* signal, TFirst& first, Ts&&... args );
		    template< typename TFirst, typename... Ts>
		    void resetThese( TFirst& first, Ts&&... args );
		    template< typename TFirst, typename... Ts>
		    void commitThese( TFirst& first, Ts&&... args );
		    template< typename TFirst, typename... Ts>
		    void cleanThese( GridReport* gridReport, size_t id, TFirst& first, Ts&&... args );
		    template< typename TFirst, typename... Ts>
		    void resizeToThese( GridReport* gridReport, int val, int pad, TFirst& first, Ts&&... args );
		   	template< typename TFirst, typename... Ts>
		    void registerToGlobalThese( GlobalBorderDS& globalBorder, TFirst& first, Ts&&... args ) const;

		private:
			X1 x1;
			X2 x2;
			Y1 y1;
			Y2 y2;

			bool bPresent;
	};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// TEMPLATE DEFINITIONS //////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//================================================= *BorderCoord* =======================================================================
	template< typename TDim, typename TSide, typename TMate, typename TOther1, typename TOther2, DynamicBorder::CoordIdx VIdx >
	void DynamicBorder::BorderCoord< TDim, TSide, TMate, TOther1, TOther2, VIdx >::clean( GridReport* gridReport, size_t id, bool bPresent )
    {
    	int start, end;
    	if( bPresent )
    	{
    		start = newValue;
    		end = value + SideType::plus();
    	}
    	else
    	{
    		start = value; 
    		end = mate->value + MateType::SideType::plus();
    	}

		for( int i = start; i != end; i = SideType::next( i ) )
        {
            for( int j = other1->value; j <= other2->value; j++ )
            	gridReport->setConcAtGridUnit( id, 0.0, DimType::getRow( i, j ), DimType::getCol( i, j ) );
        }
    }


   	template< typename TDim, typename TSide, typename TMate, typename TOther1, typename TOther2, DynamicBorder::CoordIdx VIdx >
	bool DynamicBorder::BorderCoord< TDim, TSide, TMate, TOther1, TOther2, VIdx >::update( GridReport* gridReport, const Signal* signal )
    {
        bool found = false;
        //ut::print( "calling update ", value, mate->getValue(), loopCheck( value ), mate->loopCheck( value ) );

        for( int i = value; mate->loopCheck( i ); i = SideType::next( i ) )
        {
            //ut::print( "loop check ", i );
            for( int j = other1->value; j <= other2->value; j++ )
            {
                if( gridReport->getConcAtGridUnit( signal->getId(), DimType::getRow( i, j ), DimType::getCol( i, j ) ) >= signal->getResizeThresholdConc() )
                {
                    newValue = i + SideType::signedPad( signal->getPaddingUnits() );
                    found = true;
                    break;
                }
            }
            if( found )
                break;
        }
        if( found )
        {
            //ut::print("found");
            fitToLimits( gridReport->getGridSize() );
            return true;
        }
        //ut::print( "not found");
        return false;
    }




//================================================= *DynamicGrid* =======================================================================
	inline void DynamicBorder::update( GridReport* gridReport, const Signal* signal )
    {
        //ut::print( "border: ", x1.value, x2.value, y1.value, y2.value );
    	if( !bPresent )
        {
            //ut::print( "not present" );
    		return;
        }
        //ut::print( "present" );

    	bPresent = x1.update( gridReport, signal );
    	if( bPresent )
    		updateThese( gridReport, signal, x2, y1, y2 );
    	else
    		reset();
    	//clean( gridReport, signal->getId() );
    	commit();
    }


	template< typename TFirst, typename... Ts>
    void DynamicBorder::updateThese( GridReport* gridReport, const Signal* signal, TFirst& first, Ts&&... args )
    {
    	first.update( gridReport, signal ); 
    	if constexpr( sizeof...(Ts) > 0 )
    		updateThese( gridReport, signal, args... );
    }

    template< typename TFirst, typename... Ts>
    void DynamicBorder::resetThese( TFirst& first, Ts&&... args )
    {
    	first.reset(); 
    	if constexpr( sizeof...(Ts) > 0 )
    		resetThese( args... );
    }

    template< typename TFirst, typename... Ts>
    void DynamicBorder::commitThese( TFirst& first, Ts&&... args )
    {
    	first.commit(); 
    	if constexpr( sizeof...(Ts) > 0 )
    		commitThese( args... );
    }

    template< typename TFirst, typename... Ts>
    void DynamicBorder::cleanThese( GridReport* gridReport, size_t id, TFirst& first, Ts&&... args )
    {
    	first.clean( gridReport, id, bPresent ); 
    	if constexpr( sizeof...(Ts) > 0 )
    		cleanThese( gridReport, id, args... );
    }

    template< typename TFirst, typename... Ts>
    void DynamicBorder::resizeToThese( GridReport* gridReport, int val, int pad, TFirst& first, Ts&&... args )
    {
    	first.resizeTo( gridReport, val, pad );
    	if constexpr( sizeof...(Ts) > 0 )
    		resizeToThese( gridReport, val, pad, args... );
    }

    template< typename TFirst, typename... Ts>
    void DynamicBorder::registerToGlobalThese( GlobalBorderDS& globalBorder, TFirst& first, Ts&&... args ) const
    {
    	first.registerToGlobal( globalBorder );
    	if constexpr( sizeof...(Ts) > 0 )
    		registerToGlobalThese( globalBorder, args... );
    }
}

#endif //MG_DYNAMICBORDER_HPP
