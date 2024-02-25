#ifndef GROMARKER_H
#define GROMARKER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "defines.h"
#include "GroElement.h" //base class

//PRECOMPILED
//#include <string> //name in constructor


class GroCollection;
class BGroMarker : public GroElement
{
	REGISTER_CLASS( "BGroMarker", "bm", ElemTypeIdx::BMARKER, false )

	public:
	//---ctor, dtor
		BGroMarker( const GroCollection* groCollection, size_t id, const std::string& name );

		virtual ~BGroMarker() = default;
		
	//---get

	//---set

	//---API
		void passUsedForTimers() override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForTimers(); }
	    void passUsedForPlots() override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForPlots(); }
        void passUsedForOutput( const CellsFile* cellsFile ) override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForOutput( cellsFile ); }

	private:
};

class QGroMarker : public GroElement
{
	REGISTER_CLASS( "QGroMarker", "qm", ElemTypeIdx::QMARKER, true )

	public:
	//---ctor, dtor
		QGroMarker( const GroCollection* groCollection, size_t id, const std::string& name );

		virtual ~QGroMarker() = default;
		
	//---get

	//---set

	//---API
		void passUsedForTimers() override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForTimers(); }
	    void passUsedForPlots() override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForPlots(); }
        void passUsedForOutput( const CellsFile* cellsFile ) override { for( auto& parentStat : getBackwardLink().elemsByType[ ElemTypeIdx::POP_STAT ] ) const_cast<GroElement*>( parentStat )->passUsedForOutput( cellsFile ); }

	private:
};

#endif // GROMARKER_H
