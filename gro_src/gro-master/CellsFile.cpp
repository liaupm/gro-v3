#include "CellsFile.h"
#include "Timer.h" //loadDirectElemLink()
#include "PopulationStat.h" //loadDirectElemLink()
#include "GroCollection.h" //ctor


CellsFile::CellsFile( GroCollection* groCollection, size_t id, const std::string& name
, const Timer* timer, const cg::GeneticElement* individualGate, const std::vector< const PopulationStat* >& statFields, const std::vector< const GroElement* >& otherFields
, Mode mode, bool bIncludeAncestry
, const std::string& path, const std::string& fileName, const std::string& format, uint precision )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::CELLS_FILE )
, timer( timer ), individualGate( individualGate ), statFields( statFields ), otherFields( otherFields )
, mode( mode ), bIncludeAncestry( bIncludeAncestry )
, path( path ), fileName( fileName != "" && fileName != DF_FILE_NAME ? fileName : name ), format( format ), separator( format == ".tsv" ? "\t" : "," ), precision( precision )
, popFileWholeName( path + "/" + fileName + format ), individualFileBaseName( path + "/" + fileName ) {;}



void CellsFile::loadDirectElemLink()
{ 
	this->addLink( timer, LinkDirection::BACKWARD ); 
	this->addLinks( statFields, LinkDirection::BACKWARD ); 
	this->addLinks( otherFields, LinkDirection::BACKWARD ); 
}

void CellsFile::passUsedForOutput( const CellsFile* )
{ 
	for( auto statF : statFields )
		const_cast<PopulationStat*>( statF )->passUsedForOutput( this );

	for( auto otherF : otherFields )
		const_cast<GroElement*>( otherF )->passUsedForOutput( this );
} 