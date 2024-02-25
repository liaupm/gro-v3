#ifndef CELLSFILE_H
#define CELLSFILE_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp"  //ut::NameMap

#include "Defines.h"
#include "GroElement.h" //base

//PRECOMPILED
/*#include <vector> //statFields, otherFields
#include <string> //path, file name, folder name, format, separator, name */


namespace cg { class GeneticElement; } //individualGate
class Timer;
class PopulationStat; //statFields
class GroCollection;
class CellsFile : public GroElement
{
    REGISTER_CLASS( "CellsFile", "cf", ElemTypeIdx::CELLS_FILE )
	
	public:
		enum class Mode
		{
			INDIVIDUAL, POPULATION, BOTH, COUNT
		};

		static constexpr Mode DF_MODE = Mode::BOTH;
		static constexpr bool DF_B_ANCESTRY = false;

		inline static const std::string DF_BASE_PATH = "C:";
        inline static const std::string DF_FOLDER_NAME = "gro_output";
		inline static const std::string DF_FILE_NAME = "";
		inline static const std::string DF_FORMAT = ".csv";
		static constexpr uint DF_PRECISION = 8;

		static constexpr bool DF_B_INCLUDE_POSITION = true;
		static constexpr bool DF_B_INCLUDE_SIZE = true;

		inline static const std::string AUTO_DF_FILE_NAME = "_file_df";


        inline static const ut::NameMap<Mode> modeNM = ut::NameMap<Mode>( {
                                          { "individual", CellsFile::Mode::INDIVIDUAL }
                                        , { "population", CellsFile::Mode::POPULATION }
                                        , { "both", CellsFile::Mode::BOTH } } );


	//---ctor, dtor
	    CellsFile( GroCollection* groCollection, size_t id, const std::string& name
	    , const Timer* timer, const cg::GeneticElement* individualGate, const std::vector< const PopulationStat* >& statFields, const std::vector< const GroElement* >& otherFields, Mode mode = DF_MODE, bool bIncludeAncestry = DF_B_ANCESTRY
        , const std::string& path = DF_BASE_PATH, const std::string& fileName = DF_FILE_NAME, const std::string& format = DF_FORMAT, uint precision = DF_PRECISION );

	    virtual ~CellsFile() = default;

	//---get
	  //resources
	    inline const Timer* getTimer() const { return timer; }
	    inline const cg::GeneticElement* getIndividualGate() const { return individualGate; }
	    inline const std::vector< const PopulationStat* >& getStatFields() const { return statFields; }
	    inline const std::vector< const GroElement* >& getOtherFields() const { return otherFields; }
	  //options
	    inline Mode getMode() const { return mode; }
	    inline bool getBIncludeAncestry() const { return bIncludeAncestry; }
  	  //files
	    inline const std::string& getPath() const { return path; }
		inline const std::string& getFileName() const { return fileName; }
		inline const std::string& getFormat() const { return format; }
		inline const std::string& getSeparator() const { return separator; }
		inline uint getPrecision() const { return precision; }
		inline const std::string& getPopFileWholeName() const { return popFileWholeName; }
		inline const std::string& getIndividualFileBaseName() const { return individualFileBaseName; }

	//---API
	  //precompute
		void loadDirectElemLink() override;
		void passUsedForOutput( const CellsFile* ) override;


	private:
	  //resources
		const Timer* timer;
		const cg::GeneticElement* individualGate; //filter for the cells, only for the individual 
		std::vector< const PopulationStat* > statFields; 
		std::vector< const GroElement* > otherFields; //fields that are PopulationFunction or PopulationGate
	  //options
		Mode mode; //invividual, population or both
		bool bIncludeAncestry;
	  //files
		std::string path;
		std::string fileName;
		std::string format; //.cvs or .tsv
		std::string separator; //separator or the fields (columns)
		uint precision; //number of decimal places for real fields
		std::string popFileWholeName; //path + folder + file + format
		std::string individualFileBaseName; //path + folder + file. Number and format added on file creation
};

#endif // CELLSFILE_H
