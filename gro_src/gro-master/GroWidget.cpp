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


#include "GroWidget.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////// COMPONENTS //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//=========================================================================================== NAVIGATION
bool GroWidget::Navigation::update()
{
    if( checkIsMoving() )
    {
        x += speedX;
        y += speedY;
        owner->getGroThread().setCameraPos( x, y );
        return true;
    }
    return false;
}

//-------------------------------------------------------events
bool GroWidget::Navigation::keyPressed( QKeyEvent* ev )
{
    if( ! checkAvailableForKeys() )
        return false;
    lockByKeys();
    int speed = owner->getThreadState() == GroThread::RUNNING ? SPEED_RUNNING : SPEED_PAUSED;

    switch( ev->key() )
    {
        case Qt::Key_Up : 
            speedY = speed;
            break;
        case Qt::Key_Down:
             speedY = -speed;
            break;
        case Qt::Key_Right:
            speedX = -speed;
            break;
        case Qt::Key_Left: 
            speedX = speed;
            break;
        default: return false;
    }
    return true;
}

bool GroWidget::Navigation::keyReleased( QKeyEvent* ev )
{
    switch( ev->key() )
    {
        case Qt::Key_Up : [[fallthrough]];
        case Qt::Key_Down:
            speedY = 0.0f;
            break;
        case Qt::Key_Right: [[fallthrough]];
        case Qt::Key_Left: 
            speedX = 0.0f;
            break;
        default : return false;
    }
    unlock();
    return true;
}

bool GroWidget::Navigation::mousePressed( QMouseEvent* ev )
{
    if( ! checkAvailableForMouse() )
        return false;
    if( ev->button() == Qt::MiddleButton )
    { 
        lockByMouse();
        setPosition( x - ( ev->pos().x() - owner->width() / 2 ) / owner->getZoom(), y - ( ev->pos().y() - owner->height() / 2 ) / owner->getZoom() );
        unlock();
        return true;
    }
    if( mouseTracker.mousePressed( ev ) )
    {
        lockByMouse();
        return true;
    }
    return false;
}

bool GroWidget::Navigation::mouseReleased( QMouseEvent* ev )
{
    if( mouseTracker.mouseReleased( ev ) )
    {
        resetSpeed();
        unlock();
        return true;
    }
    return false;
}

bool GroWidget::Navigation::mouseMoved( QMouseEvent* ev )
{
    if( mouseTracker.mouseMoved( ev ) )
    {
        float mouseSpeed =  owner->getThreadState() == GroThread::RUNNING ? MOUSE_SPEED_RUNNING : MOUSE_SPEED_PAUSED;

        if( ev->pos().y() < BORDER ) 
            speedY = ( BORDER - ev->pos().y() ) * mouseSpeed;  
        
        else if( ev->pos().y() > owner->height() - BORDER )
            speedY = - ( BORDER - owner->height() + ev->pos().y() ) * mouseSpeed;  
        else 
            speedY = 0.0f; 

        if( ev->pos().x() < BORDER ) 
            speedX = ( BORDER - ev->pos().x() ) * mouseSpeed;  
    
        else if( ev->pos().x() > owner->width() - BORDER )
            speedX = - ( BORDER - owner->width() + ev->pos().x() ) * mouseSpeed;  
        else 
            speedX = 0.0f; 

        return true;
    }
    resetSpeed();
    return false;
}


//=========================================================================================== ZOOM
bool GroWidget::Zoom::update()
{
    if( checkMovement() )
    {
        if( direction == Direction::BIGGER && ! checkMaxReached() )
        {
            scale( bFromButton ? BUTTON_SPEED : SPEED );
            owner->getGroThread().scaleZoom( bFromButton ? BUTTON_SPEED : SPEED );
        }
        else if( direction == Direction::SMALLER && ! checkMinReached() )
        {
            scale( bFromButton ? BUTTON_INVERSE_SPEED : INVERSE_SPEED );
            owner->getGroThread().scaleZoom( bFromButton ? BUTTON_INVERSE_SPEED : INVERSE_SPEED );
        }
        decreaseDelta();
        bFromButton = false;
        return true;
    }
    return false;
}

//-------------------------------------------------------events
bool GroWidget::Zoom::mouseWheel( QWheelEvent* ev )
{
    if( ev->angleDelta().y() < 0.0 ) //zoom out
    {
        delta -= ev->angleDelta().y();
        direction = Direction::SMALLER;
        return true;
    }
    if( ev->angleDelta().y() > 0.0 ) //zoom in
    {
        delta += ev->angleDelta().y();
        direction = Direction::BIGGER;
        return true;
    }
    return false;
}


