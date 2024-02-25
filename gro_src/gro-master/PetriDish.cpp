#include "PetriDish.h"
#include "ut/Time.hpp" //.stepSize
#include "mg/Signal.hpp" //getElementNum<mg::Signal>
#include "mg/MediumCollection.hpp" //getElementNum()
#include "mg/Grid.hpp" //create medium
#include "World.h" //getGroCollection(), getTime(), getMediumCollection()


PetriDish::PetriDish( World* world, mg::Medium::DiffuserType diffuserType )
: AgentBaseType( world->getGroCollection(), world->getTime() )
, world( world ), space( ceCreateSpace() ), medium( nullptr ) 
, bUseSignals( world->getMediumCollection()->getAny<mg::Signal>() )
, bUseConjugation( world->getGeneticCollection()->getAny<cg::Pilus>() && world->getGeneticCollection()->getAny<cg::OriT>() )
{
    if( bUseSignals )
    {
        const auto& grid = world->getMediumCollection()->getById<mg::Grid>( 0 ); //TODO
        medium = std::make_shared<mg::Medium>( world->getMediumCollection(), grid, diffuserType );
    }
    precompute();
}

PetriDish::~PetriDish()
{
    getPopulationEdit().clear(); //cells have to be deleted before the space, not after when deleting the handlers
    ceDestroySpace( space );
}

//-------------------API-precompute

void PetriDish::precompute()
{
    ceInit( 4, 1, Cell::Body::CE_MAX_LENGTH, Cell::Body::CE_MIN_LENGTH, Cell::Body::CE_WIDTH );
    if( bUseSignals )
        medium->init();
}


//-------------------API-run

void PetriDish::updateSignalsGrid() 
{ 
    if( bUseSignals ) 
        medium->update( world->getTime().stepSize, world->getGridRowsPerThread(), world->getMaxThreads() ); //in cpp due to access to world
}