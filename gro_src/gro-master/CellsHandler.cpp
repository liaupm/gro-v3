#include "CellsHandler.h"
#include "Cell.h" //population
#include "PetriDish.h" //owner, getWorldEdit(), getMediumConcs(), getWorld()
#include "World.h" //getCellsPerThread()

//PRECOMPILED
/* #include <algorithm> //min, max in updateColonyBorder()
#include <future> //concurrent update std::future, std::async */


//-------------------------------------------------------------------API common
bool CellsHandler::update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* )
{
	updatePopInternalConcurrentChunks();
    //updatePopInternal();
    calculatePopInteractions();
    updatePopInteractions();
    updatePopDeath();
    owner->setState( collection->getCellNumMarker(), population.size() );
	return true;
}


//-------------------------------------------------------------------API special
void CellsHandler::updateColonyBorder( const Cell* cell )
{
    colonyBorder.x1 = std::min( colonyBorder.x1, cell->getBody().getCenterX() );
    colonyBorder.x2 = std::max( colonyBorder.x2, cell->getBody().getCenterX() );
    colonyBorder.y1 = std::min( colonyBorder.y1, cell->getBody().getCenterY() );
    colonyBorder.y2 = std::max( colonyBorder.y2, cell->getBody().getCenterY() );
}

void CellsHandler::createCell( const cg::CellType* cellType, TReal x, TReal y, TReal theta )
{ 
    population.push_back( std::make_shared<Cell>( cellType, owner->getWorldEdit(), x, y, theta ) ); 
} 

void CellsHandler::divideCell( Cell* motherCell ) 
{ 
	SP<Cell> daughterCell = std::make_shared<Cell>( motherCell );
    population.push_back( daughterCell );
	motherCell->updateDivided( daughterCell->getSiblingId(), daughterCell->getMotherId(), daughterCell->getId() );
} //add a daughter cell from mother



//-------------------------------------------------------------------private

void CellsHandler::updatePopInternal()
{
    for( size_t c = 0; c < population.size(); c++ )
        population[c]->updateInternal( owner->getMediumConcs( population[c]->getBody().getCenterX(), population[c]->getBody().getCenterY() ) );
    //it is neccessary to update those marked for death too e.g. to check the lysis gates
}

void CellsHandler::updatePopInternalChunk( size_t start, size_t end )
{
    for( size_t c = start; c < end; c++ )
        population[c]->updateInternal( owner->getMediumConcs( population[c]->getBody().getCenterX(), population[c]->getBody().getCenterY() ) );
    //it is neccessary to update those marked for death too e.g. to check the lysis gates
}

void CellsHandler::updatePopInternalConcurrentChunks()
{
    size_t chunkSize = owner->getWorld()->getCellsPerThread(); //number of cells per chunck
    size_t chunkNum = population.size() / chunkSize; //number of chuncks

    if( population.size() % chunkSize != 0 )
        chunkNum++;

    if( chunkNum > owner->getWorld()->getMaxThreads() )
    {
    	chunkNum = owner->getWorld()->getMaxThreads();
        chunkSize = ( population.size() + chunkNum - 1 ) / chunkNum;
    }

    if( chunkNum <= 1 )
        updatePopInternal();
    else
    {
      //create and store futures
        std::vector< std::future<void> > futures;
        futures.reserve( chunkNum );
        for( size_t c = 0; c < chunkNum; c++ )
            futures.emplace_back( std::async( &CellsHandler::updatePopInternalChunk, this, c * chunkSize, std::min( (c + 1) * chunkSize, population.size() ) ) );

      //concurrent execution. All must finish for the function to return
        for( auto& fut : futures )
            fut.wait();
    }
} 

void CellsHandler::calculatePopInteractions()
{
	if( owner->getBUseConjugation() )
	{
		for ( size_t c = 0; c < getPopulationSize(); c++ )
			population[c]->calculateInteractions();  
	}
}

void CellsHandler::updatePopInteractions()
{
    size_t cellNum = getPopulationSize(); //not in the loop header because population is increased with new cells (cell division) within the loop and new cells mustn't be updated
    for ( size_t c = 0; c < cellNum; c++ )
    {        
        if( ! population[c]->checkDeath() )
        {
            updateColonyBorder( population[c].get() );
        }
        population[c]->updateInteractions(); 
        if( owner->getBUseSignals() )
            owner->getMediumEdit().addConcs( population[c]->getGenome().getMetFluxes(), population[c]->getBody().getCenterX(), population[c]->getBody().getCenterY() );

        if( ! population[c]->checkDeath() && population[c]->checkDivision() )
            divideCell( population[c].get() );
    }
}

void CellsHandler::updatePopDeath()
{
    for( auto ip = population.begin(); ip != population.end(); )
    {
        if( (*ip)->checkDeath() )
        {
            *ip = population.back();
            population.pop_back();
        }
        else
            ip++;
    }
}
