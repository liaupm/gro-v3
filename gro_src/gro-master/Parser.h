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

#ifndef _PARSER_H_
#define _PARSER_H_

#include "ut/NameMap.hpp" //enums to str names
#include "ut/DistributionInterface.hpp" //Factory subtype to name
#include "ut/GateBoolean.hpp" //type to name
#include "ut/GateQuantitative.hpp" //operator to name
#include "ut/MathFunction.hpp" //operator NM
#include "cg/GeneticCollection.hpp"
#include "mg/MediumCollection.hpp"

#include "Defines.h"
#include "GroCollection.h"
#include "GlobalParams.h" //defaults, not in GroCollection
#include "Theme.h" //not in GroCollection
#include "CclAdapter.h" //create and load fields, base

//PRECOMPILED
/*#include <vector> //many
#include <map> //parsedData and string fields
#include <string> //names, to_string
#include <limits> //numeric_limits::lowest() and max() for default values
#include <utility> //std::move 
#include <type_traits> //std::enable_if_t, is_same
#include <memory> //make_unique, make_shared */


////////////////////////////////////////////////////////////////////////////////
class World; //param of load methods
class Parser
{
    public:
        template< typename T > 
        using ValueFunct = CclAdapter::ValueFunct<T>;

        enum AllElementIdx : size_t
        { 
            DISTRIBUTION, BMARKER, QMARKER, EVENT_MARKER, RANDOMNESS, GATE, FUNCTION, ODE, HISTORIC
            , MOLECULE, REGULATION, OPERON
            , PILUS, ORI_T, PARTITION_SYSTEM, COPY_CONTROL, ORI_V, PLASMID
            , MUTATION, MUTATION_PROCESS, STRAIN, CELL_TYPE, SIGNAL, GRID, METABOLITE, FLUX, CELL_COLOUR
            , POP_QMARKER, POP_STAT, POP_FUNCTION, POP_GATE, TIMER, CHECKPOINT, CELL_PLACER, CELL_PLATING, SIGNAL_PLACER, SNAPSHOT, CELLS_FILE, CELLS_PLOT, GLOBAL_PARAMS, COUNT
        };

        template< typename T >
        static size_t getNextIdx() { static size_t idx = 0; return idx++; }

        template < typename T >
        static std::string makeDfName( T*, const std::string& parentName="" ) { return std::string("_") + T::ClassType::metadata.name + "_" + std::to_string( getNextIdx<T>() ) + "_" + parentName; }

        template<typename T>
        std::string createAutoName( const std::string& baseName = "", const std::string& sufix = "" ) const;





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////// ELEMENT DATA DESCRIPTIONS //////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////// COMMON /////////////////////////////////////////////////////////////////////////////
        
        struct DistributionData : public BaseData
        {
            using ClassType = ut::ContinuousDistInterface;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::DISTRIBUTION;
            inline static const std::string DF_TYPE = "";

            DistributionData() = default;
            virtual ~DistributionData() = default;

            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "type", type, DF_TYPE );
                addCclFieldList( "params", params, false, {} );

                fwCreateCclFields();
                initCclFields();
            }

        //---data
            std::string type;
            std::vector<double> params;
        };

        struct OptionalDist
        {
            inline static const std::string DF_DIST_TYPE = "normal";
            inline static const std::string DF_INI_DIST_TYPE = "";
            inline static const std::vector<double> DF_PARAMS = {};
            static constexpr double DF_EXACT = std::numeric_limits<double>::lowest();
            inline static const std::string DF_DIST_REF = BaseData::DF_NOELEMENT;


            OptionalDist( const std::string& distType, const std::vector<double>& params ) 
            : dfParams( params ), distType( distType ), params( params ), exact( DF_EXACT ), distDict(), distRef( "" ) {;}

            ~OptionalDist() = default;

        //---data
            std::vector<double> dfParams;
          //load
            std::string distType;
            std::vector<double> params;
            double exact;
            DistributionData distDict;
            std::string distRef;

            void preprocess( const Parser* enclosingParser );
        };

        struct OptionalDistUser : public virtual BaseData
        {
            OptionalDistUser() = default;
            //OptionalDistUser( const std::string& name ) : BaseData( name, true, false ) {;}
            virtual ~OptionalDistUser() = default;

            void createCclFields( const std::string& = BaseData::DF_NOELEMENT ) override {;}

            void addOptionalDistFields( OptionalDist& optionalDist, const std::string& prefix )
            {
                addCclFieldSimple( prefix, optionalDist.exact, OptionalDist::DF_EXACT );
                addCclFieldList( prefix, optionalDist.params, true, optionalDist.dfParams );
                addCclFieldList( prefix + "_params", optionalDist.params, true, optionalDist.dfParams );
                addCclFieldRecord( prefix + "_dist", optionalDist.distDict );
                addCclFieldSimple( prefix + "_dist", optionalDist.distRef, "" );
            }
        };



        struct OptionalScalerData
        {
        	OptionalScalerData( const std::string& fun, const std::string& rnd, const std::string marker, const std::vector<double>& params ) 
            : fun( fun ), rnd( rnd ), marker( marker ), params( params ) {;}

        	~OptionalScalerData() = default;

        	std::string fun;
        	std::string rnd;
        	std::string marker;
            std::vector<double> params;
        };

        struct OptionalScaler
        {
        	inline static const std::string DF_RND = BaseData::DF_NOELEMENT;
        	inline static const std::string DF_MARKER = BaseData::DF_NOELEMENT;

            inline static const std::vector<double> DF_PARAMS = {};
            static constexpr double DF_EXACT = std::numeric_limits<double>::lowest();
            

            OptionalScaler( const BaseData& parent, const std::string& rnd, const std::vector<double>& params, const std::string marker ) 
            : parent( parent ), dfParams( params ), rnd( rnd ), params( params ), exact( DF_EXACT ), marker( marker ) {;}

            OptionalScaler( const BaseData& parent, const OptionalScalerData& data ) 
            : parent( parent ), dfParams( data.params ), rnd( rnd ), params( data.params ), exact( DF_EXACT ), marker( data.marker ) {;}

            OptionalScaler( const BaseData& parent, const std::vector<double>& dfParams ) 
            : parent( parent ), dfParams( dfParams ), rnd( DF_RND ), params( DF_PARAMS ), exact( DF_EXACT ), marker( DF_MARKER ) {;}

            ~OptionalScaler() = default;

        //---data
            const BaseData& parent;
         	std::string prefix;
         	//std::string dfDistType;
            std::vector<double> dfParams;
          //---
            std::string rnd;
            //std::string fun;
            std::vector<double> params;
            double exact;
            std::string marker;
            
            void preprocess( bool bCreateRnd );
        };


        struct OptionalScalerUser : public virtual BaseData
        {
            OptionalScalerUser() = default;
            OptionalScalerUser( const std::string& name ) : BaseData( name, true, false ) {;}
            virtual ~OptionalScalerUser() = default;

            void createCclFields( const std::string& = BaseData::DF_NOELEMENT ) override {;}

            void addFields( OptionalScaler& optionalScaler, const std::string& prefix )
            {
            	optionalScaler.prefix = prefix;
            	addCclFieldSimple( prefix + "_rnd", optionalScaler.rnd, OptionalScaler::DF_RND );
                addCclFieldSimple( prefix, optionalScaler.exact, OptionalScaler::DF_EXACT );
                addCclFieldList( prefix, optionalScaler.params, true, {} );
                addCclFieldList( prefix + "_params", optionalScaler.params, true, {} );
                addCclFieldSimple( prefix + "_marker", optionalScaler.marker, OptionalScaler::DF_MARKER );
            }
        };










//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// CELL LOGIC ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//============================================================================================================================== MARKERS
        struct BMarkerData : public BaseData
        {
            using ClassType = cg::BMarker;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::BMARKER;

            inline static const std::string AUTO_B_DIVISION_VOL_NAME = "_bm_division_vol";
            inline static const std::string AUTO_POLE_POS_NAME = "_bm_pole_pos";

        //---ctor, dtor
            BMarkerData( const std::string& name ) : BaseData( name, true, false ) {;}
            BMarkerData() = default;
            virtual ~BMarkerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
        };

        struct QMarkerData : public BaseData
        {
            using ClassType = cg::QMarker;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::QMARKER;

            inline static const std::string AUTO_VOL_NAME = "_qm_vol";
            inline static const std::string AUTO_DVOL_NAME = "_qm_dvol";
            inline static const std::string AUTO_GR_NAME = "_qm_gr";
            inline static const std::string AUTO_NEIGH_DISTANCE_NAME = "_qm_neigh_distance";
            inline static const std::string AUTO_POLE_COUNT_NAME = "_qm_pole_count";

        //---ctor, dtor
            QMarkerData( const std::string& name ) : BaseData( name, true, false ) {;}
            QMarkerData() = default;
            virtual ~QMarkerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
        };

        struct EventMarkerData : public BaseData
        {
            using ClassType = cg::EventMarker;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::EVENT_MARKER;

            inline static const std::string AUTO_DIVISION_NAME = "_em_division"; 

        //---ctor, dtor
            EventMarkerData( const std::string& name ) : BaseData( name, true, false ) {;}
            EventMarkerData() = default;
            virtual ~EventMarkerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
        };





//============================================================================================================================== RANDOMNESS

        struct RandomnessData : public BaseData
        {
            using ClassType = cg::Randomness;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::RANDOMNESS;
            inline static const std::string DF_DIST_TYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_DIST_TYPE );


        //---ctor, dtor
            RandomnessData( const std::string& name, const std::string& distributionType, const std::vector<double>& params
            , double inheritance, double inertia, double samplingTime = ClassType::DF_SAMPLING_PERIOD, bool bRndSamplingTime = ClassType::DF_B_RND_SAMPLING_TIME, bool bCorrection = ClassType::DF_B_CORRECTION )
            : BaseData( name, true, false )
            , distributionType( distributionType ), params( params )
            , inheritance( inheritance ), inertia( inertia ), samplingTime( samplingTime ), bRndSamplingTime( bRndSamplingTime ), bCorrection( bCorrection ) {;}

            RandomnessData( const std::string& name )
            : BaseData( name, true, false )
            , distributionType( DF_DIST_TYPE ), params( ClassType::DF_PARAMS )
            , inheritance( ClassType::DF_INHERITANCE ), inertia( ClassType::DF_INERTIA ), samplingTime( ClassType::DF_SAMPLING_PERIOD ), bRndSamplingTime( ClassType::DF_B_RND_SAMPLING_TIME ), bCorrection( ClassType::DF_B_CORRECTION ) {;}

            RandomnessData() = default;
            virtual ~RandomnessData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "dist_type", distributionType, DF_DIST_TYPE );
                addCclFieldList( "params", params, true, ClassType::DF_PARAMS );
                
                addCclFieldSimple( "inheritance", inheritance, ClassType::DF_INHERITANCE );
                addCclFieldSimple( "inertia", inertia, ClassType::DF_INERTIA );
                addCclFieldSimple( "sampling_time", samplingTime, ClassType::DF_SAMPLING_PERIOD );
                addCclFieldSimple( "rnd_sampling", bRndSamplingTime, ClassType::DF_B_RND_SAMPLING_TIME );
                addCclFieldSimple( "correction", bCorrection, ClassType::DF_B_CORRECTION );
                
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;

        //---data
            std::string distributionType;
            std::vector<double> params;
            
            double inheritance;
            double inertia;
            double samplingTime;
            bool bRndSamplingTime;
            bool bCorrection;
        };




//================================================================================================================================================= GATES

        struct BGateData : public BaseData
        {
            using ClassType = cg::GateBoolean;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::GATE;

            inline static const std::string DF_FUNCTION = ut::GateBoolean::functionNM.subtype2Name( cg::GateBoolean::DF_FUNCTION );
            inline static const std::vector<std::string> DF_INPUT_NAMES_RAW = {};

            inline static const std::string AUTO_TRUE_NAME = "_ga_true";
            inline static const std::string AUTO_FALSE_NAME = "_ga_false";


        //---ctor, dtor
            BGateData( const std::string& name, const std::string& gateFunction = DF_FUNCTION, const std::vector<std::string>& inputNamesRaw = DF_INPUT_NAMES_RAW )
            : BaseData( name, true, false )
            , gateFunction( gateFunction ), inputNamesRaw( inputNamesRaw ) {;}

            BGateData() = default;
            virtual ~BGateData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "type", gateFunction, DF_FUNCTION );
                addCclFieldList( "input", inputNamesRaw, false, DF_INPUT_NAMES_RAW );
                addCclFieldList( "inputs", inputNamesRaw, false, DF_INPUT_NAMES_RAW );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
          //load
            std::string gateFunction;
            std::vector<std::string> inputNamesRaw;
          //preprocess
            std::vector<std::string> inputNames;
            std::vector<bool> inputPresence;
        };


        struct QGateData : public BaseData
        {
            using ClassType = cg::GateQuantitative;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::GATE;

            inline static const std::string DF_ELEM = BaseData::DF_NOELEMENT;
            inline static const std::string DF_COMP_OPERATOR = ut::GateQuantitative::compOperatorNM.subtype2Name( ut::GateQuantitative::DF_COMP_OPERATOR );


        //---ctor, dtor
            QGateData( const std::string& name, const std::string& elem, const std::string& compOperator = DF_COMP_OPERATOR, double value = ClassType::DF_VALUE )
            : BaseData( name )
            , elem( elem ), compOperator( compOperator ), value(value) {;}
            

            QGateData() = default;
            virtual ~QGateData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "elem", elem, DF_ELEM );
                addCclFieldSimple( "input", elem, DF_ELEM );
                addCclFieldSimple( "type", compOperator, DF_COMP_OPERATOR );
                addCclFieldSimple( "operator", compOperator, DF_COMP_OPERATOR );
                addCclFieldSimple( "value", value, ClassType::DF_VALUE );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string elem;
            std::string compOperator;
            double value;
        };




