#ifndef CG_METABOLITE_HPP
#define CG_METABOLITE_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
    class GeneticCollection;
    class QMarker; //signalMarker
	class Metabolite : public GeneticElement
	/* Any metabolite that maps to a ms::Signal or the total biomass flux */
	{
		REGISTER_CLASS( "Metabolite", "met", GeneticElement::ElemTypeIdx::METABOLITE, false )

		public:
			static constexpr bool DF_BBIOMASS = false;

		//---ctor, dtor
			Metabolite( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const QMarker* signalMarker, bool bBiomass = DF_BBIOMASS )
			: GeneticElement( id, name, GeneticElement::ElementIndex::MOLECULE, false, geneticCollection, ElemCategory::FUNCTIONAL ) 
			, signalMarker( signalMarker ), bBiomass( bBiomass ) {;}

			virtual ~Metabolite() = default;

		//---get
			inline const QMarker* getSignalMarker() const { return signalMarker; }
			inline bool getHasSignalMarker() const { return signalMarker; }
			inline bool getIsBiomass() const { return bBiomass; }

		//---set
			inline void setSignalMarker( const QMarker* xSignalMarker ) { signalMarker = xSignalMarker; }


		private:
			const QMarker* signalMarker; //exposes the signal concentration in the vecinity of the cell, only if ! bBiomass
			bool bBiomass; //whether this represents the biomass flux or not( any metabolite that maps to a medium signal)
	};
}

#endif //CG_METABOLITE_HPP