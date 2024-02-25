#ifndef GROHANDLER_H
#define GROHANDLER_H

#include "ut/Event.hpp" //template param
#include "ut/Handler.hpp" //base

#include "Defines.h"
#include "GroElement.h" //template param
#include "GroCollection.h" //template param, ctor


class PetriDish; //ctor and template param
struct GroHandlerIdx : ut::ElementIndex
{
    enum HandlerTypeIdx
    {
    	H_OUTPUT, H_PLACING, H_TIMERS, H_LOGIC, H_CELLS, COUNT
    };
};

template< typename... TReports >
class GroHandler : public ut::Handler< GroCollection, PetriDish, ut::Event<GroElement>, TReports... >
{
	public:
		using HandlerBaseType = ut::Handler< GroCollection, PetriDish, ut::Event<GroElement>, TReports... >;
		using HandlerTypeIdx = GroHandlerIdx::HandlerTypeIdx;
		using HandlerIdx = HandlerTypeIdx;
		using HandlerBaseType::OwnerType;
		using HandlerBaseType::CollectionType;
		using EventType = ut::Event<GroElement>;


	//---ctor, dtor
		GroHandler( PetriDish* owner, const GroCollection* collection ) : HandlerBaseType( owner, collection ) { ; }
		virtual ~GroHandler() = default;

	//---get
		TReal getTime() const;

	//---set

	//---API

	protected:
};

#endif // GROHANDLER_H
