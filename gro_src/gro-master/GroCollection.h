#ifndef GROCOLLECTION_H
#define GROCOLLECTION_H

#include "ut/CollectionManager.hpp" //base class

#include "Defines.h"
#include "GroMarker.h"
#include "Timer.h"
#include "Checkpoint.h"

#include "PopulationStat.h"
#include "PopulationFunction.h"
#include "PopulationGate.h"

#include "CellPlacer.h"
#include "CellPlating.h"
#include "SignalPlacer.h"

#include "Snapshot.h"
#include "CellsFile.h"
#include "CellsPlot.h"

//PRECOMPILED
//#include<memory> //SP seedGenerator


namespace ut { class SeedGeneratorBase; }

class GroCollection : public ut::CollectionManager< GroCollection, GroElement
												  , BGroMarker, QGroMarker, Timer, Checkpoint, PopulationStat, PopulationFunction, PopulationGate, CellPlacer, CellPlating, SignalPlacer, CellsFile, Snapshot, CellsPlot >
{
	public:
		using CollectionManagerBaseType = GroCollection::CollectionManager;

	//---ctor, dtor
	    GroCollection( SP<ut::SeedGeneratorBase> seedGen );
	    virtual ~GroCollection() = default;

	//---get 
	    inline const QGroMarker* getCellNumMarker() const { return cellNumMarker; } 
	    inline ut::SeedGeneratorBase* getSeedGenerator() const { return seedGenerator.get(); }
	    inline SP<ut::SeedGeneratorBase> getSeedGeneratorPtr() const { return seedGenerator; }

	//---set
	    inline void setCellNumMarker( const QGroMarker* xCellNumMarker ) { cellNumMarker = xCellNumMarker; }

	//---API
	  //common
	    void init() override { rankAndSort(); CollectionManagerBaseType::init(); passUses(); } 
	  //special precompute
	    void rankAndSort();
	    void passUses();


	private:
		const QGroMarker* cellNumMarker;
		SP<ut::SeedGeneratorBase> seedGenerator;
};

#endif // GROCOLLECTION_H
