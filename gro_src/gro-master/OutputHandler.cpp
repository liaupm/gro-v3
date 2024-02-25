#include "OutputHandler.h"
#include "Snapshot.h" //model
#include "CellsFile.h" //model
#include "CellsPlot.h" //model
#include "LogicHandler.h" //popStatReports
#include "PetriDish.h"
#include "World.h"
#include "GroThread.h"

//PRECOMPILED
/*#include <iomanip>  //std::setprecision()
#include <QDir> //QDir for SO-agnostic ccreation of output dir in init() */



//////////////////////////////////////////////////////////////////// REPORTS ///////////////////////////////////////////////////////////////////////

//======================================================================================Snapshot
void SnapshotReport::updateExternal()
{
	if( ownerHandler->getOwner()->checkBState( model->getTimer() ) )
	{
		ownerHandler->getOwner()->getWorld()->getCallingThread()->setBUpdateGraphics( true );
		ownerHandler->getOwner()->getWorld()->getCallingThread()->addSnapshot( model );
	}
}


//======================================================================================CellsFile

//------------------------------------API
void CellsFileReport::link()
{
	popStatReports.clear();
	for( const auto& popStat : model->getStatFields() )
		popStatReports.push_back( &ownerHandler->getOwner()->getHandlerEdit<GroHandlerIdx::H_LOGIC>().getPopulationStatReportsEdit()[ popStat->getRelativeId() ] );
}

bool CellsFileReport::init()
{ 
  //create the dir if it does not exist
	QDir outputDir; 
	outputDir.mkpath( ( model->getPath() ).c_str() );
  //make headers
	makeHeaders();
  //create the population file, print the header and set the precission
	if( model->getMode() != CellsFile::Mode::INDIVIDUAL )
	{
		popFileOut->open( ( model->getPath() + "/" + model->getFileName() + model->getFormat() ).c_str(), std::ios::out );
		if( ! popFileOut->is_open() )
			return false;

		printPopHeader();
		( *popFileOut ) << std::setprecision( model->getPrecision() );
		popFileOut->close();
	}
	return true;
}

void CellsFileReport::updateExternal()
{
	if( ownerHandler->getOwner()->checkBState( model->getTimer() ) )
	{
		if( model->getMode() != CellsFile::Mode::POPULATION )
			printIndividualFile();

		if( model->getMode() != CellsFile::Mode::INDIVIDUAL )
			printPopulationFile();
	}
}


//---------------------------------------------private
void CellsFileReport::makeHeaders()
{
  //individual 
	if( model->getBIncludeAncestry() )
		individualHeader = "id,mother,sibling,origin,generation,ancestry";
	else
		individualHeader = "id";
	for( auto popStatRp : popStatReports )
	{
		if( popStatRp->getModel()->getIsFree() )
			individualHeader += model->getSeparator() + popStatRp->getModel()->getName();
		else
			individualHeader += model->getSeparator() + popStatRp->getModel()->getHeader();
	}
  
  //population
	popHeader = "time";
	for( auto popStatRp : popStatReports )
	{
		for( size_t s = 0; s < popStatRp->getStatNum(); s++ )
		{
			if( popStatRp->getModel()->getIsFree() )
				popHeader += model->getSeparator() + popStatRp->getModel()->getName() + "_" + popStatRp->getStatName( s );
			else
				popHeader += model->getSeparator() + popStatRp->getModel()->getHeader() + "_" + popStatRp->getStatName( s );
		}
	}
	for( const GroElement* elem : model->getOtherFields() )
		popHeader += model->getSeparator() + elem->getName() ;
}

void CellsFileReport::printIndividualFile()
{
  //create the new file with time step in the name
	individualFileOut->open( ( model->getIndividualFileBaseName() + std::to_string( ownerHandler->getOwner()->getWorld()->getTime().step ) + model->getFormat() ).c_str(), std::ios::out );
	if( ! individualFileOut->is_open() )
		return;
  //print header and set precission
	printIndividualHeader();
	( *individualFileOut ) << std::setprecision( model->getPrecision() );
  
  //print the values for every cell that pass the gate filter
    const PetriDish::PopulationType& population =  ownerHandler->getOwner()->getPopulation();
	for( const auto& cell : population )
	{
		if( cell->getGenome().checkBState( model->getIndividualGate() ) )
		{
			( *individualFileOut ) << "\n" << cell->getId();

			if( model->getBIncludeAncestry() )
			{
				( *individualFileOut ) << model->getSeparator() << cell->getMotherId() << model->getSeparator() << cell->getSiblingId() << model->getSeparator() << cell->getOriginId()
				<< model->getSeparator() << cell->getGeneration() << model->getSeparator() << cell->getAncestry();
			}
			for( auto popStatRp : popStatReports )
	            ( *individualFileOut ) << model->getSeparator() << popStatRp->getField( cell.get() );
		}
	}
	individualFileOut->close();
}


