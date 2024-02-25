/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summarized here.
// Please see the file LICENSE.txt for the complete license.
//
// THE SOFTWARE (AS DEFINED BELOW) AND HARDWARE DESIGNS (AS DEFINED BELOW) IS PROVIDED
// UNDER THE TERMS OF THIS OPEN SOURCE LICENSE (“LICENSE”).  THE SOFTWARE IS PROTECTED
// BY COPYRIGHT AND/OR OTHER APPLICABLE LAW.  ANY USE OF THIS SOFTWARE OTHER THAN AS
// AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// BY EXERCISING ANY RIGHTS TO THE SOFTWARE AND/OR HARDWARE PROVIDED HERE, YOU ACCEPT AND
// AGREE TO BE BOUND BY THE TERMS OF THIS LICENSE.  TO THE EXTENT THIS LICENSE MAY BE
// CONSIDERED A CONTRACT, THE UNIVERSITY OF WASHINGTON (“UW”) GRANTS YOU THE RIGHTS
// CONTAINED HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF SUCH TERMS AND CONDITIONS.
//
// TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
//
//

#ifndef _CCL_ADAPTER_H_
#define _CCL_ADAPTER_H_

#include "ut/NameMap.hpp"
#include "Defines.h"

/*PRECOMPILED
#include <vector> //variable of list fields, cclFields of BaseDate
#include <list> //list<Value*> when parsing lists of data
#include <string> //name
#include <memory> //make_shared, static_pointer_cast
#include <type_traits> //for SFINAE: std::enable_if_t, std::is_same, std::decay_t,std::is_integral, std::is_floating_point
#include "ccl.h" //connection to ccl module, Value */


class Parser; //enclosingParser
class World; //load functions param
class CclAdapter
{
    public:
    //////////////////////////////////////////////////////////////////////////////// STATIC COMMON FUNCTIONS ////////////////////////////////////////////////////////////////////////////
        template< typename T > 
        using ValueFunct = T ( Value::* ) ( void );

    //get the method of ccl Value::TYPE and the method that returns it as that data type
        template <typename T, typename = std::enable_if_t< std::is_same<std::decay_t<T>, bool>::value > > //bool
        static ValueFunct<bool> getCclValueFunct( Value::TYPE& valueType ) { valueType = Value::BOOLEAN; return &Value::bool_value; }

        template <typename T, typename = std::enable_if_t< std::is_integral<std::decay_t<T>>::value && ! std::is_same<std::decay_t<T>, bool>::value > > //int
        static ValueFunct<int> getCclValueFunct( Value::TYPE& valueType ) { valueType = Value::INTEGER; return Value::int_value; }

        template <typename T, typename = std::enable_if_t< std::is_floating_point<std::decay_t<T>>::value > > //real
        static ValueFunct<double> getCclValueFunct( Value::TYPE& valueType ) { valueType = Value::REAL; return Value::real_value; }

        template <typename T, typename = std::enable_if_t< ! std::is_integral<std::decay_t<T>>::value && ! std::is_floating_point<std::decay_t<T>>::value > > //str
        static ValueFunct<std::string> getCclValueFunct( Value::TYPE& valueType ) { valueType = Value::STRING; return Value::string_value; }

    //parse field
        template< typename T >
        static bool parseCclField( Value* data, const std::string& name, T& var, Value::TYPE valueType, ValueFunct<T> valueFunct ); //single value

        template< typename T >
        static bool parseCclFieldDf( Value* data, const std::string& name, T& var, Value::TYPE valueType, ValueFunct<T> valueFunct, T dfValue = T() ); //single value with default

        template< typename T> 
        static bool parseCclList2Vector( Value* data, const std::string& name, std::vector<T>& var, ValueFunct<T> valueFunct, bool bClearPrevious = true ); //list

        template< typename T> 
        static bool parseCclList2VectorDf( Value* data, const std::string& name, std::vector<T>& var, ValueFunct<T> valueFunct, bool bClearPrevious = true, const std::vector<T>& dfValue = {} ); //list with default




////////////////////////////////////////////////////////////////// FIELD TYPES /////////////////////////////////////////////////////////////////////