//============================================================================================================================== QUANTITATIVE
        struct FunctionData : public BaseData
        {
            using ClassType = cg::Function;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::FUNCTION;

            inline static const std::string DF_TYPE = ClassType::MathFunctionType::FactoryType::idx2name( ClassType::DF_FUNTION_TYPE );
            inline static const std::vector<std::string> DF_INPUT = {};
            inline static const std::vector<double> DF_PARAMS = {};
            inline static const std::string DF_RND = DF_NOELEMENT;
            inline static const std::vector<double> DF_RND_PARAMS = {};
            inline static const std::string DF_AUTO_VOL = cg::Function::autoVolModeNM.subtype2Name( cg::Function::DF_AUTO_VOL_MODE );


        //---ctor, dtor
            FunctionData( const std::string& name, const std::string& type, const std::vector<std::string>& input, const std::vector<double>& params
            , const std::string& randomness = DF_RND, const std::vector<double>& rndParams = DF_RND_PARAMS, const std::string& autoVolMode = DF_AUTO_VOL, double min = ClassType::DF_MIN, double max = ClassType::DF_MAX, int roundPlaces = ClassType::DF_ROUND_PLACES )
            : BaseData( name, true, false )
            , type( type ), input( input ), params( params )
            , randomness( randomness ), rndParams( rndParams ), autoVolMode( autoVolMode ), min( min ), max( max ), roundPlaces( roundPlaces ) {;}

            FunctionData( const std::string& name )
            : BaseData( name, true, false )
            , type( DF_TYPE ), input( DF_INPUT ), params( DF_PARAMS )
            , randomness( DF_RND ), rndParams( DF_RND_PARAMS ), autoVolMode( DF_AUTO_VOL ), min( ClassType::DF_MIN ), max( ClassType::DF_MAX ), roundPlaces( ClassType::DF_ROUND_PLACES ) {;}

            FunctionData() = default;
            virtual ~FunctionData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "type", type, DF_TYPE );
                addCclFieldList( "inputs", input, false, DF_INPUT );
                addCclFieldList( "input", input, false, DF_INPUT );
                addCclFieldList( "params", params, true, DF_PARAMS );

                addCclFieldSimple( "randomness", randomness, DF_RND );
                addCclFieldSimple( "rnd", randomness, DF_RND );
                addCclFieldList( "rnd_params", rndParams, true, DF_RND_PARAMS );

                addCclFieldSimple( "auto_vol", autoVolMode, DF_AUTO_VOL );
                addCclFieldSimple( "min", min, ClassType::DF_MIN );
                addCclFieldSimple( "max", max, ClassType::DF_MAX );
                addCclFieldSimple( "precision", roundPlaces, ClassType::DF_ROUND_PLACES );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string type;
            std::vector<std::string> input;
            std::vector<double> params;

            std::string randomness;
            std::vector<double> rndParams;

            std::string autoVolMode;
            double min;
            double max;
            int roundPlaces;
        };


        struct OdeData : public OptionalScalerUser, public OptionalDistUser
        {
            using ClassType = cg::Ode;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::ODE;

            inline static const std::string DF_DELTA_ELEM = DF_NOELEMENT;
            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_INI_VALUE_FUN = DF_NOELEMENT;
            inline static const std::string DF_INI_VALUE_DIST_TYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_INI_VALUE_DISTTYPE );
            //inline static const std::string DF_SPLIT_DIST_TYPE = ut::ContinuousDistFactory::idx2name( ClassType::PartitionContinuous::DF_FRACTION_DISTTYPE );
            inline static const std::string DF_SPLIT_FUN = DF_NOELEMENT;
            inline static const std::string DF_SPLIT_RND = DF_NOELEMENT;
            inline static const std::string DF_SPLIT_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_SPLIT_DATA = OptionalScalerData( DF_SPLIT_FUN, DF_SPLIT_RND, DF_SPLIT_MARKER, ClassType::DF_PARTITION_PARAMS );

           
        //---ctor, dtor
            OdeData( const std::string& name, const std::string& deltaElem, const std::string& gate = DF_GATE, const std::string& iniValueFunction = DF_INI_VALUE_FUN
            , const std::string& iniValueDistType = DF_INI_VALUE_DIST_TYPE, const std::vector<double>& iniValueParams = ClassType::DF_INI_VALUE_PARAMS, double scale = ClassType::DF_SCALE
            , const OptionalScalerData& splitData = DF_SPLIT_DATA
            , double splitScale = ClassType::DF_PARTITION_SCALE, bool bBinomialSplit = ClassType::DF_B_BINOMIAL_SPLIT, bool bVolumeScalingSplit = ClassType::DF_B_VOLUME_SCALING_PARTITION )
            
            : BaseData( name, true, false ), split( *this, splitData.rnd, splitData.params, splitData.marker ), iniValue( iniValueDistType, iniValueParams )
            , deltaElem( deltaElem ), gate( gate ), iniValueFunction( iniValueFunction ), splitFunction( splitData.fun )
            , scale( scale ), splitScale( splitScale ), bBinomialSplit( bBinomialSplit ), bVolScalingSplit( bVolumeScalingSplit ) {;}

            OdeData() : BaseData(), split( *this, ClassType::DF_PARTITION_PARAMS ), iniValue( DF_INI_VALUE_DIST_TYPE, ClassType::DF_INI_VALUE_PARAMS ) {;}
            virtual ~OdeData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( split, "split" );
                OptionalDistUser::addOptionalDistFields( iniValue, "ini_value" );

                addCclFieldSimple( "elem", deltaElem, DF_DELTA_ELEM );
                addCclFieldSimple( "delta_input", deltaElem, DF_DELTA_ELEM );
                addCclFieldSimple( "input", deltaElem, DF_DELTA_ELEM );
                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "ini_value_fun", iniValueFunction, DF_INI_VALUE_FUN );
                addCclFieldSimple( "split_fun", splitFunction, DF_SPLIT_FUN );
                
                addCclFieldSimple( "scale", scale, ClassType::DF_SCALE );
                addCclFieldSimple( "split_scale", splitScale, ClassType::DF_PARTITION_SCALE );
                addCclFieldSimple( "binomial_split", bBinomialSplit, ClassType::DF_B_BINOMIAL_SPLIT );
                addCclFieldSimple( "volume_split", bVolScalingSplit, ClassType::DF_B_VOLUME_SCALING_PARTITION );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data  
            OptionalScaler split;
            OptionalDist iniValue;

            std::string deltaElem;
            std::string gate;
            std::string iniValueFunction;
            std::string splitFunction;

            double scale;
            double splitScale;
            bool bBinomialSplit;
            bool bVolScalingSplit;
        };


        struct HistoricData : public BaseData
        {
            using ClassType = cg::Historic;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::HISTORIC;

            inline static const std::string DF_TARGET = DF_NOELEMENT;
            inline static const std::string DF_INI_VALUE_ELEM = DF_NOELEMENT;


        //---ctor, dtor
            HistoricData( const std::string& name, const std::string& target, const std::string& iniValue = DF_INI_VALUE_ELEM, double delay = ClassType::DF_DELAY )
            : BaseData( name, true, false ), target( target ), iniValue( iniValue ), delay( delay ) {;}

            HistoricData() = default;
            virtual ~HistoricData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "target", target, DF_TARGET );
                addCclFieldSimple( "ini_value_fun", iniValue, DF_INI_VALUE_ELEM );
                addCclFieldSimple( "delay", delay, ClassType::DF_DELAY );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string target;
            std::string iniValue;
            double delay;
        };



//=================================================================================== VISUALS
        
        struct CellColourData : public OptionalScalerUser
        {
            using ClassType = cg::CellColour;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELL_COLOUR;

            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::vector<double> DF_RGB_COLOUR = { 0.0, 1.0, 0.0 }; //green
            inline static const std::string DF_MODE_STR = cg::CellColour::colourModeNM.subtype2Name( cg::CellColour::DF_MODE );

            inline static const std::string DF_MAXVALUE_RND = DF_NOELEMENT;
            inline static const std::string DF_DELTA_RND = DF_NOELEMENT;
            inline static const std::string DF_TARGET = DF_NOELEMENT;


        //---ctor, dtor
            CellColourData( const std::string& name, const std::string& gate, const std::vector<double>& rgbColour = DF_RGB_COLOUR, const std::string& colourMode = DF_MODE_STR
            , const std::string& maxValueRnd = DF_MAXVALUE_RND, const std::vector<double>& maxValueParams = ClassType::DF_MAX_VALUE_PARAMS, const std::string& deltaRnd = DF_DELTA_RND, const std::vector<double>& deltaParams = ClassType::DF_DELTA_PARAMS
            , const std::string& target = DF_TARGET, double scale = ClassType::DF_SCALE ) 
            
            : BaseData( name ), OptionalScalerUser(), maxValue( *this, maxValueRnd, maxValueParams, DF_NOELEMENT ), delta( *this, deltaRnd, deltaParams, DF_NOELEMENT )
            , gate( gate ), target( target ), rgbColour( rgbColour ), colourMode( colourMode ), scale( scale ) {;}

            CellColourData() : BaseData(), OptionalScalerUser(), maxValue( *this, ClassType::DF_MAX_VALUE_PARAMS ), delta( *this, ClassType::DF_DELTA_PARAMS ) {;}
            virtual ~CellColourData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( maxValue, "intensity" );
                OptionalScalerUser::addFields( delta, "delta" );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "target", target, DF_TARGET ); 

                addCclFieldList( "rgb", rgbColour, true, DF_RGB_COLOUR );
                addCclFieldSimple( "mode", colourMode, DF_MODE_STR );
                addCclFieldSimple( "scale", scale, ClassType::DF_SCALE ); 

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler maxValue;
            OptionalScaler delta;
            
            std::string gate;
            std::string target;

            std::vector<double> rgbColour;
            std::string colourMode;
            double scale;
        };



//

//

//

//

//

//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// CELL BIO ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//====================================================================================================================EXPRESSION

        struct MoleculeData : public BaseData
        {
            using ClassType = cg::Molecule;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::MOLECULE;

            inline static const std::string DF_RANDOMNESS = DF_NOELEMENT;

            inline static const std::vector<std::string> DF_TIME_FUNS = {};
            inline static const std::vector<double> DF_MEAN_TIMES = ut::castVector<double>( ClassType::DF_MEAN_TIMES );
            inline static const std::vector<double> DF_VAR_TIMES = ut::castVector<double>( ClassType::DF_VAR_TIMES );
            inline static const std::vector<double> DF_THRESHOLDS = ut::castVector<double>( ClassType::DF_THRESHOLDS );
            inline static const std::string DF_CUSTOM_RND_SYMMETRY = DF_NOELEMENT;


        //---ctor, dtor
            MoleculeData( const std::string& name, const std::string& randomness = DF_RANDOMNESS, const std::vector<std::string>& timeFuns = DF_TIME_FUNS, const std::vector<int>& timeFunsSigns = {}
            , const std::vector<double>& meanTimes = DF_MEAN_TIMES, const std::vector<double>& varTimes = DF_VAR_TIMES, const std::vector<double>& activations = DF_THRESHOLDS, const std::string& customRndSymmetry = DF_CUSTOM_RND_SYMMETRY, bool bSymmetricRnd = ClassType::DF_SYMMETRIC_RND )
            : BaseData( name, true, false )
            , randomness( randomness ), customRndSymmetry( customRndSymmetry), meanTimes( meanTimes ), varTimes( varTimes ), activations( activations ), bSymmetricRnd( bSymmetricRnd )
            , timeFuns( timeFuns ), timeFunsSigns( timeFunsSigns ){;}
            
            MoleculeData() = default;
            virtual ~MoleculeData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "randomness", randomness, DF_RANDOMNESS );
                addCclFieldSimple( "rnd", randomness, DF_RANDOMNESS );
                addCclFieldSimple( "degradation_rnd", customRndSymmetry, DF_CUSTOM_RND_SYMMETRY );
                addCclFieldList( "times_funs", timeFunsRaw, true, DF_TIME_FUNS );
                addCclFieldList( "time_funs", timeFunsRaw, true, DF_TIME_FUNS );

                addCclFieldList( "times_means", meanTimes, true, DF_MEAN_TIMES );  
                addCclFieldList( "times_mean", meanTimes, true, DF_MEAN_TIMES );
                addCclFieldList( "time_means", meanTimes, true, DF_MEAN_TIMES );
                addCclFieldList( "times", meanTimes, true, DF_MEAN_TIMES );

                addCclFieldList( "times_vars", varTimes, true, DF_VAR_TIMES );
                addCclFieldList( "times_var", varTimes, true, DF_VAR_TIMES );
                addCclFieldList( "time_vars", varTimes, true, DF_VAR_TIMES );

                addCclFieldList( "thresholds", activations, true, DF_THRESHOLDS );
                addCclFieldSimple( "symmetric_rnd", bSymmetricRnd, ClassType::DF_SYMMETRIC_RND );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string randomness;
            std::string customRndSymmetry;

            std::vector<std::string> timeFunsRaw;
            std::vector<double> meanTimes;
            std::vector<double> varTimes;
            std::vector<double> activations;
            bool bSymmetricRnd;

          //preprocess
            std::vector<std::string> timeFuns;
            std::vector<int> timeFunsSigns;
        };


//----------------------------------------------------------- Regulation
        struct RegulationData : public OptionalScalerUser
        {
            using ClassType = cg::Regulation;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::REGULATION;

            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_DOSAGE_FUN = DF_NOELEMENT;
            inline static const std::string DF_ON_FUN = DF_NOELEMENT;
            inline static const std::string DF_OFF_FUN = DF_NOELEMENT;
            inline static const std::string DF_ACTI_MARKER = DF_NOELEMENT;
            inline static const std::string DF_RANDOMNESS = DF_NOELEMENT;
            inline static const std::vector<double> DF_ON_ACTI_PARAMS = ut::castVector<double>( ClassType::DF_ON_ACTI_PARAMS );
            inline static const std::vector<double> DF_OFF_ACTI_PARAMS = ut::castVector<double>( ClassType::DF_OFF_ACTI_PARAMS );


        //---ctor, dtor
            RegulationData( const std::string& name, const std::string& gate = DF_GATE, const std::vector<double>& onActiParams = DF_ON_ACTI_PARAMS, const std::vector<double>& offActiParams = DF_OFF_ACTI_PARAMS, const std::string& dosageFun = DF_DOSAGE_FUN
            , const std::string& onFunction = DF_ON_FUN, const std::string& offFunction = DF_OFF_FUN
            , const std::string& actiMarker = DF_ACTI_MARKER, const std::string& randomness = DF_RANDOMNESS, bool bAutoDivision = ClassType::DF_B_AUTO_DIVISION )
            
            : BaseData( name ), OptionalScalerUser(), onActi( *this, DF_NOELEMENT, onActiParams, DF_NOELEMENT ), offActi( *this, DF_NOELEMENT, offActiParams, DF_NOELEMENT )
            , gate( gate ), dosageFun( dosageFun ), onFunction( onFunction ), offFunction( offFunction ), actiMarker( actiMarker ), randomness( randomness ), bAutoDivision( bAutoDivision ) {;}


            RegulationData() : BaseData(), OptionalScalerUser(), onActi( *this, ClassType::DF_ON_ACTI_PARAMS ), offActi( *this, ClassType::DF_OFF_ACTI_PARAMS ) {;}
            virtual ~RegulationData() = default; 

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( onActi, "acti_on" );
                OptionalScalerUser::addFields( offActi, "acti_off" );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "dosage_fun", dosageFun, DF_DOSAGE_FUN );
                addCclFieldSimple( "on_fun", dosageFun, DF_DOSAGE_FUN );
                addCclFieldSimple( "off_fun", dosageFun, DF_DOSAGE_FUN );
                addCclFieldSimple( "acti_marker", actiMarker, DF_ACTI_MARKER );
                addCclFieldSimple( "randomness", randomness, DF_RANDOMNESS );
                addCclFieldSimple( "rnd", randomness, DF_RANDOMNESS );

                addCclFieldSimple( "auto_division", bAutoDivision, ClassType::DF_B_AUTO_DIVISION );
                
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler onActi;
            OptionalScaler offActi;

            std::string gate;
            std::string dosageFun;
            std::string onFunction;
            std::string offFunction;
            std::string actiMarker;
            std::string randomness;

            bool bAutoDivision;
        };




