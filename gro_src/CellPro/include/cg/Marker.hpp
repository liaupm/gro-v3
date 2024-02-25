#ifndef CG_MARKER_HPP
#define CG_MARKER_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
	class GeneticCollection;
	class BMarker : public GeneticElement
	{
		REGISTER_CLASS( "BMarker", "bm", GeneticElement::ElemTypeIdx::BMARKER, false )

		public:
		//---ctor, dtor
			BMarker( const GeneticCollection* geneticCollection, size_t id, const std::string& name )
			: GeneticElement( id, name, GeneticElement::ElemTypeIdx::BMARKER, false, geneticCollection, GeneticElement::ElemCategory::LOGIC ) {;}

			virtual ~BMarker() = default;
	};

	class QMarker : public GeneticElement
	{
		REGISTER_CLASS( "QMarker", "qm", GeneticElement::ElemTypeIdx::QMARKER, true )

		public:
		//---ctor, dtor
			QMarker( const GeneticCollection* geneticCollection, size_t id, const std::string& name )
			: GeneticElement( id, name, GeneticElement::ElemTypeIdx::QMARKER, true, geneticCollection, GeneticElement::ElemCategory::LOGIC ) {;}

			virtual ~QMarker() = default;
	};


	class EventMarker : public GeneticElement
	{
		REGISTER_CLASS( "EventMarker", "em", GeneticElement::ElemTypeIdx::EVENT_MARKER, true )

		public:
		//---ctor, dtor
			EventMarker( const GeneticCollection* geneticCollection, size_t id, const std::string& name )
			: GeneticElement( id, name, GeneticElement::ElemTypeIdx::EVENT_MARKER, false, geneticCollection, GeneticElement::ElemCategory::LOGIC ) {;}

			virtual ~EventMarker() = default;
	};
}

#endif //CG_MARKER_HPP