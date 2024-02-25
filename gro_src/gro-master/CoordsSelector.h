#ifndef COORDSSELECTOR_H
#define COORDSSELECTOR_H

#include "ut/Metadata.hpp" //REGISTER_SUBTYPE
#include "ut/DistributionCombi.hpp" 
#include "ut/Factory.hpp" //CoordsSelectorFactory
#include "ut/Shape.hpp" //coords

#include "Defines.h"


class GroCollection; //ctor
using CoordsBase = ut::Shape;
using CoordsCircular = ut::Circle;
using CoordsRectangular = ut::Rectangle;

class CoordsSelectorBase
/* virtual base class */
{
	public: 
		enum CoordsMode : int
		{
			CARTESIAN, POLAR, RECTANGULAR, COUNT
		};
		using SubtypeIdx = CoordsMode;

	//---ctor, dtor
		virtual ~CoordsSelectorBase() = default;

	//--API
		virtual void sampleCoords( TReal& x, TReal& y ) const = 0; //sample random position withing
		virtual bool checkWithin( TReal x, TReal y ) const = 0; //checkif a position belongs to the shape, just a wrapper


	protected:
		CoordsMode coordsMode;

		CoordsSelectorBase( CoordsMode coordsMode ) : coordsMode(coordsMode) {;}
};


///////////////////////////////////////////////////////////// DERIVED CLASSES ////////////////////////////////////////

class CoordsSelectorRectangular : public CoordsSelectorBase
{
	REGISTER_SUBTYPE( "rectangular", "rect", CoordsSelectorBase::SubtypeIdx::RECTANGULAR );

	public: 
	//---ctor, dtor
		CoordsSelectorRectangular( GroCollection* groCollection, const ut::Rectangle& coords );
		virtual ~CoordsSelectorRectangular() = default;

	//---API
		void sampleCoords( TReal& x, TReal& y ) const override { x = xDist.sample(); y = yDist.sample(); }
		bool checkWithin( TReal x, TReal y ) const override { return coords.checkWithin( x, y ); }


	private:
		ut::Rectangle coords; //border
        mutable ut::ConcDistCombi< ut::UniformDist > xDist;
        mutable ut::ConcDistCombi< ut::UniformDist > yDist;
};

class CoordsSelectorCartesian : public CoordsSelectorBase
/* uniformly distributed between a circle in Cartesian coordinates */
{
	REGISTER_SUBTYPE( "cartesian", "cart", CoordsSelectorBase::SubtypeIdx::CARTESIAN );

	public: 
		CoordsSelectorCartesian( GroCollection* groCollection, const ut::Circle& coords );

		void sampleCoords( TReal& x, TReal& y ) const override;
		bool checkWithin( TReal x, TReal y ) const override { return coords.checkWithin( x, y ); }


	private:
		ut::Circle coords; //border
        mutable ut::ConcDistCombi< ut::UniformDist > radiusDist;
        mutable ut::ConcDistCombi< ut::UniformDist > angleDist;
};

class CoordsSelectorPolar : public CoordsSelectorBase
/* more concentrated in the center of the circle than in the borders
uniformly distributed in polar coordinates */
{
	REGISTER_SUBTYPE( "polar", "pol", CoordsSelectorBase::SubtypeIdx::POLAR );

	public: 
		CoordsSelectorPolar( GroCollection* groCollection, const ut::Circle& coords );
		virtual ~CoordsSelectorPolar() = default;

		void sampleCoords( TReal& x, TReal& y ) const override;
		bool checkWithin( TReal x, TReal y ) const override { return coords.checkWithin( x, y ); }


	private:
		ut::Circle coords; //border
        mutable ut::ConcDistCombi< ut::UniformDist > radiusDist;
        mutable ut::ConcDistCombi< ut::UniformDist > angleDist;
};


////////////////////////////////////////////////////// FACTORY //////////////////////////////////////////////////////////////////////////
class CoordsSelectorFactory : public ut::Factory< CoordsSelectorBase, CoordsSelectorCartesian, CoordsSelectorPolar > {};

#endif // COORDSSELECTOR_H
