#ifndef GROREPORT_H
#define GROREPORT_H

#include "ut/Report.hpp" //base
#include "ut/Event.hpp" //template param

#include "defines.h"
#include "GroElement.h" //template param


template< typename TModel, typename TOwnerHandler >
class GroReport : public ut::Report< TModel, TOwnerHandler, ut::Event<GroElement> >
{
	public:
		using BaseReportType = ut::Report< TModel, TOwnerHandler, ut::Event<GroElement> >;
		using ModelType = TModel;
		using HandlerType = TOwnerHandler;

	//---ctor, dtor
		GroReport( const ModelType* model, HandlerType* ownerHandler ) : BaseReportType( model, ownerHandler ) {;}
		virtual ~GroReport() = default;

	//---get

	//---set

	//---API

	protected:
};

#endif // GROREPORT_H
