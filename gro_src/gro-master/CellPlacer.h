#ifndef CELLPLACER_H
#define CELLPLACER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //cellSelectDist, rotationDist, amountDist

#include "Defines.h"
#include "GroElement.h" //base
#include "CoordsSelector.h" //coordsSelector, CoordsSelectorBase::CoordsMode
#include "Cell.h" //Cell::CellPosition


/*PRECOMPILED
#include <vector> //cellTypes, cellProbs, distribution params
#include <string> //name
#include <memory> //SP for coordsSelector */


namespace cg { class CellType; } //cellTypes
class Timer; //timer
class GroCollection; //ctor
class CellPlacer : public GroElement
{
    REGISTER_CLASS( "CellPlacer", "cp", ElemTypeIdx::CELL_PLACER )
	
	public:
		using CellPosition = Cell::CellPosition;
		using CoordsMode = CoordsSelectorBase::CoordsMode;

		static constexpr TReal DF_AMOUNT_MEAN = 1.0;
		static constexpr TReal DF_AMOUNT_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_AMOUNT_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_AMOUNT_PARAMS = { DF_AMOUNT_MEAN, DF_AMOUNT_VAR };
		inline static const ut::ContinuousDistData DF_AMOUNT_DIST = ut::ContinuousDistData( DF_AMOUNT_DISTTYPE, DF_AMOUNT_PARAMS );

		inline static const std::vector<TReal> DF_CELL_PROBS = { 1.0 };
		static constexpr bool DF_BMIXED = false;
		static constexpr TReal DF_MAX_ROTATION = 2.0 * ut::PI; //~6.28 radians;


	//---ctor, dtor
	    CellPlacer( GroCollection* groCollection, size_t id, const std::string& name
	    , const std::vector< const cg::CellType* > cellTypes, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, bool bMixed
	    , const std::vector<TReal>& cellProbs = DF_CELL_PROBS, const ut::ContinuousDistData& amountDist = DF_AMOUNT_DIST );

	//---get
	    inline const cg::CellType* getCellType() const{ return cellTypes[0]; }
	    inline const std::vector< const cg::CellType* > getCellTypes() const{ return cellTypes; }
	    inline const Timer* getTimer() const { return timer; }
	    inline const CoordsSelectorBase* getCoordsSelector() const { return coordsSelector.get(); }
	    inline bool getBMixed() const { return bMixed; }
	    inline bool getIsMixed() const { return bMixed; }

	//---API
	  //precompute
	    void loadDirectElemLink() override;
	  //sampling
	    inline const cg::CellType* sampleCellType() const { return cellTypes[ cellSelectDist.sample() ]; }
	    inline size_t sampleCellTypeId() const { return sampleCellType()->getId(); }
	    inline uint sampleCellNum() const { return ut::round<uint>( ut::fitL( amountDist.sample() ) ); }
	    inline void samplePos( TReal& x, TReal& y ) const { coordsSelector->sampleCoords( x, y ); } 
	    inline CellPosition samplePos() const { TReal x, y; samplePos( x, y ); return CellPosition( x, y, rotationDist.sample() ); } 
	    inline TReal sampleRotation() const { return rotationDist.sample(); }


	private:
	  //resources
		const std::vector< const cg::CellType* > cellTypes;
		const Timer* timer;
		SP<CoordsSelectorBase> coordsSelector;
	  //options
		bool bMixed; //wheter different CellTypes can be placed by the same event
	  //dists
        mutable ut::DiscreteDistCombi< ut::DiscreteDist > cellSelectDist; //CellType selection, created from cellProbs
        mutable ut::DistributionCombi amountDist;
        mutable ut::ConcDistCombi< ut::UniformDist > rotationDist;
};

#endif // CELLPLACER_H
