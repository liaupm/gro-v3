#ifndef LOGICHANDLER_H
#define LOGICHANDLER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "Defines.h"
#include "GroHandler.h" //base
#include "GroReport.h" //base

//PRECOMPILED
/* #include <vector> //getPopulationStatReports()
#include <tuple> //std::get<>
#include <string> //stat name
#include <memory> //SP<>, make_shared() */


struct LogicHReportIdx
{
	enum ReportTypeIdx : size_t { POPULATION_STAT, COUNT };
};

namespace ut { class Bitset; class MultiBitset; class StatsPack; } //update()

class PopulationStat;
class PopulationFunction;
class PopulationGate;
class Cell; //getField()
class PetriDish; //ctor
class GroCollection; //ctor
class LogicHandler;

//////////////////////////////////////////////////////////// REPORTS ///////////////////////////////////////////////////////////////////////////////

class PopulationStatReport : public GroReport< PopulationStat, LogicHandler >
{
    public:
        friend class LogicHandler;

    //---ctor, dtor
        PopulationStatReport( const PopulationStat* model, LogicHandler* ownerHandler );
        virtual ~PopulationStatReport() = default;

    //---get
        inline TReal getField( const Cell* cell ) { return model->getField( cell ); }
        
        inline size_t getTotalStatNum() const { return stats->getStatNum(); }
        inline size_t getStatNum() const { return stats->getVisibleStatNum(); }
        inline const ut::StatPack& getStats() const { return *stats; }
        inline ut::StatPack& getStatsEdit() { return *stats; }
        inline TReal getStat( size_t idx ) const { return stats->getVisibleStat( idx ); }
        inline std::string getStatName( size_t idx ) const { return stats->getVisibleStatName( idx ); }
        inline bool getStatVisibility( size_t idx ) const { return stats->getStatVisibility( idx ); }

    //---API
      //common
        void link() {;}
        bool init() { return true; }
        bool secondInit() { return true; }
        void reset() {;}
        bool divisionOld( PopulationStatReport* ) { return true; }
        bool divisionNew( PopulationStatReport* ) { return true; }
        bool update( const EventType* );
        bool tick() { return true; }
      //special
        bool updateForTimers();
        bool updateForOutput();
        bool checkUpdateForOutput();

        inline void resetStats() { stats->reset(); }
        TReal updateFieldFirstPass( const Cell* cell );
        TReal updateFieldSecondPass( const Cell* cell, size_t statIdx );
        inline void updateFieldFinal( size_t statIdx ) { stats->final( statIdx ); }
        

    private:
        SP<ut::StatPack> stats; //implementation
        TReal sum; //number of cells used to calculate the stats (total)
};


//////////////////////////////////////////////////////////// HANDLER ///////////////////////////////////////////////////////////////////////////////

class LogicHandler : public GroHandler< ut::ReportsDS<PopulationStatReport> >
{
	REGISTER_CLASS( "LogicHandler", "logicH", GroHandler<>::HandlerTypeIdx::H_LOGIC )
	
	public:
    //ctor, dtor
    	LogicHandler( PetriDish* owner, const GroCollection* collection ) : GroHandler( owner, collection ) {;}
    	virtual ~LogicHandler() = default;

    //---get
        inline const std::vector<PopulationStatReport>& getPopulationStatReports() const { return std::get< LogicHReportIdx::POPULATION_STAT >( this->reports ); }
        inline std::vector<PopulationStatReport>& getPopulationStatReportsEdit() { return std::get< LogicHReportIdx::POPULATION_STAT >( this->reports ); }

    //---API
      //common
        bool init() override { updateCommon( true, false ); return true; }
        bool update( ut::MultiBitset*, ut::Bitset*, const GroHandler<>::EventType* ) override { updateCommon( true, false ); return true; }
      //special
        inline void updateForOutput() { updateCommon( false, true ); } 
        void updateCommon( bool bForTimers = true, bool bForOutput = true ); //common part to init() and update()
        TReal check( const PopulationFunction* function ); //updates just this Function
        bool check( const PopulationGate* gate ); //updates just this Gate
        TReal deepCheck( const PopulationFunction* function ); //updates the Function and all its inputs Functions recursively
        bool deepCheck( const PopulationGate* gate ); //updates the Gate and all its inputs Functions and Gates recursively
};

#endif // LOGICHANDLER_H
