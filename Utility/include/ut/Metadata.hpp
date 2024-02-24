#ifndef UT_METADATA_HPP
#define UT_METADATA_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <string> //name */


namespace ut
{
	/* for access at run time */
	template< typename TIdx >
	struct Metadata
	{
		using IdxType = TIdx;
		static constexpr bool DF_BQUANTITATIVE = false;

	//---ctor, dtor
		Metadata( const std::string& name, const std::string& key, TIdx idx, bool bQuantitative = DF_BQUANTITATIVE )
		: name(name), key(key), idx( idx ), bQuantitative( bQuantitative) {;}

		virtual ~Metadata() = default;
		Metadata( const Metadata& rhs ) = default;
		Metadata( Metadata&& rhs ) noexcept = default;
		Metadata& operator=( const Metadata& rhs ) = default;
		Metadata& operator=( Metadata&& rhs ) noexcept = default;

	//---get

	//---set

	//---API
		const std::string name; //full class name
		const std::string key; //class prefix
		const IdxType idx; //enum in a base class or external
		bool bQuantitative; //digital or quantitative nature
	};


	/* for static access */
	template< const char* _name, const char* _key, int _idx, int _qIdx >
	struct StaticMetadata //TODO: make the strings static too
	{
		//static constexpr char name[32] = _name;
		//static constexpr char key[32] = _key;
		inline static const std::string name = std::string( _name ); //not static
		inline static const std::string key = std::string( _key ); //not static
		static constexpr int idx = _idx;
		static constexpr int qIdx = _qIdx;
	};


  //internal implementation of the macros
	#define REGISTER_CLASS_5( _className, _classKey, _classIdx, _classIsQ, _metaName ) \
	public: \
	    inline static ut::Metadata _metaName = ut::Metadata( _className, _classKey, _classIdx, _classIsQ );

	#define REGISTER_CLASS_4( _className, _classKey, _classIdx, _classIsQ ) REGISTER_CLASS_5(  _className, _classKey, _classIdx, _classIsQ, metadata )
	#define REGISTER_CLASS_3( _className, _classKey, _classIdx ) REGISTER_CLASS_5(  _className, _classKey, _classIdx, false,  metadata )
	#define GET_REGISTER_CLASS_MACRO( _1, _2, _3, _4, _5, NAME,... ) NAME

  //macros to use
	#define REGISTER_CLASS( ... ) GET_REGISTER_CLASS_MACRO( __VA_ARGS__ , REGISTER_CLASS_5, REGISTER_CLASS_4, REGISTER_CLASS_3 )( __VA_ARGS__ ) //basic metadata
	#define REGISTER_SUBTYPE( _name, _key, _idx ) REGISTER_CLASS( _name, _key, _idx, false, subtypeMeta ) //additional metadata for classes derived from others that already have metadata
}

#endif //UT_METADATA_HPP