//-----------------------------------------------------operon
        struct OperonData : public BaseData
        {
            using ClassType = cg::Operon;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::OPERON;
 
            inline static const std::string DF_REGULATION = DF_NOELEMENT;
            inline static const std::string DF_GATE = RegulationData::DF_GATE;
            inline static const std::vector<std::string> DF_OUTPUT = {};
            inline static const std::string DF_ACTI_MARKER = DF_NOELEMENT;


        //---ctor, dtor
            OperonData( const std::string& name, const std::string& regulation = DF_REGULATION, const std::vector<std::string>& output = DF_OUTPUT, const std::string& actiMarker = DF_ACTI_MARKER, bool bDosageEffect = ClassType::DF_BDOSAGE ) 
            : BaseData( name, true, false ), regulation( regulation ), output( output ), actiMarker( actiMarker ), bDosageEffect( bDosageEffect ) {;}

            OperonData() = default;
            virtual ~OperonData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "regulation", regulation, DF_REGULATION );
                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldList( "output", output, false, DF_OUTPUT );
                addCclFieldSimple( "acti_marker", actiMarker, DF_ACTI_MARKER );

                addCclFieldSimple( "dosage_effect", bDosageEffect, ClassType::DF_BDOSAGE );
                addCclFieldSimple( "dosage", bDosageEffect, ClassType::DF_BDOSAGE );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            std::string regulation;
            std::string gate;
            std::vector<std::string> output;
            std::string actiMarker;

            bool bDosageEffect;
        };





//=========================================================================================================================== METABOLISM

        struct MetaboliteData : public BaseData
        {
            using ClassType = cg::Metabolite;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::METABOLITE;

            inline static const std::string AUTO_BIOMASS_NAME = "_biomass";
            inline static const std::string PREFIX = "_met";


        //---ctor, dtor
            MetaboliteData( const std::string& name, bool bBiomass = ClassType::DF_BBIOMASS ) : BaseData( name ), bBiomass( bBiomass ) {;}
            MetaboliteData() = default;
            virtual ~MetaboliteData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;

        //---data
            bool bBiomass;
        };


        struct FluxData : public OptionalScalerUser
        {
            using ClassType = cg::Flux;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::FLUX;

            inline static const std::string DF_METABOLITE = MetaboliteData::AUTO_BIOMASS_NAME;
            inline static const std::string DF_INTERNAL_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_SIGNALS_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::vector<std::string> DF_OUTPUT = {};

            inline static const std::string DF_AMOUNT_FUNCTION = DF_NOELEMENT;
            inline static const std::string DF_AMOUNT_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_AMOUNT_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_AMOUNT_DATA = OptionalScalerData( DF_AMOUNT_FUNCTION, DF_AMOUNT_RANDOMNESS, DF_AMOUNT_MARKER, ClassType::DF_AMOUNT_PARAMS );

            inline static const std::string DF_THRESHOLD_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_THRESHOLD_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_THRESHOLD_DATA = OptionalScalerData( DF_NOELEMENT, DF_THRESHOLD_RANDOMNESS, DF_THRESHOLD_MARKER, ClassType::DF_THRESHOLD_PARAMS );

            inline static const std::string DF_TIMING_TYPE = ClassType::timingTypeNM.subtype2Name( ClassType::DF_TIMING_TYPE );
            inline static const std::string DF_DIRECTION_STR = ClassType::directionNM.subtype2Name( ClassType::DF_DIRECTION );
            static constexpr int DF_DIRECTION = static_cast<int>( ClassType::DF_DIRECTION );

           
        //---ctor, dtor
            FluxData( const std::string& name, const std::string& metabolite = DF_METABOLITE, const std::string& internalGate = DF_INTERNAL_GATE, const std::string& signalsGate = DF_SIGNALS_GATE, const std::vector<std::string>& output = DF_OUTPUT
            , const OptionalScalerData& amountData = DF_AMOUNT_DATA, const OptionalScalerData& thresholdData = DF_THRESHOLD_DATA
            , const std::string& timingType = DF_TIMING_TYPE, double activation = ClassType::DF_ACTIVATION, int direction = DF_DIRECTION, bool bAbsolute = ! ClassType::DF_BRELATIVE, bool bInverted = ClassType::DF_B_INVERTED )
            
            : BaseData( name ), OptionalScalerUser(), amount( *this, amountData.rnd, amountData.params, amountData.marker ), threshold( *this, thresholdData.rnd, thresholdData.params, thresholdData.marker )
            , metabolite( metabolite ), internalGate( internalGate ), signalsGate( signalsGate ), amountFunction( amountData.fun ), output( output )
            , timingType( timingType ), activation( activation ), direction( direction ), bAbsolute( bAbsolute ), bInverted( bInverted ) {;}


            FluxData() : BaseData(), OptionalScalerUser(), amount( *this, ClassType::DF_AMOUNT_PARAMS ), threshold( *this, ClassType::DF_THRESHOLD_PARAMS ) {;}
            virtual ~FluxData() = default;
    
        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( amount, "amount" );
                OptionalScalerUser::addFields( threshold, "threshold" );

                addCclFieldSimple( "metabolite", metabolite, DF_METABOLITE );
                addCclFieldSimple( "signal", metabolite, DF_METABOLITE );
                addCclFieldSimple( "gate", internalGate, DF_INTERNAL_GATE );
                addCclFieldSimple( "internal_gate", internalGate, DF_INTERNAL_GATE );
                addCclFieldSimple( "signals_gate", signalsGate, DF_SIGNALS_GATE );
                addCclFieldSimple( "s_gate", signalsGate, DF_SIGNALS_GATE );
                addCclFieldSimple( "sgate", signalsGate, DF_SIGNALS_GATE );
                addCclFieldSimple( "amount_fun", amountFunction, DF_AMOUNT_FUNCTION );
                addCclFieldList( "output", output, false, DF_OUTPUT );
                
                addCclFieldSimple( "type", timingType, DF_TIMING_TYPE );
                addCclFieldSimple( "mode", timingType, DF_TIMING_TYPE );
                addCclFieldSimple( "activation", activation, ClassType::DF_ACTIVATION );
                addCclFieldSimple( "direction", direction, DF_DIRECTION );
                addCclFieldSimple( "direction", directionStr, DF_DIRECTION_STR );
                addCclFieldSimple( "absolute", bAbsolute, ! ClassType::DF_BRELATIVE );
                addCclFieldSimple( "inverted", bInverted, ClassType::DF_B_INVERTED );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler amount;
            OptionalScaler threshold;

            std::string metabolite;
            std::string internalGate;
            std::string signalsGate;
            std::string amountFunction;
            std::vector<std::string> output;

            std::string timingType;
            double activation;
            int direction;
            std::string directionStr;
            bool bAbsolute;
            bool bInverted;
        };





//===================================================================================================================== REPLICATION

        struct CopyControlData : public OptionalScalerUser
        {
            using ClassType = cg::CopyControl;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::COPY_CONTROL;

            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_ACTIVE_MARKER = DF_NOELEMENT;

            inline static const std::string DF_W_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_W_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_W_DATA = OptionalScalerData( DF_NOELEMENT, DF_W_RANDOMNESS, DF_W_MARKER, ClassType::DF_W_PARAMS );


        //---ctor, dtor
            CopyControlData( const std::string& name, const std::string& gate = DF_GATE, const std::string& activeMarker = DF_ACTIVE_MARKER, const OptionalScalerData& wData = DF_W_DATA ) 
            : BaseData( name ), OptionalScalerUser(), w( *this, wData.rnd, wData.params, wData.marker )
            , gate( gate ), activeMarker( activeMarker ) {;}

            CopyControlData() : BaseData(), OptionalScalerUser(), w( *this, ClassType::DF_W_PARAMS ) {;}
            virtual ~CopyControlData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( w, "w" );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "active_marker", activeMarker, DF_ACTIVE_MARKER );
                
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler w;

            std::string gate;
            std::string activeMarker;
        };


        struct OriVData : public OptionalScalerUser
        {
            using ClassType = cg::OriV;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::ORI_V;
 
            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::vector<std::string> DF_COPY_CONTROLS = {};
            inline static const std::string DF_CUSTOM_FUN = DF_NOELEMENT;
            inline static const std::string DF_ECLIPSE_MARKER = DF_NOELEMENT;
            inline static const std::string DF_EVENT_MARKER = DF_NOELEMENT;

            inline static const std::string DF_VMAX_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_VMAX_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_VMAX_DATA = OptionalScalerData( DF_NOELEMENT, DF_VMAX_RANDOMNESS, DF_VMAX_MARKER, ClassType::DF_VMAX_PARAMS );

            inline static const std::string DF_W_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_W_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_W_DATA = OptionalScalerData( DF_NOELEMENT, DF_W_RANDOMNESS, DF_W_MARKER, ClassType::DF_W_PARAMS );

            inline static const std::vector<int> DF_MUTATION_KEYS = ut::castVector<int>( ClassType::DF_MUTATION_KEYS );


        //---ctor, dtor
            OriVData( const std::string& name, const std::string& gate = DF_GATE, const std::vector<std::string>& copyControls = DF_COPY_CONTROLS, const std::string& customFunction = DF_CUSTOM_FUN
            , const std::string& eclipseMarker = DF_ECLIPSE_MARKER, const std::string& eventMarker = DF_EVENT_MARKER
            , const OptionalScalerData& vmaxData = DF_VMAX_DATA, const OptionalScalerData& wData = DF_W_DATA
            , bool bEclipse = ClassType::DF_B_ECLIPSE, const std::vector<int>& mutationKeys = DF_MUTATION_KEYS ) 

            : BaseData( name ), OptionalScalerUser(), vmax( *this, vmaxData.rnd, vmaxData.params, vmaxData.marker ), w( *this, wData.rnd, wData.params, wData.marker )
            , gate( gate ), copyControls( copyControls ), customFunction( customFunction ), eclipseMarker( eclipseMarker ), eventMarker( eventMarker )
            , bEclipse( bEclipse ), mutationKeys( mutationKeys ) {;}

            OriVData() : BaseData(), OptionalScalerUser(), vmax( *this, ClassType::DF_VMAX_PARAMS ), w( *this, ClassType::DF_W_PARAMS ) {;}

            virtual ~OriVData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( vmax, "vmax" );
                OptionalScalerUser::addFields( w, "w");

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldList( "copy_controls", copyControls, true, DF_COPY_CONTROLS );
                addCclFieldSimple( "rate_fun", customFunction, DF_CUSTOM_FUN );
                addCclFieldSimple( "eclipse_marker", eclipseMarker, DF_ECLIPSE_MARKER );
                addCclFieldSimple( "event_marker", eventMarker, DF_EVENT_MARKER );
                addCclFieldSimple( "event", eventMarker, DF_EVENT_MARKER );

                addCclFieldSimple( "has_eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldSimple( "eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldList( "mutations_keys", mutationKeys, false, {} );
                addCclFieldList( "mutation_keys", mutationKeys, false, DF_MUTATION_KEYS );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler vmax;
            OptionalScaler w;

            std::string gate;
            std::vector<std::string> copyControls;
            std::string customFunction;
            std::string eclipseMarker;
            std::string eventMarker;

            bool bEclipse;
            std::vector<int> mutationKeys;
        };


        struct PartitionSystemData : public OptionalScalerUser
        {
            using ClassType = cg::PartitionSystem;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::PARTITION_SYSTEM;
        
            inline static const std::string DF_GATE = BGateData::AUTO_FALSE_NAME;
            
            inline static const std::string DF_FRACTION_FUN = DF_NOELEMENT;
            inline static const std::string DF_FRACTION_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_FRACTION_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_FRACTION_DATA = OptionalScalerData( DF_FRACTION_FUN, DF_FRACTION_RANDOMNESS, DF_FRACTION_MARKER, ClassType::DF_FRACTION_PARAMS );

            inline static const std::string DF_P_FUN = DF_NOELEMENT;
            inline static const std::string DF_P_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_P_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_P_DATA = OptionalScalerData( DF_P_FUN, DF_P_RANDOMNESS, DF_P_MARKER, ClassType::DF_P_PARAMS );


            inline static const std::string AUTO_RND_NAME = "_part_rnd";
            inline static const std::string ECLIPSE_SUFIX = "_eclipse";


        //---ctor, dtor
            PartitionSystemData( const std::string& name, const std::string& gate = DF_GATE, const OptionalScalerData& fractionData = DF_FRACTION_DATA, const OptionalScalerData& pData = DF_P_DATA, bool bVolScaling = ClassType::DF_B_VOLUME_SCALING ) 
            
            : BaseData( name, true, false ), OptionalScalerUser(), fraction( *this, fractionData.rnd, fractionData.params, fractionData.marker ), p( *this, pData.rnd, pData.params, pData.marker )
            , gate( gate ), fractionFunction( fractionData.fun ), pFunction( pData.fun ), bVolScaling( bVolScaling ) {;}


            PartitionSystemData() : BaseData(), OptionalScalerUser(), fraction( *this, ClassType::DF_FRACTION_PARAMS ), p( *this, ClassType::DF_P_PARAMS ) {;}
            virtual ~PartitionSystemData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( fraction, "fraction" );
                OptionalScalerUser::addFields( p, "p" );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "fraction_fun", fractionFunction, DF_FRACTION_FUN );
                addCclFieldSimple( "p_fun", pFunction, DF_P_FUN );

                addCclFieldSimple( "vol_scaling", bVolScaling, ClassType::DF_B_VOLUME_SCALING );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;
       
        //---data
            OptionalScaler fraction;
            OptionalScaler p;

            std::string gate;
            std::string fractionFunction;
            std::string pFunction;

            bool bVolScaling;
        };




