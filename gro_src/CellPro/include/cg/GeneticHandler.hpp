#ifndef CG_GENETICHANDLER_HPP
#define CG_GENETICHANDLER_HPP

#include "ut/Event.hpp" //tesmplate arg
#include "ut/Handler.hpp" //base

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //ut::Event<GeneticElement>
#include "cg/GeneticCollection.hpp" //used in base Handler<>

/*PRECOMPILED
#include <vector> //ReportsDS */


namespace cg
{
	class Genome;

	class GeneticHandlerIdx : ut::ElementIndex
    {
	    public:
		    enum HandlerTypeIdx
		    {
		    	H_COLOUR, H_PLASMIDS, H_HORIZONTAL, H_GROWTH, H_EXPRESSION, H_METABOLISM, H_LOGIC,  H_RANDOMNESS, COUNT
		    };
    };

    template< typename... TReports >
	class GeneticHandler : public ut::Handler< GeneticCollection, Genome, ut::Event<GeneticElement>, TReports... >
	{
		public:
			using HandlerBaseType = ut::Handler< GeneticCollection, Genome, ut::Event<GeneticElement>, TReports... >;
			using HandlerTypeIdx = GeneticHandlerIdx::HandlerTypeIdx;
			using HandlerIdx = HandlerTypeIdx;
			using HandlerBaseType::OwnerType;
			using HandlerBaseType::CollectionType;
			using EventType = ut::Event<GeneticElement>;

			template< typename TReport >
			using ReportsDS = std::vector<TReport>;


		//---ctor, dtor
			GeneticHandler( const GeneticHandler& rhs ) = default;
			virtual ~GeneticHandler() = default;
			GeneticHandler( GeneticHandler&& rhs ) noexcept = default;
			GeneticHandler& operator=( GeneticHandler&& rhs ) noexcept = default;

		protected:
			GeneticHandler( Genome* owner, const GeneticCollection* collection ) : HandlerBaseType( owner, collection ) {;}
			//GeneticHandler( const GeneticHandler& mother, Genome* owner ) : HandlerBaseType( mother, owner ) {;}
	};
}

#endif //CG_GENETICHANDLER_HPP