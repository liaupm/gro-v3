#include "CellsPlot.h"
#include "Timer.h" //loadDirectElemLink()
#include "PopulationStat.h" //loadDirectElemLink()
#include "GroCollection.h" //ctor


CellsPlot::CellsPlot( GroCollection* groCollection, size_t id, const std::string& name
, const Timer* timer, const std::vector< const PopulationStat* >& statFields, const std::vector< const GroElement* >& otherFields
, const std::vector< std::string >& hexColours
, const std::string& path, const std::string& fileName, const std::string& format, size_t historicSize, int precision )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::CELLS_PLOT )
, timer( timer ), statFields( statFields ), otherFields( otherFields )
, hexColours( hexColours ), historicSize( historicSize ), precision( precision ), roundMultiplier( std::pow( 10.0, precision ) )
, path( path ), fileName( fileName != "" && fileName != DF_FILE_NAME ? fileName : name ), format( format ) {;}


//---------------API-precompiled 

void CellsPlot::loadDirectElemLink()
{ 
	this->addLink( timer, LinkDirection::BACKWARD ); 
	this->addLinks( statFields, LinkDirection::BACKWARD ); 
	this->addLinks( otherFields, LinkDirection::BACKWARD ); 
}

void CellsPlot::passUsedForPlots()
{ 
	for( auto statF : statFields )
		const_cast<PopulationStat*>( statF )->passUsedForPlots();

	for( auto otherF : otherFields )
		const_cast<GroElement*>( otherF )->passUsedForPlots();
} 