//===================================================================================================================== MUTATION

        struct MutationData : public BaseData
        {
            using ClassType = cg::Mutation;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::MUTATION;

            inline static const std::string AUTO_NULL_NAME = "_mut_null";

            inline static const std::vector<std::string> DF_PLASMIDS = {};
            inline static const std::vector<int> DF_AMOUNTS = {};
            inline static const std::string DF_EVENT_MARKER = DF_NOELEMENT;


        //--ctor, dtor
            MutationData( const std::string& name, const std::vector<std::string>& plasmids = DF_PLASMIDS, const std::vector<int>& amounts = DF_AMOUNTS, const std::string& eventMarker = DF_EVENT_MARKER )
            : BaseData( name, true, false )
            , plasmids( plasmids ), amounts( amounts ), eventMarker( eventMarker ) {;}

            MutationData() = default;
            virtual ~MutationData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldList( "plasmids", plasmids, false, DF_PLASMIDS );
                addCclFieldList( "amounts", amounts, false, DF_AMOUNTS );
                addCclFieldSimple( "event_marker", eventMarker, DF_EVENT_MARKER );
                addCclFieldSimple( "event", eventMarker, DF_EVENT_MARKER );
                
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            std::vector<std::string> plasmids;
            std::vector<int> amounts;
            std::string eventMarker;
        };


        struct MutationProcessData : public OptionalScalerUser
        {
            using ClassType = cg::MutationProcess;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::MUTATION_PROCESS;

            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::vector<std::string> DF_MUTATIONS = {};
            inline static const std::string DF_EVENT_MARKER = DF_NOELEMENT;
            inline static const std::vector<double> DF_MUTATION_PROBS = {};

            inline static const std::string DF_RATE_FUNCTION = DF_NOELEMENT;
            inline static const std::string DF_RATE_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_RATE_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_RATE_DATA = OptionalScalerData( DF_RATE_FUNCTION, DF_RATE_RANDOMNESS, DF_RATE_MARKER, ClassType::DF_RATE_PARAMS );
            

        //---ctor, dtor
            MutationProcessData( const std::string& name, const std::vector<std::string>& mutations, const std::string& gate = DF_GATE, const OptionalScalerData& rateData = DF_RATE_DATA
            , bool bEclipse = ClassType::DF_B_ECLIPSE, const std::vector<double>& mutationProbs = DF_MUTATION_PROBS, const std::string& eventMarker = DF_EVENT_MARKER ) 

            : BaseData( name, true, false ), OptionalScalerUser(), rate( *this, rateData.rnd, rateData.params, rateData.marker )
            , mutations( mutations ), gate( gate ), rateFunction( rateData.fun ), eventMarker( eventMarker )
            , bEclipse( bEclipse ), mutationProbs( mutationProbs ) {;}

            MutationProcessData() : BaseData(), OptionalScalerUser(), rate( *this, ClassType::DF_RATE_PARAMS ) {;}
            virtual ~MutationProcessData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( rate, "rate" );

                addCclFieldList( "mutations", mutations, false, DF_MUTATIONS );
                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "rate_fun", rateFunction, DF_RATE_FUNCTION );
                addCclFieldSimple( "event_marker", eventMarker, DF_EVENT_MARKER );
                addCclFieldSimple( "event", eventMarker, DF_EVENT_MARKER );
                
                addCclFieldSimple( "has_eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldSimple( "eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldList( "mutation_probs", mutationProbs, false, DF_MUTATION_PROBS );

                fwCreateCclFields();
                initCclFields();
            }
            
            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

         //----data
            OptionalScaler rate;

            std::vector<std::string> mutations;
            std::string gate;
            std::string rateFunction;
            std::string eventMarker;
            
            bool bEclipse;
            std::vector<double> mutationProbs;
        };







//======================================================================================================== CONJUGATION

        struct PilusData : public OptionalScalerUser
        {
            using ClassType = cg::Pilus;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::PILUS;

            inline static const std::string DF_DONOR_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_RECIPIENT_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_RECIPIENT_FUNCTION = DF_NOELEMENT;

            inline static const std::string DF_DONOR_FUNCTION = DF_NOELEMENT;
            inline static const std::string DF_MAX_RATE_RND = DF_NOELEMENT;
            inline static const std::string DF_MAX_RATE_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_MAX_RATE_DATA = OptionalScalerData( DF_DONOR_FUNCTION, DF_MAX_RATE_RND, DF_MAX_RATE_MARKER, ClassType::DF_MAX_RATE_PARAMS );

            inline static const std::string DF_NEIGHBOURS_W_RND = DF_NOELEMENT;
            inline static const std::string DF_NEIGHBOURS_W_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_NEIGHBOURS_W_DATA = OptionalScalerData( DF_NOELEMENT, DF_NEIGHBOURS_W_RND, DF_NEIGHBOURS_W_MARKER, ClassType::DF_NEIGHBOURS_W_PARAMS );

            inline static const std::string DF_RATE_MARKER = DF_NOELEMENT;


        //---ctor, dtor
            PilusData( const std::string& name, const std::string& donorGate = DF_DONOR_GATE, const std::string& recipientGate = DF_RECIPIENT_GATE, const std::string& recipientFunction = DF_RECIPIENT_FUNCTION
            , const OptionalScalerData& maxRateData = DF_MAX_RATE_DATA, const OptionalScalerData& neighboursWData = DF_NEIGHBOURS_W_DATA, const std::string& rateMarker = DF_RATE_MARKER, bool bAutoVolScale = ClassType::DF_AUTO_VOL_SCALE )

            : BaseData( name ), OptionalScalerUser(), maxRate( *this, maxRateData.rnd, maxRateData.params, maxRateData.marker ), neighboursW( *this, neighboursWData.rnd, neighboursWData.params, neighboursWData.marker )
            , donorGate( donorGate ), recipientGate( recipientGate ), donorFunction( maxRateData.fun ), recipientFunction( recipientFunction ), rateMarker( rateMarker ), bAutoVolScale( bAutoVolScale ) {;}

            PilusData() : BaseData(), OptionalScalerUser(), maxRate( *this, ClassType::DF_MAX_RATE_PARAMS ), neighboursW( *this, ClassType::DF_NEIGHBOURS_W_PARAMS ) {;}
            virtual ~PilusData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( maxRate, "max_rate" );
                OptionalScalerUser::addFields( neighboursW, "neighbours_w" );

                addCclFieldSimple( "donor_gate", donorGate, DF_DONOR_GATE );
                addCclFieldSimple( "recipient_gate", recipientGate, DF_RECIPIENT_GATE );
                addCclFieldSimple( "donor_fun", donorFunction, DF_DONOR_FUNCTION );
                addCclFieldSimple( "recipient_fun", recipientFunction, DF_RECIPIENT_FUNCTION );

                addCclFieldSimple( "rate_marker", rateMarker, DF_RATE_MARKER );
                addCclFieldSimple( "vol_scaling", bAutoVolScale, ClassType::DF_AUTO_VOL_SCALE );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
          //load
            OptionalScaler maxRate;
            OptionalScaler neighboursW;

            std::string donorGate;
            std::string recipientGate;
            std::string donorFunction;
            std::string recipientFunction;

            std::string rateMarker;
            bool bAutoVolScale;

          //preproces
            std::string neighbourMarker; //created by preprocess()
        };


        struct OriTData : public OptionalScalerUser
        {
            using ClassType = cg::OriT;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::ORI_T;
 
            inline static const std::string DF_PILUS = DF_NOELEMENT;
            inline static const std::string DF_DONOR_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_RECIPIENT_GATE = BGateData::AUTO_TRUE_NAME;

            inline static const std::string DF_RATE_W_FUN = DF_NOELEMENT;
            inline static const std::string DF_RATE_W_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_RATE_W_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_RATE_W_DATA = OptionalScalerData( DF_RATE_W_FUN, DF_RATE_W_RANDOMNESS, DF_RATE_W_MARKER, ClassType::DF_RATE_W_PARAMS );

            inline static const std::string DF_DONOR_EVENT_MARKER = DF_NOELEMENT;
            inline static const std::string DF_RECIPIENT_EVENT_MARKER = DF_NOELEMENT;

            inline static const std::vector<int> DF_MUTATION_KEYS = ut::castVector<int>( ClassType::DF_MUTATION_KEYS );


        //---ctor, dtor
            OriTData( const std::string& name, const std::string& pilus, const std::string& donorGate = DF_DONOR_GATE, const std::string& recipientGate = DF_RECIPIENT_GATE, const OptionalScalerData& rateWData = DF_RATE_W_DATA
            , const std::string& donorEventMarker = DF_DONOR_EVENT_MARKER, const std::string& recipientEventMarker = DF_RECIPIENT_EVENT_MARKER
            , bool bCopyDependent = ClassType::DF_BCOPY_DEPENDENT, bool bEclipse = ClassType::DF_B_ECLIPSE, const std::vector<int>& mutationKeys = DF_MUTATION_KEYS ) 
            
            : BaseData( name ), OptionalScalerUser(), rateW( *this, rateWData.rnd, rateWData.params, rateW.marker )
            , pilus( pilus ), donorGate( donorGate ), recipientGate( recipientGate ), rateWFunction( rateWData.fun )
            , donorEventMarker( donorEventMarker ), recipientEventMarker( recipientEventMarker )
            , bCopyDependent( bCopyDependent ), bEclipse( bEclipse ), mutationKeys( mutationKeys ) {;}

            OriTData() : BaseData(), OptionalScalerUser(), rateW( *this, ClassType::DF_RATE_W_PARAMS ) {;}
            virtual ~OriTData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( rateW, "rate_w" );

                addCclFieldSimple( "pilus", pilus, DF_PILUS );
                addCclFieldSimple( "donor_gate", donorGate, DF_DONOR_GATE );
                addCclFieldSimple( "recipient_gate", recipientGate, DF_RECIPIENT_GATE );
                addCclFieldSimple( "rate_w_fun", rateWFunction, DF_RATE_W_FUN );

               	addCclFieldSimple( "donor_event", donorEventMarker, DF_DONOR_EVENT_MARKER );
                addCclFieldSimple( "recipient_event", recipientEventMarker, DF_RECIPIENT_EVENT_MARKER );

                addCclFieldSimple( "copy_dependent", bCopyDependent, ClassType::DF_BCOPY_DEPENDENT );
                addCclFieldSimple( "has_eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldSimple( "eclipse", bEclipse, ClassType::DF_B_ECLIPSE );
                addCclFieldList( "mutations_keys", mutationKeys, false, DF_MUTATION_KEYS );
                addCclFieldList( "mutation_keys", mutationKeys, false, DF_MUTATION_KEYS );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler rateW;

            std::string pilus;
            std::string donorGate;
            std::string recipientGate;
            std::string rateWFunction;

            std::string donorEventMarker;
            std::string recipientEventMarker;

            bool bCopyDependent; 
            bool bEclipse;
            std::vector<int> mutationKeys;
        };



//====================================================================================================================== HOLDERS

//--------------------------------------------------------------- plasmids

        struct PlasmidData : public OptionalDistUser, public OptionalScalerUser
        /* base class for BPlasmids and QPlasmids */
        {
            using ClassType = cg::PlasmidBase;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::PLASMID;

            inline static const std::vector<std::string> DF_OPERONS = {};
            inline static const std::vector<std::string> DF_ORITS = {};
            inline static const std::vector<std::string> DF_REPLICATION_MUTATIONS = {};

            inline static const std::vector<int> DF_REPLI_MUT_KEYS = ut::castVector<int>( ClassType::DF_REPLI_MUT_KEYS );
       

        //---dtor
            virtual ~PlasmidData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldList( "operons", operons, false, DF_OPERONS );
                addCclFieldList( "orits", oriTs, false, DF_ORITS );
                addCclFieldList( "oriTs", oriTs, false, DF_ORITS );
                addCclFieldList( "repli_mutations", repliMutations, false, DF_REPLICATION_MUTATIONS );

                addCclFieldList( "repli_mutations_keys", repliMutationKeys, true, {} );
                addCclFieldList( "repli_mutation_keys", repliMutationKeys, true, {} );
                addCclFieldList( "repli_mut_keys", repliMutationKeys, true, {} );

                fwCreateCclFields();
                initCclFields();
            }

      		void preprocess() override;
      		void loadLast( World* world ) override;

        //---data
            std::vector<std::string> operons;
            std::vector<std::string> oriTs;
            std::vector<std::string> repliMutations;

            std::vector<int> repliMutationKeys;


        //---ctor
            protected:
                PlasmidData( const std::string& name, const std::vector<std::string>& operons = DF_OPERONS, const std::vector<std::string>& oriTs = DF_ORITS, const std::vector<std::string>& repliMutations = DF_REPLICATION_MUTATIONS
                , const std::vector<int>& repliMutationKeys = DF_REPLI_MUT_KEYS ) 
                : BaseData( name, true, false ), OptionalDistUser(), OptionalScalerUser(), operons( operons ), oriTs( oriTs ), repliMutations( repliMutations ), repliMutationKeys( repliMutationKeys ) {;}

                PlasmidData() = default;
        };


        struct PlasmidBooleanData : public PlasmidData
        {
            using ClassType = cg::PlasmidBoolean;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::PLASMID;

            inline static const std::string DF_LOSS_FUN = DF_NOELEMENT;
            inline static const std::string DF_LOSS_RANDOMNESS = DF_NOELEMENT;
            inline static const std::string DF_LOSS_MARKER = DF_NOELEMENT;
            inline static const OptionalScalerData DF_LOSS_DATA = OptionalScalerData( DF_LOSS_FUN, DF_LOSS_RANDOMNESS, DF_LOSS_MARKER, ClassType::DF_LOSS_PARAMS );


        //---ctor, dtor
            PlasmidBooleanData( const std::string& name, const std::vector<std::string>& operons = PlasmidData::DF_OPERONS, const std::vector<std::string>& oriTs = PlasmidData::DF_ORITS, const OptionalScalerData& lossData = DF_LOSS_DATA
            , const std::vector<std::string>& repliMutations = PlasmidData::DF_REPLICATION_MUTATIONS, const std::vector<int>& repliMutationKeys = PlasmidData::DF_REPLI_MUT_KEYS ) 
            
            : PlasmidData( name, operons, oriTs, repliMutations, repliMutationKeys )
            , loss( *this, lossData.rnd, lossData.params, lossData.marker ), lossFunction( lossData.fun ) {;}
            
            PlasmidBooleanData() : PlasmidData(), loss( *this, ClassType::DF_LOSS_PARAMS ) {;}
            virtual ~PlasmidBooleanData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                PlasmidData::createCclFields( makeDfName( this, dfName ) );

                OptionalScalerUser::addFields( loss, "loss_prob" );
                addCclFieldSimple( "loss_prob_fun", lossFunction, DF_LOSS_FUN );
                
                fwCreateCclFields();
                initCclFields();
            }
            
            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler loss;
            std::string lossFunction;
        };


        struct PlasmidQuantitativeData  : public PlasmidData
        {
            using ClassType = cg::PlasmidQuantitative;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::PLASMID;

            inline static const std::vector<std::string> DF_ORIVS = {};
            inline static const std::vector<std::string> DF_COPY_CONTROLS = {};
            inline static const std::string DF_PARTITION_SYSTEM = PartitionSystemData::AUTO_RND_NAME;  
            
            inline static const std::string DF_ECLIPSE_FUNCTION = DF_NOELEMENT;
            inline static const std::string DF_ECLIPSE_MARKER = DF_NOELEMENT;

            inline static const std::string DF_ECLIPSE_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_ECLIPSE_DISTTYPE );
            inline static const std::string DF_COPY_NUM_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_COPY_NUM_DISTTYPE );


        //---ctor, dtor
            PlasmidQuantitativeData( const std::string& name, const std::vector<std::string>& operons = PlasmidData::DF_OPERONS, const std::vector<std::string>& oriTs = PlasmidData::DF_ORITS
            , const std::vector<std::string>& oriVs = DF_ORIVS, const std::vector<std::string>& copyControls = DF_COPY_CONTROLS, const std::string& partitionSystem = DF_PARTITION_SYSTEM, const std::vector<std::string>& repliMutations = DF_REPLICATION_MUTATIONS
            , const std::string& eclipseMarker = DF_ECLIPSE_MARKER, const std::vector<int>& repliMutationKeys = DF_REPLI_MUT_KEYS
            , const std::string& eclipseFunction = DF_ECLIPSE_FUNCTION, const std::string& eclipseDistType = DF_ECLIPSE_DISTTYPE, const std::vector<double>& eclipseParams = ClassType::DF_ECLIPSE_PARAMS, const std::string& copyNumDistType = DF_COPY_NUM_DISTTYPE, const std::vector<double>& copyNumParams = ClassType::DF_COPY_NUM_PARAMS )

            : PlasmidData( name, operons, oriTs, repliMutations, repliMutationKeys ), eclipse( eclipseDistType, eclipseParams ), copyNum( copyNumDistType, copyNumParams )
            , oriVs( oriVs ), copyControls( copyControls ), partitionSystem( partitionSystem ), eclipseFunction( eclipseFunction ), eclipseMarker( eclipseMarker ) {;}

            PlasmidQuantitativeData() : eclipse( DF_ECLIPSE_DISTTYPE, ClassType::DF_ECLIPSE_PARAMS ), copyNum( DF_COPY_NUM_DISTTYPE, ClassType::DF_COPY_NUM_PARAMS ) {;}
            virtual ~PlasmidQuantitativeData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                PlasmidData::createCclFields( makeDfName( this, dfName ) );

                OptionalDistUser::addOptionalDistFields( eclipse, "eclipse" );
                OptionalDistUser::addOptionalDistFields( copyNum, "copy_num" );

                addCclFieldList( "orivs", oriVs, false, DF_ORIVS );
                addCclFieldList( "oriVs", oriVs, false, DF_ORIVS );
                addCclFieldList( "copy_controls", copyControls, false, DF_COPY_CONTROLS );
                addCclFieldSimple( "partition_system", partitionSystem, DF_PARTITION_SYSTEM );
                addCclFieldSimple( "partition", partitionSystem, DF_PARTITION_SYSTEM );
                addCclFieldSimple( "eclipse_fun", eclipseFunction, DF_ECLIPSE_FUNCTION );
				addCclFieldSimple( "eclipse_marker", eclipseMarker, DF_ECLIPSE_MARKER );
				
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;
           
        //---data
            OptionalDist eclipse;
            OptionalDist copyNum;

            std::vector<std::string> oriVs;
            std::vector<std::string> copyControls;
            std::string partitionSystem;
            std::string eclipseFunction;
            std::string eclipseMarker;
        };



