#ifndef CG_PLASMIDS_HANDLER_HPP
#define CG_PLASMIDS_HANDLER_HPP

#include "ut/SortedVector.hpp" //eclipse times
#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports
#include <limits> //min() and max() in INI_EVENT_TIME and others */


namespace cg
{
    struct ReplicationHReportIdx
    {
        enum ReportTypeIdx : size_t { MUTATION_PROCESS, PARTITION, ORI_V, QPLASMID, COUNT };
    };

    class Genome;
    class CopyControl; //OriVReport
    class OriV; //model
    class PartitionSystem; //model
    class PlasmidBase; //model
    class PlasmidQuantitative; //storeEclipseMarkers()
    class Mutation; //
    class MutationProcess; //model
    class OriVReport;
    class PlasmidsHandler;

/////////////////////////////////////////////////////////////////////////// REPORTS /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class QPlasmidReport : public GeneticReport< PlasmidBase, PlasmidsHandler >
    /* Despite the name, BPlasmids have their report too to keep the ordering */
    {
        public:
            friend PlasmidsHandler;
            friend class OriVReport;
            friend class PartitionSystemReport;
            friend class MutationProcessReport;
            using EclipseTimesDS = ut::SimpleSortedVector< TReal, true >;

            static constexpr TReal INI_NEXT_ECLIPSE_TIME = -1.0;
            static constexpr size_t INI_NEXT_ECLIPSE_NUM = 0;

        //---ctor, dtor
            QPlasmidReport( const PlasmidBase* model, PlasmidsHandler* ownerHandler ) 
            : GeneticReport( model, ownerHandler ), eclipseTimes({}), nextEclipseTime( INI_NEXT_ECLIPSE_TIME ), nextEclipseNum( INI_NEXT_ECLIPSE_NUM ) {;}
            virtual ~QPlasmidReport() = default;

        //---API
          //common
            void link() {;}
            inline bool init() { exposeParams( true ); return true; }
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { eclipseTimes.clear(); nextEclipseTime = INI_NEXT_ECLIPSE_TIME; nextEclipseNum = INI_NEXT_ECLIPSE_NUM; }
            inline void partialReset() { nextEclipseTime = INI_NEXT_ECLIPSE_TIME; nextEclipseNum = INI_NEXT_ECLIPSE_NUM; }
            bool divisionOld( QPlasmidReport* daughter ); //from here only the BPlasmids; all the QPlasmids have a PartitionSystem
            inline bool divisionNew( QPlasmidReport* ) { return true; }
            bool update( const EventType* event ); //selects the method to call depending on the event
            inline bool tick() { return true; }
            void exposeParams( bool bInit );

          //special
            void addEclipseMarkers( int amount );
 

         private:
            EclipseTimesDS eclipseTimes; //sorted times for end of eclipse periods. Only the first one is adde to events
            TReal nextEclipseTime; //time of the next end of eclipse period event(s), already created. May not match eclipseTimes[0] because a sorter time was just added. This is the signal for updating the events. 
            uint nextEclipseNum; //number of end of eclipse period events with the same time. Required to know how many events to remove when a sorter time is added to eclipseTimes
            uint eclipseMarkers;

            bool updateAsSelected( const OriVReport* emitter ); //called when selected for replication from a OriV
            bool updateNoMutation( bool bEmitterHasEclipse, bool bRecipient ); //called when arrives by conjugation, replicated by OriT, already decided that there is no mutation

            void addEclipseTimes( size_t num = 2 ); //adds a given number of eclipse times and checks if an update is needed
            void updateEndOfEclipse(); //called when a end of eclipse event happens, to remove the time and create next events
            
            void createEclipseEvents(); //adds the events for end of eclipse period
            void removeEclipseEvents(); //removes the existing event
            void updateEclipseEvents(); //checks if the current eclipse event is the first time and modifies it if not

            void bPlasmidDivisionOld( QPlasmidReport* daughter ); //randomnly removes a BPlasmid from one of the daughter cells if loss
            void qPlasmidIndependentDivisionOld( QPlasmidReport* daughter ); //called from the PartitionSystemReport if false (random) or not shared with other plasmids
    };


    class OriVReport : public GeneticReport< OriV, PlasmidsHandler >
    {
        public:
            friend class PlasmidsHandler;
            friend class QPlasmidReport;
            using EclipseTimesDS = QPlasmidReport::EclipseTimesDS;

            static constexpr TReal INI_RATE = 0.0;
            static constexpr TReal INI_LAST_REPORTED_RATE = 0.0;
            static constexpr uint INI_ECLIPSE_MARKERS = 0;


        //---ctor, dtor
            OriVReport( const OriV* model, PlasmidsHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), rate( INI_RATE ), newRate( INI_RATE ), eclipseMarkers( INI_ECLIPSE_MARKERS ) {;}
            virtual ~OriVReport() = default;

