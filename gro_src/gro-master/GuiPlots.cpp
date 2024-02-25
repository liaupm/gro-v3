#include "GuiPlots.h"
#include "ui_GuiPlots.h" //ui member
#include "World.h" //usage of member world, get time for addData()

/*PRECOMPILED
#include <QColor> //setting the pen colour on connection to CellsPlot */


GuiPlots::GuiPlots( QWidget* parent )
: QMainWindow( parent )
, ui( std::make_shared<Ui::GuiPlots>() ), world( nullptr )
, plots( {} )
, nextPlotIdx( 0 ), bFirstCall( true )
{
	ui->setupUi( this );
    setAttribute(Qt::WA_DeleteOnClose);
}


//---------------API-precompute

void GuiPlots::connectToGro( World* xWorld )
{
	world = xWorld;
  //get MAX_PLOT_NUM QCustomPlots from ui
	plots = std::vector<QCustomPlot*>( { ui->plot0, ui->plot1, ui->plot2, ui->plot3 } );
  
  //create title row if first call or clear previous data if not
	if( bFirstCall )
	{
		for( auto& plot : plots )
			plot->plotLayout()->insertRow( 0 );
		bFirstCall = false;
	}
	else
		clearPlots();

  //connect each gro CellsPlot to a QCustomPlot
	//cellsPlots.clear();
    for( size_t p = 0; p < world->getGroCollection()->getElementNum<CellsPlot>() && p < MAX_PLOT_NUM; p++ )
        connectCellsPlot( world->getGroCollection()->getById<CellsPlot>( p ) );
}

void GuiPlots::connectCellsPlot( const CellsPlot* cellsPlot )
{
  //iterate the MAX_PLOT_NUM available charts in the GUI
	QCustomPlot* plot = getNextPlot();
	if( ! plot )
		return;

  //ser the legend and line colour for each graph (stat) in the CellPlot/QCustomPlot
	const auto& hexColours = cellsPlot->getHexColours();
	CellsPlotReport& cellsPlotRp = world->getPetriDishEdit()->getOutputHandlerEdit().getCellsPlotReportsEdit()[ cellsPlot->getRelativeId() ];
	for( size_t g = 0; g < cellsPlotRp.getHistoricNum() && g < MAX_GRAPH_NUM; g++ )
	{
		plot->addGraph();
		plot->graph( plot->graphCount() - 1 )->setPen( QPen( QColor( hexColours[ g ].c_str() ) ) );
		plot->graph( plot->graphCount() - 1 )->setName( cellsPlotRp.getLegend()[g].c_str() );
		plot->graph( plot->graphCount() - 1 )->setData( {}, {} );
	}

  //show legend, align and set the available user interactions
	plot->setLocale( QLocale( QLocale::English, QLocale::UnitedKingdom ) );
	plot->legend->setVisible( true );
	plot->setAutoAddPlottableToLegend( true );
	plot->axisRect()->insetLayout()->setInsetAlignment( 0, Qt::AlignTop | Qt::AlignLeft );
	plot->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables );
 
  //x axis = time
	plot->xAxis->setLabel( "Time (min)" );

  // add title
	if( plot->plotLayout()->elementAt( plot->plotLayout()->rowColToIndex( 0, 0 ) ) )
		plot->plotLayout()->removeAt( plot->plotLayout()->rowColToIndex( 0, 0 ) );
	plot->plotLayout()->addElement( 0, 0, new QCPTextElement( plot, cellsPlot->getName().c_str(), QFont( "sans", 10, QFont::Bold ) ) ); //the plot takes ownership and deletes the raw pointer 

	repaint();
}

//----------------------API-run

void GuiPlots::updateCellsPlots()
{ 
	for( size_t p = 0; p < world->getGroCollection()->getElementNum<CellsPlot>() && p < MAX_PLOT_NUM; p++ )
		updateCellsPlot( world->getGroCollection()->getById<CellsPlot>( p ) );
}

void GuiPlots::updateCellsPlot( const CellsPlot* cellsPlot )
{	
	QCustomPlot* plot = plots[ cellsPlot->getRelativeId() ];
	CellsPlotReport& cellsPlotRp = world->getPetriDishEdit()->getOutputHandlerEdit().getCellsPlotReportsEdit()[ cellsPlot->getRelativeId() ];

  //plot data, rescale and repaint
	for( size_t g = 0; g < cellsPlotRp.getHistoricNum() && g < MAX_GRAPH_NUM; g++ )
		plot->graph( g )->setData( cellsPlotRp.getTimeHistoric(), cellsPlotRp.getHistoric( g ), true );
	plot->rescaleAxes();
	plot->replot(); 

  //save as image if required
	if( cellsPlotRp.checkSaveToFile() )
	{
		if( cellsPlot->getFormat() == ".jpg" )
			plot->saveJpg( ( cellsPlot->getPath() + "/" + cellsPlot->getFileName() + std::to_string( world->getTime().step ) + ".jpg" ).c_str() );
		else if( cellsPlot->getFormat() == ".bmp" )
			plot->saveBmp( ( cellsPlot->getPath() + "/" + cellsPlot->getFileName() + std::to_string( world->getTime().step ) + ".bmp" ).c_str() );
		else
			plot->savePng( ( cellsPlot->getPath() + "/" + cellsPlot->getFileName() + std::to_string( world->getTime().step ) + ".png" ).c_str() );
	}
}


//------------------------private

void GuiPlots::clearPlots()
{ 
	for( size_t p = 0; p < nextPlotIdx; p++ ) 
	{
        for( int g = 0; g < plots[p]->graphCount(); g++ )
		{
			plots[p]->graph(g)->setData( {}, {} );
			plots[p]->graph(g)->rescaleAxes();
		}
		plots[p]->plotLayout()->removeAt( plots[p]->plotLayout()->rowColToIndex( 0, 0 ) );
		//plots[p]->plotLayout()->simplify();
		plots[p]->clearGraphs();
		plots[p]->replot(); 
		plots[p]->update(); 
		plots[p]->rescaleAxes();
		plots[p]->xAxis->setLabel( "" );
	}
	nextPlotIdx = 0;
	repaint();
}
