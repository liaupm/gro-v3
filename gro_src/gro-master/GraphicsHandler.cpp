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

#include "GraphicsHandler.h"
#include "ut/RgbColour.hpp" //rgbCol
#include "cg/Genome.hpp" //getColour()
#include "mg/Signal.hpp" //signal colour
#include "mg/MediumCollection.hpp" //getById Signal
#include "mg/GridReport.hpp" //getConcsAtGridUnit()

#include "Cell.h" //getGenome(), getBody() and use of Body
#include "GroPainter.h" //render methods
#include "World.h" //::PopulationType

/*PRECOMPILED
#include <vector> //scaledSignals
#include <algorithm> //min, max

#include <QColor> //col
#include <QBrush> //setBrush()  */


void GraphicsHandler::drawString( GroPainter& painter, int x, int y, const char* str ) { painter.drawText ( x, y, str ); }


void GraphicsHandler::renderWorld( const World& world, GroPainter& painter )
{
  //prepare
    painter.setBackground( theme.backgroundBrush );
    painter.clear();
    painter.scale( zoom,zoom );
    painter.translate( cameraPosX, cameraPosY );
  //medium
    if( bRenderMediumGrid && world.getBUseSignals() )
        renderMediumGrid( world, painter );
  //cells
    const World::PopulationType& population = world.getPopulation();
    for( size_t p = 0; p < population.size(); p++ )
        renderEcoli( *population[p], painter );

  //header
    painter.setBrush( QBrush() );
    painter.reset(); //upper left corner
    painter.setPen( theme.messagePen );

    char upperTestLine[1024];
    sprintf( upperTestLine, "Cells: %d, t: %.2f min, seed: %d", static_cast<int>( world.getPopulationSize() ), world.getTime().time, world.getSeed() );
    drawString( painter, - painter.getSize().width() / 2 + HEADER_PAD_H, - painter.getSize().height() / 2 + HEADER_PAD_V, upperTestLine );
}

void GraphicsHandler::renderEcoli( const Cell& cell, GroPainter& painter )
{
  //fill colour
    const ut::RgbColour& rgbCol = cell.getGenome().getColour();
    QColor col;
    col.setRgbF( rgbCol.r, rgbCol.g, rgbCol.b, 1.0 ); //float format with maximum alpha (1.0). Guaranteed in [0.0-1.0] by cg::ColourHandler
    painter.setBrush( col );
  //border colour
    painter.setPen( cell.getBSelected() ? theme.cellOutlineSelectedPen : theme.cellOutlinePen ); //currently, there are not selected cells, all are of theme.cellOutlinePen

  //rotation
    ceVector2 center = cell.getBody().getCenter();
    painter.translate( center.x, center.y );
    painter.rotate( cell.getBody().getRotation() * CE_RADIAN );
    painter.translate( - center.x, -center.y );
  //draw
    TReal length = cell.getBody().getLength();
    painter.drawRoundedRect( center.x - length / 2.0, center.y - Cell::Body::CE_HALF_WIDTH, length, Cell::Body::CE_WIDTH, Cell::Body::CE_HALF_WIDTH, Cell::Body::CE_HALF_WIDTH );
  //back to original rotation
    painter.translate( center.x, center.y );
    painter.rotate( - cell.getBody().getRotation() * CE_RADIAN );
    painter.translate( - center.x, - center.y );
}

void GraphicsHandler::renderMediumGrid( const World& world, GroPainter& painter )
{
    const mg::Medium& medium = world.getPetriDish()->getMedium();
    const mg::GridReport& gridReport = *medium.getGridReport();
    const mg::Medium::GlobalBorderDS& border = medium.getGlobalBorder();

    for( int i = border[mg::Medium::CoordIdx::COORD_Y1]; i < border[mg::Medium::CoordIdx::COORD_Y2] + 1; i++ ) //medium grid rows
    {
        for( int j = border[mg::Medium::CoordIdx::COORD_X1]; j< border[mg::Medium::CoordIdx::COORD_X2] + 1; j++ )  //medium grid columns
        {
            TReal r = 0, g = 0, b = 0; //color channels
            if( mixSignalColours( *world.getMediumCollection(), gridReport.getConcsAtGridUnit( i, j ), r, g, b ) )
            {
              //convert channels to QColor
                QColor col;
                col.setRgbF( ut::fitU( r ), ut::fitU( g ), ut::fitU( b ) );
              //get the coordinates 
                TReal firstX = -( gridReport.getGridSize() / 2.0 ) * gridReport.getUnitSize();
              //draw
                painter.fillRect( firstX + gridReport.getUnitSize() * j, firstX + gridReport.getUnitSize() * i, gridReport.getUnitSize(), gridReport.getUnitSize(), col );
            }
        }
    }
}


bool GraphicsHandler::mixSignalColours( const mg::MediumCollection& mediumCollection, const mg::Medium::ConcsDS& signalConcs, TReal& r, TReal& g,  TReal& b )
{
  //calculate the scaling factor
    std::vector<TReal> scaledSignals; //intensity of each signal after scaling
    TReal max = 0.0; //maximum unscaled intensity among all the signals 
    TReal total = 0.0; //total sum of the unscaled intensity of all the signals 

    for( uint s = 0; s < signalConcs.size(); s++ )
    {
        TReal scaledConc = ut::fitU( signalConcs[s] / mediumCollection.getById<mg::Signal>( s )->getColourSaturationConc() ); //ratio to its saturation value, ubound 1.0
        max = std::max( max, scaledConc );
        total += scaledConc;
        scaledSignals.push_back( scaledConc );
    }
    if( ! ut::isPositive( max ) ) //very small values are not printed
        return false;

    TReal factor = max / total; //scaling factor
    
  //add the signal colours scaled
    for( uint s = 0; s < scaledSignals.size(); s++ )
    {
        const ut::RgbColour& colour = mediumCollection.getById<mg::Signal>( s )->getColour();
        r += scaledSignals[s] * factor * colour.getR();
        g += scaledSignals[s] * factor * colour.getG();
        b += scaledSignals[s] * factor * colour.getB();
    }
  //add background
    TReal remaining = 1.0 - max; //background fraction
    r += remaining * theme.backgroundR;
    g += remaining * theme.backgroundG;
    b += remaining * theme.backgroundB;
    return true;
}
