/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summaraized here.
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
#include "GuiPlots.h" //guiPlots = second window with plots
#include "ui_gui.h" //layout file
#include "gui.h" 

#include "ut/Time.hpp"
#include "World.h" 
#include "GroThread.h"

/*PRECOMPILED
#include <unistd.h> //chdir, getcwd

#include <QFileDialog> //open, dump, snapshot
#include <QInputDialog> //stepToTime */


Gui::Gui( int ac, char** av, QWidget* parent ) 
: QMainWindow( parent ) //Qt auto
, ui( std::make_unique<Ui::Gui>() ) //Qt auto create connector to the gui.ui file
, growidget( ac, av, this )
, guiPlots( std::make_unique<GuiPlots>( this ) )
, bShowPlots( false )
{
    ui->setupUi( this ); //Qt auto set this object as the main window for connecting to the gui.ui file
    setFocusPolicy( Qt::StrongFocus ); //required for keyPress and keyRelease events = focus by both tab and click

  //set working dir and get absolute path
    chdir( DF_WORKINGDIR ); //platform-agnostic way of changing current dir
    getcwd( workingDir, sizeof( workingDir ) ); //get absolute path for working dir

  //splitter
    splitter.setOrientation( Qt::Vertical );
    splitter.addWidget( &growidget );
    splitter.addWidget( &console );
    splitter.setSizes( QList<int>() << SPLITTER_SIZE_X << SPLITTER_SIZE_Y );
    ui->centralWidget->layout()->addWidget( &splitter );

  //console
    console.setReadOnly( true );
    about(); //starts the console showing about

  //second window with plots
    guiPlots->move( PLOTS_WINDOW_X, PLOTS_WINDOW_Y );

  //connect signals and slots 
    connect( &growidget, SIGNAL( messageFromGroWidget( QString, bool ) ), this, SLOT( displayMessage( QString, bool ) ) );
    connect( &growidget, SIGNAL( popupFromGroWidget( QString, QString, bool ) ), this, SLOT( displayPopup( QString, QString, bool ) ) );
    connect( &growidget, SIGNAL( updateToolbarFromGroWidget() ), this, SLOT( updateActionStates() ) );
    connect( &growidget, SIGNAL( quitFromGroWidget() ), this, SLOT( quitAndExit() ) );
    connect( &growidget.getGroThread(), SIGNAL( connectGroToPlotsFromGroThread() ), this, SLOT( connectGroToPlots() ) );
    connect( &growidget.getGroThread(), SIGNAL( updatePlotsFromGroThread() ), this, SLOT( updatePlots() ) );

  //batch program if spec file given as console arg
    if( ac > 1 )
        batchProgram( av[1] );

    setAttribute( Qt::WA_DeleteOnClose );
}

Gui::~Gui() {;}











//=========================================================================================* API (out of signals and slots) *===============================================================================================================

void Gui::batchProgram( const std::string& specFile )
{
    if( specFile != "" ) 
    {
        fileName = QString::fromStdString( specFile );
        directoryIn = QDir( fileName );
        setWindowTitle( QString( "gro: " ) + fileName );
        
        growidget.open( fileName );
        updateActionStates();
        loadAutoZoom();
        loadPlotsVisibility();
    } 
    else 
        console.insertHtml( QString( "Error opening ") + fileName + "<br />"  );

    if( growidget.getGroThread().getBBatchMode() )
        startStop();
}


void Gui::applyToolbarState( const std::bitset<ToolbarIdx::IDX_COUNT>& newState )
{
    ui->actionOpen->setEnabled( newState.test( ToolbarIdx::IDX_OPEN ) ); 
    ui->actionReload->setEnabled( newState.test( ToolbarIdx::IDX_RELOAD ) ); 
    ui->actionReloadSeed->setEnabled( newState.test( ToolbarIdx::IDX_RELOAD ) );

    ui->actionStartStop->setEnabled( newState.test( ToolbarIdx::IDX_STARTSTOP ) );
    ui->actionStep->setEnabled( newState.test( ToolbarIdx::IDX_STEP ) ); 
    ui->action1step->setEnabled( newState.test( ToolbarIdx::IDX_STEP ) ); 
    ui->action10steps->setEnabled( newState.test( ToolbarIdx::IDX_STEP ) ); 
    ui->action100steps->setEnabled( newState.test( ToolbarIdx::IDX_STEP ) );

    ui->actionZoomIn->setEnabled( ( newState.test( ToolbarIdx::IDX_STARTSTOP ) || newState.test( ToolbarIdx::IDX_STEP ) ) && ! growidget.getBAutoZoom() ); 
    ui->actionZoomOut->setEnabled( ( newState.test( ToolbarIdx::IDX_STARTSTOP ) || newState.test( ToolbarIdx::IDX_STEP ) ) && ! growidget.getBAutoZoom() ); 
    ui->actionCenter->setEnabled( ( newState.test( ToolbarIdx::IDX_STARTSTOP ) || newState.test( ToolbarIdx::IDX_STEP ) ) && ! growidget.getBAutoZoom() );  
    ui->actionAutoZoom->setEnabled( newState.test( ToolbarIdx::IDX_STARTSTOP ) || newState.test( ToolbarIdx::IDX_STEP ) );   

    ui->actionScreenshot->setEnabled( newState.test( ToolbarIdx::IDX_SNAP ) );     
}

void Gui::applyPlotsVisibility() 
{
    if( bShowPlots )
        guiPlots->show();
    else
        guiPlots->hide();
}


