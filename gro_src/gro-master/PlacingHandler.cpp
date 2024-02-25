#include "PlacingHandler.h"
#include "mg/Signal.hpp" //getRelativeId()
#include "CellPlacer.h" //model
#include "CellPlating.h" //model
#include "SignalPlacer.h" //model
#include "PetriDish.h" //PetriDish::PopulationType, getPopulationEdit()


void PlacingHandler::updateCellPlacer( const CellPlacer* cellPlacer )
{
	if( owner->checkBState( cellPlacer->getTimer() ) )
	{
		size_t cellNum = cellPlacer->sampleCellNum();
		const cg::CellType* cellType = nullptr;

	  //a unique cell type for all the cells if not mixed
        if( ! cellPlacer->getIsMixed() )
            cellType = cellPlacer->sampleCellType();

        for( size_t c = 0; c < cellNum; c++ )
	    {
	      //sample position
			TReal x, y;
			cellPlacer->samplePos( x, y );
	        TReal theta = cellPlacer->sampleRotation();
	      //different cell types if mixed
	        if( cellPlacer->getIsMixed() )
	        	cellType = cellPlacer->sampleCellType();
		  //create the cell
	        owner->createCell( cellType, x, y, theta );
	    }
	}
}

void PlacingHandler::updateCellPlating( const CellPlating* cellPlating )
{
	if( owner->checkBState( cellPlating->getTimer() ) )
	{
		PetriDish::PopulationType& population = owner->getPopulationEdit();

		if( population.size() == 0 )
			return;

	  //collect the idxs of the cells that pass the filters (gate and spatial)
		std::vector<size_t> cellIdxs;
		for( size_t c = 0; c < population.size(); c++ )
		{
			if( population[c]->getGenome().checkBState( cellPlating->getGate() ) && cellPlating->getSpatialFilter()->checkWithin( population[c]->getX(), population[c]->getY() ) )
				cellIdxs.push_back( c );
		}
		if( cellIdxs.size() == 0 )
			return;

	  //sample the number of cells to take and return
		size_t takeNum = ut::round<size_t>( cellPlating->sampleTakeFraction() * cellIdxs.size() );
		if( takeNum == 0 )
			return;
		size_t putNum = cellPlating->getBPut() ? ut::round<size_t>( cellPlating->samplePutFraction() * takeNum ) : 0;

	  //randomly sample the concrete cells 
		std::shuffle( cellIdxs.begin(), cellIdxs.end(), cellPlating->getShuffleRndEngine() );
	  //kill
		for( size_t i = putNum; i < takeNum; i++ )
			population[ cellIdxs[i] ]->markForDeath();
	  //relocate
		for( size_t i = 0; i < putNum; i++ )
		{
			TReal x, y;
			cellPlating->samplePos( x, y );
			population[ cellIdxs[i] ]->getBodyEdit().setPosition( x, y, cellPlating->sampleRotation() );
		}
	}
}

void PlacingHandler::updateSignalPlacer( const SignalPlacer* signalPlacer )
{
	if( owner->checkBState( signalPlacer->getTimer() ) )
	{
	    size_t sourceNum = signalPlacer->sampleSourceNum();
        size_t signalId = 0;


        if( signalPlacer->getMode() == SignalPlacer::Mode::FULL )
        {
        	const ut::Rectangle& area = signalPlacer->getConstArea();

        	if( signalPlacer->getIsMixed() )
        	{
		    	signalId = signalPlacer->sampleSignal()->getRelativeId();
		    	owner->getMediumEdit().setConcRectangle( signalId, signalPlacer->sampleAmount(), area.x1, area.x2, area.y1, area.y2 );
        	}
        	else
        	{
        		signalId = signalPlacer->sampleSignal()->getRelativeId();
        		owner->getMediumEdit().setConcRectangle( signalId, signalPlacer->sampleAmount(), area.x1, area.x2, area.y1, area.y2 );   
        	}
        }
        else
        {
          //usique signal case
		    if( ! signalPlacer->getIsMixed() )
		    	signalId = signalPlacer->sampleSignal()->getRelativeId();

		    for( size_t i = 0; i < sourceNum; i++ )
		    {
		        TReal x, y;
		        signalPlacer->samplePos( x, y );
		      //different sinals in the same event 
		        if( signalPlacer->getIsMixed() )
		        	signalId = signalPlacer->sampleSignal()->getRelativeId();
		      //set or add the signal
		        if( signalPlacer->getMode() == SignalPlacer::Mode::SET )
		        	owner->getMediumEdit().setConc( signalId, signalPlacer->sampleAmount(), x, y );   
		        else
		        	owner->getMediumEdit().addConc( signalId, signalPlacer->sampleAmount(), x, y );     
		    }	
        }
	}
}


bool PlacingHandler::update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* )
{
  //CellsPlacer
    for( size_t cp = 0; cp < owner->getCollection()->getElementNum<CellPlacer>(); cp++ )
    	updateCellPlacer( owner->getCollection()->getById<CellPlacer>( cp ) );
  //CellsPlating
    for( size_t cpt = 0; cpt < owner->getCollection()->getElementNum<CellPlating>(); cpt++ )
    	updateCellPlating( owner->getCollection()->getById<CellPlating>( cpt ) );
  //SignalsPlacer
    for( size_t sp = 0; sp < owner->getCollection()->getElementNum<SignalPlacer>(); sp++ )
    	updateSignalPlacer( owner->getCollection()->getById<SignalPlacer>( sp ) );
    return true;
}
