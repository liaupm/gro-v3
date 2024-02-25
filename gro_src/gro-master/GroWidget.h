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

#ifndef GROWIDGET_H
#define GROWIDGET_H

#include "ut/Shape.hpp" //Rectangle colonyBorder
#include "Defines.h"
#include "GraphicsHandler.h" //IMAGE_SIZE consts 
#include "GroThread.h"

/*PRECOMPILED
#include <memory> //SP<QTimer>

#include <QObject>
#include <QPoint>
#include <QString>
#include <QImage>
#include <QSize> 
#include <QPixmap>
#include <QPainter> //paintEvent, CellsSelection::draw
#include <QWidget> //parent class
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer> 
#include <QtCore>
#include <Qt> */


class Gui;
class GroWidget : public QWidget
{
    Q_OBJECT //Qt macro that allows signals and slots

public:
    static constexpr float REFRESH_PERIOD = 3.0f; //every refreshPeriod ms, the widget is repainted

    template< Qt::MouseButton VButton >
    struct MouseTracker
    {
        inline static const Qt::MouseButton BUTTON = VButton;

        MouseTracker() : bMousePressed( false ) {;}
        ~MouseTracker() = default;

        bool mousePressed( QMouseEvent* ev );
        bool mouseReleased( QMouseEvent* ev );
        inline bool mouseMoved( QMouseEvent* ev ) { if( bMousePressed ) currentPoint = ev->pos(); return bMousePressed; }

        QPoint pressedPoint, releasedPoint, currentPoint; 
        bool bMousePressed;
    };

    class Navigation
    {
        public:
            friend GroWidget;

            enum class Availability
            {
                AVAILABLE, LOCKED_BY_KEYS, LOCKED_BY_MOUSE
            };
          
          //camera movement speed in pixels/s
            static constexpr float SPEED_FACTOR_PAUSED = 5.0f;
            static constexpr float SPEED_PAUSED = SPEED_FACTOR_PAUSED * GroWidget::REFRESH_PERIOD;
            static constexpr float SPEED_FACTOR_RUNNING = 1.0f;
            static constexpr float SPEED_RUNNING = SPEED_FACTOR_RUNNING * GroWidget::REFRESH_PERIOD;
            
            static constexpr float MOUSE_SPEED_FACTOR_PAUSED = 0.025f;
            static constexpr float MOUSE_SPEED_PAUSED = MOUSE_SPEED_FACTOR_PAUSED * GroWidget::REFRESH_PERIOD;
            static constexpr float MOUSE_SPEED_FACTOR_RUNNING = 0.005f;
            static constexpr float MOUSE_SPEED_RUNNING = MOUSE_SPEED_FACTOR_RUNNING * GroWidget::REFRESH_PERIOD;

            static constexpr int BORDER = 100;
            

        //---ctor, dtor
            Navigation( GroWidget* owner )
            : owner( owner ), mouseTracker(), availability( Availability::AVAILABLE )
            , x( 0.0f ), y( 0.0f ), speedX( 0.0f ), speedY( 0.0f ) {;}

            ~Navigation() = default;

        //---get
            bool checkIsMoving() const { return speedX != 0.0f || speedY != 0.0f; }
            Availability getAvailability() const { return availability; }
            bool checkAvailableForKeys() const { return availability != Availability::LOCKED_BY_MOUSE; }
            bool checkAvailableForMouse() const { return availability != Availability::LOCKED_BY_KEYS; }

        //---set
            inline void setPosition( float xX, float xY ) { x = xX; y = xY; owner->getGroThread().setCameraPos( x, y ); }
            inline void setToCenter() { setPosition( 0.0f, 0.0f ); }
            inline void resetSpeed() { speedX = 0.0f; speedY = 0.0f; }
            inline void reset() { setToCenter(); }
            inline void lockByKeys() { availability = Availability::LOCKED_BY_KEYS; }
            inline void lockByMouse() { availability = Availability::LOCKED_BY_MOUSE; }
            inline void unlock() { availability = Availability::AVAILABLE; }

        //---API
            bool update();
          //events
            bool keyPressed( QKeyEvent* ev );
            bool keyReleased( QKeyEvent* ev );
            bool mousePressed( QMouseEvent* ev );
            bool mouseReleased( QMouseEvent* ev );
            bool mouseMoved( QMouseEvent* ev );

    private:
        GroWidget* owner;
        MouseTracker< Qt::RightButton > mouseTracker;
        Availability availability;

        float x; //current camera position in X
        float y; //current camera position in Y
        float speedX;
        float speedY;
    };


    class Zoom
    {
        public:
            friend GroWidget;

