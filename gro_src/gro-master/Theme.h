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

#ifndef THEME_H
#define THEME_H

#include "Defines.h"

/*PRECOMPILED
#include <QBrush> //backgroundBrush
#include <QPen> //cellOutlinePen, cellOutlineSelectedPen, messagePen, mousePen */


class Theme 
{
    public:
        friend class GraphicsHandler;
        friend class GroWidget;
        inline static const std::string DF_BACKGROUND_HEXCOL = "#000000";
        inline static const std::string DF_CELL_OUTLINE_HEXCOL = "#444444";
        inline static const std::string DF_CELL_OUTLINE_SELECTED_HEXCOL = "#880000";
        inline static const std::string DF_MESSAGE_HEXCOL = "#ffffff";
        inline static const std::string DF_MOUSE_HEXCOL = "#ffffff";

    //---ctor, dtor
        Theme();
        Theme( const std::string& backgroundHexCol, const std::string& cellOutlineHexCol, const std::string& cellOutlineSelectedHexCol, const std::string& messageHexCol, const std::string& mouseHexCol );
        inline ~Theme() = default; 


    private:
        QBrush backgroundBrush;
        QPen cellOutlinePen;
        QPen cellOutlineSelectedPen;
        QPen messagePen;
        QPen mousePen;

        TReal backgroundR, backgroundG, backgroundB;
};

#endif // THEME_H