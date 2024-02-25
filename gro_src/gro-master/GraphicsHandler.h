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

#ifndef GRAPHICS_HANDLER_H
#define GRAPHICS_HANDLER_H

#include "mg/Medium.hpp" //::ConcsDS in mixSignalColours()
#include "Defines.h"
#include "Theme.h" //theme

/*PRECOMPILED
#include <QSize> //size
#include <QImage> //::Format_RGB32
#include <QString> //snapshot() pathname */


namespace mg { class MediumCollection; } //param of mixSignalColours()
class Cell; //renderCell()
class World; //param of renderWorld() and renderMediumGrid
class GroPainter; //render methods
class GraphicsHandler
{
	public:
		static constexpr int IMAGE_SIZE = 800;
    	static constexpr int IMAGE_SIZE_MIN = 400;
    	static constexpr bool DF_B_RENDER_SIGNALS = true;
    	static constexpr int HEADER_PAD_H = 10;
    	static constexpr int HEADER_PAD_V = 20;

		static void drawString( GroPainter& painter, int x, int y, const char* str );


	//---ctor, dtor
		GraphicsHandler() 
		: size( IMAGE_SIZE, IMAGE_SIZE ), image( size, QImage::Format_RGB32 ), bRenderMediumGrid( DF_B_RENDER_SIGNALS )
		, zoom( 1.0 ), cameraPosX( 0.0 ), cameraPosY( 0.0 ), bResized( false ) {;}

		~GraphicsHandler() = default;

	//---get
		inline const QSize& getSize() const { return size; }
		inline QSize& getSizeEdit() { return size; }
		inline const QImage& getImage() const { return image; }
		inline QImage& getImageEdit() { return image; }
		inline const Theme& getTheme() const { return theme; }
		inline TReal getZoom() const { return zoom; }
		inline bool getBResized() const { return bResized; }

	//---set
		inline void setImage( QImage& xImage ) { image = xImage; }
		inline void resetImage() { image = QImage( size, QImage::Format_RGB32 ); }
		inline void setTheme( const Theme& xTheme ) { theme = xTheme; }
		inline void setBRenderMediumGrid( bool xRenderMediumGrid ) { bRenderMediumGrid = xRenderMediumGrid; }
		inline void setZoom( TReal xZoom ) { zoom = xZoom; }
		inline void setCameraPos( TReal x, TReal y ) { cameraPosX = x; cameraPosY = y; }
		inline void setBResized( bool xResized = true ) { bResized = xResized; }

	//---API
		void renderWorld( const World& world, GroPainter& painter ); //render all: cells, medium and header
		void renderEcoli( const Cell& cell, GroPainter& painter ); //render the cells (position, size, otation and colour)
    	void renderMediumGrid( const World& world, GroPainter& painter ); //render the medium (colored signals)
    
    	inline bool snapshot( const QString& pathname ) {  return image.save( pathname ); }

    	
	private:
	  //resources
		QSize size; //size of the image, used to create image
		QImage image; //current rendered image
		Theme theme; //colour palette
	  //options
		bool bRenderMediumGrid; //from global params
	  //state
		TReal zoom;
		TReal cameraPosX;
        TReal cameraPosY;
    	bool bResized; //requires rendering 


    	bool mixSignalColours( const mg::MediumCollection& mediumCollection, const mg::Medium::ConcsDS& signalConcs, TReal& r, TReal& g,  TReal& b ); //calculates the colour of a grid unit with different signals
};

#endif // GRAPHICS_HANDLER_H
