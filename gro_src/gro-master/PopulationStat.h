 #ifndef POPULATIONSTAT_H
#define POPULATIONSTAT_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp"  //typeNM, autoVolModeNM
#include "ut/Stat.hpp" //implementation of logic, ::Type
#include "ut/StatPack.hpp" //implementation of logic, ::Type
#include "cg/Genome.hpp" //checkBState() for getField() of StatElem

#include "Defines.h"
#include "GroElement.h" //basee class
#include "Cell.h" //special functions to getField(), getGenome()

//PRECOMPILED
/* #include <vector> //cmarkers, statTypes
#include <string> //name, header */


namespace cg { class GeneticElement; } //elem
class QGroMarker; //getMarkers()
class GroCollection; //ctor
class CellsFile; 
class PopulationStat : public GroElement
/* virtual base class */
{
    REGISTER_CLASS( "PopulationStat", "stat", ElemTypeIdx::POP_STAT )

	public:
	    enum AutoVolMode
        {
            NONE, DIVISION, PRODUCT, COUNT
        };

        static constexpr AutoVolMode DF_AUTO_VOL_MODE = AutoVolMode::NONE;
		static constexpr bool DF_B_FREE = true;
		static constexpr bool DF_B_USED_FOR_TIMERS = false;
		static constexpr bool DF_B_USED_FOR_PLOTS = false;

		inline static ut::NameMap<AutoVolMode> autoVolModeNM = ut::NameMap<AutoVolMode>( { 
                                          { "none", AutoVolMode::NONE }
                                        , { "division", AutoVolMode::DIVISION }
                                        , { "product", AutoVolMode::PRODUCT } } );

	//---cotr, dtor
	    virtual ~PopulationStat() = default;

	//---get
	  //resources
	    inline const cg::GeneticElement* getFilterGate() const { return filterGate; }
	    inline const std::vector<const QGroMarker*>& getMarkers() const { return markers; }
	  //options
	    inline const std::string& getHeader() const { return header; }
	    inline ut::StatPack::Type getStatsType() const { return statsType; }
	    inline const std::vector< ut::StatBase::StatType >& getStatTypes() const { return statTypes; }
	    inline AutoVolMode getAutoVolMode() const { return autoVolMode; }
	    inline bool getBFree() const { return bFree; }
		inline bool getIsFree() const { return bFree; }
	  //state
		inline bool getBUsedForTimers() const { return bUsedForTimers; } 
		inline bool getBUsedForPlots() const { return bUsedForPlots; } 
		inline const std::vector<const CellsFile*>& getUserFiles() const { return userFiles; } 

	//---set
		inline void setBUsedForTimers( bool xBUsedForTimers ) { bUsedForTimers = xBUsedForTimers; } 
		inline void setBUsedForPlots( bool xBUsedForPlots ) { bUsedForPlots = xBUsedForPlots; } 
		inline void addUserFile( const CellsFile* cellsFile ) { userFiles.push_back( cellsFile ); }

	 //---API
	   //precompute
		void loadDirectElemLink() override { this->addLinks( markers, LinkDirection::FORWARD ); }
	   //run
	    virtual TReal getField( const Cell* cell ) const = 0;
	    void passUsedForTimers() override { setBUsedForTimers( true ); }
	    void passUsedForPlots() override { setBUsedForPlots( true ); }
        void passUsedForOutput( const CellsFile* cellsFile ) override { addUserFile( cellsFile ); }


	protected:
	  //resources
		const cg::GeneticElement* filterGate; //only the cells where it holds are used for the calculation
		std::vector<const QGroMarker*> markers; //markers to store the value of every stat in the qstate vector
	  //options
		std::string header; //name of the stat, for output files and plot legends
		ut::StatPack::Type statsType; //type of stat pack (which visible stats it includes)
		std::vector< ut::StatBase::StatType > statTypes; //types of the individual stats of the stat pack
		AutoVolMode autoVolMode;
		bool bFree; //false if it is owned by a CellsFile or CellsPlot (updated only when updating the owner for efficiency)

		
	  //state
		bool bUsedForTimers;
		bool bUsedForPlots;
		std::vector<const CellsFile*> userFiles;

		
		PopulationStat( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, const std::string& header, ut::StatPack::Type statsType, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE );

		PopulationStat( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, const std::string& header, const std::vector< ut::StatBase::StatType >& statTypes, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE );
};


