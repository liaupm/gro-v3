#include "Theme.h"

/*PRECOMPILED
#include <QColor> //backgroundCol in ctor */


Theme::Theme()
: backgroundBrush( QColor( DF_BACKGROUND_HEXCOL.c_str() ) ) 
, cellOutlinePen( QColor( DF_CELL_OUTLINE_HEXCOL.c_str() ) )
, cellOutlineSelectedPen( QColor( DF_CELL_OUTLINE_SELECTED_HEXCOL.c_str() ) )
, messagePen( QColor( DF_MESSAGE_HEXCOL.c_str() ) )
, mousePen( QBrush( QColor( DF_MOUSE_HEXCOL.c_str() ), Qt::SolidPattern ), 1, Qt::DashDotLine )
{
    QColor backgroundCol( DF_BACKGROUND_HEXCOL.c_str() );
    backgroundR = backgroundCol.red() / 255.0;
    backgroundG = backgroundCol.green() / 255.0;
    backgroundB = backgroundCol.blue() / 255.0;
}

Theme::Theme( const std::string& backgroundHexCol, const std::string& cellOutlineHexCol, const std::string& cellOutlineSelectedHexCol, const std::string& messageHexCol, const std::string& mouseHexCol )
: backgroundBrush( QColor( backgroundHexCol.c_str() ) ) 
, cellOutlinePen( QColor( cellOutlineHexCol.c_str() ) )
, cellOutlineSelectedPen( QColor( cellOutlineSelectedHexCol.c_str() ) )
, messagePen( QColor( messageHexCol.c_str() ) )
, mousePen( QBrush( QColor( mouseHexCol.c_str() ), Qt::SolidPattern ), 1, Qt::DashDotLine )
{
    QColor backgroundCol( backgroundHexCol.c_str() );
    backgroundR = backgroundCol.red() / 255.0;
    backgroundG = backgroundCol.green() / 255.0;
    backgroundB = backgroundCol.blue() / 255.0;
}