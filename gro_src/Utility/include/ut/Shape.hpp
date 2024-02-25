#ifndef UT_SHAPE_HPP
#define UT_SHAPE_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <cmath> //pow */


namespace ut
{
	struct Shape
	/* virtual base */
	{
		public:
		//---ctor, dtor
			virtual ~Shape() = default;
			/*Shape( const Shape& rhs ) = default;
			Shape( Shape&& rhs ) noexcept = default;
			Shape& operator=( const Shape& rhs ) = default;
			Shape& operator=( Shape&& rhs ) noexcept = default;*/

		//---API
			bool virtual checkWithin( TReal x, TReal y ) const = 0;

		protected:
			Shape() = default;
	};


	struct Circle : public Shape
	{
		TReal xCenter;
		TReal yCenter; 
		TReal r;

		Circle( TReal xCenter, TReal yCenter, TReal r ) : xCenter( xCenter ), yCenter( yCenter ), r( r ) {;}

		bool checkWithin( TReal x, TReal y ) const override { return std::pow( x - xCenter, 2 ) + std::pow( y - yCenter, 2 ) <= std::pow( r, 2 ); }
	};

	struct Rectangle : public Shape
	{
	    static constexpr TReal DF_COORD_LOWER = 0.0;
	    static constexpr TReal DF_COORD_HIGHER = 0.0;
	    
	    TReal x1, x2;
	    TReal y1, y2;

	//---ctor, dtor
	    Rectangle() : Shape()
	    , x1( DF_COORD_LOWER ), x2( DF_COORD_HIGHER ), y1( DF_COORD_LOWER ), y2( DF_COORD_HIGHER ) {;}

	    Rectangle( TReal x1, TReal x2, TReal y1, TReal y2 ) : x1( x1 ), x2( x2 ), y1( y1 ), y2( y2 ) {;}

	//---API
	    bool checkWithin( TReal x, TReal y ) const override { return x >= x1 && x <= x2 && y >= y1 && y <= y2; }
	};
}

#endif //UT_SHAPE_HPP