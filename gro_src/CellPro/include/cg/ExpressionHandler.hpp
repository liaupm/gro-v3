#ifndef CG_EXPRESSIONHANDLER_HPP
#define CG_EXPRESSIONHANDLER_HPP

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports */


namespace cg
{
    struct ExpressionHReportIdx
    {
        enum ReportTypeIdx : size_t { MOLECULE, OPERON, REGULATION, COUNT };
    };

    class Genome; //owner
    class Regulation; //model
    class Operon; //model
    class Molecule; //model
    class FluxReport; //std::vector<const FluxReport*> metSourceReports of MoleculeReport
    class ExpressionHandler;

/////////////////////////////////////////////////////////////// REPORTS //////////////////////////////////////////////////////////////////////////

    class RegulationReport : public GeneticReport< Regulation, ExpressionHandler >
    {
        public:
            friend ExpressionHandler;
            friend class OperonReport;
            static constexpr TReal INI_ACTIVATION = 0.0;

        //---ctor, dtor
            RegulationReport( const Regulation* model, ExpressionHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), activationPerCopy( INI_ACTIVATION ) {;}
            virtual ~RegulationReport() = default;

        //---get
            inline TReal getActivationPerCopy() const { return activationPerCopy; }

        //---API
          //common
            void link() {;}
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { activationPerCopy = INI_ACTIVATION; }
            //nothing is done directly. If the copy number (or even the presence) is modified due to asymmetry in the split of plasmid copies, this is done 
            //in the divisionOld() of the PlasmidsHandler and then update() is called after if the number of Regulations is modified
            inline bool divisionOld( RegulationReport* ) { return true; } 
            inline bool divisionNew( RegulationReport* ) { return true; }
            bool update( const EventType* );
            inline bool tick() { return true; }
            

        private:
            TReal activationPerCopy; //activation (total if the operon doen't have dosage effect and per copy if it does)
    };


    class OperonReport : public GeneticReport< Operon, ExpressionHandler >
    {
        public:
            friend class ExpressionHandler;
            friend class MoleculeReport;
            static constexpr TReal INI_ACTIVATION = 0.0;

        //---ctor, dtor
            OperonReport( const Operon* model, ExpressionHandler* ownerHandler )
            : GeneticReport( model, ownerHandler ), regReport( nullptr ), activation( INI_ACTIVATION ) {;}

            virtual ~OperonReport() = default;

        //---get
            inline TReal getActivation() const { return activation; }

        //---API
            void link();
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { activation = INI_ACTIVATION; }
            inline bool divisionOld( OperonReport* ) { return true; }
            inline bool divisionNew( OperonReport* ) { return true; }
            bool update( const EventType* );
            inline bool tick() { return true; }


        private:
            const RegulationReport* regReport;
            TReal activation; //total activation of all the copies of this operon, after optional dosage effects
    };


    class MoleculeReport : public GeneticReport< Molecule, ExpressionHandler >
    {
        public:
            friend class ExpressionHandler;
            static constexpr TReal INI_ACTIVATION = 0.0;
            static constexpr int INI_IDX = -1;
            static constexpr Molecule::ExpressionDirection INI_DIRECTION = Molecule::ExpressionDirection::DIR_NONE;
            static constexpr TReal INI_WHOLE_TIME = -1.0;
            static constexpr TReal INI_EVENT_TIME = -1.0;

            inline static bool checkDirection( bool state, Molecule::ExpressionDirection dir ) { return ( state && dir == Molecule::DIR_OFF ) || ( ! state && dir == Molecule::DIR_ON ); } //check that the expression direction is the opposite of the molecule state
            

        //---ctor, dtor
            MoleculeReport( const Molecule* model, ExpressionHandler* ownerHandler )
            : GeneticReport( model, ownerHandler )
            , sourceReports( {} ), metSourceReports( {} )
            , activation( INI_ACTIVATION ), expressionIdx( INI_IDX ), direction( INI_DIRECTION ), wholeTime( INI_WHOLE_TIME ), eventTime( INI_EVENT_TIME ) {;}

            virtual ~MoleculeReport() = default;

        //---get 
            inline TReal getActivation() const { return activation; }
        
        //---API
          //common
            void link();
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { activation = INI_ACTIVATION; expressionIdx = INI_IDX; direction = INI_DIRECTION; eventTime = INI_EVENT_TIME; wholeTime = INI_WHOLE_TIME; }
            inline bool divisionOld( MoleculeReport* ) { return true; }
            bool divisionNew( MoleculeReport* );
            bool update( const EventType* );
            inline bool tick() { return true; }


        private:
          //resources
            std::vector<const OperonReport*> sourceReports;
            std::vector<const FluxReport*> metSourceReports;
          //state
            TReal activation; //total activation, collected from operons + fluxes
            int expressionIdx; //current index of the expression time distribution the activation maps to
            Molecule::ExpressionDirection direction; //synthesis or degradation
            TReal wholeTime; //used to scale the remaining time in case the expression speed changes before completion (but not the direction)
            TReal eventTime; //end of the sysntehsis/degradation time in the current conditions. Matchces an event.

        //---fun
            void collectActivation(); //adds up the activation from operons and fluxes into activation
            bool updateOn2on(); //change in time but not in direction -> scale remaining time
            bool updateOn2Off( Molecule::ExpressionDirection newDirection ); //from ongoing time (synthesis or degradation) to inactive
            bool updateOff2On( Molecule::ExpressionDirection newDirection ); //start time (synthesis or degradation) from inactive
    };



/////////////////////////////////////////////////////////////// HANDLER //////////////////////////////////////////////////////////////////////////

    class ExpressionHandler : public GeneticHandler< ut::ReportsDS<MoleculeReport>, ut::ReportsDS<OperonReport>, ut::ReportsDS<RegulationReport> >
	{
        REGISTER_CLASS( "ExpressionHandler", "expH", GeneticHandler<>::HandlerTypeIdx::H_EXPRESSION )

		public:
		//---ctor, dtor
			ExpressionHandler( OwnerType* owner, const GeneticCollection* collection ) : GeneticHandler( owner, collection ) {;}
            //ExpressionHandler( const ExpressionHandler& original, OwnerType* owner ) : GeneticHandler( original, owner ) {;}
			virtual ~ExpressionHandler() = default;
			ExpressionHandler( const ExpressionHandler& rhs ) = default;
			ExpressionHandler& operator=( const ExpressionHandler& rhs ) = default;
            ExpressionHandler( ExpressionHandler&& rhs ) noexcept = default;
            ExpressionHandler& operator=( ExpressionHandler&& rhs ) noexcept = default;

		//---get
            inline const std::vector<RegulationReport>& getRegulationReports() const { return std::get<ExpressionHReportIdx::REGULATION>( reports ); }
            inline const std::vector<OperonReport>& getOperonReports() const { return std::get<ExpressionHReportIdx::OPERON>( reports ); }
            inline const std::vector<MoleculeReport>& getMoleculeReports() const { return std::get<ExpressionHReportIdx::MOLECULE>( reports ); }

		//---set

		//---API
	};
}

#endif //CG_EXPRESSIONHANDLER_HPP