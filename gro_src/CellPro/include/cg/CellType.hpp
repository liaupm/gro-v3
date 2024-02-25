#ifndef CG_CELLTYPE_HPP
#define CG_CELLTYPE_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //qPlasmidAmounts

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <vector> //bPlasmids, qPlasmids, qPlasmidAmounts, molecules */


namespace cg
{
	class GeneticCollection;
	class Strain;
	class Molecule;
	class PlasmidBase;
	class CellType : public GeneticElement
	/* The combination of a Strain with the initial elements present in a newly created cell. Fixed, cannot change during the simulation (can be used as a cell type/role id) */
	{
		REGISTER_CLASS( "CellType", "ctype", GeneticElement::ElemTypeIdx::CELL_TYPE, false )

		public:
			inline static const std::vector<const Molecule*> DF_MOLECULES = {};
			inline static const std::vector<ut::ContinuousDistData> DF_QPLASMID_AMOUNTS = {};
			inline static const std::vector<const PlasmidBase*> DF_QPLASMIDS = {};
			static constexpr bool DF_B_QPLASMIDS_AS_CONC = true;


		//---ctor, dtor
			CellType( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const Strain* strain, const std::vector<const PlasmidBase*>& bPlasmids, const std::vector<const Molecule*>& molecules = DF_MOLECULES
			, const std::vector<const PlasmidBase*>& qPlasmids = DF_QPLASMIDS, const std::vector< ut::ContinuousDistData >& qPlasmidAmountsData = DF_QPLASMID_AMOUNTS, bool bQPlasmidsAsConc = DF_B_QPLASMIDS_AS_CONC );
			
			virtual ~CellType() = default;

		//---get
			inline const Strain* getStrain() const { return strain; }
			inline const std::vector<const PlasmidBase*>& getBPlasmids() const { return bPlasmids; }
			inline size_t getBPlasmidNum() const { return bPlasmids.size(); }
			inline const std::vector<const Molecule*>& getMolecules() const { return molecules; }
			inline size_t getMoleculeNum() const { return molecules.size(); }
			
			inline const std::vector<const PlasmidBase*>& getQPlasmids() const { return qPlasmids; }
			inline size_t getQPlasmidNum() const { return qPlasmids.size(); }
			inline bool getBQPlasmidsAsConc() const { return bQPlasmidsAsConc; }
		
		//---API
		  //precompute
			void loadDirectElemLink() override;
		  //run
			uint sampleAmount( size_t idx, TReal vol = 1.0 ) const; //sample initial amount of one qplasmid
			std::vector<uint> sampleAmounts( TReal vol = 1.0 ) const; //sample initial amount of all the qplasmids


		private:
			const Strain* strain;
			std::vector<const PlasmidBase*> bPlasmids; //initial bplasmids
			std::vector<const Molecule*> molecules; //initial molecules

			std::vector<const PlasmidBase*> qPlasmids; //initial qplasmids
			std::vector< SP<ut::ContinuousDistCombi> > qPlasmidAmounts; //initial amounts of the qplasmids
			bool bQPlasmidsAsConc; //whether qPlasmidAmounts are concentrations (multiplied by the initial volume of the cell) or not (absolute amount, independent of volume)
	};
}

#endif //CG_CELLTYPE_HPP