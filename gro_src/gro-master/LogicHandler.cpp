#include "LogicHandler.h"
#include "ut/StatPack.hpp" //stats

#include "PopulationStat.h" //model
#include "PopulationFunction.h" //model
#include "PopulationGate.h" //model
#include "CellsFile.h" //static cast in updateForOutput()
#include "GroCollection.h" //getElementNum(), getById(), ::ElemTypeIdx
#include "PetriDish.h" //::PopulationType, get, check and set state, getCollection(), getWorld()
#include "World.h" //getCallingThread()


//////////////////////////////////////////////////////////// POPULATION STAT REPORT ////////////////////////////////////////////////////////////

PopulationStatReport::PopulationStatReport( const PopulationStat* model, LogicHandler* ownerHandler ) 
: GroReport( model, ownerHandler )
, sum( 0.0 ) 
{
	if( model->getStatsType() == ut::StatPack::Type::CUSTOM )
		stats = std::make_shared<ut::StatPack>( model->getStatTypes() ); //by individual stat types
	else
		stats = std::make_shared<ut::StatPack>( model->getStatsType() );   //by stat pack type
}

//--------------------------------------------------------------API
bool PopulationStatReport::update( const EventType* )
{
	const PetriDish::PopulationType& population = ownerHandler->getOwner()->getPopulation();
	resetStats();

  //first pass, all the stats at the same time
	for( const auto& cell : population )
	{
		if( ! cell->getGenome().checkBState( model->getFilterGate() ) )
			continue;
		updateFieldFirstPass( cell.get() );
	}
  //second pass, stat by stat because of the dependencies among them
	for( size_t s = 0; s < getTotalStatNum(); s++ )
	{
		for( const auto& cell : population )
		{
			if( ! cell->getGenome().checkBState( model->getFilterGate() ) )
				continue;
			updateFieldSecondPass( cell.get(), s );
		}
		updateFieldFinal( s );
	}
  //update the state via the stat markers
	for( size_t s = 0; s < getStatNum(); s++ )
		ownerHandler->getOwner()->setState( model->getMarkers()[s], getStat( s ) );
	ownerHandler->getOwner()->setState( model, getStat( 0 ) ); //the first stat is repeated as the qstate of the elem
	return true;
}

bool PopulationStatReport::updateForTimers()
{
	if( model->getBUsedForTimers() )
	{
		update( nullptr );
		if( ! checkUpdateForOutput() )
			stats->commit();
		return true;
	}
	return false;
}

bool PopulationStatReport::checkUpdateForOutput()
{
	if( model->getBUsedForPlots() )
		return true;

	for( const CellsFile* file : model->getUserFiles() )
	{
		if(  ownerHandler->getOwner()->checkBState( file->getTimer() ) )
			return true;
	}
	return false;
}

bool PopulationStatReport::updateForOutput()
{
	if( checkUpdateForOutput() )
	{
		update( nullptr );
		stats->commit();
		return true;
	}
	return false;
}

TReal PopulationStatReport::updateFieldFirstPass( const Cell* cell )
{
	if( ! cell->getGenome().checkBState( model->getFilterGate() ) )
		return 0.0;
	TReal fieldVal = model->getField( cell );
	stats->firstPass( fieldVal );
	return fieldVal;
}

TReal PopulationStatReport::updateFieldSecondPass( const Cell* cell, size_t statIdx )
{
	if( ! cell->getGenome().checkBState( model->getFilterGate() ) )
		return 0.0;

	TReal fieldVal = model->getField( cell );
	stats->secondPass( fieldVal, statIdx );
	return fieldVal;
}






//////////////////////////////////////////////////////////// HANDLER  ////////////////////////////////////////////////////////////

void LogicHandler::updateCommon( bool bForTimers, bool bForOutput )
{
  //popStat (has report)
	for( auto& popStatReport : getPopulationStatReportsEdit() )
	{
		if( bForTimers )
			popStatReport.updateForTimers();
		else if( bForOutput )
			popStatReport.updateForOutput();
	}
  //popFunction 
	for( size_t f = 0; f < owner->getCollection()->getElementNum<PopulationFunction>(); f++ )
		check( owner->getCollection()->getById<PopulationFunction>( f ) ); //just check because they are sorted by rank
  //popGate
	for( size_t g = 0; g < owner->getCollection()->getElementNum<PopulationGate>(); g++ )
		check( owner->getCollection()->getById<PopulationGate>( g ) ); //just check because they are sorted by rank
}

TReal LogicHandler::check( const PopulationFunction* function )
{
	TReal newState = function->evaluate( owner->getQState() );

	if( newState != owner->checkQState( function ) )
		owner->setState( function, newState );
		//no need to forward because all the affected elements are evaluated after and every time step
	return newState;
}

bool LogicHandler::check( const PopulationGate* gate )
{
	bool newState = gate->check( owner->getBState(), owner->getQState() );
	if( newState != owner->checkBState( gate ) )
		owner->setState( gate, newState );
		//no need to forward because all the affected elements are evaluated after and every time step
	return newState;
}

TReal LogicHandler::deepCheck( const PopulationFunction* function )
{
	for( const auto& in : function->getInput() )
	{
		if( in->getElemType() == GroElement::ElemTypeIdx::POP_FUNCTION )
			deepCheck( static_cast<const PopulationFunction*>( in ) );
		//PopGates cannot be inputs to PopFunctions, PopFunctions are always evaluated before PopGates
	}	
	return check( function );
}	

bool LogicHandler::deepCheck( const PopulationGate* gate )
{
  //Q: the input may be a PopFunction
	if( gate->getGateType() == PopulationGate::GateType::QGATE && static_cast<const PopulationGateQuantitative*>( gate )->getElem()->getElemType() ==GroElement::ElemTypeIdx::POP_FUNCTION )
		deepCheck( static_cast<const PopulationFunction*>( static_cast<const PopulationGateQuantitative*>( gate )->getElem() ) );
  //B: the input may be another PopGate or a PopFunction (rare)
	else
	{
		for( const auto& in : static_cast< const PopulationGateBoolean* >( gate )->getInput() )
		{
			if( in->getElemType() == GroElement::ElemTypeIdx::POP_FUNCTION )
				deepCheck( static_cast< const PopulationFunction* >( in ) );
			else if( in->getElemType() == GroElement::ElemTypeIdx::POP_GATE )
				deepCheck( static_cast< const PopulationGate* >( in ) );
		}	
	}
	return check( gate );
}
