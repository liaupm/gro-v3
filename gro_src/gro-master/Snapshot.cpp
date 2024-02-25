#include "Snapshot.h"
#include "GroCollection.h" //ctor, conversion to base
#include "Timer.h" //loadDirectElemLink()


Snapshot::Snapshot( GroCollection* groCollection, size_t id, const std::string& name
, const Timer* timer, const std::string& path, const std::string& fileName, const std::string& imgFormat )
: GroElement( groCollection, id, name, ElementTypeIdx::SNAPSHOT )
, timer( timer ), path( path ), fileName( fileName ), imgFormat( imgFormat ) {;}


void Snapshot::loadDirectElemLink() { this->addLink( timer, LinkDirection::BACKWARD ); }