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

#include "MessageHandler.h"
#include "GroPainter.h" //render()

/*PRECOMPILED
#include <QString> //render()
#include <QSize> //render()
#include <QPoint> //render()
#include <QRect> //render() 
#include <Qt> //Qt::AlignLeft, Qt::AlignRight */


//////////////////////////////////////////////////////////// MESSAGE BUFFER ///////////////////////////////////////////////////////////////////////////
void MessageHandler::MessageBuffer::render( GroPainter* painter, uint h, uint w )
{
    uint x = inverterX * ( h + offsetX );
    uint y = inverterY * ( w + offsetY );;
    QSize messageSize = QSize( w + sizeY, sizeX );

    for ( std::list<std::string>::iterator k = messageQueue.begin(); k != messageQueue.end(); k++ )
        painter->drawText( QRect( QPoint( y, x + std::distance( messageQueue.begin(), k ) * DF_FONTSIZE ), messageSize ), QString( (*k).c_str() ), aligmentOption );
}



//////////////////////////////////////////////////////////// MESSAGE HANDLER ///////////////////////////////////////////////////////////////////////////
MessageHandler::MessageHandler()
{
    buffers.push_back( MessageBuffer( -1, -1, - 3 * DF_BUFSIZE, -DF_BUFSIZE, MessageBuffer::DF_SIZE_X, -DF_BUFSIZE, DF_BUFSIZE, QTextOption( Qt::AlignLeft ) ) ); //left up
    buffers.push_back( MessageBuffer( -1, 0, - DF_BUFSIZE, 0, MessageBuffer::DF_SIZE_X, -DF_BUFSIZE, DF_BUFSIZE, QTextOption( Qt::AlignRight ) ) ); //right up
    buffers.push_back( MessageBuffer( 1, -1, - DF_FONTSIZE * DF_BUFSIZE, -DF_BUFSIZE, MessageBuffer::DF_SIZE_X, -DF_BUFSIZE, DF_BUFSIZE, QTextOption( Qt::AlignLeft ) ) ); //left down
    buffers.push_back( MessageBuffer( 1, 0, - DF_FONTSIZE * DF_BUFSIZE, 0, MessageBuffer::DF_SIZE_X, -DF_BUFSIZE, DF_BUFSIZE, QTextOption( Qt::AlignRight ) ) ); //right down
}

void MessageHandler::render( GroPainter* painter )
{
    uint w = painter->getSize().width() / 2,
      h = painter->getSize().height() / 2;

    for( size_t b = 0; b < buffers.size(); b++ )
        buffers[b].render( painter, h, w );
}