void CellsFileReport::printPopulationFile()
{
  //open to append
	popFileOut->open( model->getPopFileWholeName().c_str(), std::ios::app );
	if( ! popFileOut->is_open() )
		return;
  //print time
	( *popFileOut ) << "\n" << ownerHandler->getOwner()->getWorld()->getTime().time;
  
  //print stats
	for( auto* popStatRp : popStatReports )
	{
		for( size_t s = 0; s < popStatRp->getStatNum(); s++ )
			( *popFileOut ) << model->getSeparator() << popStatRp->getStat( s );
	}
  //print non-stat fields
	for( const GroElement* elem : model->getOtherFields() )
		( *popFileOut ) << model->getSeparator() << ownerHandler->getOwner()->checkQState( elem );
	popFileOut->close();
}




//======================================================================================CellsPlot

//-----------------------------API
void CellsPlotReport::link()
{
	popStatReports.clear();
	for( const auto& popStat : model->getStatFields() )
		popStatReports.push_back( &ownerHandler->getOwner()->getHandlerEdit<GroHandlerIdx::H_LOGIC>().getPopulationStatReportsEdit()[ popStat->getRelativeId() ] );

	makeLegend();
  //create a vector to store the historic values of each fied
	for( auto popStatRp : popStatReports )
	{
		for( size_t s = 0; s < popStatRp->getStatNum(); s++ )
			historic.push_back( std::make_shared< HistoricUnitType >() );
	}
    for( size_t h = 0; h < model->getOtherFields().size(); h++ )
		historic.push_back( std::make_shared< HistoricUnitType >() );
}

bool CellsPlotReport::init()
{ 
  //create the dir for storing the graph images if it does not exist
	QDir outputDir; 
	outputDir.mkpath( ( model->getPath() ).c_str() );
    return true;
}

void CellsPlotReport::updateExternal()
{
  //check if an image has to be saved and tell the tGroThread
	if( model->getTimer() && ownerHandler->getOwner()->checkBState( model->getTimer() ) )
	{
		bSaveToFile = true;
		ownerHandler->getOwner()->getWorld()->getCallingThread()->setBUpdatePlots( true );
	}
  //calculate the owned stats (the free stats are already calculated)
	updateHistoric();
}


//---------------------------------------private
void CellsPlotReport::makeLegend()
{
  //stat fields
	for( auto popStatRp : popStatReports )
	{
		for( size_t s = 0; s < popStatRp->getStatNum(); s++ )
		{
			if( popStatRp->getModel()->getIsFree() )
				legend.push_back( popStatRp->getModel()->getName() + "_" + popStatRp->getStatName( s ) );
			else
				legend.push_back( popStatRp->getModel()->getHeader() + "_" + popStatRp->getStatName( s ) );
		}
	}
  //non-stat fields
	for( const GroElement* elem : model->getOtherFields() )
		legend.push_back( elem->getName() );
}

void CellsPlotReport::updateHistoric()
{
	QMutexLocker ml( &mutex ); //avoid different sizes

  //clear all if maximum size reached
    if( timeHistoric.size() >= (int) model->getHistoricSize() )
	{
		timeHistoric.clear();
		for( size_t s = 0; s < historic.size(); s++ )
			historic[s]->clear();
	}
  //time
	timeHistoric.push_back( ownerHandler->getOwner()->getWorld()->getTime().time );
  //stat fields
	size_t curentIdx = 0;
	for( auto popStatRp : popStatReports )
	{
		for( size_t s = 0; s < popStatRp->getStatNum(); s++ )
		{
			historic[ curentIdx++ ]->push_back( model->getBPrecision() ? ut::trimDecimals( popStatRp->getStat( s ), model->getRoundMultiplier() ) : popStatRp->getStat( s ) );
		}
	}
  //non-stat fields
	for( const GroElement* elem : model->getOtherFields() )
		historic[ curentIdx++ ]->push_back( ownerHandler->getOwner()->checkQState( elem ) );
}







//////////////////////////////////////////////////////////////////// HANDLER ///////////////////////////////////////////////////////////////////////

bool OutputHandler::update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* )
{
	owner->getHandlerEdit<GroHandlerIdx::H_LOGIC>().updateForOutput();
    return true;
}

void OutputHandler::updateExternal()
{
	for( auto& snapshotReport : getSnapshotReportsEdit() )
        snapshotReport.updateExternal();

	for( auto& cellsFileReport : getCellsFileReportsEdit() )
        cellsFileReport.updateExternal();

    for( auto& cellsPlotReport : getCellsPlotReportsEdit() )
        cellsPlotReport.updateExternal();
}
