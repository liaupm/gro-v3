#ifndef UT_REPORT_HPP
#define UT_REPORT_HPP

#include "ut/defines.hpp"


namespace ut
{
	class HandlerBase;
	template< typename TModel, typename TOwner, typename TEvent >
	class Report
	{
		public:
			using ModelType = TModel;
			using OwnerType = TOwner;
			using EventType = TEvent;

		//---ctor, dtor
			virtual ~Report() = default;

		//---get
			inline const ModelType* getModel() const { return model; }
			
		//---set
			inline void setOwner( HandlerBase* xOwnerHandler ) { ownerHandler = static_cast<OwnerType*>( xOwnerHandler ); }

		//---API


		protected:
			const ModelType* model; //LinkedElement-derived class that contains the parameters
			OwnerType* ownerHandler; //derived from Handler<>

			Report( const ModelType* model, OwnerType* ownerHandler ) : model( model ), ownerHandler( ownerHandler ) {;}
	};
}

#endif //UT_REPORT_HPP