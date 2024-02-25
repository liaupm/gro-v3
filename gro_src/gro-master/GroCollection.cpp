#include "GroCollection.h"
#include "ut/SeedGenerator.hpp" //seedGenerator


GroCollection::GroCollection( SP<ut::SeedGeneratorBase> seedGen ) : cellNumMarker( nullptr ), seedGenerator( seedGen ) {;}


void GroCollection::rankAndSort()
{
  //functions
	for( size_t f = 0; f < elements[ ElemTypeIdx::POP_FUNCTION ]->size(); f++ )
		getByIdEdit<PopulationFunction>( f )->calculateRank();
	
	std::vector< SP<PopulationFunction> > functions = std::static_pointer_cast< ut::PolyVector< PopulationFunction, GroElement > >( elements[ ElemTypeIdx::POP_FUNCTION ] )->getElements();
	std::sort( functions.begin(), functions.end(), []( SP<PopulationFunction> a, SP<PopulationFunction> b ) { return a->getRank() < b->getRank(); } );
	elements[ ElemTypeIdx::POP_FUNCTION ]->clear();

	for( size_t f = 0; f < functions.size(); f++ )
	{
		functions[ f ]->setRelativeId( f );
		elements[ ElemTypeIdx::POP_FUNCTION ]->push_back( functions[ f ] );
	}

  //gates
	std::vector< SP<PopulationGate> > bGates;
	std::vector< SP<PopulationGate> > qGates;

	for( size_t ga = 0; ga < elements[ ElemTypeIdx::POP_GATE ]->size(); ga++ )
	{
		getByIdEdit<PopulationGate>( ga )->calculateRank();

		if( getById<PopulationGate>( ga )->getGateType() == PopulationGate::GateType::BGATE )
			bGates.push_back( std::static_pointer_cast<PopulationGate>( elements[ ElemTypeIdx::POP_GATE ]->getElementPtr( ga ) ) );
		else if( getById<PopulationGate>( ga )->getGateType() == PopulationGate::GateType::QGATE )
			qGates.push_back( std::static_pointer_cast<PopulationGate>( elements[ ElemTypeIdx::POP_GATE ]->getElementPtr( ga ) ) );
	}
	std::sort( bGates.begin(), bGates.end(), []( SP<PopulationGate> a, SP<PopulationGate> b ) { return a->getRank() < b->getRank(); } );
	elements[ ElemTypeIdx::POP_GATE ]->clear();

	for( size_t qga = 0; qga < qGates.size(); qga++ )
	{
		qGates[ qga ]->setRelativeId( qga );
		elements[ ElemTypeIdx::POP_GATE ]->push_back( qGates[ qga ] );
	}
	for( size_t bga = 0; bga < bGates.size(); bga++ )
	{
		bGates[ bga ]->setRelativeId( qGates.size() + bga );
		elements[ ElemTypeIdx::POP_GATE ]->push_back( bGates[ bga ] );
	}
}


void GroCollection::passUses()
{
	for( size_t t = 0; t < elements[ ElemTypeIdx::TIMER ]->size(); t++ )
		getByIdEdit<Timer>( t )->passUsedForTimers();
	for( size_t f = 0; f < elements[ ElemTypeIdx::CELLS_FILE ]->size(); f++ )
		getByIdEdit<CellsFile>( f )->passUsedForOutput( nullptr );
	for( size_t p = 0; p < elements[ ElemTypeIdx::CELLS_PLOT ]->size(); p++ )
		getByIdEdit<CellsPlot>( p )->passUsedForPlots();
}