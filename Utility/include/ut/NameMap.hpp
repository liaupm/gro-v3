#ifndef UT_NAMEMAP_HPP
#define UT_NAMEMAP_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <vector>
#include <map>
#include <string> */


namespace ut
{
	template< typename TSubtypeEnum >
    //TSubtypeEnum : enum for the types
	struct NameMap
	{
		using UnderDS = std::map< std::string, TSubtypeEnum >;
        inline static const std::string NO_NAME = "";


    //---ctor, dtor
        NameMap() = default;
		explicit NameMap( UnderDS underDS ) : underDS(underDS) {;}
		virtual ~NameMap() = default;
		NameMap( const NameMap& rhs ) = default;
		NameMap( NameMap&& rhs ) noexcept = default;
		NameMap& operator=( const NameMap& rhs ) = default;
		NameMap& operator=( NameMap&& rhs ) noexcept = default; 

    //---API
      //check presence
		inline bool getIsRegistered( const std::string& name ) const { return underDS.find( name ) != underDS.end(); }
        inline bool getIsRegistered( TSubtypeEnum subtype ) const { for( const auto& pair : underDS ) { if( pair->second == subtype ) return true; } return false; }
      //conversion
        inline TSubtypeEnum name2Subtype( const std::string& name ) const { auto it = underDS.find( name ); return it != underDS.end() ? it->second : TSubtypeEnum::COUNT; }
        inline TSubtypeEnum getSubtype( const std::string& name ) const { return name2Subtype( name ); }
        inline const std::string& subtype2Name( TSubtypeEnum subtype ) const { for( const auto& it : underDS ) { if( it.second == subtype ) return it.first; } return NO_NAME; }
        inline std::vector<TSubtypeEnum> names2Subtypes( const std::vector<std::string>& names ) const { std::vector<TSubtypeEnum> subtypes; for( const auto& name : names ) subtypes.push_back( name2Subtype( name ) ); return subtypes; }
        inline std::vector<std::string> subtypes2Names( const std::vector<TSubtypeEnum>& subtypes ) const { std::vector<std::string> names; for( const auto& subtype : subtypes ) names.push_back( subtype2Name( subtype ) ); return names; }
      //modify
        bool registerSubclass( const std::string& name, TSubtypeEnum subtype );
        bool unregisterSubclass( const std::string& name );

    //---data
        UnderDS underDS;
	};



////////////////////////////////////// METHOD DEFINITIONS ///////////////////////////////////////////////

    template< typename TSubtypeEnum >
    bool NameMap<TSubtypeEnum>::registerSubclass( const std::string& name, TSubtypeEnum subtype ) 
    { 
    	if( getIsRegistered( name ) )
            return false;
        underDS.emplace( name, subtype ); 
        return true; 
    }

    template< typename TSubtypeEnum >
    bool NameMap<TSubtypeEnum>::unregisterSubclass( const std::string& name ) 
    { 
        auto it = underDS.find( name );
        if( it == underDS.end() )
            return false;
        underDS.erase( it ); 
        return true; 
    }
}

#endif //UT_NAMEMAP_HPP