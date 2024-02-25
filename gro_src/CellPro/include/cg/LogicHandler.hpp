#ifndef CG_LOGICHANDLER_HPP
#define CG_LOGICHANDLER_HPP

#include "ut/Bitset.hpp"
#include "ut/MultiBitset.hpp"
#include "ut/ExtendedVector.hpp"

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base


namespace cg
{
	struct LogicHReportIdx
    {
    	enum ReportTypeIdx : size_t { ODE, HISTORIC, COUNT };
    };

    class Genome;
    class Function;
    class Ode;
    class Historic;
    class GateBase;
    class LogicHandler;


/////////////////////////////////////////////////////////////// REPORTS //////////////////////////////////////////////////////////////////////////

    class HistoricReport : public GeneticReport< Historic, LogicHandler >
    {
        public:
        	friend LogicHandler;

        	struct FutureValue
        	{
        		TReal value;
        		TReal time;

        		FutureValue( TReal value, TReal time ) : value( value ), time( time ) {;}
        	};
        	using FutureValuesDS = std::vector<FutureValue>;
            static constexpr TReal INI_VAL = 0.0;
		
		//---ctor, dtor
            HistoricReport( const Historic* model, LogicHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), value( INI_VAL ) {;}
            virtual ~HistoricReport() = default;

        //---get
            inline TReal getValue() const { return value; }

        //---API
          //common
            void link() {;}
            bool init(); //set the initial value
            bool secondInit();
            inline void reset() { value = INI_VAL; futureValues.clear(); }
            inline void partialReset() { value = INI_VAL; }
            inline bool divisionOld( HistoricReport* ) { return true; }
            bool divisionNew( HistoricReport* ); //create the value update event because events are not copied
            bool update( const EventType* event ); //update the value or store future values if the target changes
			inline bool tick() { return true; }

	
        private:
        	FutureValuesDS futureValues; //the next values, from times between t and t - model->delay
        	TReal value; //current value of the time that matches t - model->delay

        	void storeValue(); //pushes a value to futureValues
    };


    class OdeReport : public GeneticReport< Ode, LogicHandler >
    {
        public:
        	friend class LogicHandler;
            static constexpr TReal INI_DELTA = 0.0;
            static constexpr TReal INI_LAST_VAL = 0.0;
            static constexpr TReal INI_LAST_UPDATE_TIME = 0.0;
		
		//---ctor, dtor
            OdeReport( const Ode* model, LogicHandler* ownerHandler );
            virtual ~OdeReport() = default;

        //---get
            inline TReal getDelta() const { return delta; }
            inline TReal getLastVal() const { return lastVal; }
            inline TReal getLastUpdateTime() const { return lastUpdateTime; }

        //---API
          //common
            void link() {;}
            bool init(); //set the initial integral value (qstate) and derivative (lastVal)
            bool secondInit();
            void reset();
            void partialReset();
            bool divisionOld( OdeReport* daughter ); //partition between daughter cells
            inline bool divisionNew( OdeReport* ) { return true; }
            bool update( const EventType* ); //in case the target changes, accumulate the delta and update the derivative
			bool tick(); //integration. The delta is added to the qstate and reset
			void exposeParams( bool bInit );


        private:
        	TReal delta; //accumulated during the time step, with potentially different derivatives
        	TReal lastVal; //current derivative
        	TReal lastUpdateTime; //used to average the value when changing in between time steps
    };



/////////////////////////////////////////////////////////////// HANDLER //////////////////////////////////////////////////////////////////////////

	class LogicHandler : public GeneticHandler< ut::ReportsDS<OdeReport>, ut::ReportsDS<HistoricReport> >
	{
		REGISTER_CLASS( "LogicHandler", "logicH", GeneticHandler<>::HandlerTypeIdx::H_LOGIC )

		public:
		//---ctor, dtor
			LogicHandler( Genome* owner, const GeneticCollection* collection ) : GeneticHandler( owner, collection ) {;}
			//LogicHandler( const LogicHandler& rhs, Genome* owner ) : GeneticHandler( rhs, owner ) {;}
			virtual ~LogicHandler() = default;
			LogicHandler( const LogicHandler& rhs ) = default;
			LogicHandler& operator=( const LogicHandler& rhs ) = default;
			LogicHandler( LogicHandler&& rhs ) noexcept = default;
			LogicHandler& operator=( LogicHandler&& rhs ) noexcept = default;

		//---get
			inline const std::vector<OdeReport>& getOdeReports() const { return std::get< LogicHReportIdx::ODE >( this->reports ); }
			inline std::vector<OdeReport>& getOdeReportsEdit() { return std::get< LogicHReportIdx::ODE >( this->reports ); }
			inline const std::vector<HistoricReport>& getHistoricReports() const { return std::get< LogicHReportIdx::HISTORIC >( this->reports ); }
			inline std::vector<HistoricReport>& getHistoricReportsEdit() { return std::get< LogicHReportIdx::HISTORIC >( this->reports ); }

		//---API
		  //common
			bool init() override;
			bool secondInit( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask ) override;
		  //special
			TReal check( const Function* function );
			bool check( const GateBase* gate );
			TReal deepCheck( const Function* function ); //checks the Function and all its input Functions and Gates, transitive
			bool deepCheck( const GateBase* gate ); //checks the Gate and all its input Functions and Gates, transitive
			bool deepCheck( const GeneticElement* elem );

			bool refresh(); //update Functions and Gates, called by their inputs online
			inline bool refreshFrom( const GeneticElement* elem ) { return elem->getHasForwardLogic() ? refresh() : false; }
			inline bool refreshFromContent( const GeneticElement* elem ) { return elem->getHasForwardLogicContent() ? refresh() : false; }
	};	
}

#endif //CG_LOGICHANDLER_HPP