            enum class Direction
            {
                SMALLER, SAME, BIGGER
            };

            static constexpr float MAX_VALUE = 3.5f;
            static constexpr float MIN_VALUE = 0.005f;
            static constexpr int WHEEL_THRESHOLD = 120;
            static constexpr float SPEED = 1.2;
            static constexpr float INVERSE_SPEED = 1.0 / SPEED;
            static constexpr float BUTTON_SPEED = 2.0;
            static constexpr float BUTTON_INVERSE_SPEED = 1.0 / SPEED;


        //---ctor, dtor
            Zoom( GroWidget* owner ) : owner( owner ), value( 1.0f ), direction( Direction::SAME ), delta( 0.0f ), bFromButton( false ) {;}
            ~Zoom() = default;

        //---get
            inline float getValue() const { return value; }

            inline bool checkMaxReached() const { return value >= MAX_VALUE; }
            inline bool checkMinReached() const { return value <= MIN_VALUE; }
            inline bool checkRepaintNeeded() const { return delta > 0.0f; }

        //---set
            inline void setValue( float xValue ) { value = xValue; }
            inline void scale( float factor ) { value *= factor; }
            inline void resetDelta() { delta = 0.0f, direction = Direction::SAME; }
            inline void resetValue() { value = 1.0f; owner->getGroThread().setZoom( 1.0f ); }
            inline void resetAll() { resetDelta(); resetValue(); }

        //---API
            inline bool checkMovement() { if ( delta >= WHEEL_THRESHOLD ) return true; resetDelta(); return false; }
            inline bool zoomIn() { if( ! checkMaxReached() ) { delta = WHEEL_THRESHOLD; direction = Direction::BIGGER; bFromButton = true; return true; } return false; }
            inline bool zoomOut() { if( ! checkMinReached() ) { delta = WHEEL_THRESHOLD; direction = Direction::SMALLER; bFromButton = true; return true; } return false; }
            inline void decreaseDelta() { delta = std::max( delta - WHEEL_THRESHOLD, 0.0f ); }
            bool update();
          //events
            bool mouseWheel( QWheelEvent* ev );
            

        private:
            GroWidget* owner;
            float value;
            Direction direction;
            float delta;
            bool bFromButton;
    };


    class CellsSelection //TODO
    //for a) setting box for selecting cells within ( pressedPoint, releasedPoint ) 
    //and b) drawing the current box ( pressedPoint, currentPoint )
    {
        public:
            friend GroWidget;

        //---ctor, dtor
            CellsSelection( GroWidget* owner ) : owner( owner ), mouseTracker() {;}
            ~CellsSelection() = default;

        //---get
            bool checkIsSelecting() const { return mouseTracker.bMousePressed; }

        //---API
            bool draw( QPainter& painter );
          //events
            inline bool mousePressed( QMouseEvent* ev ) { return mouseTracker.mousePressed( ev ); }
            inline bool mouseReleased( QMouseEvent* ev ) { if( mouseTracker.mouseReleased( ev ) ){ owner->repaint(); return true; } return false; }
            inline bool mouseMoved( QMouseEvent* ev ) { { if( mouseTracker.mouseMoved( ev ) ) owner->repaint(); return true; } return false; }

        private:
            GroWidget* owner;
            MouseTracker< Qt::LeftButton > mouseTracker;
    };





///////////////////////////////////////////////////////////////// GroWidget /////////////////////////////////////////////////////////////////////////
    static constexpr int IMAGE_SIZE = GraphicsHandler::IMAGE_SIZE;
    static constexpr int IMAGE_SIZE_MIN = GraphicsHandler::IMAGE_SIZE_MIN;
    inline static const QImage ERROR_IMAGE = QImage( rsc::RESOURCE_ERRORIMAGE );
    static constexpr const char* STARTING_MSG = "Click \"Open\" to open a program. Then click \"Start\".";
    static constexpr int AUTOZOOM_PAD = 50;
    static constexpr float AUTOZOOM_FACTOR = 0.999;
    static constexpr bool AUTOZOOM_INI_STATE = false;


//---ctor, dtor
    GroWidget( int, char**, QWidget* parent = nullptr );
    GroWidget( const GroWidget& rhs ) = delete;
    virtual ~GroWidget() = default;

//---get
    inline GroThread& getGroThread() { return grothread; }
    inline GroThread::State getThreadState() const { return grothread.getState(); }
    inline float getZoom() const { return zoom.getValue(); }
    inline const ut::Rectangle& getColonyBorder() const { return grothread.getColonyBorder(); }
    inline bool getBAutoZoom() const { return bAutoZoom; }

//---set
    inline void setZoom( float xZoom ) { zoom.setValue( xZoom ); }
    inline void scaleZoom( float factor ) { zoom.scale( factor ); }
    inline void setCameraPos( float xCameraPosX, float xCameraPosY ) { navigation.setPosition( xCameraPosX, xCameraPosY ); navigation.resetSpeed(); }
    inline void setCameraPosToCenter() { navigation.setToCenter(); navigation.resetSpeed(); zoom.resetAll(); }
    inline void resetCamera() { navigation.reset(); zoom.resetValue(); }
    inline void setToRest() { navigation.resetSpeed(); zoom.resetDelta(); }
    inline void setBRepaintRequired( bool val = true ) { bRepaintRequired = val; }
    inline void setAutoZoom( bool val ) { bAutoZoom = val; }

//---gui actions
    //gui main toolbar
    void open( QString path, bool bSameSeed = true ); //open .gro file
    void startStop(); //start or pause simulation
    

//---celled from gui
    inline QSize sizeHint() { return QSize( IMAGE_SIZE, IMAGE_SIZE ); } //Qt stuff. Returns preferred/default image size. Used if sizePolicy = preferred
    inline QSize minimumSizeHint() {  return QSize( IMAGE_SIZE_MIN, IMAGE_SIZE_MIN ); } //Qt stuff //ELE_S: no funciona
    void showCenteredMessage( QString msg );