        //---API
          //common
            void link();
            inline bool init() { calculateRate(); exposeParams( true ); return true; }
            inline bool secondInit() { return true; }
            inline void reset() { rate = INI_RATE; newRate = INI_RATE; eclipseMarkers = INI_ECLIPSE_MARKERS; }
            inline bool divisionOld( OriVReport* ) { return true; }
            inline bool divisionNew( OriVReport* ) { return true; }
            bool update( const EventType* event );
            inline bool tick() { return true; }
            void exposeParams( bool bInit );
          //special
            TReal accessRate() { rate = newRate; return rate; } //returns the last calculated value and updates rate with it
            bool updateRate(); //accessRate() + returns whether the change is greater than sensitivity
            inline void addEclipseMarkers( int amount ) { eclipseMarkers += amount; } 
            void exposeEclipseMarkers();


         private:
            std::vector<QPlasmidReport*> qplasmidReports;
            TReal rate; //the last accessed vale by Gillespie algorithm. Do not access directly, always via accessRate()
            TReal newRate; //the last calculated value. To compare with and update rate on access via accessRate()
            uint eclipseMarkers;

            bool updateAsSelected(); //response to replication events
            void calculateRate(); //updates newRate
    };


    class PartitionSystemReport : public GeneticReport< PartitionSystem, PlasmidsHandler >
    {
        public:
            friend class PlasmidsHandler;
            friend class QPlasmidReport;
            using BaseReportType = PartitionSystemReport::Report;
            using EclipseTimesDS = OriVReport::EclipseTimesDS;

            static constexpr uint INI_ECLIPSE_MARKERS = 0;


        //---ctor, dtor
            PartitionSystemReport( const PartitionSystem* model, PlasmidsHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), eclipseMarkers( INI_ECLIPSE_MARKERS ) {;}
            virtual ~PartitionSystemReport() = default;

        //---API
          //common
            void link();
            inline bool init() { exposeParams( true ); return true; }
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { eclipseMarkers = INI_ECLIPSE_MARKERS; }
            bool divisionOld( PartitionSystemReport* daughter );
            inline bool divisionNew( PartitionSystemReport* ) { return true; }
            bool update( const EventType* );
            inline bool tick() { return true; }
            void exposeParams( bool bInit );
          //special
            inline void addEclipseMarkers( int amount ) { eclipseMarkers += amount; } 
            void exposeEclipseMarkers();


         private:
            std::vector<QPlasmidReport*> qplasmidReports;
            uint eclipseMarkers;

            //apply the copy number split to the given daughter cell. From all the idxs of plasmid copies, this cell receivees from start to end
            void apply( const std::vector<size_t>& idxs, const std::vector<EclipseTimesDS>& eclipseTimesBaks, PartitionSystemReport* daughter, size_t start, size_t end ); 
    };


    class MutationProcessReport : public GeneticReport< MutationProcess, PlasmidsHandler >
    {
        public:
            friend class PlasmidsHandler;
            friend class QPlasmidReport;
            using BaseReportType = MutationProcessReport::Report;
            using EclipseTimesDS = QPlasmidReport::EclipseTimesDS;

            static constexpr TReal INI_RATE = 0.0;
            static constexpr TReal INI_LAST_REPORTED_RATE = 0.0;
            static constexpr size_t INI_MUTATION_IDX = std::numeric_limits<size_t>::max();

        //---ctor, dtor
            MutationProcessReport( const MutationProcess* model, PlasmidsHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), rate( INI_RATE ), newRate( INI_RATE ) {;}
            virtual ~MutationProcessReport() = default;

        //---API
          //common
            void link() {;}
            inline bool init() { calculateRate(); exposeParams( true ); return true; }
            inline bool secondInit() { return true; }
            inline void reset() { rate = INI_RATE; newRate = INI_RATE; }
            inline bool divisionOld( MutationProcessReport* ) { return true; }
            inline bool divisionNew( MutationProcessReport* ) { return true; }
            bool update( const EventType* event );
            inline bool tick() { return true; }
            void exposeParams( bool bInit );
          //special
            TReal accessRate() { if( model->getType() == MutationProcess::Type::SPONTANEOUS ) { rate = newRate; return rate; } return 0.0; } //returns the last calculated value and updates rate with it
            bool updateRate(); //accessRate() + returns whether the change is greater than sensitivity


         private:
         	TReal rate; //the last accessed vale by Gillespie algorithm. Do not access directly, always via accessRate()
            TReal newRate; //the last calculated value. To compare with and update rate on access via accessRate()

