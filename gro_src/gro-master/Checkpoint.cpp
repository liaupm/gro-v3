#include "Checkpoint.h"
#include "Timer.h" //loadDirectElemLink()
#include "GroCollection.h" //ctor


Checkpoint::Checkpoint( GroCollection* groCollection, size_t id
, const std::string& name, const Timer* timer, const std::string& message, bool bSound )

: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::CHECKPOINT )
, timer( timer ), message( message ), bSound( bSound ) {;}


void Checkpoint::loadDirectElemLink() { this->addLink( timer, LinkDirection::BACKWARD ); }