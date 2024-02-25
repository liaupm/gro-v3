#ifndef CELLSHANDLER_H
#define CELLSHANDLER_H

#include "ut/Shape.hpp" //Rectangle colonyBorder
#include "Defines.h"
#include "GroHandler.h" //base

//PRECOMPILED
/*#include <vector> //PopulationType
#include <memory> //SP PopulationType*/


struct CellsHReportIdx
{
	enum ReportTypeIdx : size_t { COUNT };
};

namespace ut { class Bitset; class MultiBitset; } //params of update()
namespace cg { class CellType; } //param in createCell()
class GroCollection; //ctor
class Cell; //population
class PetriDish; //owner in ctor
class CellsHandler : public GroHandler<>
{
	REGISTER_CLASS( "CellsHandler", "cellsH", GroHandler<>::HandlerTypeIdx::H_CELLS )
	
	public:
        using PopulationType = std::vector< SP<Cell> >;

    //---ctor, dtor
    	CellsHandler( PetriDish* owner, const GroCollection* collection ) : GroHandler( owner, collection ) {;}
    	//CellsHandler( const CellsHandler& rhs, PetriDish* owner ) : GroHandler( rhs, owner ) {;}
    	virtual ~CellsHandler() = default;

    //---get
    	inline const PopulationType& getPopulation() const { return population; }
        inline PopulationType& getPopulationEdit() { return population; }
        inline size_t getPopulationSize() const { return population.size(); } //get number of cells //ELE_S: no se puede pasar a uint porque es usado por ccl
        inline const ut::Rectangle& getColonyBorder() const { return colonyBorder; }

    //---API
      //common
        bool update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* ) override;
      //special
        void updateColonyBorder( const Cell* cell ); //compare the coordinates of the cell to the current border
        void createCell( const cg::CellType* cellType, TReal x, TReal y, TReal theta ); //create cell from scratch (from CellPlacer)
        void divideCell( Cell* motherCell ); //add a daughter cell from mother


    private:
    	PopulationType population; //the agents
    	ut::Rectangle colonyBorder; //outer border of the cells, for the auto zoom and growth of the signals grid

      //fun
        void updatePopInternal(); //call updateInternal() for all the cells 
        void updatePopInternalChunk( size_t start, size_t end ); //call updateInternal() for a section of the cells, from start to end
        void updatePopInternalConcurrentChunks(); //same effect as updatePopInternal() but concurrent, calling several times updatePopInternalChunk() in parallel
        void calculatePopInteractions(); //calculate the interactions to apply by updatePopInteractions()
        void updatePopInteractions(); //call to updateInteractions() for all the cells (this one is not concurrent due to the access to common resources)
        void updatePopDeath(); //delete the cells that must die
};

#endif // CELLSHANDLER_H