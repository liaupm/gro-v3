#ifndef GRO_ELEMENT_H
#define GRO_ELEMENT_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/LinkedElement.hpp" //base
#include "ut/ElementIndex.hpp" //base

#include "Defines.h"

//PRECOMPILED
//#include <string> //name


class GroCollection; //ctor
struct GroElementIdx : public ut::ElementIndex
{
    enum ElemTypeIdx : size_t
    {
        BMARKER, QMARKER, TIMER, CHECKPOINT, POP_STAT, POP_FUNCTION, POP_GATE, CELL_PLACER, CELL_PLATING, SIGNAL_PLACER, CELLS_FILE, SNAPSHOT, CELLS_PLOT, COUNT
    };
};

class CellsFile;
class GroElement : public ut::LinkedElement<GroElement, GroElementIdx>
{
    REGISTER_CLASS( "GroElement", "gro", GroElementIdx::ElemTypeIdx::COUNT ) //used by Parser() for all the classes that are not real GroElements (do not derive from GroElement)

	public:
        using ElementTypeIdx = GroElementIdx::ElemTypeIdx;
        using ElemTypeIdx = GroElementIdx::ElemTypeIdx;
        //using Mask = ut::MultiBitset;


		virtual ~GroElement() = default;

    //---API
        virtual void passUsedForTimers() {;}
        virtual void passUsedForPlots() {;}
        virtual void passUsedForOutput( const CellsFile* ) {;}


	protected:
		GroElement( const GroCollection* collection, size_t relativeId, const std::string& name, ElemTypeIdx elemTypeIdx );
};

#endif // GRO_ELEMENT