            void updateAsSelected(); //response to spontaneous mutation event or called from qplasmid to replication mutation
            void calculateRate(); //updates newRate
    };






/////////////////////////////////////////////////////////////////////////// HANDLER /////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class PlasmidsHandler : public GeneticHandler< ut::ReportsDS<MutationProcessReport>, ut::ReportsDS<PartitionSystemReport>, ut::ReportsDS<OriVReport>, ut::ReportsDS<QPlasmidReport> >
	{
		REGISTER_CLASS( "PlasmidsHandler", "repH", HandlerTypeIdx::H_PLASMIDS )

		public:
            using GeneticHandlerType = PlasmidsHandler::GeneticHandler;
            friend QPlasmidReport;
            friend OriVReport;
            friend MutationProcessReport;
            
            static constexpr TReal NULL_EVENT_TIME = -1.0;
            static constexpr TReal INI_EVENT_TIME = NULL_EVENT_TIME;
            static constexpr TReal MAX_TIME = 1000.0;

            inline static bool checkGillespieEvent( const EventType* ev ) { return ev->emitter && ( ev->emitter->getElemType() == ElemTypeIdx::ORI_V || ev->emitter->getElemType() == ElemTypeIdx::MUTATION_PROCESS ); }


		//---ctor, dtor
			PlasmidsHandler( OwnerType* owner, const GeneticCollection* collection ) : GeneticHandler( owner, collection ), nextEventTime( INI_EVENT_TIME ), selectedOriVReport( nullptr ), selectedMutationProcessReport( nullptr ) {;}
			virtual ~PlasmidsHandler() = default;
			PlasmidsHandler( const PlasmidsHandler& rhs ) = default;
			PlasmidsHandler& operator=( const PlasmidsHandler& rhs ) = default;

		//---get
            inline const std::vector<QPlasmidReport>& getQPlasmidReports() const { return std::get< ReplicationHReportIdx::QPLASMID >( reports ); }
			inline const std::vector<OriVReport>& getOriVReports() const { return std::get< ReplicationHReportIdx::ORI_V >( reports ); }
            inline const std::vector<PartitionSystemReport>& getPartitionSystemReports() const { return std::get< ReplicationHReportIdx::PARTITION >( reports ); }
            inline const std::vector<MutationProcessReport>& getMutationProcessReports() const { return std::get< ReplicationHReportIdx::MUTATION_PROCESS >( reports ); }
            
            inline std::vector<QPlasmidReport>& getQPlasmidReportsEdit() { return std::get< ReplicationHReportIdx::QPLASMID >( reports ); }
            inline std::vector<OriVReport>& getOriVReportsEdit() { return std::get< ReplicationHReportIdx::ORI_V >( reports ); }
            inline std::vector<PartitionSystemReport>& getPartitionSystemReportsEdit() { return std::get< ReplicationHReportIdx::PARTITION >( reports ); }
            inline std::vector<MutationProcessReport>& getMutationProcessReportsEdit() { return std::get< ReplicationHReportIdx::MUTATION_PROCESS >( reports ); }

		//---API
          //common
            void reset() { nextEventTime = INI_EVENT_TIME; selectedOriVReport = nullptr; selectedMutationProcessReport = nullptr; }
            void partialReset() { selectedOriVReport = nullptr; selectedMutationProcessReport = nullptr; }
            bool init() override;
            bool secondInit( ut::MultiBitset*, ut::Bitset* ) override;
            bool divisionOld( HandlerBaseType* daughter ) override;
            bool divisionNew( HandlerBaseType* mother ) override;
            bool update( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask, const EventType* event ) override; //removes changes, forwards CopyControls and updates the Gillespie event if needed

          //special
            void applyMutation( const Mutation* mutation, bool bCallerHasEclipse ); //apply replication mutation from conjugation
            //add plasmid from conjugation, already decided that there is no replication mutation from the OriT, but may be eclipse period
            inline void applyPlasmid( const PlasmidBase* plasmid, bool bCallerHasEclipse, bool bRecipient ) { getQPlasmidReportsEdit()[ plasmid->getRelativeId() ].updateNoMutation( bCallerHasEclipse, bRecipient ); }
            void storeEclipseMarkers( const PlasmidQuantitative* plasmid, int amount );


		private:
	        TReal nextEventTime; //time of next Gillespie event. Matches an event
	        OriVReport* selectedOriVReport; //in case the selected Gillespie event is replication, null otherwise
            MutationProcessReport* selectedMutationProcessReport; //in case the selected Gillespie event is mutation, null otherwise


            bool selectNextEvent( bool bRequired ); //if bRequired or enough change in rates, removes the previous Gillespie event and samples a new one
            bool checkRateChange(); //check if any OriV or MutationProcess changes enough to re-sample the gillespie event
            bool findNextEventGillespie( bool bAlreadyUpdated ); //Gillespie algorithm, samples next event time and emitter element using the rates of OriVs and MutationProcess
            bool removeCurrentEvent(); //removes the existing Gillespie event
            
            bool checkChanges(); //check if there are changes affecting the Gillespie events i.e. OriVs or MutationProcess
            void forawardCopyControls(); //forward the CopyControl changes to their OriVs (canonical rate calculation) and updates active marker*
            
            void exposeEclipseMarkers();
	};
}

#endif //CG_PLASMIDS_HANDLER_HPP