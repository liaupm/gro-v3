#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

int main( int argc, char* argv[] )
{
//---parse args
	std::string className = argv[1];
	std::string namespaceName = "";
	if( argc > 2 )
		namespaceName = argv[2];

	std::string dirName = "";
	if( argc > 3 )
		dirName = argv[3];

//---open files

	std::string hppFileName = dirName + "\\include\\" + namespaceName + "\\" + className + ".hpp";
	if( dirName == "" )
		 hppFileName = className + ".hpp";
	std::cout << "hpp file: " << hppFileName << "\n";
	std::ofstream hppFile( hppFileName, std::ofstream::out );

	std::string cppFileName = dirName + "\\src\\" + namespaceName + "\\" + className + ".cpp";
	if( dirName == "" )
		 cppFileName = className + ".cpp";
	std::cout << "cpp file: " << cppFileName << "\n";
	std::ofstream cppFile( cppFileName, std::ofstream::out );

//---write cpp
	cppFile << "#include \"" << namespaceName << "/" << className << ".hpp\"\n\n";
	cppFile << "using namespace " << namespaceName << ";\n";


//---write hpp
	std::string classNameUC( className );
	std::transform( classNameUC.begin(), classNameUC.end(), classNameUC.begin(), ::toupper );
	std::string namespaceNameUC( namespaceName );
	std::transform( namespaceNameUC.begin(), namespaceNameUC.end(), namespaceNameUC.begin(), ::toupper );

	hppFile << "#ifndef " << namespaceNameUC << "_" << classNameUC << "_HPP\n";
	hppFile << "#define " << namespaceNameUC << "_" << classNameUC << "_HPP\n\n";

	hppFile << "#include " << "\"" << namespaceName << "/" << "defines.hpp\"\n\n";

	if( namespaceName != "" )
		hppFile << "namespace " << namespaceName << "\n{\n";

	hppFile << "\tclass " << className << "\n\t{\n";
	hppFile << "\t\tpublic:\n";

	hppFile << "\t\t//---ctor, dtor\n";
	hppFile << "\t\t\t" << className << "() = default;\n";
	hppFile << "\t\t\tvirtual ~" << className << "() = default;\n";
	hppFile << "\t\t\t" << className << "( const " << className << "& rhs ) = default;\n";
	hppFile << "\t\t\t" << className << "( " << className << "&& rhs ) noexcept = default;\n";
	hppFile << "\t\t\t" << className << "& operator=( const " << className << "& rhs ) = default;\n";
	hppFile << "\t\t\t" << className << "& operator=( " << className << "&& rhs ) noexcept = default;\n\n";

	hppFile << "\t\t//---get\n\n";
	hppFile << "\t\t//---set\n\n";
	hppFile << "\t\t//---API\n\n";	  


	hppFile << "\t\tprivate:\n";  

	hppFile << "\t};\n";
	hppFile << "}\n\n";

	hppFile << "#endif //" << namespaceNameUC << "_" << classNameUC << "_HPP";

	std::cout << "done\n";
}