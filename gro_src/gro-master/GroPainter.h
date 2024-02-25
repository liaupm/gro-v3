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

#ifndef GROPAINTER_H
#define GROPAINTER_H

/*PRECOMPILED
#include <QSize> //size, middle
#include <QPoint> //in reset() 
#include <QPainter> //base class
#include <QPaintDevice> //device */


class GroPainter : public QPainter
{
	public:
	//---ctor, dtor
	    GroPainter( const QSize& s, QPaintDevice* device );
	    virtual ~GroPainter() = default;
	
	//---get
        inline const QSize& getMiddle() { return middle; }
        inline const QSize& getSize() { return size; }
	
	//---API
	    inline void clear() { eraseRect( -middle.width(), -middle.height(), size.width(), size.height() ); }
	    inline void reset() { resetTransform(); translate( QPoint( middle.width(), middle.height() ) ); }


	private:
	    QSize size;
	    QSize middle;
};

#endif // GROPAINTER_H
