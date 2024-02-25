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

#ifndef GROTHREAD_H
#define GROTHREAD_H

#include "Defines.h"
#include "Checkpoint.h"
#include "Snapshot.h"
#include "World.h"
#include "Parser.h"

#include "Theme.h"
#include "MessageHandler.h"
#include "GraphicsHandler.h"

/*PRECOMPILED
#include <memory>

#include <QObject>
#include <QString>
#include <QSize>
#include <QPoint>
#include <QImage> 
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QElapsedTimer>
#include <QWaitCondition>
#include <QThread>
#include <Qt>
#include <QtCore> 
#include <QtGui> */


namespace ut { class Rectangle; } //passing colonyBorder

class GroThread : public QThread
{
    Q_OBJECT //Qt auto use of signals and slots

    public:
        static constexpr uint RUN_REFRESH_PERIOD = 33;
        static constexpr uint PLOTS_REFRESH_PERIOD = 50;
        static constexpr int CELL_SELECTION_PAD = 5;

        enum State
        {
            NO_PROGRAM, READY, RUNNING, STEPPING, DEAD
        };

    //static
        static inline GroThread* currentThread = nullptr;

    //ctor, dtor
        GroThread( QObject* parent )
        : QThread( parent ), world( nullptr ), parser( nullptr )
        , state( State::NO_PROGRAM ), bAbort( false )
        , bBatchMode( false ), bAutoZoom( false ), bUpdatePlots( false ), bUpdateGraphics( false ), bShowPlotsOnStartup( false ), bShowPerformance( false ) {;}

        virtual ~GroThread();

    ///---get 
      //resources
        inline World* getWorld() { return world.get(); }
        inline bool hasValidWorld() const { return world != nullptr; }
        inline Parser* getParser() { return parser.get(); }
        inline bool hasValidParser() const { return parser != nullptr; }
        inline GraphicsHandler& getGraphicsHandler() { return graphicsHandler; }
        inline MessageHandler& getMessageHandler() { return messageHandler; }
      //state
        inline State getState() const { return state; }
        inline std::string getError() { return errorStr; }
      //flags
        inline bool getBAutoZoom() const { return bAutoZoom; }
        inline bool getBBatchMode() const { return bBatchMode; }
        inline bool getBShowPlotsOnStartup() const { return bShowPlotsOnStartup; }
        inline bool getBShowPerformance() const { return bShowPerformance; }
      //wrapper
        inline const ut::Rectangle& getColonyBorder() const { return world->getColonyBorder(); }

    //---set
      //state
        inline void setState( State xState ) { state = xState; } //just set, to update the toolbar too, use changeState() instead
        inline void changeState( State xState ) { state = xState; emit stateChange(); } 
      //flags
        inline void setBBatchMode( bool val ) { bBatchMode = val; }
        inline void setBUpdatePlots( bool val ) { bUpdatePlots = val; }
        inline void setBUpdateGraphics( bool val ) { bUpdateGraphics = val; }
        inline void setBShowPlotsOnStartup( bool val ) { bShowPlotsOnStartup = val; }
        inline void setBShowPerformance( bool val ) { bShowPerformance = val; }
      //gui zoom and navigation
        void setZoom( float z );
        inline void scaleZoom( float zoomIncrease ) { setZoom( graphicsHandler.getZoom() * zoomIncrease ); }
        void setCameraPos( float x, float y );
        void setBAutoZoom( bool xBAutoZoom ) { bAutoZoom = xBAutoZoom; }
      //pass to GroWidget
        inline void addCheckpoint( const Checkpoint* checkpoint ) { currentCheckpoints.push_back( checkpoint ); }
        inline void addSnapshot( const Snapshot* snapshot ) { currentSnapshots.push_back( snapshot ); }

    //----API
      //toolbar and menus
        bool parse( const char* path, bool bSameSeed = true ); //parse .gro file
        void startRunning(); 
        inline void stopRunning() { QMutexLocker locker( &mutex ); bAbort = true; }
        void init(); //first update cycle at time 0 
        void run();
        void step();
        void stepToTime( TReal targetTime ) { startPerformance(); while( world->getTime().time < targetTime ) step(); showPerformance(); }
        void snapshot( QString filename );
      //graphics
        void resize( QSize s );
        void render();
      //pass to GroWidget
        void applyCheckpoints();
        void applySnapshots();
	  //misc
        void startPerformance();
        void showPerformance();
       	inline void lockMutex() { mutex.lock(); }
        inline void unlockMutex() { mutex.unlock(); }

     //emit signals (passed to GroWidget and then to GroGui)
        void emitStop() { emit stopFromGroThread(); }
        void emitToolbarUpdate() { emit updateToolbarFromGroThread(); }
        void emitMessage( std::string msg, bool clear = false ) { emit messageFromGroThread( msg, clear ); }
        void emitPopup( const std::string& title, const std::string& message, bool bBeep ) { emit popupFromGroThread( title, message, bBeep ); }
        void emitQuit() { emit quitFromGroThread(); }
        void emitConnectGroToPlots() { emit connectGroToPlotsFromGroThread(); }
        void emitUpdatePlots() { emit updatePlotsFromGroThread(); }
        void emitClearPlots() { emit clearPlotsFromGroThread(); }
     

    private:
      //resources
        SP<World> world;
        SP<Parser> parser;
        MessageHandler messageHandler;
        GraphicsHandler graphicsHandler;

        QElapsedTimer renderTimer;
        QElapsedTimer plotsTimer;
        QElapsedTimer performanceTimer;
        TReal firstTime, lastTime;

      //state
        State state;
        uint currentSeed;
        std::string errorStr;
        std::vector<const Checkpoint*> currentCheckpoints;
        std::vector<const Snapshot*> currentSnapshots;

      //flags
        bool bAbort;
        bool bBatchMode;
        bool bAutoZoom;
        bool bUpdatePlots;
        bool bUpdateGraphics;
        bool bShowPlotsOnStartup;
        bool bShowPerformance;

      //QThread stuff
        QMutex mutex;
        QWaitCondition condition;

      //misc
        bool tryUpdateWorld(); //update gro and catch errors
        bool abort(); //stop running
        void checkAndUpdateGraphics(); //update graphics if needed (change in camera position, zoom or snapshot)


    signals:
        void stateChange();
        void renderedImage( const QImage &image );
        void stopFromGroThread();
        void updateToolbarFromGroThread();
        void messageFromGroThread( std::string, bool );
        void popupFromGroThread( const std::string&, const std::string&, bool bBeep );
        void quitFromGroThread();
        //void connectGroToPlotsFromGroThread( const std::vector<const CellsPlot*>& cellsPlots );
        void connectGroToPlotsFromGroThread();
        void updatePlotsFromGroThread();
        void clearPlotsFromGroThread();
};

#endif // GROTHREAD_H