//--------------------------------------------------------------- cell and strain
        struct StrainData : public OptionalScalerUser, public OptionalDistUser// public BaseData
        {
            using ClassType = cg::Strain;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::STRAIN;

            inline static const std::string DF_GR_CUSTOM_FUN = BaseData::DF_NOELEMENT;
            inline static const std::string DF_BASE_GR_RND = BaseData::DF_NOELEMENT;
            inline static const std::string DF_DIVISION_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_DEATH_GATE = BGateData::AUTO_TRUE_NAME;

            inline static const std::string DF_INI_VOL_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_INI_VOL_DISTTYPE );
            inline static const std::string DF_DIV_VOL_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_DIV_VOL_DISTTYPE );
            inline static const std::string DF_DIV_FRACTION_VOL_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_DIV_FRACTION_VOL_DISTTYPE );
            inline static const std::string DF_DIV_ROTATION_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_DIV_ROTATION_DISTTYPE );

            inline static const std::string AUTO_WT_NAME = "_str_wt";
            inline static const std::vector<double> AUTO_WT_GR = { ClassType::DF_GROWTH_RATE_MEAN, ClassType::DF_GROWTH_RATE_MEAN * 0.01 };
            inline static const std::string AUTO_SLOW_NAME = "_str_slow";
            inline static const std::vector<double> AUTO_SLOW_GR = { 0.01, 0.0001 };
            inline static const std::string AUTO_NOGROWTH_NAME = "_str_nogrowth";


        //---ctor, dtor
            StrainData( const std::string& name, const std::string& grCustomFun = DF_GR_CUSTOM_FUN, const std::string baseGrowthRateRnd = DF_BASE_GR_RND, const std::vector<double>& baseGrowthRateParams = ClassType::DF_GROWTH_RATE_PARAMS
            , const std::string& divisionGate = DF_DIVISION_GATE, const std::string& deathGate = DF_DEATH_GATE
            , const OptionalDist& iniVol = OptionalDist( DF_INI_VOL_DISTTYPE, ClassType::DF_INI_VOL_PARAMS ), const OptionalDist& divisionVol = OptionalDist( DF_DIV_VOL_DISTTYPE, ClassType::DF_DIV_VOL_PARAMS )
            , const OptionalDist& divisionFraction = OptionalDist( DF_DIV_FRACTION_VOL_DISTTYPE, ClassType::DF_DIV_FRACTION_VOL_PARAMS ), const OptionalDist& divisionRotation = OptionalDist( DF_DIV_ROTATION_DISTTYPE, ClassType::DF_DIV_ROTATION_PARAMS )
            , double deathThreshold = ClassType::DF_DEATH_THRESHOLD, bool bNegativeGrowthAllowed = ClassType::DF_NEGATIVE_GROWTH_ALLOWED, bool bGrowthCorrection = ClassType::DF_GROWTH_CORRECTION )
            
            : BaseData( name, true, false ), OptionalScalerUser(), OptionalDistUser(), growthRate( *this, baseGrowthRateRnd, baseGrowthRateParams, DF_NOELEMENT )
            , iniVol( iniVol ), divisionVol( divisionVol ), divisionFraction( divisionFraction ), divisionRotation( divisionRotation )
            , grCustomFun( grCustomFun ), divisionGate( divisionGate ), deathGate( deathGate )
            , deathThreshold( deathThreshold ), bNegativeGrowthAllowed( bNegativeGrowthAllowed ), bGrowthCorrection( bGrowthCorrection ) {;}

            StrainData() : BaseData(), OptionalScalerUser(), OptionalDistUser(), growthRate( *this, ClassType::DF_GROWTH_RATE_PARAMS ) 
            , iniVol( DF_INI_VOL_DISTTYPE, ClassType::DF_INI_VOL_PARAMS ), divisionVol( DF_DIV_VOL_DISTTYPE, ClassType::DF_DIV_VOL_PARAMS )
            , divisionFraction( DF_DIV_FRACTION_VOL_DISTTYPE, ClassType::DF_DIV_FRACTION_VOL_PARAMS ), divisionRotation( DF_DIV_ROTATION_DISTTYPE, ClassType::DF_DIV_ROTATION_PARAMS ) {;}

            virtual ~StrainData() = default; 

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
            
            	OptionalScalerUser::addFields( growthRate, "growth_rate" );
                OptionalScalerUser::addFields( growthRate, "growth" );
                OptionalDistUser::addOptionalDistFields( iniVol, "ini_vol" );
                OptionalDistUser::addOptionalDistFields( divisionVol, "division_vol" );
                OptionalDistUser::addOptionalDistFields( divisionFraction, "division_fraction" );
                OptionalDistUser::addOptionalDistFields( divisionRotation, "division_rotation" );

                addCclFieldSimple( "growth_fun", grCustomFun, DF_GR_CUSTOM_FUN );
                addCclFieldSimple( "division_gate", divisionGate, DF_DIVISION_GATE );
                addCclFieldSimple( "death_gate", deathGate, DF_DEATH_GATE );

                addCclFieldSimple( "death_threshold", deathThreshold, ClassType::DF_DEATH_THRESHOLD );
                addCclFieldSimple( "negative_growth", bNegativeGrowthAllowed, ClassType::DF_NEGATIVE_GROWTH_ALLOWED );
                addCclFieldSimple( "growth_correction", bGrowthCorrection, ClassType::DF_GROWTH_CORRECTION );

                fwCreateCclFields();
                initCclFields();
            }
            
            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            OptionalScaler growthRate;
            OptionalDist iniVol;
            OptionalDist divisionVol;
            OptionalDist divisionFraction;
            OptionalDist divisionRotation;

            std::string grCustomFun;
            std::string divisionGate;
            std::string deathGate;

            double deathThreshold;
            bool bNegativeGrowthAllowed;
            bool bGrowthCorrection;
        };


        struct CellTypeData : public BaseData
        {
            using ClassType = cg::CellType;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELL_TYPE;

            inline static const std::string DF_STRAIN = StrainData::AUTO_WT_NAME;
            inline static const std::vector<std::string> DF_BPLASMIDS = {};
            inline static const std::vector<std::string> DF_MOLECULES = {};
            
            inline static const std::vector<std::string> DF_QPLASMIDS = {};
            inline static const std::vector<double> DF_QPLASMID_AMOUNTS = {};
            inline static const std::vector<double> DF_QPLASMID_VARS = {};
            inline static const std::vector<std::string> DF_QPLASMID_DISTS = {};


        //---ctor, dtor
            CellTypeData( const std::string& name, const std::string& strain = DF_STRAIN, const std::vector<std::string>& bPlasmids = DF_BPLASMIDS, const std::vector<std::string>& molecules = DF_MOLECULES
            , const std::vector<std::string>& qPlasmids = DF_QPLASMIDS, const std::vector<double>& qPlasmidAmounts = DF_QPLASMID_AMOUNTS, const std::vector<double>& qPlasmidAmountVars = DF_QPLASMID_VARS, const std::vector<std::string>& qPlasmidDists = DF_QPLASMID_DISTS )
            : BaseData( name ), strain( strain ), bPlasmids( bPlasmids ), molecules( molecules )
            , qPlasmids( qPlasmids ), qPlasmidAmounts( qPlasmidAmounts ), qPlasmidAmountVars( qPlasmidAmountVars ), qPlasmidDists( qPlasmidDists ) {;}

            CellTypeData() = default;
            virtual ~CellTypeData() = default;
    
        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "strain", strain, DF_STRAIN );
                addCclFieldList( "b_plasmids", bPlasmids, true, DF_BPLASMIDS );
                addCclFieldList( "plasmids", bPlasmids, true, DF_BPLASMIDS );
                addCclFieldList( "bplasmids", bPlasmids, true, DF_BPLASMIDS );
                addCclFieldList( "molecules", molecules, true, DF_MOLECULES );

                addCclFieldList( "q_plasmids", qPlasmids, true, DF_QPLASMIDS );
                addCclFieldList( "qplasmids", qPlasmids, true, DF_QPLASMIDS );
                addCclFieldList( "qplasmids_amounts", qPlasmidAmounts, true, DF_QPLASMID_AMOUNTS );
                addCclFieldList( "qplasmid_amounts", qPlasmidAmounts, true, DF_QPLASMID_AMOUNTS );
                addCclFieldList( "qplasmids_means", qPlasmidAmounts, true, DF_QPLASMID_AMOUNTS );
                addCclFieldList( "qplasmid_means", qPlasmidAmounts, true, DF_QPLASMID_AMOUNTS );
                addCclFieldList( "qplasmidd_vars", qPlasmidAmountVars, true, DF_QPLASMID_VARS );
                addCclFieldList( "qplasmid_vars", qPlasmidAmountVars, true, DF_QPLASMID_VARS );
                addCclFieldList( "qplasmids_dists", qPlasmidDists, true, DF_QPLASMID_DISTS );
                addCclFieldList( "qplasmid_dists", qPlasmidDists, true, DF_QPLASMID_DISTS );

                addCclFieldSimple( "qplasmids_as_concs", bQPlasmidsAsConc, ClassType::DF_B_QPLASMIDS_AS_CONC );
                addCclFieldSimple( "qplasmids_as_conc", bQPlasmidsAsConc, ClassType::DF_B_QPLASMIDS_AS_CONC );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            std::string strain;
            std::vector<std::string> bPlasmids;
            std::vector<std::string> molecules;

            std::vector<std::string> qPlasmids;
            std::vector<double> qPlasmidAmounts;
            std::vector<double> qPlasmidAmountVars;
            std::vector<std::string> qPlasmidDists;

            bool bQPlasmidsAsConc;
        };










//

//

//

//

//

