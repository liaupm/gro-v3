#ifndef UT_RGBCOLOUR_HPP
#define UT_RGBCOLOUR_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <vector> //ctor
#include <algorithm> //min, max */


namespace ut
{
	class RgbColour
	{
		public:
			static constexpr TReal DF_COLOUR_VAL = 0.0;
			static constexpr TReal MAX_VAL = 1.0;
		

		//---ctor, dtor
			RgbColour() : r(DF_COLOUR_VAL), g(DF_COLOUR_VAL), b(DF_COLOUR_VAL) {;}
			RgbColour( TReal r, TReal g, TReal b ) : r(r), g(g), b(b) {;}
			RgbColour( const std::vector<TReal>& rgb ) : r( rgb.size() > 0 ? rgb[0] : DF_COLOUR_VAL ), g( rgb.size() > 1 ? rgb[1] : DF_COLOUR_VAL ), b( rgb.size() > 2 ? rgb[2] : DF_COLOUR_VAL ) {;}

			virtual ~RgbColour() = default;
			RgbColour( const RgbColour& rhs ) = default;
			RgbColour( RgbColour&& rhs ) noexcept = default;
			RgbColour& operator=( const RgbColour& rhs ) = default;
			RgbColour& operator=( RgbColour&& rhs ) noexcept = default;

		//---get
			inline TReal getR() const { return r; }
			inline TReal getG() const { return g; }
			inline TReal getB() const { return b; }
			inline TReal getMax() const { return std::max( r, std::max( g, b ) ); }
			inline TReal getMin() const { return std::min( r, std::min( g, b ) ); }

		//---set
			inline void set( TReal xR, TReal xG, TReal xB ) { r = xR; g = xG; b = xB; }
			inline void setR( TReal xR ) { r = xR; }
			inline void setG( TReal xG ) { r = xG; }
			inline void setB( TReal xB ) { r = xB; }

		//---API
			inline void scale( TReal factor ) { r *= factor; g *= factor; b *= factor; }
			inline void add( TReal xR, TReal xG, TReal xB ) { r += xR; g += xG; b += xB; }
			inline void add( const RgbColour& rhs ) { r += rhs.r; g += rhs.g; b += rhs.b; }
			inline void normalize() { TReal maxVal = getMax(); if( maxVal > MAX_VAL ) scale( 1.0 / maxVal ); }
			inline void reset() { r = DF_COLOUR_VAL; g = DF_COLOUR_VAL; b = DF_COLOUR_VAL; }

			inline friend RgbColour operator+( const RgbColour& obj1, const RgbColour& obj2 ) { return RgbColour( obj1.r + obj2.r, obj1.g + obj2.g, obj1.b + obj2.b ); }
			inline friend RgbColour operator*( const RgbColour& obj, TReal factor ) { return RgbColour( obj.r * factor, obj.g * factor, obj.b * factor ); }


		//private:
			TReal r, g, b;
	};
}

#endif //UT_RGBCOLOUR_HPP