        struct CclFieldBase
        /* virtual base class */
        {
            enum  FieldType { SIMPLE, LIST, RECORD, AUTO_RECORD, MULTIRECORD, COUNT };

            std::string name;
            FieldType fieldType;
            Value::TYPE valueType;
            bool bParsed;

        //---ctor, dtor
            virtual ~CclFieldBase() = default;
        //---API
            virtual void initVar() {;}
            virtual bool loadVar( Value*, Parser* ) { return false; }
            virtual void setValueFrom( const CclFieldBase* ) {;}
            virtual void setDefaultFrom( const CclFieldBase* ) {;}
            virtual void setDefaultFromAndInit( const CclFieldBase* ) {;}
          //for record
            virtual void fwCreateCclFields() {;}
            virtual void fwInitCclFields() {;}
            virtual void fwPreprocess() {;}


            protected:
                CclFieldBase( const std::string& name, FieldType fieldType ) : name(name), fieldType(fieldType), bParsed(false) {;}
        };
        using FieldType = CclFieldBase::FieldType;


        template< typename T>
        struct CclFieldSimple : public CclFieldBase
        /* for single values of any type */
        {
        //---ctor, dtor
            template< typename... Ts >
            CclFieldSimple( const std::string& name, T& var, Ts... dfArgs ) 
            : CclFieldBase( name, FieldType::SIMPLE )
            , var(var), valueFunct( CclAdapter::getCclValueFunct<T>( valueType ) ), dfValue( dfArgs... ) {;}

            virtual ~CclFieldSimple() = default;

        //---API
            void initVar() override { var = dfValue; }
            bool loadVar( Value* data, Parser* ) override { bParsed = CclAdapter::parseCclField( data, this->name, var, this->valueType, valueFunct ); return bParsed; }
            void setValueFrom( const CclFieldBase* original ) override { var = static_cast<const CclFieldSimple<T>*>( original )->var; }
            void setDefaultFrom( const CclFieldBase* original ) override { dfValue = static_cast<const CclFieldSimple<T>*>( original )->dfValue; }
            void setDefaultFromAndInit( const CclFieldBase* original ) override { setDefaultFrom( original ); initVar(); }

        //---data
            T& var;
            ValueFunct<T> valueFunct;
            T dfValue;
        };

        template< typename T>
        struct CclFieldList : public CclFieldBase
        /* for lists */
        {
        //---ctor, dtor
            CclFieldList( const std::string& name, std::vector<T>& var, bool bClearPrevious, const std::vector<T>& dfValue )
            : CclFieldBase( name, FieldType::LIST )
            , var(var), valueFunct( CclAdapter::getCclValueFunct<T>( valueType ) ), bClearPrevious(bClearPrevious), dfValue( dfValue ) {;}

            virtual ~CclFieldList() = default;
        
        //---API
            void initVar() override { var = dfValue; }
            bool loadVar( Value* data, Parser* ) override { bParsed = CclAdapter::parseCclList2Vector( data, this->name, var, valueFunct, bClearPrevious ); return bParsed; }
            void setValueFrom( const CclFieldBase* original ) override { var = static_cast<const CclFieldList<T>*>( original )->var; }
            void setDefaultFrom( const CclFieldBase* original ) override { dfValue = static_cast<const CclFieldList<T>*>( original )->dfValue; }
            void setDefaultFromAndInit( const CclFieldBase* original ) override { setDefaultFrom( original ); initVar(); }

        //---data
            std::vector<T>& var;
            ValueFunct<T> valueFunct;
            bool bClearPrevious;
            std::vector<T> dfValue;
        };

        template< typename T>
        struct CclFieldRecord : public CclFieldBase
        /* for dictionaries */
        {
            using VarType = T;

        //---ctor, dtor
            template< typename... Ts >
            CclFieldRecord( const std::string& name, T& var, Ts... dfArgs ) 
            : CclFieldBase( name, FieldType::RECORD ), var(var), dfValue( dfArgs... ) {;}
            
            virtual ~CclFieldRecord() = default;

        //---API
            void initVar() override {;}
            bool loadVar( Value* data, Parser* parser ) override;
            void fwCreateCclFields() override { var.createCclFields(); }
            void fwInitCclFields() override { var.initCclFields(); }

        //---data
            T& var;
            T dfValue;
        };



    //////////////////////////////////////////////////////////////////////////////// CCL ADAPTER ////////////////////////////////////////////////////////////////////////////

    //---ctor, dtor
        inline CclAdapter( const std::string& path )
        : scope( nullptr ), path( path ), parsingError( false ), bParsingDone( false ) {;}
        virtual ~CclAdapter() = default;

    //---get
        inline Scope* getScope() { return scope.get(); }
        inline const std::string& getPath() const { return path; }
        

    //---API
        void init();
        inline void destroy() { scope = nullptr; }

    private:
      //resources
        SP<Scope> scope; //ccl
      //options
        std::string path;
      //state
        bool parsingError;
        bool bParsingDone;
};



