#ifndef CG_MUTATION_HPP
#define CG_MUTATION_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <vector> //plasmids, amounts
#include <string> //name */


namespace cg
{
	class GeneticCollection;
	class QMarker; //eventMarker
	class PlasmidBase; //plasmids
	class PlasmidBoolean; //bplasmids
	class PlasmidQuantitative; //qplasmdids
	class Mutation : public GeneticElement
	/* Description of just the stoichiometry of a reaction between Plasmids that removes some and creates others. Used for mutations, editing, dimerization, etc. 
	Used with MutationProcess */
	{
		REGISTER_CLASS( "Mutation", "mut", GeneticElement::ElemTypeIdx::MUTATION, true )
		
		public:
			inline static const QMarker* DF_EVENT_MARKER = nullptr;

		//---ctor, dtor
			Mutation( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const std::vector<const PlasmidBase*>& plasmids, const std::vector<int>& amounts, const QMarker* eventMarker = DF_EVENT_MARKER );

			virtual ~Mutation() = default;

		//---get
		  //resources
			inline const std::vector<const PlasmidBase*>& getPlasmids() const { return plasmids; }
			inline const std::vector<const PlasmidBoolean*>& getBPlasmids() const { return bplasmids; }
			inline const std::vector<const PlasmidQuantitative*>& getQPlasmids() const { return qplasmids; }
			inline const QMarker* getEventMarker() const { return eventMarker; }
		  //options
			inline const std::vector<int>& getAmounts() const { return amounts; }
			inline const std::vector<int>& getBAmounts() const { return bamounts; }
			inline const std::vector<int>& getQAmounts() const { return qamounts; }
			inline bool getHasBPlasmids() const { return bplasmids.size() > 0; }
			inline bool getHasQPlasmids() const { return qplasmids.size() > 0; }
			inline bool getBNull() const { return bNull; }
			inline bool getIsNull() const { return bNull; }


		private:
		  //resources
			std::vector<const PlasmidBase*> plasmids; //all the affected plasmids, both bplasmids and qplasmids
			std::vector<const PlasmidBoolean*> bplasmids; //bplasmids only, from plasmids
			std::vector<const PlasmidQuantitative*> qplasmids; //qplasmids only, from plasmids
			const QMarker* eventMarker;
		  //options
			std::vector<int> amounts; //stoichiometry of plasmids
			std::vector<int> bamounts; //stoichiometry of bplasmids
			std::vector<int> qamounts; //stoichiometry of qlasmids
			bool bNull; //true if plasmids is empty
	};
}

#endif //CG_MUTATION_HPP