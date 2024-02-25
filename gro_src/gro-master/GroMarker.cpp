#include "GroMarker.h"
#include "GroCollection.h" //ctor, conversion to base


BGroMarker::BGroMarker( const GroCollection* groCollection, size_t id, const std::string& name )
: GroElement( groCollection, id, name, GroElementIdx::ElemTypeIdx::BMARKER ) {;}


QGroMarker::QGroMarker( const GroCollection* groCollection, size_t id, const std::string& name )
: GroElement( groCollection, id, name, GroElementIdx::ElemTypeIdx::QMARKER ) {;}