//////////////////////////////////////////////////////////////////////////////// BASE DATA ////////////////////////////////////////////////////////////////////////////
struct BaseData
/* virtual base class to all the Parser data elements */
{
    inline static const std::string DF_NOELEMENT = ""; 
    inline static const std::string UNDEFINED_ELEM_MSG = "undefined element: ";
    inline static const std::string LACKING_FIELD_MSG = "lacking mandatory field ";
    inline static const std::string WRONG_VAL_MSG = "wrong enum value: "; 
	inline static const std::string WRONG_SUBTYPE_MSG = "wrong subtype of: "; 


//---ctor, dtor
    virtual ~BaseData() = default;

//---get
    inline Parser* getEnclosingParser() const { return enclosingParser; }
    inline size_t getCclFieldNum() const { return cclFields.size(); }
    CclAdapter::CclFieldBase* getCclFieldByName( const std::string& fieldName ) { for( auto field : cclFields ) { if( field->name == fieldName ) return field.get(); } return nullptr; }

//---set
    inline void setEnclosingParser( Parser* xParser ) { enclosingParser = xParser; }

//---API
  //create fields
    template< typename T, typename... Ts >
    inline void addCclFieldSimple( const std::string& name, T& var, Ts... dfArgs ) { cclFields.push_back( std::make_shared< CclAdapter::CclFieldSimple<T> >( name, var, dfArgs... ) ); }
    template< typename T > 
    inline void addCclFieldList( const std::string& name, std::vector<T>& var, bool bClearPrevious, const std::vector<T>& dfValue ) { cclFields.push_back( std::make_shared< CclAdapter::CclFieldList<T> >( name, var, bClearPrevious, dfValue ) ); }
    template< typename T, typename... Ts >
    inline void addCclFieldRecord( const std::string& name, T& var, Ts... dfArgs ) { cclFields.push_back( std::make_shared< CclAdapter::CclFieldRecord<T> >( name, var, dfArgs... ) ); }

    virtual void createCclFields( const std::string& dfName = DF_NOELEMENT ) { cclFields.clear(); addCclFieldSimple( "name", name, dfName ); }
    inline void fwCreateCclFields() { for( auto field : cclFields ) field->fwCreateCclFields(); }
    inline void initCclFields() { if ( bInitCcl == false ) return; for( auto field : cclFields ) { field->initVar(); } }
    inline void loadCclFields( Value* data, Parser* parser = nullptr ) { for( auto field : cclFields ) { field->loadVar( data, parser ); } }
                                                                                                            //if( name == BaseData::DF_NONAME ) name = makeIdxName<T>(); }
  //common interface
    virtual void preprocess() {;}
    virtual void loadFirst( World* ) {;};
    virtual void loadSecond( World* ) {;};
    virtual void loadLast( World* ) {;};
    
  //misc
    inline void copyCclFieldsValues( const std::vector<CclAdapter::CclFieldBase*>& originalFields ) { for( size_t f = 0; f < cclFields.size(); f++ ) cclFields[f]->setValueFrom( originalFields[f] ); }
    inline void copyCclFieldsDefaults( const std::vector<CclAdapter::CclFieldBase*>& originalFields ) {  for( size_t f = 0; f < cclFields.size(); f++ ) cclFields[f]->setDefaultFrom( originalFields[f] ); }
    inline void copyCclFieldsDefaultsAndInit( const std::vector<CclAdapter::CclFieldBase*>& originalFields ) { copyCclFieldsDefaults( originalFields ); initCclFields(); }
    inline void setCclFieldName( const std::string newName, size_t idx = 1 ) { cclFields[idx]->name = newName; }

  //name 2 elem (called from load)
    template< typename TElem, typename TCollection >
    const TElem* name2elem( const std::string& name, const TCollection* collection, const std::string& parentName, bool bMandatory = true, bool bThrow = true );

    template< typename TElem, typename TCollection >
    const TElem* name2elemGeneric( const std::string& name, const TCollection* collection, const std::string& parentName, bool bMandatory = true, bool bThrow = true );

    template< typename TElem, typename TCollection >
    std::vector<const TElem*> names2elems( const std::vector<std::string>& names, const TCollection* collection, const std::string& parentName, bool bMandatory = false, bool bThrow = true );

    template< typename TElem, typename TCollection >
    std::vector<const TElem*> names2elemsGeneric( const std::vector<std::string>& names, const TCollection* collection, const std::string& parentName, bool bMandatory = false, bool bThrow = true );

  //---
    template< typename TFactory >
    typename TFactory::SubtypeIdx name2val( std::string& name, const std::string& parentName = "", bool bLowerCase = true, bool bThrow = true );

    template< typename TVal >
    TVal name2val( std::string& name, const ut::NameMap<TVal>& nameMap, const std::string& parentName = "", bool bLowerCase = true, bool bThrow = true );


    template< typename TFactory >
    std::vector<typename TFactory::SubtypeIdx> names2vals( std::vector<std::string>& names, const std::string& parentName = "", bool bLowerCase = true, bool bThrow = true );

    template< typename TVal >
    std::vector<TVal> names2vals( std::vector<std::string>& names, const ut::NameMap<TVal>& nameMap, const std::string& parentName = "", bool bLowerCase = true, bool bThrow = true );

  //---
    template< typename TElem, typename TFun, typename TEnum >
    void checkSubtype( const TElem* elem, TFun fun, TEnum val );

    template< typename TElem, typename TFun, typename TEnum >
    void checkSubtypes( const std::vector<const TElem*>& elems, TFun fun, TEnum val );

//---data
  //resources
    BaseData* parent; //in case it is a nested field of the record type, nullptr otherwise
    Parser* enclosingParser;
    std::vector< SP<CclAdapter::CclFieldBase> > cclFields; 
  //options
    std::string name; //name used in the .gro file
    bool bLoadCcl;
    bool bInitCcl;


    protected:
        BaseData( bool bLoadCcl = true, bool bInitCcl = true ) : enclosingParser( nullptr ), bLoadCcl( bLoadCcl ), bInitCcl( bInitCcl ) { addCclFieldSimple( "name", name, DF_NOELEMENT ); }
        BaseData( const std::string& name, bool bLoadCcl = true, bool bInitCcl = true ) : enclosingParser( nullptr ), name(name), bLoadCcl(bLoadCcl), bInitCcl(bInitCcl) {;}
};




