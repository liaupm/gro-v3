#ifndef CELLSPLOT_H
#define CELLSPLOT_H

#include "Defines.h"
#include "GroElement.h" //base class

//PRECOMPILED
/*#include <vector> //statFields, otherField, hexColours
#include <string> //name, path, fileName, format*/


class Timer; //timer
class PopulationStat; //statFields
class GroCollection;
class CellsPlot : public GroElement
{
    REGISTER_CLASS( "CellsPlot", "cpt", ElemTypeIdx::CELLS_PLOT )
	
	public:
		inline static const std::string DF_BASE_PATH = "C:";
		inline static const std::string DF_FOLDER_NAME = "gro_plots";
		inline static const std::string DF_FILE_NAME = "gro_plot";
		inline static const std::string DF_FORMAT = ".png";
		static constexpr size_t DF_HISTORIC_SIZE = 10000;
		inline static const std::vector<std::string> PRESET_HEX_COLOURS = { "#034efc", "#ff8800", "#018a18", "#590245", "#02b5a3", "#bd371c", "#c880ff", "#fa69dd" }; 
		static constexpr int DF_PRECISION = -1;

	//---ctor, dtor
	    CellsPlot( GroCollection* groCollection, size_t id, const std::string& name
	    , const Timer* timer, const std::vector< const PopulationStat* >& statFields, const std::vector< const GroElement* >& otherFields
	    , const std::vector< std::string >& hexColours
	    , const std::string& path, const std::string& fileName = DF_FILE_NAME, const std::string& format = DF_FORMAT, size_t historicSize = DF_HISTORIC_SIZE, int precision = DF_PRECISION );

	    virtual ~CellsPlot() = default;

	//---get
	  //resources
	    inline const Timer* getTimer() const { return timer; }
	    inline const std::vector< const PopulationStat* >& getStatFields() const { return statFields; }
		inline const std::vector< const GroElement* >& getOtherFields() const { return otherFields; }
	  //options
	    inline const std::vector<std::string>& getHexColours() const { return hexColours; }
	    inline size_t getHistoricSize() const { return historicSize; }
	    inline int getPrecision() const { return precision; }
	    inline bool getBPrecision() const { return precision >= 0; }
	    inline TReal getRoundMultiplier() const { return roundMultiplier; }
	  //files
	    inline const std::string& getPath() const { return path; }
		inline const std::string& getFileName() const { return fileName; }
		inline const std::string& getFormat() const { return format; }

	//---API
	  //precompute
		void loadDirectElemLink() override;
		void passUsedForPlots() override;


	private:
	  //resources
		const Timer* timer;
		std::vector< const PopulationStat* > statFields; //fields that are PopulationStat
		std::vector< const GroElement* > otherFields; //fields that are PopulationFunction or PopulationGate
	  //options
		std::vector<std::string> hexColours; //colours of the plot lines
		size_t historicSize; //maximum number of data points in the plot
		int precision; //number of decimal places for real fields
		TReal roundMultiplier;
	  //files
		std::string path;
		std::string fileName;
		std::string format;
};

#endif // CELLSPLOT_H
