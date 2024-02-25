#include "GroElement.h"
#include "GroCollection.h" //ctor, cast to base


GroElement::GroElement( const GroCollection* collection, size_t relativeId, const std::string& name, ElemTypeIdx elemTypeIdx )
: ut::LinkedElement<GroElement, GroElementIdx>( relativeId, name, elemTypeIdx, false, false, collection ) {;}