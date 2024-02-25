#ifndef PETRIDISH_H
#define PETRIDISH_H

#include "ut/Event.hpp" //template param
#include "ut/Agent.hpp" //base
#include "mg/Medium.hpp" //link to M signals, ::ConcsDS

#include "GroElement.h" //template param
#include "GroCollection.h" //template param
#include "GroHandler.h" //::HandlerTypeIdx

#include "CellsHandler.h"
#include "TimersHandler.h"
#include "LogicHandler.h"
#include "PlacingHandler.h"
#include "OutputHandler.h"

//PRECOMPILED
/*#include <memory> //SP
#include "CellEngine.h" //master file of ce module
#include "CESpace.h" //ceSpace* space, link to CellEngine */


namespace ut { class Rectangle; } //passing colony border
namespace cg { class CellType; } //param in createCell()
class World; //ctor
class Cell; //CellsHandler wrapper functions

struct GroScheduler
{	
	using EventType = ut::Event<GroElement>;
};

class PetriDish : public ut::Agent< PetriDish, GroScheduler, GroCollection, GroHandler<>::HandlerTypeIdx
                                  , OutputHandler, PlacingHandler, TimersHandler, LogicHandler, CellsHandler>
{
	public:
        using AgentBaseType = PetriDish::Agent;
        using PopulationType = CellsHandler::PopulationType;
        inline static const mg::Medium::ConcsDS EMPTY_CONCS = {};

	 //---ctor, dtor
	    PetriDish( World* world, mg::Medium::DiffuserType diffuserType = mg::Medium::DF_DIFFUSER );
        PetriDish( const PetriDish& original ) = delete;
        PetriDish& operator=( const PetriDish& original ) = delete;
	    virtual ~PetriDish();
		
	//---get
	  //resources
	    inline const World* getWorld() const { return world; } 
	    inline World* getWorldEdit() { return world; } 
        inline ceSpace* getSpace() const { return space; }
        inline const mg::Medium& getMedium() const { return *medium; }
        inline mg::Medium& getMediumEdit() { return *medium; }
      //options
        inline bool getBUseSignals() const { return bUseSignals; }
        inline bool getBUseConjugation() const { return bUseConjugation; }
      //wrapper
        //CellsH
        inline const PopulationType& getPopulation() const { return getCellsHandler().getPopulation(); }
        inline PopulationType& getPopulationEdit() { return getCellsHandlerEdit().getPopulationEdit(); }
        inline size_t getPopulationSize() const { return getCellsHandler().getPopulationSize(); } //get number of cells
        inline const ut::Rectangle& getColonyBorder() const { return getCellsHandler().getColonyBorder(); }
        //medium
        inline const mg::Medium::ConcsDS& getMediumConcs( TReal centerX, TReal centerY ) const { return bUseSignals ? medium->getConcs( centerX, centerY ) : EMPTY_CONCS; }
      //handlers
        inline const CellsHandler& getCellsHandler() const { return getHandler<GroHandlerIdx::H_CELLS>(); }
        inline const TimersHandler& getTimersHandler() const { return getHandler<GroHandlerIdx::H_TIMERS>(); }
        inline const LogicHandler& getLogicHandler() const { return getHandler<GroHandlerIdx::H_LOGIC>(); }
        inline const PlacingHandler& getPlacingHandler() const { return getHandler<GroHandlerIdx::H_PLACING>(); }
        inline const OutputHandler& getOutputHandler() const { return getHandler<GroHandlerIdx::H_OUTPUT>(); }

        inline CellsHandler& getCellsHandlerEdit() { return getHandlerEdit<GroHandlerIdx::H_CELLS>(); }
        inline TimersHandler& getTimersHandlerEdit() { return getHandlerEdit<GroHandlerIdx::H_TIMERS>(); }
        inline LogicHandler& getLogicHandlerEdit() { return getHandlerEdit<GroHandlerIdx::H_LOGIC>(); }
        inline PlacingHandler& getPlacingHandlerEdit() { return getHandlerEdit<GroHandlerIdx::H_PLACING>(); }
        inline OutputHandler& getOutputHandlerEdit() { return getHandlerEdit<GroHandlerIdx::H_OUTPUT>(); }

    //---API
      //precompute
    	void precompute();
      //run
        inline void init() { initHandlers(); }
    	inline void update() { updateHandlers( nullptr ); updateSpace(); updateSignalsGrid(); }
      //wrapper
    	inline void updateSpace() { ceStep( space ); }
    	void updateSignalsGrid();
        inline void updatePopulation() { getCellsHandlerEdit().update( &changes, &changesSummary, nullptr ); }
        inline void updateOutputExternal() { getOutputHandlerEdit().updateExternal(); }
        inline void createCell( const cg::CellType* cellType, TReal x, TReal y, TReal theta ) { getCellsHandlerEdit().createCell( cellType, x, y, theta ); } 


	private:
	  //resources
        World* world;
        ceSpace* space; //link to M cellEngine
        SP<mg::Medium> medium; //link to M signals
      //options
        bool bUseSignals;
        bool bUseConjugation;
};

#endif // PETRIDISH_H