////////////////////////////////////////////////////// METHOD DEFINITIONS ////////////////////////////////////////////////////////////////////////////////////

//=========================================== FIELDS
template< typename T>
bool CclAdapter::CclFieldRecord<T>::loadVar( Value* data, Parser* parser )
{ 
    Value* recordField = nullptr;
    bool controlDelete = false;
    if( data->getField( name.c_str() ) && data->getField( name.c_str() )->get_type() == Value::RECORD )
       recordField = data->getField( name.c_str() );
    else
    {
        recordField = new Value( Value::TYPE::RECORD );
        controlDelete = true;
    }
    var.loadCclFields( recordField, parser );
    if( controlDelete )
        delete recordField;
    bParsed = ! controlDelete;
    return bParsed;
}


//======================================== CCL ADAPTER
template< typename T >
bool CclAdapter::parseCclField( Value* data, const std::string& name, T& var, Value::TYPE valueType, T (Value::* valueFunct ) ( void ) ) 
{
    if( data->getField( name.c_str() ) && data->getField( name.c_str() )->get_type() == valueType )
    {
        var = ( data->getField( name.c_str() )->*valueFunct )();
        return true;
    }
    return false;
}

template< typename T >
bool CclAdapter::parseCclFieldDf( Value* data, const std::string& name, T& var, Value::TYPE valueType, T (Value::* valueFunct ) ( void ), T dfValue )
{
    bool bfieldExists = parseCclField( data, name, var, valueType, valueFunct );
    if( ! bfieldExists )
        var = dfValue;
    return bfieldExists;
} 

template< typename T> 
 bool CclAdapter::parseCclList2Vector( Value* data, const std::string& name, std::vector<T>& var, T (Value::* valueFunct ) ( void ), bool bClearPrevious )
{
    if( data->getField( name.c_str() ) && data->getField( name.c_str() )->get_type() == Value::LIST )
    {
        std::list<Value*>* valuesList = data->getField( name.c_str() )->list_value();
        if( valuesList != NULL && ! valuesList->empty() )
        {
            if( bClearPrevious )
                var.clear();
            for( auto i = valuesList->begin(); i != valuesList->end(); i++ )
            {
                var.push_back(  ( (*i)->*valueFunct )() );
            }
        }
        return true;
    }
    return false;
}

template< typename T> 
bool CclAdapter::parseCclList2VectorDf( Value* data, const std::string& name, std::vector<T>& var, T (Value::* valueFunct ) ( void ), bool bClearPrevious, const std::vector<T>& dfValue )
{
    bool bfieldExists = parseCclList2Vector( data, name, var, valueFunct, bClearPrevious );
    if( ! bfieldExists )
        var = dfValue;
    return bfieldExists;
}