//=========================================================================================== CELLS SELECTION
bool GroWidget::CellsSelection::draw( QPainter& painter )
{
    if ( checkIsSelecting() ) //if mouse pressed and .gro file ok loaded, graw cell-selection box
    {
        painter.setPen( owner->getGroThread().getGraphicsHandler().getTheme().mousePen );
        painter.setBrush( QBrush() );
        painter.drawRect( QRect( mouseTracker.pressedPoint, mouseTracker.currentPoint ) );
        return true;
    }
    return false;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////  GRO WIDGET  /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GroWidget::GroWidget( int, char**, QWidget* parent ) 
: QWidget( parent )
, grothread( this ), navigation( this ), zoom( this ), cellsSelection( this )
, error_flag( false ), bRepaintRequired( false ), bAutoZoom( AUTOZOOM_INI_STATE )
{
    GroThread::currentThread = &grothread; 
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

//connect signals and slots
//register datatypes used in connect
    qRegisterMetaType<QImage>( "QImage" );
    qRegisterMetaType<std::string>( "std::string" );

    connect( &grothread, SIGNAL( renderedImage( QImage ) ), this, SLOT( updatePixmap( QImage ) ) ); //if rendered image changes, update the pixmap
    connect( &grothread, SIGNAL( stateChange() ), this, SLOT( handleStateChange() ) ); //if thread state changes, handle
    connect( &grothread, SIGNAL( messageFromGroThread( std::string, bool ) ), this, SLOT( passMessage( std::string, bool ) ) ); //if message from gro emitted, pass it to the gui for showing it in the console
    connect( &grothread, SIGNAL( popupFromGroThread( const std::string&, const std::string&, bool ) ), this, SLOT( passPopupToGui( const std::string&, const std::string&, bool ) ) );
    connect( &grothread, SIGNAL( updateToolbarFromGroThread() ), this, SLOT( passToolbarUpdateToGui() ) );
    connect( &grothread, SIGNAL( quitFromGroThread() ), this, SLOT( passQuitToGui() ) );
    connect( &grothread, SIGNAL( stopFromGroThread() ), this, SLOT( onlyStop() ), Qt::BlockingQueuedConnection );

    repaintTimer = std::make_shared<QTimer>( this );
    connect( repaintTimer.get(), SIGNAL( timeout() ), this, SLOT( repaintIfRequired() ) );
    repaintTimer->start( REFRESH_PERIOD );
}




//==============================================* GUI ACTIONS *========================================================================
void GroWidget::open( QString path, bool bSameSeed ) 
{
    emit messageFromGroWidget ( "", true ); //just for cleaning the console
    
    if ( !grothread.parse( path.toLatin1(), bSameSeed ) )
    {
        emit messageFromGroWidget( tr("") + grothread.getError().c_str() + "<br>" + rsc::IO_SPECERROR_MSG, true ); //show ccl errors in console
        error_flag = true;
        pixmap = QPixmap("");
        setToRest();
        repaint();
    }
    else
    {
        error_flag = false;
        bAutoZoom = grothread.getBAutoZoom();
        resetCamera();
        grothread.init();
    }
}

void GroWidget::startStop() 
{
    setToRest();
    if( grothread.getState() == GroThread::State::READY ) 
        grothread.startRunning();
    else if( grothread.getState() == GroThread::State::RUNNING ) 
        grothread.stopRunning();
}

void GroWidget::onlyStop() 
{
    setToRest();
    grothread.stopRunning();
    emit updateToolbarFromGroWidget();
}


//========================================* SIGNALS AND SLOTS *============================================================
void GroWidget::handleStateChange() 
///reports thread state in the console
{
    switch( grothread.getState() ) 
    {
        case GroThread::State::NO_PROGRAM:
            emit messageFromGroWidget( QString( "No gro program" ) );
            break;

        case GroThread::State::DEAD:
            emit messageFromGroWidget( QString( "The thread is dead" ) );
            break;
        default:
            break;
    }
    emit updateToolbarFromGroWidget();
}


//=================================* Qt PRIVATE AUTO EVENTS *=======================================
void GroWidget::paintEvent( QPaintEvent* ) 
{
    QPainter painter(this); //create painter (temp object)
    navigation.update(); 
    zoom.update();

    if( !painter.isActive() ) 
        return;

    if( pixmap.isNull() ) //if just started, print starting message
    {
        if( error_flag ) //if error loading .gro file, paint errorimage
            painter.drawImage( ( width() - ERROR_IMAGE.width() ) / 2, ( height() - ERROR_IMAGE.height() ) / 2, ERROR_IMAGE );
        else
        {
            painter.fillRect( rect(), Qt::white );
            painter.setPen( Qt::black );
            painter.drawText( rect(), Qt::AlignCenter, tr( STARTING_MSG ) ); 
        }
    } 
    else //if regular case, draw the pixmap 
    {
        painter.drawPixmap( 0,0, pixmap );
        cellsSelection.draw( painter );
    }
}


//=================================* USER INTERACTION EVENTS *=======================================
void GroWidget::keyPressed( QKeyEvent* ev )
{
    if( ev->isAutoRepeat() || bAutoZoom )
        return;
    bRepaintRequired = true;
    navigation.keyPressed( ev );
}

void GroWidget::keyReleased( QKeyEvent* ev )
{
    if( ev->isAutoRepeat() )
        return;
    navigation.keyReleased( ev );
    if( ! navigation.checkIsMoving() )
        bRepaintRequired = false;
}

void GroWidget::mouseMoveEvent( QMouseEvent* ev  )
{ 
    if( ! this->rect().contains( ev->pos() ) ) 
        return;
    navigation.mouseMoved( ev );
    cellsSelection.mouseMoved( ev ); 
}


void GroWidget::updateAutoZoom()
{
    if( bAutoZoom )
    {
        ut::Rectangle colonyBorders = getColonyBorder();
        while( true )
        {
            int colonyBorderX1 = ( colonyBorders.x1 - navigation.x ) * getZoom() + width() / 2; 
            int colonyBorderX2 = ( colonyBorders.x2 - navigation.x ) * getZoom() + width() / 2; 
            int colonyBorderY1 = ( colonyBorders.y1 - navigation.y ) * getZoom() + height()/ 2; 
            int colonyBorderY2 = ( colonyBorders.y2 - navigation.y ) * getZoom() + height()/ 2; 

            if( colonyBorderX1 < AUTOZOOM_PAD || colonyBorderX2 > width() - AUTOZOOM_PAD || colonyBorderY1 < AUTOZOOM_PAD || colonyBorderY2 > height() - AUTOZOOM_PAD )
            {
                zoom.scale( AUTOZOOM_FACTOR );
                getGroThread().scaleZoom( AUTOZOOM_FACTOR );
                repaint();
            }
            else
                break;
        }
    }
}