//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// MEDIUM SIGNALS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        struct SignalData : public BaseData
        {
            using ClassType = mg::Signal;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::SIGNAL;

            inline static const std::string CONC_PREFIX = "_conc_";
            inline static const std::string DF_CONC_MARKER = DF_NOELEMENT;
            inline static const std::vector<double> DF_RGB_COLOUR = { 1.0, 0.0, 1.0 };

			
			SignalData( const std::string& name, double kdiff, double kdeg, double concThreshold = ClassType::DF_CONC_THRESHOLD, const std::vector<double>& rgbColour = DF_RGB_COLOUR, double colourSaturationConc = ClassType::DF_COLOUR_SAT )
			: BaseData( name ), kdiff( kdiff ), kdeg( kdeg ), concThreshold( concThreshold ),  rgbColour( rgbColour ), colourSaturationConc( colourSaturationConc ) {;}

            SignalData() = default;
    		virtual ~SignalData() = default;


            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "kdiff", kdiff, ClassType::DF_KDIFF );
                addCclFieldSimple( "k_diff", kdiff, ClassType::DF_KDIFF );
                addCclFieldSimple( "kdeg", kdeg, ClassType::DF_KDEG );
                addCclFieldSimple( "k_deg", kdeg, ClassType::DF_KDEG );
                addCclFieldSimple( "conc_threshold", concThreshold, ClassType::DF_CONC_THRESHOLD );

                addCclFieldList( "color", rgbColour, true, DF_RGB_COLOUR );
                addCclFieldList( "colour", rgbColour, true, DF_RGB_COLOUR );
				addCclFieldSimple( "color_sat", colourSaturationConc, ClassType::DF_COLOUR_SAT );
                addCclFieldSimple( "colour_sat", colourSaturationConc, ClassType::DF_COLOUR_SAT );
    
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;


        //---data
            double kdiff;
            double kdeg;
            double concThreshold;
            std::vector<double> rgbColour;
            double colourSaturationConc;
        };


        struct GridData : public BaseData
        {
            using ClassType = mg::Grid;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::GRID;

            inline static const std::string AUTO_NAME = "_grid";
            inline static const std::string AUTO_CUSTOM_NAME = "_custom_grid";


            GridData( const std::string& name, int unitSize = ClassType::DF_UNIT_SIZE, int iniGridSize = ClassType::DF_GRID_SIZE, int deltaGridSize = ClassType::DF_D_GRID_SIZE ) 
            : BaseData( name ), unitSize( unitSize ), iniGridSize( iniGridSize ), deltaGridSize( deltaGridSize ) {;}

            GridData() = default;
            virtual ~GridData() = default;


            void createCclFields( const std::string& dfName = AUTO_CUSTOM_NAME ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );


                addCclFieldSimple( "unit_size", unitSize, ClassType::DF_UNIT_SIZE );
                addCclFieldSimple( "ini_size", iniGridSize, ClassType::DF_GRID_SIZE );
                addCclFieldSimple( "delta_size", deltaGridSize, ClassType::DF_D_GRID_SIZE );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;


        //---data
            int unitSize;
            int iniGridSize;
            int deltaGridSize;
        };












//

//

//

//

//

//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// SIMULATION /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//====================================================================================================================================================================
//==================================================================* GLOBAL *======================================================================================
//====================================================================================================================================================================

        struct GlobalParamsData : public BaseData
        {
            using ClassType = GroElement;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::GLOBAL_PARAMS;
            inline static const std::string DF_NAME = "_global_params";

            struct ThemeData : public BaseData
            {
                using ClassType = GroElement;

              //load
                std::string backgroundCol;
                std::string cellOutlineCol;
                std::string cellOutlineSelectedCol;
                std::string messageCol;
                std::string mouseCol;


                ThemeData() = default;
                //ThemeData( ) {;}
                virtual ~ThemeData() = default;

                void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
                {
                    BaseData::createCclFields( makeDfName( this, dfName ) );

                    addCclFieldSimple( "background", backgroundCol, Theme::DF_BACKGROUND_HEXCOL );
                    addCclFieldSimple( "cell_outline", cellOutlineCol, Theme::DF_CELL_OUTLINE_HEXCOL );
                    addCclFieldSimple( "cell_selected", cellOutlineSelectedCol, Theme::DF_CELL_OUTLINE_SELECTED_HEXCOL );
                    addCclFieldSimple( "message", messageCol, Theme::DF_MESSAGE_HEXCOL );
                    addCclFieldSimple( "mouse", mouseCol, Theme::DF_MOUSE_HEXCOL );
                    
                    fwCreateCclFields();
                    initCclFields();
                }
            };

            GlobalParamsData() = default;

            GlobalParamsData( const ThemeData& theme, int seed, int seedOffset, double stepSize = GlobalParams::DF_STEP_SIZE, double sensitivity = GlobalParams::DF_SENSITIVITY, int cellsPerThread = GlobalParams::DF_CELLS_PER_THREAD
            , int gridRowsPerThread = GlobalParams::DF_GRID_ROWS_PER_THREAD, int maxThreads = GlobalParams::DF_MAX_THREADS, bool bAutoZoom = GlobalParams::DF_B_AUTO_ZOOM, bool bShowPlots = GlobalParams::DF_B_SHOW_PLOTS
            , bool bRenderSignals = GlobalParams::DF_B_RENDER_SIGNALS, bool bBatchMode = GlobalParams::DF_B_BATCH_MODE, bool bShowPerformance = GlobalParams::DF_B_SHOW_PERFORMANCE )
            : BaseData( DF_NAME ), theme( theme )
            , seed( seed ), seedOffset( seedOffset ), stepSize( stepSize ), sensitivity( sensitivity ), cellsPerThread( cellsPerThread ), gridRowsPerThread( gridRowsPerThread ), maxThreads( maxThreads )
            , bAutoZoom( bAutoZoom ), bShowPlots( bShowPlots ), bRenderSignals( bRenderSignals ), bBatchMode( bBatchMode ), bShowPerformance( bShowPerformance ) {;}

            virtual ~GlobalParamsData() = default;

            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldRecord( "theme", theme );
                addCclFieldSimple( "seed", seed, GlobalParams::DF_SEED );
                addCclFieldSimple( "seed_offset", seedOffset, GlobalParams::DF_SEED_OFFSET );
                addCclFieldSimple( "step_size", stepSize, GlobalParams::DF_STEP_SIZE );
                addCclFieldSimple( "sensitivity", sensitivity, GlobalParams::DF_SENSITIVITY );
                addCclFieldSimple( "cells_per_thread", cellsPerThread, GlobalParams::DF_CELLS_PER_THREAD );
				addCclFieldSimple( "grid_rows_per_thread", gridRowsPerThread, GlobalParams::DF_GRID_ROWS_PER_THREAD );
                addCclFieldSimple( "max_threads", maxThreads, GlobalParams::DF_MAX_THREADS );

                addCclFieldSimple( "auto_zoom", bAutoZoom, GlobalParams::DF_B_AUTO_ZOOM );
                addCclFieldSimple( "show_plots", bShowPlots, GlobalParams::DF_B_SHOW_PLOTS );
                addCclFieldSimple( "render_signals", bRenderSignals, GlobalParams::DF_B_RENDER_SIGNALS );
                addCclFieldSimple( "batch_mode", bBatchMode, GlobalParams::DF_B_BATCH_MODE );
                addCclFieldSimple( "show_performance", bShowPerformance, GlobalParams::DF_B_SHOW_PERFORMANCE );

                fwCreateCclFields();
                initCclFields();
            }

        //---data
            ThemeData theme;
            int seed;
            int seedOffset;
            double stepSize;
            double sensitivity;
            int cellsPerThread;
            int gridRowsPerThread;
            int maxThreads;

            bool bAutoZoom;
            bool bShowPlots;
            bool bRenderSignals;
            bool bBatchMode;
            bool bShowPerformance;
        };




//============================================================================================================ LOGIC
        struct PopQMarkerData : public BaseData
        {
            using ClassType = QGroMarker;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::POP_QMARKER;

            inline static const std::string AUTO_CELLNUM_NAME = "_cellnum";

        //---ctor, dtor
            PopQMarkerData( const std::string& name ) : BaseData( name, true, false ) {;}
            PopQMarkerData() = default;
            virtual ~PopQMarkerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
        };


        struct PopulationStatData : public BaseData
        {
            using ClassType = PopulationStat;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::POP_STAT;

            inline static const std::string DF_FILTER_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_STATS_PACK = "avg";
            inline static const std::vector<std::string> DF_STATS = {};
            inline static const std::string DF_FIELD = DF_NOELEMENT;
            inline static const std::string DF_AUTO_VOL = ClassType::autoVolModeNM.subtype2Name( ClassType::DF_AUTO_VOL_MODE );

            inline static const std::string AUTO_CELLNUM_NAME = "_stat_cellnum";

        //---ctor, dtor
            PopulationStatData( const std::string& name, const std::string& field, const std::string& filterGate = DF_FILTER_GATE, const std::string& statsPack = DF_STATS_PACK, const std::string& autoVolMode = DF_AUTO_VOL, bool bFree = ClassType::DF_B_FREE ) //stats pack
            : BaseData( name ), field( field ), filterGate( filterGate ), statsPack( statsPack ), autoVolMode( autoVolMode ), bFree( bFree ) {;}

            PopulationStatData( const std::string& name, const std::string& field, const std::string& filterGate, const std::vector<std::string>& stats, const std::string& autoVolMode = DF_AUTO_VOL, bool bFree = ClassType::DF_B_FREE ) //free stats
            : BaseData( name ), field( field ), filterGate( filterGate ), stats( stats ), autoVolMode( autoVolMode ), bFree( bFree ) {;}

            PopulationStatData() : bFree( true ) {;}
            virtual ~PopulationStatData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "elem", field, DF_FIELD );
                addCclFieldSimple( "input", field, DF_FIELD );
                addCclFieldSimple( "filter_gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "stats", statsPack, DF_STATS_PACK );
                addCclFieldList( "stats", stats, true, DF_STATS );
                addCclFieldSimple( "auto_vol", autoVolMode, DF_AUTO_VOL );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;

        //---data
          //load
            std::string field;
            std::string filterGate;
            std::string statsPack;
            std::vector<std::string> stats;
            std::string autoVolMode;
            bool bFree;
        };


        struct PopulationFunctionData : public BaseData
        {
            using ClassType = PopulationFunction;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::POP_FUNCTION;

            inline static const std::string DF_TYPE = ClassType::MathFunctionType::FactoryType::idx2name( ClassType::DF_FUNTION_TYPE );
            inline static const std::vector<std::string> DF_INPUT = {};
            inline static const std::vector<double> DF_PARAMS = {};
            

        //---ctor, dtor
            PopulationFunctionData( const std::string& name, const std::string& type = DF_TYPE, const std::vector<std::string>& input = DF_INPUT, const std::vector<double>& params = DF_PARAMS
            , double min = ClassType::DF_MIN, double max = ClassType::DF_MAX, int roundPlaces = ClassType::DF_ROUND_PLACES )
            : BaseData( name, true, false ), type( type ), input( input ), params( params ), min( min ), max( max ), roundPlaces( roundPlaces ) {;}

            PopulationFunctionData() = default;
            virtual ~PopulationFunctionData() = default;

        //--API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "type", type, DF_TYPE );
                addCclFieldList( "inputs", input, false, DF_INPUT );
                addCclFieldList( "input", input, true, DF_INPUT );
                addCclFieldList( "params", params, true, DF_PARAMS );

                addCclFieldSimple( "min", min, ClassType::DF_MIN );
                addCclFieldSimple( "max", max, ClassType::DF_MAX );
                addCclFieldSimple( "precision", roundPlaces, ClassType::DF_ROUND_PLACES );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string type;
            std::vector<std::string> input;
            std::vector<double> params;

            double min;
            double max;
            int roundPlaces;
        };


        struct QPopulationGateData : public BaseData
        {
            using ClassType = PopulationGateQuantitative;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::POP_GATE;

            inline static const std::string DF_ELEM = BaseData::DF_NOELEMENT;
            inline static const std::string DF_COMP_OPERATOR_STR = ut::GateQuantitative::compOperatorNM.subtype2Name( ut::GateQuantitative::DF_COMP_OPERATOR );


        //---ctor, dtor
            QPopulationGateData( const std::string& name, const std::string& elem, const std::string& compOperator = DF_COMP_OPERATOR_STR, double value = ClassType::DF_VALUE )
            : BaseData( name )
            , elem( elem ), compOperator( compOperator ), value(value) {;}
            
            QPopulationGateData() = default;
            virtual ~QPopulationGateData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "elem", elem, DF_ELEM );
                addCclFieldSimple( "input", elem, DF_ELEM );
                addCclFieldSimple( "operator", compOperator, DF_COMP_OPERATOR_STR );
                addCclFieldSimple( "value", value, ClassType::DF_VALUE );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
            std::string elem;
            std::string compOperator;
            double value;
        };


        struct BPopulationGateData : public BaseData
        {
            using ClassType = PopulationGateBoolean;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::POP_GATE;

            inline static const std::string DF_FUNCTION_STR = ut::GateBoolean::functionNM.subtype2Name( cg::GateBoolean::DF_FUNCTION );
            inline static const std::vector<std::string> DF_INPUT_NAMES_RAW = {};

            inline static const std::string AUTO_TRUE_NAME = "_pga_true";
            inline static const std::string AUTO_FALSE_NAME = "_pga_false";


        //---ctor, dtor
            BPopulationGateData( const std::string& name, const std::string& gateFunction = DF_FUNCTION_STR, const std::vector<std::string>& inputNamesRaw = DF_INPUT_NAMES_RAW )
            : BaseData( name ), gateFunction( gateFunction ), inputNamesRaw( inputNamesRaw ) {;}

            BPopulationGateData() = default;
            virtual ~BPopulationGateData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "type", gateFunction, DF_FUNCTION_STR );
                addCclFieldList( "input", inputNamesRaw, true, DF_INPUT_NAMES_RAW );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;
            void loadLast( World* world ) override;

        //---data
          //load
            std::string gateFunction;
            std::vector<std::string> inputNamesRaw;
          //preprocess
            std::vector<std::string> inputNames;
            std::vector<bool> inputPresence;
        };






