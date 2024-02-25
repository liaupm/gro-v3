#ifndef CG_PLASMIDQUANTITATIVE_HPP
#define CG_PLASMIDQUANTITATIVE_HPP

#include "ut/DistributionCombi.hpp" //eclipseDist

#include "cg/defines.hpp"
#include "cg/PlasmidBase.hpp" //base class

/*PRECOMPILED
#include <vector> //oriVs, copyControls
#include <string> //name in constructor */


namespace cg
{
	class GeneticCollection;
	class QMarker; //eclipseMarker
	class CopyControl;
	class OriV;
	class PartitionSystem;
	class MutationProcess; //ctor, repliMutations to pass to PlasmidBase

	class PlasmidQuantitative : public PlasmidBase
	/*A plasmid or DNA mlecule with explicit copy number, replication and split on cell division */
	{
		public:
			inline static const GeneticElement* DF_ECLIPSE_FUN = nullptr;
			inline static const QMarker* DF_ECLIPSE_MARKER = nullptr;

			static constexpr double DF_COPY_NUM_MEAN = 1.0;
            static constexpr double DF_COPY_NUM_VAR = 0.0;
            static constexpr ut::ContinuousDist::DistributionType DF_COPY_NUM_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
			inline static const std::vector<TReal> DF_COPY_NUM_PARAMS = { DF_COPY_NUM_MEAN, DF_COPY_NUM_VAR };
			inline static const ut::ContinuousDistData DF_COPY_NUM_DIST = ut::ContinuousDistData( DF_COPY_NUM_DISTTYPE, DF_COPY_NUM_PARAMS );

			static constexpr double DF_ECLIPSE_MEAN = 0.0;
			static constexpr double DF_ECLIPSE_VAR = 0.0;
			static constexpr ut::ContinuousDist::DistributionType DF_ECLIPSE_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
			inline static const std::vector<TReal> DF_ECLIPSE_PARAMS = { DF_ECLIPSE_MEAN, DF_ECLIPSE_VAR };
			inline static const ut::ContinuousDistData DF_ECLIPSE_DIST = ut::ContinuousDistData( DF_ECLIPSE_DISTTYPE, DF_ECLIPSE_PARAMS );


		//---ctor, dtor
			PlasmidQuantitative( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const std::vector<const Operon*>& operons, const std::vector<const OriT*>& oriTs, const std::vector<const OriV*>& oriVs, const std::vector<const CopyControl*>& copyControls, const PartitionSystem* partitionSystem, const std::vector<const MutationProcess*>& repliMutations
			, const GeneticElement* eclipseFunction = DF_ECLIPSE_FUN, const QMarker* eclipseMarker = DF_ECLIPSE_MARKER, const ut::ContinuousDistData& copyNumber = DF_COPY_NUM_DIST, const ut::ContinuousDistData& eclipse = DF_ECLIPSE_DIST, const std::vector<size_t>& repliMutationKeys = PlasmidBase::DF_REPLI_MUT_KEYS );

			virtual ~PlasmidQuantitative() = default;

		//---get
		  //resources
			inline const std::vector<const OriV*>& getOriVs() const { return oriVs; }
			inline const std::vector<const CopyControl*>& getCopyControls() const { return copyControls; }
			inline const PartitionSystem* getPartitionSystem() const { return partitionSystem; }
			inline const GeneticElement* getEclipseFunction() const { return eclipseFunction; }
			inline const QMarker* getEclipseMarker() const { return eclipseMarker; }
			inline bool getHasEclipseMarker() const { return eclipseMarker; }
		  //options
			inline bool getHasEclipse() const { return bHasEclipse; }
			inline const ut::ContinuousDistData& getCopyNumber() const { return copyNumber; }

		//---set
			inline void setAll( const GeneticElement* xEclipseFunction ) { setEclipseFunction( xEclipseFunction ); }
			void setEclipseFunction( const GeneticElement* xEclipseFunction );

		//---API
		  //precompute
			void loadDirectElemLink() override;
		  //run
			inline TReal calculateEclipse( TReal funVal ) const { return eclipseFunction ? ut::fitL( funVal ) + sampleEclipse() : sampleEclipse(); }
			inline TReal sampleEclipse() const { return ut::fitL( eclipseDist.sample() ); } //non-negative


		private:
		  //resources
			std::vector<const OriV*> oriVs;
			std::vector<const CopyControl*> copyControls;
			const PartitionSystem* partitionSystem;
			const GeneticElement* eclipseFunction;
			const QMarker* eclipseMarker; //optional, exposes the number of copies of the plasmid that are in eclipse period. To be used for strong copy number conditions
		  //options
			bool bHasEclipse;
		  //dists
			ut::ContinuousDistData copyNumber; //distribution data for the initial copy numbers in newly created cells
			mutable ut::ContinuousDistCombi eclipseDist; //to sample eclipse times
	};

	using QPlasmid = PlasmidQuantitative;
}

#endif //CG_PLASMIDQUANTITATIVE_HPP