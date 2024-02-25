#ifndef UT_SUBTYPEREGISTER_HPP
#define UT_SUBTYPEREGISTER_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <map> //std::map<std::string, Subtype> subtypeNM, std::map<Subtype, CreationFunction> creationFunctions
#include <string>
#include <functional> //std::function for subobject creation functions map: using CreationFunction = std::function<OwnerType* ( const ParamsForm& )> */


namespace ut
{
	template< typename OwnerType, typename Subtype, typename ParamsForm >
    struct SubtypeRegister
    {
        using CreationFunction = std::function<OwnerType* ( const ParamsForm& )>;
        inline static const std::string NO_NAME = "";

    //---ctor, dtor
		//explicit SubtypeRegister( const std::string& defaultName, Subtype defaultSubtype = static_cast<Subtype>(0) ) : defaultName( defaultName ), defaultSubtype( defaultSubtype ) {;}
		virtual ~SubtypeRegister() = default;
		SubtypeRegister( const SubtypeRegister& rhs ) = default;
		SubtypeRegister( SubtypeRegister&& rhs ) noexcept = default;
		SubtypeRegister& operator=( const SubtypeRegister& rhs ) = default;
		SubtypeRegister& operator=( SubtypeRegister&& rhs ) noexcept = default; 

    //---API
      //check presence
        inline bool getIsRegistered( const std::string name ) const { return subtypeNM.find( name ) != subtypeNM.end(); }
        inline bool getIsRegistered( Subtype subtype ) const { return creationFunctions.find( subtype ) != creationFunctions.end(); }
        //inline bool getIsOtherName( Subtype subtype ) const { for( const auto& pair : subtypeNM ) { if( pair->second == subtype ) return true; }; return false; }
      //conversions
        inline Subtype name2Subtype( const std::string name ) const { auto it = subtypeNM.find( name ); return it != subtypeNM.end() ? it->second : Subtype::COUNT; }
        inline Subtype getSubtype( const std::string& name ) const { return name2Subtype( name ); }
        inline const std::string& subtype2Name( Subtype subtype ) const { for( const auto& it : subtypeNM ) { if( it.second == subtype ) return it.first; } return NO_NAME; }
        inline CreationFunction getCreationFunction( Subtype subtype ) { assert( getIsRegistered( subtype ) ); return creationFunctions.find( subtype )->second; }
        inline CreationFunction getCreationFunction( const std::string name ) { return getCreationFunction( getSubtype( name ) ); }
      //modify
        bool registerSubclass( const std::string& name, Subtype subtype, CreationFunction creationFunction );
        bool unregisterSubclass( const std::string& name );
      //factory
        inline OwnerType* createSubobject( const ParamsForm& params, Subtype subtype ) { return getCreationFunction( subtype )( params ); } //factory. Params meaning depends on the distribution
        inline OwnerType* createSubobject( const ParamsForm& params, const std::string& name ) { return getCreationFunction( name )( params ); } //factory. Params meaning depends on the distribution


    //---data
        std::map<std::string, Subtype> subtypeNM; //name to enum idx
        std::map<Subtype, CreationFunction> creationFunctions; //enum idx to factory function

        //std::string defaultName; //default name
        //Subtype defaultSubtype;
    };




////////////////////////////////////// METHOD DEFINITIONS ///////////////////////////////////////////////

    template< typename OwnerType, typename Subtype, typename ParamsForm >
    bool SubtypeRegister<OwnerType, Subtype, ParamsForm >::registerSubclass( const std::string& name, Subtype subtype, CreationFunction creationFunction ) 
    { 
        subtypeNM.emplace( name, subtype ); 
        if( getIsRegistered( subtype) )
            return false;
        creationFunctions.emplace( subtype, creationFunction ); 
        return true; 
    }

    template< typename OwnerType, typename Subtype, typename ParamsForm >
    bool SubtypeRegister<OwnerType, Subtype, ParamsForm >::unregisterSubclass( const std::string& name ) 
    { 
        auto it = subtypeNM.find( name );
        if( it == subtypeNM.end() )
            return false;

        if ( ! getIsOtherName( it->second ) )
            creationFunctions.erase( it->second );

        subtypeNM.erase( it ); 
        return true; 
    }
}

#endif //UT_SUBTYPEREGISTER_HPP