class PopulationStatElem : public PopulationStat
/* for cg elements whose value is stored in the Genome's qstate vector and can be retrieved directly by idx */
{
	public:
	//---ctor, dtor
		PopulationStatElem( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, const cg::GeneticElement* elem, ut::StatPack::Type statsType, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, elem->getName(), statsType, markers, autoVolMode, bFree ), elem( elem ) {;}

		PopulationStatElem( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, const cg::GeneticElement* elem, const std::vector< ut::StatBase::StatType >& statTypes, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, elem->getName(), statTypes, markers, autoVolMode, bFree ), elem( elem ) {;}
		
		virtual ~PopulationStatElem() = default;
	
	//---get
		TReal getField( const Cell* cell ) const override;

	private:
		const cg::GeneticElement* elem; //intracellular element  that is aggregated
};


class PopulationStatSpecial : public PopulationStat
/* for special variables of Cell or cg::Genome that are not cg elements
some of them are curretly saved in the Genome's qstate vector through markers but this method is still used
for the variables of cg, it is necessary a wrapper function in Cell so that all use the same function type */
{
	public:
		using FunType = TReal( Cell::* )( void ) const;

		enum Type : size_t
		{
			POS_X, POS_Y, GR, GT, LENGTH, D_LENGTH, VOLUME, D_VOLUME, COUNT
		};

		inline static const std::vector<std::string> HEADER_NAMES = { "x", "y", "GR", "GT", "length", "d_length", "volume", "d_volume" };
		inline static const std::vector<FunType> FUNCTIONS = { Cell::getX, Cell::getY, Cell::getGrowthRate, Cell::getGenerationTime, Cell::getTotalLength, Cell::getDLength, Cell::getVolume, Cell::getDVolume };

		static ut::NameMap< Type > typeNM; //from str to Type

		static const std::string& selectHeaderName( Type type ) { return HEADER_NAMES[ type ]; } //Type to header
		static FunType selectFunction( Type type ) { return FUNCTIONS[ type ]; } //Type to function that retrieves the value


	//---ctor, dtor
		PopulationStatSpecial( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, FunType fun, const std::string& header, ut::StatPack::Type statsType, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, header, statsType, markers, autoVolMode, bFree ), fun( fun ) {;} //stats pack, custom header

		PopulationStatSpecial( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, FunType fun, const std::string& header, const std::vector< ut::StatBase::StatType >& statTypes, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, header, statTypes, markers, autoVolMode, bFree ), fun( fun ) {;} //individual stats, custom header

		PopulationStatSpecial( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, Type type, ut::StatPack::Type statsType, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, selectHeaderName( type ), statsType, markers, autoVolMode, bFree ), fun( selectFunction( type ) ) {;} //stats pack, default header

		PopulationStatSpecial( GroCollection* groCollection, size_t id, const std::string& name
		, const cg::GeneticElement* filterGate, Type type, const std::vector< ut::StatBase::StatType >& statTypes, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode = PopulationStat::DF_AUTO_VOL_MODE, bool bFree = DF_B_FREE ) 
		: PopulationStat( groCollection, id, name, filterGate, selectHeaderName( type ), statTypes, markers, autoVolMode, bFree ), fun( selectFunction( type ) ) {;} //individual stats, default header

		virtual ~PopulationStatSpecial() = default;

	//---API
		TReal getField( const Cell* cell ) const override { return ( *cell.*fun )(); }
		

	private:
		FunType fun; //function to get the variable to aggregate
};

#endif // POPULATIONSTAT_H
