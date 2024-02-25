#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include "Defines.h"
#include "GroHandler.h" //base
#include "GroReport.h" //base

/*PRECOMPILED
#include <fstream> //file writting

#include <QVector> //historic for CellsPlot
#include <QDir> //QDir for SO-agnostic ccreation of snapshots dir in init() 
#include <QMutex> //in CellsPlot */


namespace ut { class Bitset; class MultiBitset; } //update()

class Snapshot;
class CellsFile;
class CallsPlot;
class GroCollection;
class PopulationStatReport; //popStatReports
class PetriDish; //owner in ctor
class OutputHandler;
struct OutputHReportIdx
{
	enum ReportTypeIdx : size_t { CELLS_PLOT, CELLS_FILE, SNAPSHOT, COUNT };
};


//////////////////////////////////////////////////////////////////// REPORTS ///////////////////////////////////////////////////////////////////////

class SnapshotReport : public GroReport< Snapshot, OutputHandler >
{
    public:
        friend OutputHandler;
    	static constexpr size_t INI_IMG_IDX = 0;


	//---ctor, dtor
        SnapshotReport( const Snapshot* model, OutputHandler* ownerHandler ) 
        : GroReport( model, ownerHandler ), nextImgIdx( INI_IMG_IDX ) {;}

        virtual ~SnapshotReport() = default;

    //---API
      //common
        void link() {;}
        inline bool init() { QDir snapshotDir; snapshotDir.mkpath( ( model->getPath() ).c_str() ); return true; } //create the directory if it does not exist
        bool secondInit() { return true; }
        void reset() {;}
        bool divisionOld( SnapshotReport* ) { return true; }
        bool divisionNew( SnapshotReport* ) { return true; }
        bool update( const EventType* ) { return true; }
		bool tick() { return true; }
      //special
        void updateExternal();
        inline std::string makeImgName() { return model->getPath() + "/" + model->getFileName() + std::to_string( nextImgIdx++ ) + model->getImgFormat(); }


    private:
    	size_t nextImgIdx; //idx of the next image to save
};


class CellsFileReport : public GroReport< CellsFile, OutputHandler >
{
    public:
        friend OutputHandler;

    //---ctor, dtor
        CellsFileReport( const CellsFile* model, OutputHandler* ownerHandler ) 
        : GroReport( model, ownerHandler ), individualFileOut( std::make_shared<std::ofstream>() ), popFileOut( std::make_shared<std::ofstream>() )  {;}

        virtual ~CellsFileReport() = default;

    //---API
      //common
        void link();
        bool init();
        bool secondInit() { return true; }
        void reset();
        bool divisionOld( CellsFileReport* ) { return true; }
        bool divisionNew( CellsFileReport* ) { return true; }
        bool update( const EventType* ) { return true; }
        bool tick() { return true; }
      //special
        void updateExternal(); //it is actually updated by this, called from GroThread


    private:
        std::vector<PopulationStatReport*> popStatReports; //link to the reports of the fields
        std::string individualHeader; //header of the individual files
        SP<std::ofstream> individualFileOut; //std stream to write the individual files
        std::string popHeader; //header of the population file
        SP<std::ofstream> popFileOut; //std stream to write the population file


        void makeHeaders(); 
        inline void printPopHeader() { (*popFileOut) << popHeader; }
        inline void printIndividualHeader() { (*individualFileOut) << individualHeader; }
        void printIndividualFile();
        void printPopulationFile();
};


class CellsPlotReport : public GroReport< CellsPlot, OutputHandler >
{
    public:
        friend OutputHandler;
        using HistoricUnitType = QVector<double>;
        using HistoricDSType = std::vector< SP< HistoricUnitType > >;


    //---ctor, dtor
        CellsPlotReport( const CellsPlot* model, OutputHandler* ownerHandler ) 
        : GroReport( model, ownerHandler ), mutex(), bSaveToFile( false ) {;}

        CellsPlotReport( const CellsPlotReport& original ) 
        : GroReport( original.model, original.ownerHandler )
        , mutex(), popStatReports( original.popStatReports ), timeHistoric( original.timeHistoric ), historic( original.historic ), bSaveToFile( original.bSaveToFile ) {;}

        virtual ~CellsPlotReport() = default;

