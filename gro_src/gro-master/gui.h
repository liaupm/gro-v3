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

#ifndef GUI_H
#define GUI_H

#include "Defines.h"
#include "GroWidget.h" //growidget

/*PRECOMPILED
#include <bitset>
#include <memory> //ui, plotsGui 

#include <QObject>
#include <QDir> //directoryIn.  directoryOut
#include <QSplitter> //splitter
#include <QTextEdit> //console 
#include <QMessageBox> //checkpoints
#include <QWheelEvent> //zoom with mouse wheel
#include <QKeyEvent> //move camera with keyboard 
#include <Qt>
#include <QMainWindow> //parent class */


namespace Ui 
{ 
    class Gui; //Qt auto required for connecting to the gui.ui file
}

class GuiPlots;
class Gui : public QMainWindow
{
    Q_OBJECT //Qt auto use of signals and slots
    
public:
    enum ToolbarIdx : size_t
    {
        IDX_OPEN, IDX_RELOAD, IDX_STARTSTOP, IDX_STEP, IDX_ZOOM, IDX_SNAP, IDX_COUNT
    };

    enum class StartStopState : bool
    {
        START, STOP
    };

  //toolbar
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_ALL_DISABLED = std::bitset<ToolbarIdx::IDX_COUNT>( "000000" );
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_ALL_ENABLED = std::bitset<ToolbarIdx::IDX_COUNT>( "111111" );
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_NO_PROGRAM = std::bitset<ToolbarIdx::IDX_COUNT>( "000001" );
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_BAD_PROGRAM = std::bitset<ToolbarIdx::IDX_COUNT>( "000011" );
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_READY = TOOLBAR_ALL_ENABLED;
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_RUNNING = std::bitset<ToolbarIdx::IDX_COUNT>( "010100" );
    inline static const std::bitset<ToolbarIdx::IDX_COUNT> TOOLBAR_STEPPING = TOOLBAR_ALL_DISABLED;

    static constexpr const char* START_ICON = rsc::RESOURCE_START_ICON;
    static constexpr const char* STOP_ICON = rsc::RESOURCE_STOP_ICON;
  
  //visuals
    static constexpr int SPLITTER_SIZE_X = 775;
    static constexpr int SPLITTER_SIZE_Y = 180;
    static constexpr int PLOTS_WINDOW_X = 1200;
    static constexpr int PLOTS_WINDOW_Y = 500;

  //paths and messages
    inline static char workingDir[ FILENAME_MAX ];
    static constexpr const char* DF_WORKINGDIR = rsc::IO_DF_WORKINGDIR;
    inline static const QString DF_SNAPFILE_NAME = "untitled_gro_snapshot.bmp";
    inline static const char* SNAPFILE_EXTENSIONS = "Image (*.bmp);; Image (*.png);; Image (*.jpg);; Image (*.tiff)";
    inline static const QString DF_DUMPFILE_NAME = "untitled_gro_dump.csv";
    inline static const char* DUMPFILE_EXTENSIONS = "Comma Separated Value files (*.csv)";
    inline static const QString ABOUT_TEXT = QString( rsc::IO_ABOUT_TITLE ) + rsc::IO_ABOUT_AUTHOR + rsc::IO_ABOUT_ATTRIBUTIONS + rsc::IO_ABOUT_LICENSE + rsc::IO_ABOUT_LINK;
    
  //misc
    static constexpr double MAX_TIME_LEAP = 1000.0;
    static constexpr uint AUTOZOOM_TIMER_PERIOD = 10;


//---ctor, dtor
    Gui( int ac, char** av, QWidget* parent = nullptr ); //QMainWindow params
    Gui( const Gui& rhs ) = delete;
    virtual ~Gui();
    
//---get
    inline GroWidget& getGroWidget() { return growidget; }

//---API
    void batchProgram( const std::string& specFile ); //run in batch i.e. load the .gro file automatically
    void applyToolbarState( const std::bitset<ToolbarIdx::IDX_COUNT>& newState ); //modify the state of the toolbar (enable and disable buttons)
    void applyPlotsVisibility(); //show/hide the plots secondary GUI


private:
  //resources
    UP<Ui::Gui> ui; //Qt auto, used as connector to the gui.ui file. To access the widgets created in the .ui file
    QSplitter splitter; //user-adjustable split between graphic part and messages part
    GroWidget growidget; //main parent widget = graphics part
    QTextEdit console; //messages part
    UP<GuiPlots> guiPlots; //second window with plots
  //paths
    QDir directoryIn; //dir with .gro specs
    QDir directoryOut; //dir for dump
    QString fileName; //.gro file

  //state
    bool bShowPlots;

  //events passed to growidget
    inline void wheelEvent( QWheelEvent* event) { growidget.mouseWheel( event ); } //zoom
    inline void keyPressEvent( QKeyEvent* event) { growidget.keyPressed( event ); } //camera movement
    inline void keyReleaseEvent( QKeyEvent* event) { growidget.keyReleased( event ); } //camera movement
  //autozoom
    void loadAutoZoom(); //load initial state of the autozoom option
    inline void setAutoZoomTimer() { if( growidget.getBAutoZoom() ) QTimer::singleShot( AUTOZOOM_TIMER_PERIOD, this, SLOT( updateAutoZoom() ) ); } //rules the autozoom update frequency
  //misc
    void loadPlotsVisibility(); //load the showPlots option
    void toggleStartStopIcon( StartStopState newState ); //change the icon to play/pause depending on the current state


public slots: //connected to toolbar in the GUI
  //mainTooolBar
    void open(); //parse and load a .gro
    inline void reload() { if( ! fileName.isNull() ) { growidget.open( fileName, true ); updateActionStates(); loadAutoZoom(); loadPlotsVisibility(); } } //parse the current .gro again with the given seed
    inline void reloadSeed() { if( ! fileName.isNull() ) { growidget.open( fileName, false ); updateActionStates(); loadAutoZoom(); loadPlotsVisibility(); } } //parse the current .gro with a different seed
    
    inline void startStop() { growidget.startStop(); updateActionStates(); } 
    void step();
    void stepToTime();

    inline void zoomIn() { growidget.zoomIn(); }
    inline void zoomOut() { growidget.zoomOut(); }
    inline void centerCamera() { growidget.setCameraPosToCenter(); }
    inline void toggleAutoZoom( bool enabled ) { growidget.setAutoZoom( enabled ); updateAutoZoom(); updateActionStates(); }
    inline void toggleShowPlots( bool enabled ) { bShowPlots = enabled; applyPlotsVisibility(); }
    
  //menu-file
    void snapshot();
  //menu-view
    inline void resetZoom() { growidget.resetCamera(); }
  //menu-help
    void help() { system( rsc::IO_OPEN_DOC_LINK ); }
    void about() { displayMessage( ABOUT_TEXT, true ); displayMessage( QString( "Working directory: " ) + workingDir ); }

  //response to signals from GroWidget
    void displayMessage( QString msg, bool clear = false );
    void displayPopup( QString title, QString msg, bool bBeep );
    void updateActionStates();
    void quitAndExit() { QApplication::quit(); } //quits the app, deleting all the windows

  //plots
    void connectGroToPlots();
    void updatePlots();


private slots:
    void updateAutoZoom() { growidget.updateAutoZoom(); setAutoZoomTimer(); } //update zoom and reset timer
};

#endif // GUI_H