//========================================================= BASE DATA
template< typename TElem, typename TCollection >
const TElem* BaseData::name2elem( const std::string& name, const TCollection* collection, const std::string& parentName, bool bMandatory, bool bThrow ) 
{ 
    const TElem* elem = collection->template getByName<TElem>( name );  
    if( ! elem && bThrow )
    {
        if( name != DF_NOELEMENT )
            ut::throwMessage( UNDEFINED_ELEM_MSG + name + " at " + parentName );
        else if( bMandatory )
            ut::throwMessage( LACKING_FIELD_MSG + "at " + parentName );
    }
    return elem;
}

template< typename TElem, typename TCollection >
const TElem* BaseData::name2elemGeneric( const std::string& name, const TCollection* collection, const std::string& parentName, bool bMandatory, bool bThrow ) 
{ 
    const TElem* elem = collection->getByName( name );  
    if( ! elem && bThrow )
    {
        if( name != DF_NOELEMENT )
            ut::throwMessage( UNDEFINED_ELEM_MSG + name + " at " + parentName );
        else if( bMandatory  )
            ut::throwMessage( LACKING_FIELD_MSG + "at " + parentName );
    }
    return elem;
}

template< typename TElem, typename TCollection >
std::vector<const TElem*> BaseData::names2elems( const std::vector<std::string>& names, const TCollection* collection, const std::string& parentName, bool bMandatory, bool bThrow )
{
    std::vector<const TElem*> elems;
    for( auto name : names )
        elems.push_back( name2elem<TElem>( name, collection, parentName, bMandatory, bThrow ) );
    return elems;
}

template< typename TElem, typename TCollection >
std::vector<const TElem*> BaseData::names2elemsGeneric( const std::vector<std::string>& names, const TCollection* collection, const std::string& parentName, bool bMandatory, bool bThrow )
{
    std::vector<const TElem*> elems;
    for( auto name : names )
        elems.push_back( name2elemGeneric<TElem>( name, collection, parentName, bMandatory, bThrow ) );
    return elems;
}


//---
template< typename TFactory >
typename TFactory::SubtypeIdx BaseData::name2val( std::string& name, const std::string& parentName, bool bLowerCase, bool bThrow )
{
    if( bLowerCase )
        std::transform( name.begin(), name.end(), name.begin(), ::tolower );

    typename TFactory::SubtypeIdx val = TFactory::name2idx( name );

    if( bThrow && val == TFactory::SubtypeIdx::COUNT )
        ut::throwMessage( WRONG_VAL_MSG + name + " at " + parentName );
    return val;
}

template< typename TVal >
TVal BaseData::name2val( std::string& name, const ut::NameMap<TVal>& nameMap, const std::string& parentName, bool bLowerCase, bool bThrow )
{
    if( bLowerCase )
        std::transform( name.begin(), name.end(), name.begin(), ::tolower );

    TVal val = nameMap.name2Subtype( name );

    if( bThrow && val == TVal::COUNT )
        ut::throwMessage( WRONG_VAL_MSG + name + " at " + parentName );
    return val;
}

template< typename TFactory >
std::vector<typename TFactory::SubtypeIdx> BaseData::names2vals( std::vector<std::string>& names, const std::string& parentName, bool bLowerCase, bool bThrow )
{
    std::vector<typename TFactory::SubtypeIdx> vals;
    for( auto& name : names )
        vals.push_back( name2val<TFactory>( name, parentName, bLowerCase, bThrow ) );
    return vals;
}

template< typename TVal >
std::vector<TVal> BaseData::names2vals( std::vector<std::string>& names, const ut::NameMap<TVal>& nameMap, const std::string& parentName, bool bLowerCase, bool bThrow )
{
    std::vector<TVal> vals;
    for( auto& name : names )
        vals.push_back( name2val( name, nameMap, parentName, bLowerCase, bThrow ) );
    return vals;
}

template< typename TElem, typename TFun, typename TEnum >
void BaseData::checkSubtype( const TElem* elem, TFun fun, TEnum val )
{
	if( ( elem->*fun )() != val )
		ut::throwMessage( WRONG_SUBTYPE_MSG + elem->getName() + " at " + name );

}

template< typename TElem, typename TFun, typename TEnum >
void BaseData::checkSubtypes( const std::vector<const TElem*>& elems, TFun fun, TEnum val )
{
	for( const TElem* elem : elems )
		checkSubtype( elem, fun, val );
}


#endif //_CCL_ADAPTER_H_