    //---get
        const std::vector<PopulationStatReport*>& getPopStatReports() const { return popStatReports; }
        //avoid different sizes when plotting
        inline const HistoricUnitType& getTimeHistoric() const { QMutexLocker ml( &mutex ); return timeHistoric; }
        //inline const HistoricDSType& getHistoric( ) const { QMutexLocker ml( &mutex ); return historic; }
        inline const HistoricUnitType& getHistoric( size_t idx ) const { QMutexLocker ml( &mutex ); return *historic[ idx ]; } 

        inline size_t getHistoricNum() { return historic.size(); }

        inline bool getBSaveToFile() const { return bSaveToFile; }
        inline bool checkSaveToFile() { bool val = bSaveToFile; bSaveToFile = false; return val; }
        inline const std::vector<std::string>& getLegend() const { return legend; }

    //---API
      //common
        void link();
        bool init();
        bool secondInit() { return true; }
        void reset();
        bool divisionOld( CellsPlotReport* ) { return true; }
        bool divisionNew( CellsPlotReport* ) { return true; }
        bool update( const EventType* ) { return true; }
        bool tick() { return true; }
      //special
        void updateExternal(); //it is actually updated by this, called from GroThread

        inline void clearHistoric() { QMutexLocker ml( &mutex ); timeHistoric.clear(); for( auto& historicUnit : historic ) historicUnit->clear(); } 
        inline HistoricUnitType accessTimeHistoric() { QMutexLocker ml( &mutex ); HistoricUnitType copy = timeHistoric; timeHistoric.clear(); return copy; }
		inline HistoricUnitType accessHistoric( size_t idx ) { QMutexLocker ml( &mutex ); HistoricUnitType copy = *historic[ idx ]; historic[ idx ]->clear(); return copy; } 


    private:
      //resources
        mutable QMutex mutex; //to prevent the times and values vector from having different sizes when plotted
        std::vector<PopulationStatReport*> popStatReports; //link to the reports of the stats
        std::vector<std::string> legend; //to print in the graph
        QVector<double> timeHistoric; //stored times for the plot, a single vector
        HistoricDSType historic; //stored values for the plot (y), a vector for each field
      //state
        bool bSaveToFile; //whether it is time to save a picture of the plot

        void makeLegend();
        void updateHistoric(); //store times and values in the historic vectors for plotting
};



//////////////////////////////////////////////////////////////////// HANDLER ///////////////////////////////////////////////////////////////////////

class OutputHandler : public GroHandler< ut::ReportsDS<CellsPlotReport>, ut::ReportsDS<CellsFileReport>, ut::ReportsDS<SnapshotReport> >
{
	REGISTER_CLASS( "OutputHandler", "outH", GroHandler<>::HandlerTypeIdx::H_OUTPUT )
	
	public:
    //---ctor, dtor
    	OutputHandler( PetriDish* owner, const GroCollection* collection ) : GroHandler( owner, collection ) {;}
    	//OutputHandler( const OutputHandler& rhs, PetriDish* owner ) : GroHandler( rhs, owner ) {;}

    //---get
        inline const std::vector<SnapshotReport>& getSnapshotReports() const { return std::get< OutputHReportIdx::SNAPSHOT >( this->reports ); }
        inline const std::vector<CellsFileReport>& getCellsFileReports() const { return std::get< OutputHReportIdx::CELLS_FILE >( this->reports ); }
        inline const std::vector<CellsPlotReport>& getCellsPlotReports() const { return std::get< OutputHReportIdx::CELLS_PLOT >( this->reports ); }

        inline std::vector<SnapshotReport>& getSnapshotReportsEdit() { return std::get< OutputHReportIdx::SNAPSHOT >( this->reports ); }
        inline std::vector<CellsFileReport>& getCellsFileReportsEdit() { return std::get< OutputHReportIdx::CELLS_FILE >( this->reports ); }
        inline std::vector<CellsPlotReport>& getCellsPlotReportsEdit() { return std::get< OutputHReportIdx::CELLS_PLOT >( this->reports ); }
    	virtual ~OutputHandler() = default;

    //---API
      //special
        bool init() override { HandlerBaseType::init(); return update( nullptr, nullptr, nullptr ); }
        bool update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* ) override;
        void updateExternal();
};

#endif // OUTPUTHANDLER_H
