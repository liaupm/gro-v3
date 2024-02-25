#ifndef CG_HORIZONTALHANDLER_HPP
#define CG_HORIZONTALHANDLER_HPP

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports */


namespace cg
{
    struct HorizontalHReportIdx
    {
        enum ReportTypeIdx : size_t { PILUS, ORI_T, COUNT };
    };
    
	class Genome;
	class PlasmidBase; //model, selectPlasmid()
	class Pilus; //model
	class OriT; //model
    class HorizontalHandler;


//////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////

    class OriTReport : public GeneticReport< OriT, HorizontalHandler >
    {
        public:
            friend HorizontalHandler;
            friend class PilusReport;
            static constexpr bool INI_BACTIVE = false;
        
        //---ctor, dtor
            OriTReport( const OriT* model, HorizontalHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), bActive( INI_BACTIVE ) {;}
            virtual ~OriTReport() = default;

        //---get 
            inline bool getBActive() const { return bActive; }
            inline bool getIsActive() const { return bActive; }

        //---API
          //common
            void link() {;}
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            void reset() { bActive = INI_BACTIVE; }
            bool divisionOld( OriTReport* ) { return true; }
            inline bool divisionNew( OriTReport* ) { return true; }
            bool update( const EventType* );
            inline bool tick() { return true; }
            void exposeParams( bool bInit );


        private: 
            bool bActive; //donorGate true AND OriT present

            const PlasmidBase* selectPlasmid(); //selects the plasmid to conjugate from those which bear the oriT and are present
            size_t selectMutation( const std::vector<size_t>& keys ); //whether a replication mutation occurs on conjugation
    };


    class PilusReport : public GeneticReport< Pilus, HorizontalHandler >
    {
        public:
            friend class HorizontalHandler;
            friend OriTReport;

           
        //---ctor, dtor
            PilusReport( const Pilus* model, HorizontalHandler* ownerHandler ) : GeneticReport( model, ownerHandler ) {;}
            virtual ~PilusReport() = default;

        //---API
          //common
            void link();
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            void reset() {;}
            inline bool divisionOld( PilusReport* ) { return true; }
            inline bool divisionNew( PilusReport* ) { return true; }
            bool update( const EventType* );
            inline bool tick() { return true; }
            void exposeParams( bool bInit );
          //special
            bool updateExternal( const std::vector<Genome*>& neighbours, const std::vector<TReal>& distances ); //calculate whether conjugation and select recipient, oriT and plasmid and transmit


        private:    
            std::vector<OriTReport*> oriTReports; //not const because they are to be modified from here

            bool checkState() const; //DonorGate AND any compatible OriT present and active
            TReal calculateDonorRate(); //calculate the base conjugation rate considering only the donor. May then be scaled by the potential recipients
            std::vector<size_t> filterNeighbours( const std::vector<Genome*>& neighbours ); //filters the neighbours by the recipient gate (possible recipients)
            ut::DiscreteDist::ParamsType calculateRecipientWeights( const std::vector<Genome*>& neighbours, const std::vector<size_t>& validNeighbourIdxs, const std::vector<TReal>& distances );
            
            void transmitPlasmid( Genome* recipient ); //select oriT and specific plasmid and add the conjugation to the recipient (changes are not commited until the recipient validates it)
            OriTReport* selectOriT(); //selects one oriT from the compatible ones that are present and active, potentially taking the copy number into account
    };



//////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////

	class HorizontalHandler : public GeneticHandler<ut::ReportsDS<PilusReport>, ut::ReportsDS<OriTReport> >
	{
		REGISTER_CLASS( "HorizontalHandler", "hzH", GeneticHandler<>::HandlerTypeIdx::H_HORIZONTAL )

		public:
			friend OriTReport;
            friend PilusReport;


			struct ConjugationData
			{
				Genome* donor; //ensured that the pointer is valid as death round is performed after conjugation commit round for all the cells
				const Pilus* pilus;
				const OriT* oriT;
				const PlasmidBase* plasmid;
				const MutationProcess* mutationProcess; //nullptr if no mutation

				ConjugationData( Genome* donor, const Pilus* pilus, const OriT* oriT, const PlasmidBase* plasmid, const MutationProcess* mutationProcess )
				: donor( donor ), pilus( pilus ), oriT( oriT ), plasmid( plasmid ), mutationProcess( mutationProcess ) {;}

				~ConjugationData() = default;
			};

			using PendingConjugationsDS = std::vector<ConjugationData>;


		//---ctor, dtor
			HorizontalHandler( OwnerType* owner, const GeneticCollection* collection  ) : GeneticHandler( owner, collection ) {;}
            //HorizontalHandler( const HorizontalHandler& original, OwnerType* owner ) : GeneticHandler( original, owner ) {;}
            virtual ~HorizontalHandler() = default;
			HorizontalHandler( const HorizontalHandler& rhs ) = default;
			HorizontalHandler& operator=( const HorizontalHandler& rhs ) = default;

		//---get
            inline const std::vector<OriTReport>& getOriTReports() const { return std::get< HorizontalHReportIdx::ORI_T >( reports ); }
            inline std::vector<OriTReport>& getOriTReportsEdit() { return std::get< HorizontalHReportIdx::ORI_T >( reports ); }
            inline const std::vector<PilusReport>& getPilusReports() const { return std::get< HorizontalHReportIdx::PILUS >( reports ); }
            inline std::vector<PilusReport>& getPilusReportsEdit() { return std::get< HorizontalHReportIdx::PILUS >( reports ); }

            inline bool getHasPendingConjugations() const { return pendingConjugations.size() > 0; }

		//---API
          //special
            bool checkActivePili() const; //check if there is any active PilusReport. External call
            bool checkActivePiliDistance() const; //check if there is any active PilusReport that requires the distance to potential recipients as an input, to avoid calculating it if not (efficiency). External call
			inline void updateExternal( const std::vector<Genome*>& neighbours, const std::vector<TReal>& distances = {} ) { for( auto& pilusReport : getPilusReportsEdit() ) pilusReport.updateExternal( neighbours, distances ); }

			//called from donor, to add conjugation proposals to pendingConjugations
			inline void addConjugation( const ConjugationData& newConjugation ) { pendingConjugations.push_back( newConjugation ); }
			inline void addConjugation( Genome* donor, const Pilus* pilus, const OriT* oriT, const PlasmidBase* plasmid, const MutationProcess* mutationProcess ) { pendingConjugations.push_back( ConjugationData( donor, pilus, oriT, plasmid, mutationProcess ) ); }

			void selectAndApplyConjugations(); //select which of the pendingConjugations to commit in case they are incompatible; commit all otherwhise
			void applyConjugation( const ConjugationData& conj ); //commit one conjugation, affecting donor and this (recipient)


		private:
			PendingConjugationsDS pendingConjugations; //conjugations received during the same time step. If they are incompatible, the ones to commit are selected randomnly; if compatible, all commited
	};
}

#endif //CG_HORIZONTALHANDLER_HPP