//-------------------private

void Gui::loadAutoZoom()
{ 
	ui->actionAutoZoom->setChecked( growidget.getBAutoZoom() ); 
	toggleAutoZoom( growidget.getBAutoZoom() ); 
}

void Gui::loadPlotsVisibility()
{ 
    ui->actionShowPlots->setChecked( growidget.getGroThread().getBShowPlotsOnStartup() ); 
    toggleShowPlots( growidget.getGroThread().getBShowPlotsOnStartup() ); 
}

void Gui::toggleStartStopIcon( StartStopState newState )
{
    if( newState == StartStopState::START )
    {
        ui->actionStartStop->setText( "Start" );
        ui->actionStartStop->setIcon( QIcon( START_ICON ) );
    }
    else
    {
        ui->actionStartStop->setText( "Stop" );
        ui->actionStartStop->setIcon( QIcon( STOP_ICON ) );
    }
}





//=========================================================================================* SLOTS *===============================================================================================================

//------------------- toolbar

void Gui::open() 
{
    QString tempFilename = QFileDialog::getOpenFileName( this, tr( "Open a .gro file" ), directoryIn.absolutePath(), tr( "Gro files (*.gro)" ) );

    if( !tempFilename.isNull() ) 
    {
        fileName = tempFilename;
        directoryIn = QDir( fileName );
        setWindowTitle( QString( "gro: " ) + fileName );
        
        growidget.open( fileName );
        updateActionStates();  
        loadAutoZoom();
        loadPlotsVisibility();
    } 
    else 
        console.insertHtml ( QString( "Error opening ") + fileName + "<br />"  );
}

void Gui::step() 
{ 
    growidget.getGroThread().startPerformance();

  //get number of steps from user
    QAction* action = static_cast<QAction*>( sender() );
    uint steps = 1;
    if( action == ui->action10steps )
        steps = 10;
    else if( action == ui->action100steps )
        steps = 100;

  //apply
    growidget.getGroThread().changeState( GroThread::State::STEPPING );
    //updateActionStates();
    for( size_t i = 0; i < steps; i++ )
        growidget.getGroThread().step(); 

    growidget.getGroThread().showPerformance();
  //back to stop state
    growidget.getGroThread().changeState( GroThread::State::READY );
}

void Gui::stepToTime()
{
  //get final time from user
    //to ensure a dot is used as decimal separator:
    QInputDialog dialog = QInputDialog();
    dialog.setInputMode( QInputDialog::DoubleInput );
    dialog.setLocale( QLocale( QLocale::English, QLocale::UnitedKingdom ) );
    dialog.setDoubleRange( 0.0, growidget.getGroThread().getWorld()->getTime().time + 1000.0 );
    dialog.setDoubleStep( growidget.getGroThread().getWorld()->getTime().stepSize );
    bool ok = dialog.exec();

  //apply
    if( ok )
    {
        growidget.getGroThread().changeState( GroThread::State::STEPPING );
        //updateActionStates();
        growidget.getGroThread().stepToTime( dialog.doubleValue() );
    }
  //back to stop state
    growidget.getGroThread().changeState( GroThread::State::READY );
}


//------------------- menu

void Gui::snapshot() 
{
    QString snapfile = QFileDialog::getSaveFileName( this, "Save snapshot as ...", directoryOut.absolutePath() + "/" + DF_SNAPFILE_NAME, tr( SNAPFILE_EXTENSIONS ) );

    if( !snapfile.isNull() )
    {
        directoryOut = QDir( snapfile );
        console.insertHtml( QString( "Saving snapshot to " ) + snapfile + "<br />"  );
        growidget.getGroThread().snapshot( snapfile );
    }
    else
        console.insertHtml( QString( "Error saving snapshot to " ) + snapfile + "<br />" );
}


//---------------- response to signals from GroWidget

void Gui::displayMessage( QString msg, bool clear ) 
{
    if( clear )
        console.clear();

    if( !msg.isEmpty() ) 
    {
        console.insertHtml( msg + "<br />" );
        console.moveCursor( QTextCursor::End );
    }
}

void Gui::displayPopup( QString title, QString msg, bool bBeep )
{ 
	QMessageBox::information( this, title, msg );
	if( bBeep )
		QApplication::beep();
}

void Gui::updateActionStates() 
{
    switch( growidget.getThreadState() ) 
    {
        case GroThread::NO_PROGRAM:
            if( fileName.isNull() )
                applyToolbarState( TOOLBAR_NO_PROGRAM );
            else
                applyToolbarState( TOOLBAR_BAD_PROGRAM );
            break;

        case GroThread::READY:
            applyToolbarState( TOOLBAR_READY );
            toggleStartStopIcon( StartStopState::START );
            break;

        case GroThread::DEAD:
            applyToolbarState( TOOLBAR_BAD_PROGRAM );
            toggleStartStopIcon( StartStopState::START );
            break;

        case GroThread::RUNNING:
            applyToolbarState( TOOLBAR_RUNNING );
            toggleStartStopIcon( StartStopState::STOP );
            break;

        case GroThread::STEPPING:
            applyToolbarState( TOOLBAR_STEPPING );
            toggleStartStopIcon( StartStopState::START );
            break;
    }
}


//---------plots
void Gui::connectGroToPlots() { guiPlots->connectToGro( growidget.getGroThread().getWorld() ); }
void Gui::updatePlots() { guiPlots->updateCellsPlots(); }
