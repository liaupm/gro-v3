#ifndef CELLPLATING_H
#define CELLPLATING_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp"
#include "cg/GeneticElement.hpp" //gate

#include "Defines.h"
#include "GroElement.h" //base class
#include "CoordsSelector.h" //CoordsSelectorBase::CoordsMode
#include "Cell.h" //Cell::CellPosition


//PRECOMPILED
/*#include <vector> //distribution params
#include <string> //name
#include <memory> //SP in spatialFilter and coordsSelector*/

namespace cg { class GeneticElemnt; } //gate
class Timer; //timer
class GroCollection;
class CellPlating : public GroElement
{
    REGISTER_CLASS( "CellPlating", "cpl", ElemTypeIdx::CELL_PLATING )
	
	public:
		using CellPosition = Cell::CellPosition;
		using CoordsMode = CoordsSelectorBase::CoordsMode;

		static constexpr double DF_TAKE_MEAN = 0.9;
		static constexpr double DF_TAKE_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_TAKE_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_TAKE_PARAMS = { DF_TAKE_MEAN, DF_TAKE_VAR };
		inline static const ut::ContinuousDistData DF_TAKE_DIST = ut::ContinuousDistData( DF_TAKE_DISTTYPE, DF_TAKE_PARAMS );

		static constexpr double DF_PUT_MEAN = 0.0;
		static constexpr double DF_PUT_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_PUT_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_PUT_PARAMS = { DF_PUT_MEAN, DF_PUT_VAR };
		inline static const ut::ContinuousDistData DF_PUT_DIST = ut::ContinuousDistData( DF_PUT_DISTTYPE, DF_PUT_PARAMS );

		static constexpr double DF_MAX_ROTATION = 2.0 * ut::PI; //~6.28 radians;


	//---ctor, dtor
		CellPlating( GroCollection* groCollection, size_t id, const std::string& name
	    , const cg::GeneticElement* gate, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, const CoordsCircular& spatialFilter
	    , const ut::ContinuousDistData& takeFractionDist = DF_TAKE_DIST, const ut::ContinuousDistData& putFractionDist = DF_PUT_DIST );

	//---get
	    inline const cg::GeneticElement* getGate() const{ return gate; }
	    inline const Timer* getTimer() const { return timer; }
	    inline const CoordsBase* getSpatialFilter() const { return spatialFilter.get(); }
	    inline const CoordsSelectorBase* getCoordsSelector() const { return coordsSelector.get(); }
	    inline RandomEngine& getShuffleRndEngine() const { return shuffleRndEngine; }
	    inline bool getBPut() const { return bPut; }

	//---API
	  //precompute
	    void loadDirectElemLink() override;
	  //sampling
	    inline TReal sampleTakeFraction() const { return ut::fit( takeDist.sample() ); }
		inline TReal samplePutFraction() const { return ut::fit( putDist.sample() ); }
	   	inline void samplePos( TReal& x, TReal& y ) const { coordsSelector->sampleCoords( x, y ); } 
		inline CellPosition samplePos() const { TReal x, y; samplePos( x, y ); return CellPosition( x, y, rotationDist.sample() ); } 
		inline TReal sampleRotation() const { return rotationDist.sample(); }


	private:
	  //resources
		const cg::GeneticElement* gate; //cell condition. Only the cells where it holds are affected
		const Timer* timer;
		SP<CoordsBase> spatialFilter; //spatial condition. Only the cells withing this geometry are affected. AND with gate
		SP<CoordsSelectorBase> coordsSelector; //new location
	  //options
		bool bPut; //whether the (some of) the taken cells are returned to the world
	  //dists
        mutable ut::DistributionCombi takeDist;
        mutable ut::DistributionCombi putDist;
        mutable ut::ConcDistCombi< ut::UniformDist > rotationDist;
        mutable RandomEngine shuffleRndEngine; //to randomly select the specific cells to take and put
};

#endif // CELLPLATING_H
