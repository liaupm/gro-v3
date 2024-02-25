/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summarized here.
// Please see the file LICENSE.txt for the complete license.
//
// THE SOFTWARE (AS DEFINED BELOW) AND HARDWARE DESIGNS (AS DEFINED BELOW) IS PROVIDED
// UNDER THE TERMS OF THIS OPEN SOURCE LICENSE (“LICENSE”).  THE SOFTWARE IS PROTECTED
// BY COPYRIGHT AND/OR OTHER APPLICABLE LAW.  ANY USE OF THIS SOFTWARE OTHER THAN AS
// AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// BY EXERCISING ANY RIGHTS TO THE SOFTWARE AND/OR HARDWARE PROVIDED HERE, YOU ACCEPT AND
// AGREE TO BE BOUND BY THE TERMS OF THIS LICENSE.  TO THE EXTENT THIS LICENSE MAY BE
// CONSIDERED A CONTRACT, THE UNIVERSITY OF WASHINGTON (“UW”) GRANTS YOU THE RIGHTS
// CONTAINED HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF SUCH TERMS AND CONDITIONS.
//
// TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
//
//

#ifndef _WORLD_H_
#define _WORLD_H_

#include "ut/Time.hpp" //time
#include "Defines.h"
#include "GlobalParams.h" //default options
#include "GroCollection.h" //groCollection
#include "PetriDish.h" //petriDish, ::PopulationType

//PRECOMPILED
//#include <memory> //SP, make_shared


namespace ut { class Rectangle; class SeedGeneratorBase; } //passing colonyBorder
namespace cg { class CellType; class GeneticCollection; } //param of createCell()
namespace mg { class MediumCollection; } //mediumCollection, link to M genetics

class GroThread; //callingThread
class World
{ 
    public:
        using PopulationType = PetriDish::PopulationType;
        static constexpr TReal DF_STEP_SIZE = GlobalParams::DF_STEP_SIZE;


    //---ctor, dtor
        World( GroThread* callingThread )
        : callingThread( callingThread ), petriDish( nullptr ), time( DF_STEP_SIZE )
        , groCollection( nullptr ), geneticCollection( nullptr ), mediumCollection( nullptr )
        , seed( GlobalParams::DF_SEED ), cellsPerThread( GlobalParams::DF_CELLS_PER_THREAD ), gridRowsPerThread( GlobalParams::DF_GRID_ROWS_PER_THREAD ), maxThreads( GlobalParams::DF_MAX_THREADS )
        , stopFlag( false ), bFirstUpdate( true ), nextCellId(0) {;}

        virtual ~World() = default;

    //---get
      //resources
        inline GroThread* getCallingThread() const { return callingThread; }
        inline const PetriDish* getPetriDish() const { return petriDish.get(); }
        inline PetriDish* getPetriDishEdit() { return petriDish.get(); }
        inline const ut::Time& getTime() const { return time; }
        inline const GroCollection* getGroCollection() const { return groCollection.get(); }
        inline GroCollection* getGroCollectionEdit() { return groCollection.get(); }
        inline const cg::GeneticCollection* getGeneticCollection() const { return geneticCollection.get(); }
        inline cg::GeneticCollection* getGeneticCollectionEdit() { return geneticCollection.get(); } 
        inline const mg::MediumCollection* getMediumCollection() const { return mediumCollection.get(); } 
        inline mg::MediumCollection* getMediumCollectionEdit() { return mediumCollection.get(); } 
      //options
        inline uint getSeed() const { return seed; }
        inline size_t getCellsPerThread() const { return cellsPerThread; }
        inline size_t getGridRowsPerThread() const { return gridRowsPerThread; }
        inline size_t getMaxThreads() const { return maxThreads; }
      //state
        inline bool getAndSwitchStopFlag() { bool b = stopFlag; stopFlag = false; return b; } //return whether the simulation must stop and switch
        inline size_t getNextCellId() { return nextCellId++; }
      //wrapper
        inline ut::SeedGeneratorBase* getSeedGenerator() const { return groCollection->getSeedGenerator(); }
        inline const ut::Rectangle& getColonyBorder() const { return petriDish->getColonyBorder(); }
        inline const PopulationType& getPopulation() const  { return petriDish->getPopulation(); }
        inline size_t getPopulationSize() const { return petriDish->getPopulationSize(); } 
        inline bool getBUseSignals() const { return petriDish->getBUseSignals(); } 
        
    //---set
        inline void setCellsPerThread( size_t xCellsPerThread ) { cellsPerThread = xCellsPerThread; }
        inline void setGridRowsPerThread( size_t xGridRowsPerThread ) { gridRowsPerThread = xGridRowsPerThread; }
        inline void setMaxThreads( size_t xMaxThreads ) { maxThreads = xMaxThreads; }
        inline void setStopFlag( bool xStopFlag ) { stopFlag = xStopFlag; }

    //---API
      //precompute
        void precompute( TReal stepSize, uint xSeed ); 
        inline void createDish() { petriDish = std::make_shared<PetriDish>( this ); } 
        inline void initCollections() { geneticCollection->init(); mediumCollection->init(); groCollection->init(); }
      //run
        inline void init() { petriDish->init(); }
        inline void update() { time.advance(); petriDish->update(); }
        inline void updateOutputExternal() { petriDish->updateOutputExternal(); }


    private:
      //resources
        GroThread* callingThread; //back ptr to the owner thread
        SP<PetriDish> petriDish; //curretly one, but prepared for several
        ut::Time time; //time, step and step size
        SP<GroCollection> groCollection; //sim elements
        SP<cg::GeneticCollection> geneticCollection; //link to M genetics
        SP<mg::MediumCollection> mediumCollection; //link to M genetics
      //options
        uint seed; //master random seed, used to start the seed generator
        size_t cellsPerThread; //checked by CellsHandler
        size_t gridRowsPerThread; //checked by CellsHandler
        size_t maxThreads; //maximum number of threads dedicated to updating the cells
      //state
        bool stopFlag; //whether simulation must end
        bool bFirstUpdate; //TODO
        size_t nextCellId; //unique for every cell in the simulation
};

#endif
