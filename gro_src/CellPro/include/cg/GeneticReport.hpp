#ifndef CG_GENETICREPORT_HPP
#define CG_GENETICREPORT_HPP

#include "ut/Report.hpp" //base class 
#include "ut/Event.hpp" //for base report type

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //for base report type

namespace cg
{
	class Genome;

	template< typename TModel, typename TOwnerHandler >
	class GeneticReport : public ut::Report< TModel, TOwnerHandler, ut::Event<GeneticElement> >
	{
		public:
			using BaseReportType = ut::Report< TModel, TOwnerHandler, ut::Event<GeneticElement> >;
			using ModelType = TModel;
			using HandlerType = TOwnerHandler;


		//---ctor, dtor
			virtual ~GeneticReport() = default;
			

		protected:
			GeneticReport( const ModelType* model, HandlerType* ownerHandler ) : BaseReportType( model, ownerHandler ) {;}
			GeneticReport( const GeneticReport& rhs, HandlerType* ownerHandler ) : BaseReportType( rhs.model, ownerHandler ) {;}
	};
}

#endif //CG_GENETICREPORT_HPP