//============================================================================================== TIMER

        struct TimerData : public BaseData
        {
            using ClassType = Timer;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::TIMER;

            inline static const std::string DF_GATE = BPopulationGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_MODE = ClassType::modeNM.subtype2Name( ClassType::DF_MODE );

            inline static const std::string AUTO_START_NAME = "_t_start";
            inline static const std::string AUTO_EVERY_STEP_NAME = "_t_every_step";
            inline static const std::string AUTO_EVERY_MIN_NAME = "_t_every_min";
            inline static const std::string AUTO_NEVER_NAME = "_t_never";

            
        //---ctor, dtor
            TimerData( const std::string& name, const std::string& gate = DF_GATE, const std::string& mode = DF_MODE
            , double start = ClassType::DF_START, double end = ClassType::DF_END, double period = ClassType::DF_PERIOD, bool bRnd = ClassType::DF_B_RND, double periodMin = ClassType::DF_PERIOD_MIN, bool bIgnoreStart = ClassType::DF_B_IGNORE_START ) 
            : BaseData( name ), gate( gate ), mode( mode ), start( start ), end( end ), period( period ), bRnd( bRnd ), periodMin( periodMin ), bIgnoreStart( bIgnoreStart ) {;}

            TimerData() = default;
            virtual ~TimerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "mode", mode, DF_MODE );
                addCclFieldSimple( "start", start, ClassType::DF_START );
                addCclFieldSimple( "stop", end, ClassType::DF_END ); // "end" keyword is reserved by ccl
                addCclFieldSimple( "period", period, ClassType::DF_PERIOD );

                addCclFieldSimple( "is_rnd", bRnd, ClassType::DF_B_RND );
                addCclFieldSimple( "min_period", periodMin, ClassType::DF_PERIOD_MIN );
                addCclFieldSimple( "ignore_start", bIgnoreStart, ClassType::DF_B_IGNORE_START );

                fwCreateCclFields();
                initCclFields();
            }
            
            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            std::string gate;
            std::string mode;
            double start;
            double end;
            double period;

            bool bRnd;
            double periodMin;
            bool bIgnoreStart;
        };
        

        struct CheckpointData : public BaseData
        {
            using ClassType = Checkpoint;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CHECKPOINT;

            inline static const std::string DF_TIMER = DF_NOELEMENT;

        //---ctor, dtor
            CheckpointData( const std::string& name, const std::string& timer, const std::string& message = ClassType::DF_MESSAGE, bool bSound = ClassType::DF_B_SOUND ) 
            : BaseData( name ), timer( timer ), message( message ), bSound( bSound ) {;}

            CheckpointData() = default;
            virtual ~CheckpointData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "timer", timer, DF_TIMER );
                addCclFieldSimple( "message", message, ClassType::DF_MESSAGE );
                //addCclFieldSimple( "sound", bSound, ClassType::DF_B_SOUND );

                fwCreateCclFields();
                initCclFields();
            }

            void loadFirst( World* world ) override;

        //---data
            std::string timer;
            std::string message; 
            bool bSound;
        };






//============================================================================================== PLACERS

        struct CoordsData : public BaseData
        {
            using ClassType = GroElement;

            static constexpr double DF_X = 0.0;
            static constexpr double DF_Y = 0.0;
            static constexpr double DF_R = 0.0;
            inline static const std::string DF_MODE = "cartesian";


        //---ctor, dtor
            CoordsData( const std::string& name, double x = DF_X, double y = DF_Y, double r = DF_R, const std::string& mode = DF_MODE ) 
            : BaseData( name ), x( x ), y( y ), r( r ), mode( mode ) {;}
            
            CoordsData() = default;
            virtual ~CoordsData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "x", x, DF_X );
                addCclFieldSimple( "y", y, DF_Y );
                addCclFieldSimple( "r", r, DF_R );
                addCclFieldSimple( "mode", mode, DF_MODE );

                fwCreateCclFields();
                initCclFields();
            }

        //---data
            double x;
            double y;
            double r;
            std::string mode;
        };

        struct RectangleData : public BaseData
        {
            using ClassType = GroElement;

            static constexpr double DF_X1 = 0.0;
            static constexpr double DF_X2 = 0.0;
            static constexpr double DF_Y1 = 0.0;
            static constexpr double DF_Y2 = 0.0;


        //---ctor, dtor
            RectangleData( const std::string& name, double x1 = DF_X1, double x2 = DF_X2, double y1 = DF_Y1, double y2 = DF_Y2 ) 
            : BaseData( name ), x1( x1 ), x2( x2 ), y1( y1 ), y2( y2 ) {;}
            
            RectangleData() = default;
            virtual ~RectangleData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "x1", x1, DF_X1 );
                addCclFieldSimple( "x2", x2, DF_X2 );
                addCclFieldSimple( "y1", y1, DF_Y1 );
                addCclFieldSimple( "y2", y2, DF_Y2 );

                fwCreateCclFields();
                initCclFields();
            }

        //---data
            double x1;
            double x2;
            double y1;
            double y2;
        };


        struct CellPlacerData : public OptionalDistUser
        {
            using ClassType = CellPlacer;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELL_PLACER;

            inline static const std::vector<std::string> DF_CELL_TYPES = {};
            inline static const std::string DF_TIMER = TimerData::AUTO_START_NAME;
            inline static const std::vector<double> DF_CELL_PROBS = {};
            inline static const std::string DF_AMOUNT_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_AMOUNT_DISTTYPE );
            static constexpr double DF_AUTOCOMPLETE_PROB = 1.0; 


        //---ctor, dtor
            CellPlacerData( const std::string& name, const std::vector<std::string>& cellTypes, const std::string& timer = DF_TIMER, const std::string& amountDistType = DF_AMOUNT_DISTTYPE, const std::vector<double>& amountParams = ClassType::DF_AMOUNT_PARAMS
            , const CoordsData& coordsData = CoordsData(), const std::vector<double>& cellProbs = DF_CELL_PROBS, bool bMixed = ClassType::DF_BMIXED ) 
            : BaseData( name, true, false ), OptionalDistUser(), amount( amountDistType, amountParams )
            , cellTypes( cellTypes ), timer( timer ), coordsData( coordsData ), cellProbs( cellProbs ), bMixed( bMixed ) {;}

            CellPlacerData() : amount( DF_AMOUNT_DISTTYPE, ClassType::DF_AMOUNT_PARAMS ) {;} 
            virtual ~CellPlacerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalDistUser::addOptionalDistFields( amount, "amount" );

                addCclFieldList( "cell_types", cellTypes, false, DF_CELL_TYPES );
                addCclFieldSimple( "timer", timer, DF_TIMER );
                
                addCclFieldRecord( "coords", coordsData );
                addCclFieldList( "cells_probs", cellProbs, false, DF_CELL_PROBS );
                addCclFieldList( "cell_probs", cellProbs, false, DF_CELL_PROBS );
                addCclFieldSimple( "mixed", bMixed, ClassType::DF_BMIXED );
                addCclFieldSimple( "mix", bMixed, ClassType::DF_BMIXED );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            OptionalDist amount;
            
            std::vector<std::string> cellTypes;
            std::string timer;

            CoordsData coordsData;
            std::vector<double> cellProbs;
            bool bMixed;
        };


        struct CellPlatingData : public OptionalDistUser
        {
            using ClassType = CellPlating;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELL_PLATING;

            inline static const std::string DF_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_TIMER = TimerData::AUTO_START_NAME;
            inline static const std::string DF_TAKE_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_TAKE_DISTTYPE );
            inline static const std::string DF_PUT_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_PUT_DISTTYPE );


        //---ctor, dtor
            CellPlatingData( const std::string& name, const std::string& gate = DF_GATE, const std::string& timer = DF_TIMER
            , const std::string& takeDistType = DF_TAKE_DISTTYPE, const std::vector<double>& takeParams = ClassType::DF_TAKE_PARAMS, const std::string& putDistType = DF_PUT_DISTTYPE, const std::vector<double>& putParams = ClassType::DF_PUT_PARAMS
            , const CoordsData& coordsData = CoordsData(), const CoordsData& oldCoordsData = CoordsData() ) 
            : BaseData( name, true, false ), OptionalDistUser(), takeFraction( takeDistType, takeParams ), putFraction( putDistType, putParams )
            , gate( gate ), timer( timer ), coordsData( coordsData ), oldCoordsData( oldCoordsData ) {;}

            CellPlatingData() : takeFraction( DF_TAKE_DISTTYPE, ClassType::DF_TAKE_PARAMS ), putFraction( DF_PUT_DISTTYPE, ClassType::DF_PUT_PARAMS ) {;} 
            virtual ~CellPlatingData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                OptionalDistUser::addOptionalDistFields( takeFraction, "take" );
                OptionalDistUser::addOptionalDistFields( putFraction, "put" );

                addCclFieldSimple( "gate", gate, DF_GATE );
                addCclFieldSimple( "timer", timer, DF_TIMER );

                addCclFieldRecord( "new_coords", coordsData );
                addCclFieldRecord( "coords", coordsData );
                addCclFieldRecord( "old_coords", oldCoordsData );
                
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override { takeFraction.preprocess( enclosingParser ); putFraction.preprocess( enclosingParser ); }
            void loadFirst( World* world ) override;

        //---data
            OptionalDist takeFraction;
            OptionalDist putFraction;

            std::string gate;
            std::string timer;
            
            CoordsData coordsData;
            CoordsData oldCoordsData;
        };


        struct SignalPlacerData : public OptionalDistUser
        {
            using ClassType = SignalPlacer;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::SIGNAL_PLACER;

            inline static const std::vector<std::string> DF_MSIGNALS = {};
            inline static const std::string DF_TIMER = TimerData::AUTO_START_NAME;
            inline static const std::string DF_AMOUNT_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_AMOUNT_DISTTYPE );
            inline static const std::string DF_SOURCE_NUM_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_SOURCE_NUM_DISTTYPE );
            inline static const std::string DF_SIZE_DISTTYPE = ut::ContinuousDistFactory::idx2name( ClassType::DF_SIZE_DISTTYPE );
            inline static const std::string DF_MODE = ClassType::DF_MODE == ClassType::Mode::SET ? "set" : "add";
            inline static const std::vector<double> DF_SIGNAL_PROBS = {};
            static constexpr double DF_AUTOCOMPLETE_PROB = 1.0; 


        //---ctor, dtor
            SignalPlacerData( const std::string& name, const std::vector<std::string>& msignals, const std::string& timer = DF_TIMER
            , const std::string& amountDistType = DF_AMOUNT_DISTTYPE, const std::vector<double>& amountParams = ClassType::DF_AMOUNT_PARAMS, const std::string& sourcesDistType = DF_SOURCE_NUM_DISTTYPE, const std::vector<double>& sourcesParams = ClassType::DF_SOURCE_NUM_PARAMS
            , const std::string& sizeDistType = DF_SIZE_DISTTYPE, const std::vector<double>& sizeParams = ClassType::DF_SIZE_PARAMS
            , const CoordsData& coordsData = CoordsData(), const std::string& mode = DF_MODE, const std::vector<double>& signalProbs = DF_SIGNAL_PROBS, bool bMixed = ClassType::DF_BMIXED, const RectangleData& constAreaData = RectangleData() ) 
            
            : BaseData( name, true, false ), OptionalDistUser(), amount( amountDistType, amountParams ), sourceNum( sourcesDistType, sourcesParams ), size( sizeDistType, sizeParams )
            , msignals( msignals ), timer( timer )
            , coordsData( coordsData ), mode( mode ), bMixed( bMixed ), signalProbs( signalProbs ), constAreaData( constAreaData ) {;}

            SignalPlacerData() : amount( DF_AMOUNT_DISTTYPE, ClassType::DF_AMOUNT_PARAMS ), sourceNum( DF_SOURCE_NUM_DISTTYPE, ClassType::DF_SOURCE_NUM_PARAMS ), size( DF_SIZE_DISTTYPE, ClassType::DF_SIZE_PARAMS ) {;} 
            virtual ~SignalPlacerData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );
                
                OptionalDistUser::addOptionalDistFields( amount, "amount" );
                OptionalDistUser::addOptionalDistFields( sourceNum, "sources" );
                OptionalDistUser::addOptionalDistFields( size, "size" );

                addCclFieldList( "signals", msignals, false, DF_MSIGNALS );
                addCclFieldSimple( "timer", timer, DF_TIMER );

                addCclFieldRecord( "coords", coordsData );
                addCclFieldSimple( "mode", mode, DF_MODE );
                addCclFieldSimple( "mixed", bMixed, ClassType::DF_BMIXED );
                addCclFieldSimple( "mix", bMixed, ClassType::DF_BMIXED );
                addCclFieldList( "signals_probs", signalProbs, false, DF_SIGNAL_PROBS );
                addCclFieldList( "signal_probs", signalProbs, false, DF_SIGNAL_PROBS );

                addCclFieldRecord( "area", constAreaData );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
            OptionalDist amount;
            OptionalDist sourceNum;
            OptionalDist size;

            std::vector<std::string> msignals;
            std::string timer;

            CoordsData coordsData;
            std::string mode;
            bool bMixed;
            std::vector<double> signalProbs;

            RectangleData constAreaData;
        };





