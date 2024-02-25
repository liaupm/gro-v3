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

#include "Cell.h"

#include "ut/Time.hpp" //.time, .stepSize
#include "cg/CellType.hpp" //getStrain()
#include "cg/Strain.hpp" //sampleDistribution() for division rotation

#include "World.h" //getPetriDish()
#include "PetriDish.h" //getSpace()

/*PRECOMPILED
#include <math.h> //sqrt distance to neighbours when updating conjugation */



////////////////////////////////////////////////////////////////////////// BODY WRAPPER ////////////////////////////////////////////////////////////////////////////////

Cell::BodyWrapper::BodyWrapper( World* world, Cell* enclosingCell, TReal length, TReal x, TReal y, TReal rotation )
{
    rawBody = ceCreateBody( world->getPetriDish()->getSpace(), DF_SCALE * length, ceGetVector2(x,y), rotation );
    bValidData = updateData( enclosingCell );
}

Cell::BodyWrapper::BodyWrapper( BodyWrapper& motherBody, Cell* enclosingCell, TReal dRotation, TReal lengthMother, TReal lengthDaughter ) 
: rawBody( ceDivideBodyNew( motherBody.rawBody, dRotation, DF_SCALE * lengthMother, DF_SCALE * lengthDaughter ) ) { bValidData = updateData( enclosingCell ); }

void Cell::BodyWrapper::destroy()
{
    if( rawBody != nullptr )
    {
        ceDestroyBody( rawBody );
        rawBody = nullptr;
        bValidData = false;
    }
}



/////////////////////////////////////////////////////////////////////////////////// CELL ////////////////////////////////////////////////////////////////////////////////

//============================================================================================== ctor, dtor
Cell::Cell( const cg::CellType* cellType, World* world, TReal x, TReal y, TReal rotation ) //new cells
: world( world )
, genome( world->getGeneticCollectionEdit(), cellType, world->getTime(), world->getPetriDish()->getMediumConcs( x, y ) )
, body( world, this, genome.getTotalLength(), x, y, rotation ) 
, id( world->getNextCellId() ), motherId( id ), siblingId( id ), originId( id ), generation( 0 )
, bMarkedForDeath( false ), bSelected( false ) {;}

Cell::Cell( Cell* motherCell ) //from division
: world( motherCell->world ), genome( &motherCell->genome )
, body( motherCell->body, this, motherCell->genome.getCellType()->getStrain()->sampleDistribution( cg::Strain::DistIdx::DIVISION_ROTATION ), motherCell->genome.getTotalLength(), genome.getTotalLength() ) 
, id( motherCell->world->getNextCellId() ), motherId( motherCell->getId() ), siblingId( motherCell->world->getNextCellId() ), originId( motherCell->originId )
, generation( motherCell->generation + 1 ), ancestry( motherCell->ancestry + std::to_string( 0 ) )
, bMarkedForDeath( motherCell->bMarkedForDeath ), bSelected( motherCell->bSelected ) {;}


void Cell::updateDivided( size_t xId, size_t xMotherId, size_t xSiblingId )
{
	updateIds( xId, xMotherId, xSiblingId );
	generation++;
	ancestry = ancestry + std::to_string( 1 );
}

void Cell::updateInternal( const std::vector<TReal>& mediumConcs )
{
    if( mediumConcs.size() > 0 )
    	genome.updateMetExternal( mediumConcs ); //update of metabolism (concentrations track) from the medium concentrations
    genome.update<true, true, true>( world->getTime().time ); //update from events, without external information
}


void Cell::calculateInteractions()
{
	if( genome.checkActivePili() )
    {
      //get the neighbours' Genomes to pass them to the donor's (current cell) Horizontalhandler
        uint size;
        ceBody** neighbors = ceGetNearBodies( body.getRaw(), BodyWrapper::DF_CONJ_DISTANCE, &size ); //neighboring cells = potential recipients, size = number of neighbours
        std::vector<cg::Genome*> neighGenomes;
        neighGenomes.reserve( size );
        for( size_t n = 0; n < size; n++ )
            neighGenomes.push_back( &static_cast<Cell*>( neighbors[n]->data )->getGenomeEdit() );

      //calculate the distance to each neighbour if that is relevant for the rate calculations
        std::vector<TReal> neighDistances;
        if( genome.checkActivePiliDistance() )
        {
            for( size_t n = 0; n < size; n++ )
                neighDistances.push_back( std::sqrt( std::pow( getX() - neighbors[n]->center.x, 2 ) + std::pow( getY() - neighbors[n]->center.y, 2 ) ) );
        }
        genome.updateConjExternal( neighGenomes, neighDistances );
    }
}

void Cell::updateInteractions()
{
  //conjugation
	genome.applyConjugations();
  //CellEngine grow
    body.grow( genome.getDLength(), genome.getTime().stepSize, genome.getBMustGrow() );
}