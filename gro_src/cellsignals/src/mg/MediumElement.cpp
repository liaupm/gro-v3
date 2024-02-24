#include "mg/MediumElement.hpp"
#include "mg/MediumCollection.hpp"

using namespace mg;


MediumElement::MediumElement( size_t relativeId, const std::string& name, MediumElementIdx::ElemTypeIdx elementType, bool bQuantitative, const MediumCollection* collection ) 
: ut::LinkedElement<MediumElement, MediumElementIdx>::LinkedElement( relativeId, name, elementType, bQuantitative, false, collection ) {;}