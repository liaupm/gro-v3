
#include <vector>
#include <string>

#include <cstdlib>
#include <stdlib.h> //system
#include <algorithm> //min, max 

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>


using uint = unsigned int;
using ParamValuesDS = std::vector< std::vector<std::string> >;


class Parser
{
	public: 
		inline static const std::string MASTER_FILE_NAME = "batchgro_master";
		inline static const std::string BATCH_FILE_NAME = "batchgro_batch";
		inline static const std::string PARAMS_FILE_NAME = "batchgro_params";

		inline static const std::string WDIR_PATH = "../Files";

		inline static const std::string GRO_PATH = "C:/swLIA/GRO/elegro/TEMP_EXE/gro.exe";
		//inline static const std::string GRO_PATH = "gro.exe";

	//---cotr, dtor
		Parser( const std::string& path ) : path( path ), idx(0) { init(); }
		~Parser() = default;

	//---get
		inline size_t getSimNum() const { return products.size() > 0 ? products.back() : 0; }

	//---API
		bool parse();
		bool executeSim( size_t idx );


	private:
		std::string path;
		std::vector<std::string> paramNames;
		std::vector<std::string> singleParamNames;
		ParamValuesDS paramValues;
		std::vector<std::string> singleParamValues;
		std::vector<uint> products;
		size_t idx;

	//---fun
		inline void init() { std::filesystem::create_directory( path + "/params" ); std::filesystem::current_path( WDIR_PATH ); }
		bool write( size_t idx );
		void createProducts();
	  //aux
		std::vector<uint> n2Idxs( size_t idx );
		std::vector<std::string> selectValues( size_t idx );
		std::string makeSingleVal( const std::string& base, size_t idx );
		std::string trim( const std::string& str );
		inline std::string makeFileName( const std::string& file ) { return path + "/" + file + ".gro"; } 
		inline std::string makeParamsFileName( const std::string& file, size_t idx ) { return path + "/params/" + file + "_" + std::to_string( idx ) + ".gro"; } 
};


int main( int argc, char* argv[] )
{
  //get folder and parse batch file
	std::string folderName = argv[1];

	Parser parser( folderName );
	if( ! parser.parse() )
		return 1;

  //get and correct sim range
	size_t iniIdx = 0;
	if( argc >= 3 )
		iniIdx = std::atoi( argv[2] );
	iniIdx = std::max( iniIdx, (size_t)0 );

	size_t lastIdx = parser.getSimNum() - 1;
	if( argc >= 4 )
		lastIdx = std::atoi( argv[3] );
	lastIdx = std::min( lastIdx, parser.getSimNum() - 1 );

	std::cout << "simulate " << folderName << " from " << iniIdx << " to "  << lastIdx << "\n";
	

  //simulate
	for( size_t i = iniIdx; i < lastIdx + 1; i++ )
	{
		std::cout << "current sim: " << i << "/" << lastIdx << "\n";
		if( ! parser.executeSim( i ) )
		{
			std::cout << "failed\n";
			return 1;
		}
	}

	//for( size_t i = 0; i < 4; i++ )
		//std::system( ( Parser::GRO_PATH + " prueba.gro" ).c_str() );

	return 0;
}





//////////////////////////////////////////////////////////////////// METHOD IMPLEMENTATION /////////////////////////////////////////////////////////////////


//========================================================== API

bool Parser::parse()
{
	std::ifstream inFile( makeFileName( BATCH_FILE_NAME ) );

	if( ! inFile.is_open() )
	{
		std::cout << "unable to open file: " << makeFileName( BATCH_FILE_NAME ) << "\n";
		return false;
	}

	paramNames.clear();
	paramValues.clear();

	std::string line; 
	std::string paramName;
	std::string value;

	while( std::getline( inFile, line ) )
	{
		std::stringstream lineStream( line );

		std::getline( lineStream, paramName, '=' );
		std::getline( lineStream, value, '=' );
		

		std::stringstream valueStream( value );
		std::string valueUnit; 
		
		std::vector<std::string> temp;
		while( std::getline( valueStream, valueUnit, ';' ) )
			temp.push_back( trim( valueUnit ) );

		if( temp.size() > 1 )
		{
			paramNames.push_back( trim( paramName ) );
			paramValues.push_back( temp );
		}
		else if( temp.size() == 1 )
		{
			singleParamNames.push_back( trim( paramName ) );
			singleParamValues.push_back( temp[0] );
		}
	}

	inFile.close();
	createProducts();
	return true;
}

bool Parser::executeSim( size_t idx )
{
	if( write( idx ) )
	{
		std::system( ( GRO_PATH + " " + path + "/batchgro_master.gro" ).c_str() );
		return true;
	}
	else
		return false;
}



//========================================================== PRIVATE

bool Parser::write( size_t idx )
{
	std::ofstream outFile( makeFileName( PARAMS_FILE_NAME ) );
	std::ofstream outFile2( makeParamsFileName( PARAMS_FILE_NAME, idx ) );

	if( ! outFile.is_open() )
	{
		std::cout << "unable to open file: " << makeFileName( PARAMS_FILE_NAME ) << "\n";
		return false;
	}
	if( ! outFile2.is_open() )
	{
		std::cout << "unable to open file: " << makeParamsFileName( PARAMS_FILE_NAME, idx ) << "\n";
		return false;
	}

    std::vector<std::string> vals = selectValues( idx );
	for( size_t p = 0; p < paramNames.size(); p++ )
	{
		outFile << paramNames[p] << " := " << vals[p] << ";\n";
		outFile2 << paramNames[p] << " := " << vals[p] << ";\n";
	}
	for( size_t p = 0; p < singleParamNames.size(); p++ )
	{
		outFile << singleParamNames[p] << " := " << makeSingleVal( singleParamValues[p], idx ) << ";\n";
		outFile2 << singleParamNames[p] << " := " << makeSingleVal( singleParamValues[p], idx ) << ";\n";
	}

	outFile.close();
	outFile2.close();
	return true;
}

void Parser::createProducts() 
{
	products.clear();
	products.push_back( 1 );
	for( int i = paramValues.size() - 1; i >= 0; i-- )
		products.push_back( products.back() * paramValues[i].size() );
}

//---aux
std::vector<uint> Parser::n2Idxs( size_t idx )
{
	std::vector<uint> result;
	uint current = idx;
    for( int i = products.size() - 2; i >= 0; i-- )
	{
		result.push_back( current / products[i] );
		current = current % products[i];
	}
	return result;
}

std::vector<std::string> Parser::selectValues( size_t idx )
{
	std::vector<uint> idxs = n2Idxs( idx );
	std::vector<std::string> result;
	for( size_t i = 0; i < idxs.size(); i++ )
		result.push_back( paramValues[i][ idxs[i] ] );
	return result;
}

std::string Parser::makeSingleVal( const std::string& base, size_t idx )
{
	if( base.find( '"' ) == std::string::npos )
		return base;

	std::string result = base;
	result.erase( std::remove( result.begin(), result.end(), '"'), result.end() );
	return "\"" + result + "_" + std::to_string( idx ) + "\"";
}

std::string Parser::trim( const std::string& str ) 
{
	const char* spaceChars = " \t";
	std::string result = str;
	result.erase( result.find_last_not_of( spaceChars ) + 1 );
	result.erase( 0, result.find_first_not_of( spaceChars ) );
	return result;
}