    void updateAutoZoom();
    
    inline void zoomIn() { zoom.zoomIn(); }
    inline void zoomOut() { zoom.zoomOut(); }
  //events
    void keyPressed( QKeyEvent* ev );
    void keyReleased( QKeyEvent* ev );
    void mouseWheel( QWheelEvent* ev ) { if( ! bAutoZoom ) zoom.mouseWheel( ev ); }
    

private:
  //resources
    GroThread grothread;
    Navigation navigation;
    Zoom zoom;
    CellsSelection cellsSelection;
    QPixmap pixmap; //graphics
    SP<QTimer> repaintTimer; //timer that rules refresh by thriggering repaintIfRequired() every refreshPeriod
  
  //flags
    bool error_flag; //true if error parsing .gro file
    bool bRepaintRequired; //repaintIfRequired() calls repaint() if true. Used for updating camera pos when simulation paused
    bool bAutoZoom;

  //response to events
    void paintEvent( QPaintEvent* ev );
    inline void resizeEvent( QResizeEvent* ) { grothread.resize( size() ); }
    void mousePressEvent( QMouseEvent* ev ) { if( ! bAutoZoom ) {navigation.mousePressed( ev ); cellsSelection.mousePressed( ev ); } }
    void mouseReleaseEvent( QMouseEvent* ev ) { navigation.mouseReleased( ev ); cellsSelection.mouseReleased( ev ); repaint(); }
    void mouseMoveEvent( QMouseEvent* ev );
    bool event( QEvent* ev ) override { return QWidget::event(ev); }


signals: 
    void messageFromGroWidget( QString msg, bool clear = false ); //for transmiting messages from GroThread to Gui through GroWidget
    void popupFromGroWidget( QString title, QString msg, bool bBeep ); 
    void updateToolbarFromGroWidget();
    void quitFromGroWidget();

public slots:
    void onlyStop();
    
private slots:
    inline void updatePixmap( const QImage &image ) { pixmap.convertFromImage( image ); update(); }
    void handleStateChange();
    inline void passMessage( std::string msg, bool clear = false ) { emit messageFromGroWidget( msg.c_str(), clear ); }
    inline void passPopupToGui( const std::string& title, const std::string& msg, bool bBeep ) { emit popupFromGroWidget( title.c_str(), msg.c_str(), bBeep ); }
    inline void passQuitToGui() { emit quitFromGroWidget(); } 
    inline void passToolbarUpdateToGui() { emit updateToolbarFromGroWidget(); }
    inline void repaintIfRequired() { if( navigation.checkIsMoving() || zoom.checkRepaintNeeded() ) repaint(); }
};





//================================================================= template definitions
template< Qt::MouseButton VButton >
bool GroWidget::MouseTracker< VButton >::mousePressed( QMouseEvent* ev )
{
    if( ev->button() == BUTTON ) 
    {
        currentPoint = releasedPoint = pressedPoint = ev->pos(); 
        bMousePressed = true; 
        return true;
    } 
    return false;
}

template< Qt::MouseButton VButton >
bool GroWidget::MouseTracker< VButton >::mouseReleased( QMouseEvent* ev )
{
    if( ev->button() == BUTTON )
    {
        releasedPoint = ev->pos();
        bMousePressed = false;
        return true;
    }
    return false;
}

#endif // GROWIDGET_H