//============================================================================================== OUTPUT

        struct SnapshotData : public BaseData
        {
            using ClassType = Snapshot;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::SNAPSHOT;

            inline static const std::string DF_TIMER = TimerData::AUTO_EVERY_STEP_NAME;


        //---ctor, dtor
            SnapshotData( const std::string& name, const std::string& timer = DF_TIMER
            , const std::string& basePath = ClassType::DF_BASE_PATH, const std::string& folderName = ClassType::DF_FOLDER_NAME, const std::string& fileName = ClassType::DF_FILE_NAME, const std::string& imgFormat = ClassType::DF_FORMAT ) 
            : BaseData( name ), timer( timer ), basePath( basePath ), folderName( folderName ), fileName( fileName ), imgFormat( imgFormat ), path( basePath + folderName ) {;}

            SnapshotData() = default;
            virtual ~SnapshotData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "timer", timer, DF_TIMER );

                addCclFieldSimple( "path", basePath, ClassType::DF_BASE_PATH );
                addCclFieldSimple( "folder", folderName, ClassType::DF_FOLDER_NAME );
                addCclFieldSimple( "file_name", fileName, ClassType::DF_FILE_NAME );
                addCclFieldSimple( "format", imgFormat, ClassType::DF_FORMAT );
        
                fwCreateCclFields();
                initCclFields();
            }
            void preprocess() override { path = basePath + "/" + folderName; }
            void loadFirst( World* world ) override;

        //---data
            std::string timer;

            std::string basePath;
            std::string folderName;
            std::string fileName;
            std::string imgFormat;
          //preprocess
            std::string path;
        };

        
        struct CellsFileData : public BaseData
        {
            using ClassType = CellsFile;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELLS_FILE;

            inline static const std::string DF_TIMER = TimerData::AUTO_EVERY_STEP_NAME;
            inline static const std::string DF_MODE = CellsFile::modeNM.subtype2Name( CellsFile::DF_MODE );
            
            inline static const std::vector<std::string> DF_FIELDS = {};
            inline static const std::vector<std::string> DF_POPULATION_FIELDS = {};

            inline static const std::string DF_FILTER_GATE = BGateData::AUTO_TRUE_NAME;
            inline static const std::string DF_INCLUDE_ELEMS_MODE = "none";
            
            inline static const std::string DF_STATS_PACK = "avg";
            inline static const std::vector<std::string> DF_STATS = {}; 

            inline static const std::vector<AllElementIdx> ALL_ELEMS = { BMARKER, QMARKER, EVENT_MARKER, FUNCTION, ODE, HISTORIC, GATE, MOLECULE, REGULATION, OPERON, ORI_T, PARTITION_SYSTEM, COPY_CONTROL, ORI_V, PLASMID, STRAIN, CELL_TYPE, METABOLITE, FLUX, CELL_COLOUR };
            inline static const std::vector<AllElementIdx> CLASSIC_ELEMS = { CELL_TYPE, MOLECULE, PLASMID };


        //---ctor, dtor
            CellsFileData( const std::string& name, const std::string& timer = DF_TIMER, const std::string& mode = DF_MODE
            , const std::string& basePath = ClassType::DF_BASE_PATH, const std::string& folderName = ClassType::DF_FOLDER_NAME, const std::string& fileName = ClassType::DF_FILE_NAME, const std::string& format = ClassType::DF_FORMAT, int precision = ClassType::DF_PRECISION
            , const std::vector<std::string>& statFields = {}, const std::vector<std::string>& otherFields = {} )
            : BaseData( name ), timer( timer ), mode( mode )
            , basePath( basePath ), folderName( folderName ), fileName( fileName ), format( format ), precision( precision )
            , path( basePath + folderName ), statFields( statFields ), otherFields( otherFields ) {;}

            CellsFileData( const std::string& name, const std::string& timer = DF_TIMER, const std::string& mode = DF_MODE
            , const std::string& basePath = ClassType::DF_BASE_PATH, const std::string& folderName = ClassType::DF_FOLDER_NAME, const std::string& fileName = ClassType::DF_FILE_NAME, const std::string& format = ClassType::DF_FORMAT, int precision = ClassType::DF_PRECISION
            , const std::vector<std::string>& fields = DF_FIELDS, const std::vector<std::string>& populationFields = DF_POPULATION_FIELDS, const std::string& filterGate = DF_FILTER_GATE, const std::string& includeElemsMode = DF_INCLUDE_ELEMS_MODE
            , const std::string& statsPack = DF_STATS_PACK, const std::vector<std::string>& stats = DF_STATS, const std::string& autoVolMode = PopulationStatData::DF_AUTO_VOL )

            : BaseData( name ), timer( timer ), mode( mode )
            , basePath( basePath ), folderName( folderName ), fileName( fileName ), format( format ), precision( precision )
            , fields( fields ), populationFields( populationFields ), filterGate( filterGate ), includeElemsMode( includeElemsMode )
            , statsPack( statsPack ), stats( stats ), autoVolMode( autoVolMode )
            , path( basePath + folderName ) {;}

            CellsFileData() = default;
            virtual ~CellsFileData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "timer", timer, DF_TIMER );
                addCclFieldSimple( "mode", mode, DF_MODE );
                addCclFieldSimple( "ancestry", bIncludeAncestry, ClassType::DF_B_ANCESTRY );

                addCclFieldSimple( "path", basePath, ClassType::DF_BASE_PATH );
                addCclFieldSimple( "folder", folderName, ClassType::DF_FOLDER_NAME );
                addCclFieldSimple( "file_name", fileName, ClassType::DF_FILE_NAME );
                addCclFieldSimple( "format", format, ClassType::DF_FORMAT );
                addCclFieldSimple( "precision", precision, ClassType::DF_PRECISION );

                addCclFieldList( "fields", fields, true, DF_FIELDS );
                addCclFieldList( "pop_fields", populationFields, true, DF_POPULATION_FIELDS );
                
                addCclFieldSimple( "filter_gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "include_elems", includeElemsMode, DF_INCLUDE_ELEMS_MODE  );
                addCclFieldSimple( "stats", statsPack, DF_STATS_PACK );
                addCclFieldList( "stats", stats, true, DF_STATS );
                addCclFieldSimple( "auto_vol", autoVolMode, PopulationStatData::DF_AUTO_VOL );
                
                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

          //special
            void addElem( const std::string& elemName );
            void addElemType( AllElementIdx elemTypeIdx );
            inline void addElemTypes( const std::vector<AllElementIdx>& elemTypeIdxs ) { for( AllElementIdx elemTypeIdx : elemTypeIdxs ) addElemType( elemTypeIdx ); }

        //---data
          //load
            std::string timer;
            std::string mode;
            bool bIncludeAncestry;
            
            std::string basePath;
            std::string folderName;
            std::string fileName;
            std::string format;
            int precision;
          //fields raw
            std::vector<std::string> fields;
            std::vector<std::string> populationFields;
    
            std::string filterGate;
            std::string includeElemsMode; 
            std::string statsPack;
            std::vector<std::string> stats;
            std::string autoVolMode;

          //preprocess
            std::string path;
            std::vector<std::string> statFields;
            std::vector<std::string> otherFields;
        };


        struct CellsPlotData : public BaseData
        {
            using ClassType = CellsPlot;
            static constexpr Parser::AllElementIdx typeIdx = Parser::AllElementIdx::CELLS_PLOT;

            inline static const std::string DF_TIMER = TimerData::AUTO_NEVER_NAME;
            inline static const std::vector<std::string> DF_COLOURS = {};
            inline static const std::vector<std::string> DF_FIELDS = {};
            inline static const std::vector<std::string> DF_POPULATION_FIELDS = {};
            inline static const std::string DF_FILTER_GATE = BGateData::AUTO_TRUE_NAME;
            
            inline static const std::string DF_STATS_PACK = "avg";
            inline static const std::vector<std::string> DF_STATS = {}; 


        //--ctor, dtor
            CellsPlotData( const std::string& name, const std::string& timer = DF_TIMER, const std::vector<std::string>& hexColours = DF_COLOURS
            , const std::string& basePath = ClassType::DF_BASE_PATH, const std::string& folderName = ClassType::DF_FOLDER_NAME, const std::string& fileName = ClassType::DF_FILE_NAME, const std::string& format = ClassType::DF_FORMAT
            , const std::vector<std::string>& statFields = {}, const std::vector<std::string>& otherFields = {}, int historicSize = ClassType::DF_HISTORIC_SIZE, int precision = ClassType::DF_PRECISION )
            : BaseData( name ), timer( timer ), hexColours( hexColours ), historicSize( historicSize ), precision( precision )
            , basePath( basePath ), folderName( folderName ), fileName( fileName ), format( format )
            , path( basePath + folderName ), statFields( statFields ), otherFields( otherFields ) {;}

            CellsPlotData( const std::string& name, const std::string& timer = DF_TIMER, const std::vector<std::string>& hexColours = DF_COLOURS
            , const std::string& basePath = ClassType::DF_BASE_PATH, const std::string& folderName = ClassType::DF_FOLDER_NAME, const std::string& fileName = ClassType::DF_FILE_NAME, const std::string& format = ClassType::DF_FORMAT
            , std::vector<std::string> fields = DF_POPULATION_FIELDS, std::vector<std::string> populationFields = DF_POPULATION_FIELDS, std::string filterGate = DF_FILTER_GATE
            , std::string statsPack = DF_STATS_PACK, std::vector<std::string> stats = DF_STATS, int historicSize = ClassType::DF_HISTORIC_SIZE, int precision = ClassType::DF_PRECISION )
            : BaseData( name ), timer( timer ), hexColours( hexColours ), historicSize( historicSize ), precision( precision )
            , basePath( basePath ), folderName( folderName ), fileName( fileName ), format( format )
            , fields( fields ), populationFields( populationFields ), filterGate( filterGate ), statsPack( statsPack ), stats( stats )
            , path( basePath + folderName ) {;}

            CellsPlotData() = default;
            virtual ~CellsPlotData() = default;

        //---API
            void createCclFields( const std::string& dfName=BaseData::DF_NOELEMENT ) override
            {
                BaseData::createCclFields( makeDfName( this, dfName ) );

                addCclFieldSimple( "timer", timer, DF_TIMER );
                addCclFieldList( "colors", hexColours, true, DF_COLOURS );
                addCclFieldList( "colours", hexColours, true, DF_COLOURS );
                addCclFieldSimple( "historic_size", historicSize, ClassType::DF_HISTORIC_SIZE );
                addCclFieldSimple( "precision", precision, ClassType::DF_PRECISION );

                addCclFieldSimple( "path", basePath, ClassType::DF_BASE_PATH );
                addCclFieldSimple( "folder", folderName, ClassType::DF_FOLDER_NAME );
                addCclFieldSimple( "file_name", fileName, ClassType::DF_FILE_NAME );
                addCclFieldSimple( "format", format, ClassType::DF_FORMAT );

                addCclFieldList( "fields", fields, true, DF_FIELDS );
                addCclFieldList( "pop_fields", populationFields, true, DF_POPULATION_FIELDS );
                addCclFieldSimple( "filter_gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "gate", filterGate, DF_FILTER_GATE );
                addCclFieldSimple( "stats", statsPack, DF_STATS_PACK );
                addCclFieldList( "stats", stats, true, DF_STATS );
                addCclFieldSimple( "auto_vol", autoVolMode, PopulationStatData::DF_AUTO_VOL );

                fwCreateCclFields();
                initCclFields();
            }

            void preprocess() override;
            void loadFirst( World* world ) override;

        //---data
          //load
            std::string timer;
            std::vector<std::string> hexColours;
            int historicSize;
            int precision;

            std::string basePath;
            std::string folderName;
            std::string fileName;
            std::string format;
          //fields raw
            std::vector<std::string> fields;
            std::vector<std::string> populationFields;

            std::string filterGate;
            std::string statsPack;
            std::vector<std::string> stats;
            std::string autoVolMode;

          //preprocess
            std::string path;
            std::vector<std::string> statFields;
            std::vector<std::string> otherFields;
        };








//*******************************************************************************************************************************************************************************************************
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////          PARSER          /////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************************************************************************************************************

        friend BaseData;
        using ParsedDataElement = UP<BaseData>;
        using ParsedDataUnitDS = std::map< std::string, ParsedDataElement >;
        using ParsedDataDS = std::vector< ParsedDataUnitDS >;
        
        inline static const std::string AUTO_NAME_PREFIX = "_";


    //---ctor, dtor
        inline Parser( const std::string& path ) 
        : parsedData( static_cast<size_t>( AllElementIdx::COUNT ) )
        , cclAdapter( std::make_shared<CclAdapter>( path ) ) { createAutoFirst(); }
        
        virtual ~Parser() = default;

    //---get
        inline const ParsedDataDS& getParsedData() const { return parsedData; } 
        inline ParsedDataDS& getParsedDataEdit()  { return parsedData; } 
        inline const ParsedDataUnitDS& getParsedData( AllElementIdx elemTypeIdx ) const { return parsedData[ elemTypeIdx ]; }
        inline ParsedDataUnitDS& getParsedDataEdit( AllElementIdx elemTypeIdx ) { return parsedData[ elemTypeIdx ]; }
        inline CclAdapter* getCclAdapter() { return cclAdapter.get(); } 

    //---API
        template< typename T, typename... Ts >
        static UP<T> constructElement( Ts... ctorArgs ) { UP<T> elementData = std::make_unique<T>( ctorArgs... ); elementData->createCclFields( "" ); return elementData; }

        template< typename T, typename... Ts >
        static T constructElementObj( Ts... ctorArgs ) { T elementData( ctorArgs... ); elementData.createCclFields( "" ); return elementData; }

        template< typename T >
        void newElement( Value* data );

        template<typename T, typename... Ts >
        void justCreate( Ts... args );

        template<typename T, typename... Ts >
        void createAndPreprocess( Ts&&... args );

        template<typename T, typename TCaller, typename... Ts >
        std::string createFrom( TCaller* caller, const std::string& sufix, Ts... args );

        template< typename T >
        void addElementData( T* elementData ) { parsedData[T::typeIdx].emplace( elementData->name, UP<T>( elementData ) ); }

        inline void parseFile() { cclAdapter->init(); } 

        void loadAll( World* world, uint seed = 0 );

        void createAutoFirst();


    private:
        ParsedDataDS parsedData;
        SP<CclAdapter> cclAdapter;
};





//*******************************************************************************************************************************************************************************************************
//////////////////////////////////////////////////////////////////////////          TEMPLATE DEFNITIONS          ////////////////////////////////////////////////////////////////////////////////////////
//*******************************************************************************************************************************************************************************************************
    
    template<typename T>
    std::string Parser::createAutoName( const std::string& baseName, const std::string& sufix ) const
    { 
        std::string name = AUTO_NAME_PREFIX + std::string( T::metadata.key ) + "_" + baseName;
        if( sufix != "" )
            return  name + "_" + sufix;
        return name;
    } 

//---
    template< typename T >
    void Parser::newElement( Value* data )
    {
        if( data->get_type() == Value::RECORD )
        {
            UP<T> elementData = constructElement<T>();
            elementData->setEnclosingParser( this );
            elementData->loadCclFields( data, this );
            elementData->preprocess();
            parsedData[T::typeIdx].emplace( elementData->name, std::move( elementData ) );
        }
    }

    template<typename T, typename... Ts >
    void Parser::justCreate( Ts... args )
    {
        UP<T> elementData = std::make_unique<T>( args... );
        elementData->setEnclosingParser( this );
        parsedData[T::typeIdx].emplace( elementData->name, std::move( elementData ) );
    }

    template<typename T, typename... Ts >
    void Parser::createAndPreprocess( Ts&&... args )
    {
        UP<T> elementData = std::make_unique<T>( args... );
        elementData->setEnclosingParser( this );
        elementData->preprocess();
        parsedData[T::typeIdx].emplace( elementData->name, std::move( elementData ) );
    }

    template<typename T, typename TCaller, typename... Ts >
    std::string Parser::createFrom( TCaller* caller, const std::string& sufix, Ts... args )
    {
        std::string name = createAutoName<typename T::ClassType>( caller->name, sufix );

        UP<T> elementData = std::make_unique<T>( name, args... );
        elementData->setEnclosingParser( this );
        elementData->preprocess();
        parsedData[T::typeIdx].emplace( elementData->name, std::move( elementData ) );
        
        return name;
    }

#endif //_PARSER_H_
