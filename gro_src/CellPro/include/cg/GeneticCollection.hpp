#ifndef CG_GENETIC_COLLECTION_HPP
#define CG_GENETIC_COLLECTION_HPP

#include "ut/CollectionManager.hpp" //base class
#include "ut/DistributionCombi.hpp" //Gillespie dists

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp"

#include "cg/Marker.hpp"
#include "cg/Randomness.hpp"
#include "cg/Function.hpp"
#include "cg/GateBase.hpp"
#include "cg/GateBoolean.hpp"
#include "cg/GateQuantitative.hpp"
#include "cg/Ode.hpp"
#include "cg/Historic.hpp"
#include "cg/CellColour.hpp"

#include "cg/Molecule.hpp"
#include "cg/Regulation.hpp"
#include "cg/Operon.hpp"
#include "cg/Metabolite.hpp"
#include "cg/Flux.hpp"

#include "cg/Pilus.hpp"
#include "cg/OriT.hpp"
#include "cg/PartitionSystem.hpp"
#include "cg/CopyControl.hpp"
#include "cg/OriV.hpp"

#include "cg/PlasmidBase.hpp"
#include "cg/PlasmidBoolean.hpp"
#include "cg/PlasmidQuantitative.hpp"
#include "cg/Mutation.hpp"
#include "cg/MutationProcess.hpp"
#include "cg/Strain.hpp"
#include "cg/CellType.hpp"

#include "cg/HorizontalCommon.hpp"

/*PRECOMPILED
#include <memory> //smart pointers SP<> seedGenerator */


namespace cg
{
      class Genome; 
      class GeneticCollection : public ut::CollectionManager< GeneticCollection, GeneticElement
                                                            , BMarker, QMarker, EventMarker, Randomness, Function, GateBase, Ode, Historic, CellColour
                                                            , Molecule, Regulation, Operon, Metabolite, Flux
                                                            , Pilus, OriT, PartitionSystem, CopyControl, OriV
                                                            , PlasmidBase, Mutation, MutationProcess, Strain, CellType
                                                            , HorizontalCommon >
      {
            public:
				using CollectionManagerBaseType = GeneticCollection::CollectionManager;
				static constexpr TReal DF_SENSITIVITY = 0.01;

			//---ctor, dtor
				GeneticCollection( SP<ut::SeedGeneratorBase> seedGen );
				virtual ~GeneticCollection() = default;

      		//---get
      			inline ut::SeedGeneratorBase* getSeedGenerator() const { return seedGenerator.get(); }
      			inline const Metabolite* getBiomass() const { return biomass; } 
      			inline TReal getSensitivity() const { return sensitivity; }
      			inline bool getBGillespie() const { return bGillespie; }

				inline const QMarker* getVolMarker() const { return volMarker; }
				inline const QMarker* getDvolMarker() const { return dvolMarker; }
				inline const QMarker* getGrMarker() const { return grMarker; }
				inline const BMarker* getDivisionVolBMarker() const { return divisionVolBMarker; }
				inline const QMarker* getNeighDistanceMarker() const { return neighDistanceMarker; }
				inline const EventMarker* getDivisionEventMarker() const { return divisionEventMarker; }
				inline const QMarker* getPoleCountMarker() const { return poleCountMarker; }
				inline const BMarker* getPolePositionMarker() const { return polePositionMarker; }

            //---set
				inline void setSensitivity( TReal xSensitivity ) { sensitivity = xSensitivity; }
				inline void setVolMarker( const QMarker* xVolMarker ) { volMarker = xVolMarker; }
				inline void setDvolMarker( const QMarker* xDvolMarker ) { dvolMarker = xDvolMarker; }
				inline void setGrMarker( const QMarker* xGrMarker ) { grMarker = xGrMarker; }
				inline void setDivisionVolBMarker( const BMarker* xDivisionVolBMarker ) { divisionVolBMarker = xDivisionVolBMarker; }
				inline void setNeighDistanceMarker( const QMarker* xNeighDistanceMarker ) { neighDistanceMarker = xNeighDistanceMarker; }
				inline void setDivisionEventMarker( const EventMarker* xDivisionEventMarker ) { divisionEventMarker = xDivisionEventMarker; }
				inline void setPoleCountMarker( const QMarker* xPoleCountMarker ) { poleCountMarker = xPoleCountMarker; }
				inline void setPolePositionMarker( const BMarker* xPolePositionMarker ) { polePositionMarker = xPolePositionMarker; }
                 
      		//---API
			  //precompute
				void init() override { calculateBGillespie(); rankAndSort(); CollectionManagerBaseType::init(); } 
				void calculateBGillespie(); //calculates bGillespie
				void rankAndSort(); //sorts the gates and the functions by their within-group dependencies to increase efficiency on evaluation
			  //run
				inline TReal sampleGillespieTimeDist( TReal rate ) const { return ut::fitL( gillespieTimeDist.sample( rate ) ); } //fit just in case
				inline size_t sampleGillespieSelectDist( const ut::DiscreteDist::RealParamsType& params ) const { return gillespieSelectDist.sample( params ); }
				inline bool samplePolePosDist() const { return iniPolePosDist.sample(); }


            private:
				SP<ut::SeedGeneratorBase> seedGenerator; //master for all the generators in cg
				const Metabolite* biomass; //the Metabolite that represents the total biomass flux
				TReal sensitivity; //relative change, used to forward cell growth related values and to re-sample Gillespie event
				bool bGillespie; //whether the Gillespie algorithm is used: there are at least one OriV and/or spontaneous MutationProcess
			  //markers
				const QMarker* volMarker; //volume
				const QMarker* dvolMarker; //dvolume
				const QMarker* grMarker; //growth rate
				const BMarker* divisionVolBMarker; //whether division size reached
				const QMarker* neighDistanceMarker; //
				const EventMarker* divisionEventMarker; //in the time step of division
				const QMarker* poleCountMarker; //age of the oldest pole
				const BMarker* polePositionMarker; //position of the newest pole with respect to the cell's body
			  //dists
				mutable ut::ConcDistCombi< ut::ExponentialDist > gillespieTimeDist; //waiting time dist for reactions using the Gillespie algorithm. Curretly, PlasmidsHandler
				mutable ut::DiscreteDistCombi< ut::DiscreteDist > gillespieSelectDist; //reaction selector for the Gillespie algorithm
				mutable ut::ConcDistCombi< ut::DiscreteUniformDist > iniPolePosDist; //initial pole orientation
      };
}


#endif //CG_GENETIC_COLLECTION_HPP
