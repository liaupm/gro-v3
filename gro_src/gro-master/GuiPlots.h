#ifndef GUIPLOTS_H
#define GUIPLOTS_H

#include "Defines.h"
#include "CellsPlot.h"
#include "LogicHandler.h"
#include "OutputHandler.h"

//#include "qcustomplot.h"
//#include "ui_GuiPlots.h" //ui member

/*PRECOMPILED
#include <vector> //plots, cellsPlots
#include <memory> //UP ui 
#include <QMainWindow> //base */


namespace Ui 
{
	class GuiPlots;
}

class World;
class GuiPlots : public QMainWindow
{
    Q_OBJECT

public:
	static constexpr size_t MAX_PLOT_NUM = 4;
	static constexpr size_t MAX_GRAPH_NUM = 8;
	static constexpr size_t MAX_PLOT_TIME = 100000;

//---ctor, dtor
    explicit GuiPlots( QWidget* parent = nullptr );
    GuiPlots( const GuiPlots& ) = delete; // non-copiable to allow for UP members
    virtual ~GuiPlots() = default;
	
//---get
	const std::vector< QCustomPlot* >& getPlots() { return plots; }
	QCustomPlot* getPlot( size_t idx ) { return plots[ idx ]; }

//---API
  //precompute
	void connectToGro( World* xWorld ); //calls connectCellsPlot() for all the CellPlots (up to 4)
	void connectCellsPlot( const CellsPlot* cellsPlot ); //connect a gro CellsPlot elements to a GUI QCustomPlot object
	void clearPlots();
  //run
	void updateCellsPlots();
	void updateCellsPlot( const CellsPlot* cellsPlot );
	

private:
  //resources
	SP< Ui::GuiPlots > ui;
	World* world;
	std::vector<QCustomPlot*> plots; //GUI charts
  //state
    size_t nextPlotIdx;
    bool bFirstCall; //to know whether to remove data from provious simulations

  //fun
	QCustomPlot* getNextPlot() { return nextPlotIdx < MAX_PLOT_NUM ? plots[ nextPlotIdx++ ] : nullptr; }
};

#endif // GUIPLOTS_H
