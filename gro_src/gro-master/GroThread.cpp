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



#include "GroThread.h"
#include "Gro.h"
#include "World.h"
#include "PetriDish.h"
#include "OutputHandler.h"
#include "GroPainter.h"

/*PRECOMPILED
#include <QApplication> //quit
#include <QCoreApplication> 
#include <QApplication>
#include <QtGui> */


GroThread::~GroThread()
{
    mutex.lock();
    bAbort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void GroThread::setZoom( float z ) 
{
    QMutexLocker locker(&mutex);
    graphicsHandler.setBResized( true );
    graphicsHandler.setZoom( z );

    if ( !isRunning() ) 
    {
        graphicsHandler.resetImage();
        render();
    }
}

void GroThread::setCameraPos( float x, float y )
{
    QMutexLocker locker(&mutex);
    graphicsHandler.setCameraPos( x, y );

    if( !isRunning() ) 
    {
        graphicsHandler.resetImage();
        render();
    }
}




//================================ API (NOT SIGNALS )

//--------------------------toolbar and menus

bool GroThread::parse( const char* path, bool bSameSeed ) 
{
    QMutexLocker locker(&mutex);

    registerGroFunctions();
    emitClearPlots();
    parser = std::make_shared<Parser>( path );
    world = std::make_shared<World>( this );
    
    try
    {
        parser->parseFile();
        if( bSameSeed )
            parser->loadAll( world.get() );
        else
            parser->loadAll( world.get(), currentSeed );
        currentSeed = world->getSeed();
    }
    catch( std::string err ) // checks for parse errors
    {  
        errorStr = err;
        world = nullptr;
        changeState( State::NO_PROGRAM );
        return false;
    }

   //parsing successful!
    emitConnectGroToPlots();
    changeState( State::READY );
     
    graphicsHandler.resetImage();
    render();
    return true;
}

void GroThread::startRunning()
{
    QMutexLocker locker(&mutex);

    if( state == State::READY ) 
    {
        changeState( State::RUNNING );
        
        if( !isRunning() ) 
        {
            start();
            bAbort = false;
        } 
        else
            condition.wakeOne();
    }
}

//---
void GroThread::init()
{
    world->init();
    render();
    
    checkAndUpdateGraphics();
    world->updateOutputExternal();
    applySnapshots();
    applyCheckpoints();
}

void GroThread::run()
{
    renderTimer.start();
    plotsTimer.start();
	startPerformance();

    forever 
    {
        if( abort() )
        {
        	showPerformance();
            return;
        }
        if( ! tryUpdateWorld() )
            return;
        
        checkAndUpdateGraphics();
        world->updateOutputExternal();
        if ( renderTimer.elapsed() >= RUN_REFRESH_PERIOD || bUpdateGraphics ) 
        {
            render();
            renderTimer.start();
            bUpdateGraphics = false;
        }
        
        applySnapshots();
        if( plotsTimer.elapsed() >= PLOTS_REFRESH_PERIOD || bUpdatePlots ) 
        {
            emitUpdatePlots();
            plotsTimer.start();
            bUpdatePlots = false;
        }
        
        fflush( stdout );
        QCoreApplication::processEvents();
        applyCheckpoints();
    }
}

void GroThread::step() 
{
    render();
    if( ! tryUpdateWorld() )
        return;
    checkAndUpdateGraphics();
    render();
    world->updateOutputExternal();
    applySnapshots();
    QCoreApplication::processEvents();
    emitUpdatePlots();
    fflush(stdout);
    QCoreApplication::processEvents();
    applyCheckpoints();
}

void GroThread::snapshot( QString filename ) 
{
    QMutexLocker locker(&mutex);
    if( hasValidWorld() ) 
        graphicsHandler.snapshot( filename );
}



//-------------------------graphics

void GroThread::resize( QSize s ) 
{
    QMutexLocker locker(&mutex);
    graphicsHandler.setBResized( true );
    graphicsHandler.getSizeEdit() = s;

    if ( !isRunning() ) 
    {
        graphicsHandler.resetImage();
        render();
    }
}

void GroThread::render()
{
    if( ! hasValidWorld() )
        return;
    GroPainter painter( graphicsHandler.getSize(), &graphicsHandler.getImageEdit() );
    graphicsHandler.renderWorld( *world, painter );
    messageHandler.render( &painter );

    emit renderedImage( graphicsHandler.getImage() );
}


//-------------------pass to GroWidget

void GroThread::applyCheckpoints()
{
    if( currentCheckpoints.size() > 0 )
    {
        if( bBatchMode )
        	emitQuit();
        if( getState() == State::RUNNING )
            emitStop();
        for( const Checkpoint* chp : currentCheckpoints )
            emitPopup( chp->getName(), chp->getMessage(), chp->getBSound() );
        currentCheckpoints.clear();
    }
}

void GroThread::applySnapshots()
{
    if( currentSnapshots.size() > 0 )
    {
        for( const Snapshot* snpt : currentSnapshots )
            graphicsHandler.snapshot( world->getPetriDishEdit()->getHandlerEdit<GroHandlerIdx::H_OUTPUT>().getSnapshotReportsEdit()[ snpt->getRelativeId() ].makeImgName().c_str() );
        currentSnapshots.clear();
    }
}


//-------------------misc

void GroThread::startPerformance()
{
    if( bShowPerformance )
    {
    	performanceTimer.start();
    	firstTime = world->getTime().time;
    }
}

void GroThread::showPerformance()
{
    if( bShowPerformance )
    {
    	TReal realElapsedMin = performanceTimer.elapsed() * 10E-4 / 60.0;
    	TReal groElapsedTime = world->getTime().time - firstTime;
    	TReal elapsedRatio = groElapsedTime / realElapsedMin;
        emitMessage( "*PERFORMANCE* real time: " + std::to_string( realElapsedMin ) + " min, gro time: " + std::to_string( groElapsedTime ) + " min, ratio: " + std::to_string( elapsedRatio ) );
    }
}



//================================ PRIVATE

bool GroThread::tryUpdateWorld()
{
    try 
    {
        world->update();
    }
    catch( std::string err ) 
    {
        mutex.lock();
        errorStr = err;
        changeState( State::DEAD );
        mutex.unlock();
        return false;
    }
    return true;
}

bool GroThread::abort()
{
    if( bAbort ) 
    {
        mutex.lock();
        changeState( State::READY );
        mutex.unlock();
        emitUpdatePlots();
        render();
        return true;
    }  
    return false;
}

void GroThread::checkAndUpdateGraphics()
{
    if( graphicsHandler.getBResized() ) 
    {
        mutex.lock();
        graphicsHandler.setBResized( false );
        graphicsHandler.resetImage();
        mutex.unlock();
        render();
    } 
}