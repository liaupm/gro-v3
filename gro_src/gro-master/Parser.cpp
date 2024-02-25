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

#include "Parser.h"
#include "ut/Shape.hpp" //circle of CoordSelectors
#include "ut/DistributionCombi.hpp" //DistributionData
#include "ut/DistributionScaler.hpp" //Molecule
#include "ut/RgbColour.hpp" //CellColour, Signal

#include "CoordsSelector.h" //placers
#include "World.h" //getGeneticCollection() and create()
#include "GroThread.h" //set global params

//PRECOMPILED
/* #include <algorithm> // std::transform for uppercase
#include <cmath> //std::abs for expresion mean times */


/////////////////////////////////////////////////////////////////////////////////////// COMMON /////////////////////////////////////////////////////////////////////////////

void Parser::OptionalDist::preprocess( const Parser* enclosingParser )
{
    if( distDict.type != "" )
    {
        distType = distDict.type;
        params = distDict.params;
    }
    else if( distRef != BaseData::DF_NOELEMENT )
    {
        const DistributionData* distData = static_cast<const DistributionData*>( enclosingParser->getParsedData()[ AllElementIdx::DISTRIBUTION ].find( distRef )->second.get() );
        distType = distData->type;
        params = distData->params;
    }
    else if( exact != DF_EXACT )
    {
        if( distType == "uniform" )
            params = { exact, exact };
        else
            params = { exact, 0.0 };
    }
}


void Parser::OptionalScaler::preprocess( bool bCreateRnd )
{
    if( bCreateRnd && params.size() >= 2 && ut::isPositive( params[1] ) && rnd == BaseData::DF_NOELEMENT ) 
        rnd = parent.getEnclosingParser()->createFrom<RandomnessData>( &parent, prefix );

    if( params.size() < 2 && rnd == BaseData::DF_NOELEMENT )
	{
		if( exact != DF_EXACT )
        	params = { exact, 0.0 };
        else
        	params = { dfParams[0], 0.0 };
	}
	else if( params.size() == 0 )
		params = dfParams;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// CELL LOGIC ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//============================================================================================================================== MARKERS
void Parser::BMarkerData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::BMarker>( name );
}

void Parser::QMarkerData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::QMarker>( name );
}

void Parser::EventMarkerData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::EventMarker>( name );
}



//=============================================================================================================================================================== Randomness
void Parser::RandomnessData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::Randomness>( name, name2val<ut::ContinuousDistFactory>( distributionType, name, true, true ), params, inheritance, inertia, samplingTime, bRndSamplingTime, bCorrection );
}



//=============================================================================================================================================================== GATES

//-------------------------------------------------------------------------------------------------- BGate
void Parser::BGateData::preprocess()
{
  //process raw inputs to inputNames and inputsPresence
    inputNames.clear();
    inputPresence.clear();
    for( auto inputName : inputNamesRaw )
    {
        if( inputName.at(0) == '-' )
        {
            inputName.erase( inputName.begin() );
            inputPresence.push_back( false );
        }
        else
            inputPresence.push_back( true );

        inputNames.push_back( inputName );
    }
}

void Parser::BGateData::loadFirst( World* world )
{
    ut::toUpper( gateFunction );
    world->getGeneticCollectionEdit()->create<cg::GateBoolean>( name, std::vector<const cg::GeneticElement*>( {} ), inputPresence, name2val( gateFunction, ut::GateBoolean::functionNM, name, false ) ); //inputs empty because the element types may be load after
}

void Parser::BGateData::loadLast( World* world )
{
  //set inputs
    std::vector<const cg::GeneticElement*> inputElems = names2elemsGeneric<cg::GeneticElement>( inputNames, world->getGeneticCollection(), name );
    static_cast<cg::GateBoolean*>( world->getGeneticCollectionEdit()->getByNameEdit<cg::GateBase>( name ) )->setInput( inputElems );
}


//-------------------------------------------------------------------------------------------------- QGate
void Parser::QGateData::loadFirst( World* world )
{
    cg::GateQuantitative::CompOperator compOperatorVal = name2val( compOperator, ut::GateQuantitative::compOperatorNM, name );
    world->getGeneticCollectionEdit()->create<cg::GateQuantitative>( name, nullptr, value, compOperatorVal ); //input elem nullptr because the element types may be load after
}

void Parser::QGateData::loadLast( World* world )
{
  //set input elem
    const cg::GeneticElement* elemElem = name2elemGeneric<cg::GeneticElement>( elem, world->getGeneticCollection(), name );
    static_cast<cg::GateQuantitative*>( world->getGeneticCollectionEdit()->getByNameEdit<cg::GateBase>( name ) )->setInput( elemElem );
}



//=============================================================================================================================================================== QUANTITATIVE

//-------------------------------------------------------------------------------------------------- Function
void Parser::FunctionData::preprocess()
{
    if( randomness == DF_NOELEMENT && rndParams != DF_RND_PARAMS )
        randomness = enclosingParser->createFrom<RandomnessData>( this, "" );
}

void Parser::FunctionData::loadFirst( World* world )
{
    const cg::GeneticElement* randomnessElem = name2elemGeneric<cg::GeneticElement>( randomness, world->getGeneticCollection(), name, false ); //nullptr if deterministic

    ClassType::Type typeVal = name2val<ClassType::MathFunctionType::FactoryType>( type, name );
    ClassType::AutoVolMode autoVolModeVal = name2val( autoVolMode, cg::Function::autoVolModeNM, name );

    world->getGeneticCollectionEdit()->create<cg::Function>( name, std::vector<const cg::GeneticElement*>( {} ), params, typeVal, min, max, randomnessElem, rndParams, autoVolModeVal, roundPlaces ); //inputs empty because the element types may be load after
}

void Parser::FunctionData::loadLast( World* world )
{
  //set the inputs
    std::vector<const cg::GeneticElement*> inputElems = names2elemsGeneric<cg::GeneticElement>( input, world->getGeneticCollection(), name );
    world->getGeneticCollectionEdit()->getByNameEdit<cg::Function>( name )->setInput( inputElems );
}

//-------------------------------------------------------------------------------------------------- Ode

void Parser::OdeData::preprocess()
{ 
	iniValue.preprocess( enclosingParser ); 
	split.preprocess( true ); 

	if( split.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( split.marker );
}

void Parser::OdeData::loadFirst( World* world )
{
	const cg::QMarker* splitMarkerElem = name2elem<cg::QMarker>( split.marker, world->getGeneticCollection(), name, false );

	world->getGeneticCollectionEdit()->create<cg::Ode>( name, nullptr, nullptr, nullptr //elem nullptr because the element type may be load after
	, ut::ContinuousDistData( name2val<ut::ContinuousDistFactory>( iniValue.distType, name ), iniValue.params ), scale
	, typename ClassType::ParamDataType( nullptr, nullptr, splitMarkerElem, split.params )
    , bBinomialSplit ? ClassType::PartitionType::BINOMIAL : ClassType::PartitionType::CONTINUOUS, splitScale, bVolScalingSplit );
}

void Parser::OdeData::loadLast( World* world )
{   
	const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* deltaElemElem = name2elemGeneric<cg::GeneticElement>( deltaElem, world->getGeneticCollection(), name );
    const cg::GeneticElement* iniValueFunctionElem = name2elemGeneric<cg::GeneticElement>( iniValueFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* splitFunctionElem = name2elemGeneric<cg::GeneticElement>( splitFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* splitRandomnessElem = name2elemGeneric<cg::GeneticElement>( split.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::Ode>( name )->setAll( gateElem, deltaElemElem, iniValueFunctionElem, splitFunctionElem, splitRandomnessElem );
}

//-------------------------------------------------------------------------------------------------- Historic
void Parser::HistoricData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::Historic>( name, nullptr, nullptr, ut::fitL( delay ) ); //target and initial value may be element types that are loaded after
}

void Parser::HistoricData::loadLast( World* world )
{
  //set the target and ini value elements
    const cg::GeneticElement* targetElem = name2elemGeneric<cg::GeneticElement>( target, world->getGeneticCollection(), name );
    const cg::GeneticElement* iniValueElem = name2elemGeneric<cg::GeneticElement>( iniValue, world->getGeneticCollection(), name, false );
    world->getGeneticCollectionEdit()->getByNameEdit<cg::Historic>( name )->setAll( targetElem, iniValueElem );
}


//=============================================================================================================================================================== Cell Colour

void Parser::CellColourData::preprocess()
{
	maxValue.preprocess( true );
	delta.preprocess( name2val( colourMode, cg::CellColour::colourModeNM, name ) == cg::CellColour::ColourMode::DELTA );
}

void Parser::CellColourData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::CellColour>( name, nullptr, name2val( colourMode, cg::CellColour::colourModeNM, name )
    , ut::RgbColour( rgbColour ), nullptr, maxValue.params, nullptr, delta.params, nullptr, scale ); //target nullptr because it can be of a type that is loaded after
}

void Parser::CellColourData::loadLast( World* world )
{
	const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* maxValueRndElem = name2elemGeneric<cg::GeneticElement>( maxValue.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* deltaRndElem = name2elemGeneric<cg::GeneticElement>( delta.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* targetElem = name2elemGeneric<cg::GeneticElement>( target, world->getGeneticCollection(), name, false );
    
    world->getGeneticCollectionEdit()->getByNameEdit<cg::CellColour>( name )->setAll( gateElem, maxValueRndElem, deltaRndElem, targetElem );
}










//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// CELL BIO ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================================================================================================== EXPRESSION

//-------------------------------------------------------------------------------------------------- Molecule
void Parser::MoleculeData::preprocess()
{
    timeFuns.clear();
    timeFunsSigns.clear();
    for( auto fun : timeFunsRaw )
    {
        if( fun.length() > 0 && fun.at(0) == '-' )
        {
            fun.erase( fun.begin() );
            timeFunsSigns.push_back( -1 );
        }
        else
            timeFunsSigns.push_back( 1 );

        timeFuns.push_back( fun );
    }

//correct array sizes to match meanTimes' size
  //variability
    while( varTimes.size() < meanTimes.size() )
        varTimes.push_back( ClassType::DF_VAR_TIME );
  //activation
    if( activations.size() == 0 )
        activations.push_back( ClassType::DF_FIRST_THRESHOLD );

    while( activations.size() < meanTimes.size() -1 )
        activations.push_back( activations.back() + ClassType::DF_THRESHOLD_INCREASE );

    activations.insert( activations.begin(), std::numeric_limits<TReal>::lowest() );

  //create automatic Randomness if not deterministic
    bool isRnd = false;
    for( auto val : varTimes )
    {
        if( ut::notZero( val ) ) //in case randomness == DF_NOELEMENT, the meaning is always stddeev
        {
            isRnd = true;
            break;
        }
    }
    if( isRnd && randomness == DF_NOELEMENT )
        randomness = enclosingParser->createFrom<RandomnessData>( this, "" );
}

void Parser::MoleculeData::loadFirst( World* world )
{
    world->getGeneticCollectionEdit()->create<cg::Molecule>( name, nullptr, cg::Molecule::DistVectorDS(), nullptr, bSymmetricRnd );
}

void Parser::MoleculeData::loadLast( World* world )
{
	const cg::GeneticElement* randomnessElem = name2elemGeneric<cg::GeneticElement>( randomness, world->getGeneticCollection(), name, false );
	const cg::GeneticElement* customRndSymmetryElem = name2elemGeneric<cg::GeneticElement>( customRndSymmetry, world->getGeneticCollection(), name, false );

  //create time distributions vector from time params, direction and activation thresholds (requires randomnessElem )
	cg::Molecule::DistVectorDS timeDistributions;
    for( size_t ed = 0; ed < std::max( meanTimes.size(), timeFuns.size() ); ed++ )
    {
    	const cg::GeneticElement* timeFunElem =  nullptr;
    	if( ed < timeFuns.size() )
    		timeFunElem = ed < timeFuns.size() ? name2elemGeneric<cg::GeneticElement>( timeFuns[ed], world->getGeneticCollection(), name, false ) : nullptr;

    	if( timeFunElem )
    	{
    		timeDistributions.emplace_back( activations[ed], cg::Molecule::TimeDistribution( timeFunElem, nullptr, static_cast<cg::Molecule::ExpressionDirection>( timeFunsSigns[ed] ) ) ); 
    	}
    	else if( ed < meanTimes.size() )
    	{
		    SP<ut::DistributionScaler> scaler = ut::DistributionScalerFactory::create( cg::Randomness::getScalerType( randomnessElem ), ut::DistributionScaler::ParamsType( { std::abs( meanTimes[ed] ), varTimes[ed] } ) );
		    timeDistributions.emplace_back( activations[ed], cg::Molecule::TimeDistribution( nullptr, scaler, static_cast<cg::Molecule::ExpressionDirection>( ut::sign( meanTimes[ed] ) ) ) ); 
    	}
    }
    
    world->getGeneticCollectionEdit()->getByNameEdit<cg::Molecule>( name )->setAll( randomnessElem, customRndSymmetryElem, timeDistributions );
}



//-------------------------------------------------------------------------------------------------- Regulations
void Parser::RegulationData::preprocess()
{
    if( actiMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( actiMarker );

    onActi.preprocess( false );
    offActi.preprocess( false );

    if( randomness == DF_NOELEMENT && ( ut::isPositive( onActi.params[1] ) || ut::isPositive( offActi.params[1] ) )  )
        randomness = enclosingParser->createFrom<RandomnessData>( this, "" );
}

void Parser::RegulationData::loadFirst( World* world )
{
    const cg::QMarker* actiMarkerElem = name2elem<cg::QMarker>( actiMarker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::Regulation>( name, nullptr, nullptr, onActi.params, offActi.params, nullptr, nullptr, nullptr, actiMarkerElem, bAutoDivision );
}

void Parser::RegulationData::loadLast( World* world )
{
    const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* dosageFunElem = name2elemGeneric<cg::GeneticElement>( dosageFun, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* onFunctionElem = name2elemGeneric<cg::GeneticElement>( onFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* offFunctionElem = name2elemGeneric<cg::GeneticElement>( offFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* randomnessElem = name2elemGeneric<cg::GeneticElement>( randomness, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::Regulation>( name )->setAll( gateElem, dosageFunElem, onFunctionElem, offFunctionElem, randomnessElem );
}



//-------------------------------------------------------------------------------------------------- Operon
void Parser::OperonData::preprocess()
{
	if( actiMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( actiMarker );

    if( regulation == DF_NOELEMENT ) 
        regulation = enclosingParser->createFrom<RegulationData>( this, "", gate, RegulationData::DF_ON_ACTI_PARAMS, RegulationData::DF_OFF_ACTI_PARAMS ); //exact, without Randomness and without dosage Function or effect
}

void Parser::OperonData::loadFirst( World* world )
{
    const cg::Regulation* regulationElem = name2elem<cg::Regulation>( regulation, world->getGeneticCollection(), name );
    std::vector<const cg::Molecule*> outputElems = names2elems<cg::Molecule>( output, world->getGeneticCollection(), name );
    const cg::QMarker* actiMarkerElem = name2elem<cg::QMarker>( actiMarker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::Operon>( name, regulationElem, outputElems, actiMarkerElem, bDosageEffect );
}





//=============================================================================================================================================================== METABOLISM

//-------------------------------------------------------------------------------------------------- Metabolite
void Parser::MetaboliteData::loadFirst( World* world )
{
    //this is only called from here for _biomass, the other ones are loaded from SignalData
    world->getGeneticCollectionEdit()->create<cg::Metabolite>( name, nullptr, bBiomass ); //no QMarker for concentration because it is biomass
}


//-------------------------------------------------------------------------------------------------- Flux
void Parser::FluxData::preprocess()
{
	amount.preprocess( true );
	threshold.preprocess( true );

	if( threshold.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( threshold.marker );
}

void Parser::FluxData::loadFirst( World* world )
{
    const cg::Metabolite* metaboliteElem = name2elem<cg::Metabolite>( metabolite, world->getGeneticCollection(), name );
    std::vector<const cg::Molecule*> outputElems = names2elems<cg::Molecule>( output, world->getGeneticCollection(), name );
    const cg::QMarker* amountMarkerElem = name2elem<cg::QMarker>( amount.marker, world->getGeneticCollection(), name, false );
    const cg::QMarker* thresholdMarkerElem = name2elem<cg::QMarker>( threshold.marker, world->getGeneticCollection(), name, false );

    ClassType::TimingType timingTypeVal = name2val( timingType, ClassType::timingTypeNM, name );
    ClassType::FluxDirection directionVal = name2val( directionStr, ClassType::directionNM, name );

    if( direction < 0 )
        directionVal = ClassType::FluxDirection::ABSORPTION;
    else if( direction > 0 )
        directionVal = ClassType::FluxDirection::EMISSION;

    world->getGeneticCollectionEdit()->create<cg::Flux>( name, metaboliteElem, nullptr, nullptr, outputElems
    , typename ClassType::ParamDataType( nullptr, nullptr, amountMarkerElem, amount.params ) 
    , typename ClassType::ParamDataType( nullptr, nullptr, thresholdMarkerElem, threshold.params ) 
    , timingTypeVal, activation, directionVal, ! bAbsolute, bInverted );
}

void Parser::FluxData::loadLast( World* world )
{
	const cg::GeneticElement* internalGateElem = name2elemGeneric<cg::GeneticElement>( internalGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* signalsGateElem = name2elemGeneric<cg::GeneticElement>( signalsGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* amountFunctionElem = name2elemGeneric<cg::GeneticElement>( amountFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* amountRandomnessElem = name2elemGeneric<cg::GeneticElement>( amount.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* thresholdRandomnessElem = name2elemGeneric<cg::GeneticElement>( threshold.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::Flux>( name )->setAll( internalGateElem, signalsGateElem, amountFunctionElem, amountRandomnessElem, thresholdRandomnessElem );
}





//=============================================================================================================================================================== REPLICATION

//-------------------------------------------------------------------------------------------------- CopyControl
void Parser::CopyControlData::preprocess()
{
	if( activeMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( activeMarker );
    w.preprocess( true );

    if( w.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( w.marker );
}

void Parser::CopyControlData::loadFirst( World* world )
{
    const cg::QMarker* activeMarkerElem = name2elem<cg::QMarker>( activeMarker, world->getGeneticCollection(), name, false );
    const cg::QMarker* wMarkerElem = name2elem<cg::QMarker>( w.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::CopyControl>( name, nullptr, typename ClassType::ParamDataType( nullptr, nullptr, wMarkerElem, w.params ), activeMarkerElem );
}

void Parser::CopyControlData::loadLast( World* world )
{
	const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
	const cg::GeneticElement* wRndElem = name2elemGeneric<cg::GeneticElement>( w.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::CopyControl>( name )->setAll( gateElem, wRndElem );
}


//-------------------------------------------------------------------------------------------------- OriV
void Parser::OriVData::preprocess()
{
	vmax.preprocess( true );
	w.preprocess( true );
	
	if( eclipseMarker != DF_NOELEMENT )
		enclosingParser->createAndPreprocess<QMarkerData>( eclipseMarker );
	if( eventMarker != DF_NOELEMENT )
		enclosingParser->createAndPreprocess<QMarkerData>(eventMarker );

    if( vmax.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( vmax.marker );
	if( w.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( w.marker );
}

void Parser::OriVData::loadFirst( World* world )
{
	const std::vector<const cg::CopyControl*> copyControlElems = names2elems<cg::CopyControl>( copyControls, world->getGeneticCollection(), name );
	const cg::QMarker* eclipseMarkerElem = name2elem<cg::QMarker>( eclipseMarker, world->getGeneticCollection(), name, false );
	const cg::QMarker* eventMarkerElem = name2elem<cg::QMarker>( eventMarker, world->getGeneticCollection(), name, false );
	const cg::QMarker* vmaxMarkerElem = name2elem<cg::QMarker>( vmax.marker, world->getGeneticCollection(), name, false );
	const cg::QMarker* wMarkerElem = name2elem<cg::QMarker>( w.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::OriV>( name, nullptr, copyControlElems, nullptr
   	, typename ClassType::ParamDataType( nullptr, nullptr, vmaxMarkerElem, vmax.params )
    , typename ClassType::ParamDataType( nullptr, nullptr, wMarkerElem, w.params )
    , eclipseMarkerElem, eventMarkerElem, bEclipse, ut::castVector<size_t>( mutationKeys ) );
}

void Parser::OriVData::loadLast( World* world )
{
    const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* customFunctionElem = name2elemGeneric<cg::GeneticElement>( customFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* vmaxRndElem = name2elemGeneric<cg::GeneticElement>( vmax.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* wRndElem = name2elemGeneric<cg::GeneticElement>( w.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::OriV>( name )->setAll( gateElem, customFunctionElem, vmaxRndElem, wRndElem );
}


//-------------------------------------------------------------------------------------------------- PartitionSystem
void Parser::PartitionSystemData::preprocess()
{ 
	fraction.preprocess( true ); 
	p.preprocess( true ); 

	enclosingParser->createAndPreprocess<QMarkerData>( "_" + name + ECLIPSE_SUFIX );

	if( fraction.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( fraction.marker );
    if( p.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( p.marker );
}

void Parser::PartitionSystemData::loadFirst( World* world )
{
    const cg::QMarker* eclipseMarkerElem = name2elem<cg::QMarker>( "_" + name + ECLIPSE_SUFIX, world->getGeneticCollection(), name );
    const cg::QMarker* fractionMarkerElem = name2elem<cg::QMarker>( fraction.marker, world->getGeneticCollection(), name, false );
	const cg::QMarker* pMarkerElem = name2elem<cg::QMarker>( p.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::PartitionSystem>( name, nullptr, eclipseMarkerElem
    , typename ClassType::ParamDataType( nullptr, nullptr, fractionMarkerElem, fraction.params )
    , typename ClassType::ParamDataType( nullptr, nullptr, pMarkerElem, p.params )
    , bVolScaling );
}

void Parser::PartitionSystemData::loadLast( World* world )
{
    const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* fractionFunElem = name2elemGeneric<cg::GeneticElement>( fractionFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* pFunElem = name2elemGeneric<cg::GeneticElement>( pFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* fractionRndElem = name2elemGeneric<cg::GeneticElement>( fraction.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* pRndElem = name2elemGeneric<cg::GeneticElement>( p.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::PartitionSystem>( name )->setAll( gateElem, fractionFunElem, pFunElem, fractionRndElem, pRndElem );
}



//=============================================================================================================================================================== MUTATION

//-------------------------------------------------------------------------------------------------- Mutation
void Parser::MutationData::preprocess()
{
    while( amounts.size() < plasmids.size() )
        amounts.push_back( 1 );

    if( eventMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>(eventMarker );
}

void Parser::MutationData::loadFirst( World* world )
{
    std::vector<const cg::PlasmidBase*> plasmidElems = names2elems<cg::PlasmidBase>( plasmids, world->getGeneticCollection(), name );
    const cg::QMarker* eventMarkerElem = name2elem<cg::QMarker>( eventMarker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::Mutation>( name, plasmidElems, amounts, eventMarkerElem );
}


//-------------------------------------------------------------------------------------------------- MutationProcess
void Parser::MutationProcessData::preprocess()
{
	rate.preprocess( true );
    while( mutationProbs.size() < mutations.size() )
        mutationProbs.push_back( 1.0 );
    mutationProbs = ut::normalizeVector( mutationProbs );

    if( eventMarker != DF_NOELEMENT )
		enclosingParser->createAndPreprocess<QMarkerData>(eventMarker );

	if( rate.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( rate.marker );
}

void Parser::MutationProcessData::loadFirst( World* world )
{
    std::vector<const cg::Mutation*> mutationsElems = names2elems<cg::Mutation>( mutations, world->getGeneticCollection(), name );
    std::vector<TReal> mutationProbsVals = ut::castVector<TReal>( mutationProbs ); //required if TReal is not double
    const cg::QMarker* eventMarkerElem = name2elem<cg::QMarker>( eventMarker, world->getGeneticCollection(), name, false );
    const cg::QMarker* rateMarkerElem = name2elem<cg::QMarker>( rate.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::MutationProcess>( name, nullptr, mutationsElems
	, typename ClassType::ParamDataType( nullptr, nullptr, rateMarkerElem, rate.params )
    , eventMarkerElem, bEclipse, mutationProbsVals );
}

void Parser::MutationProcessData::loadLast( World* world )
{
    const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    const cg::GeneticElement* rateFunctionElem = name2elemGeneric<cg::GeneticElement>( rateFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* rateRndElem = name2elemGeneric<cg::GeneticElement>( rate.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::MutationProcess>( name )->setAll( gateElem, rateFunctionElem, rateRndElem );
}




//=============================================================================================================================================================== CONJUGATION

//-------------------------------------------------------------------------------------------------- Pilus
void Parser::PilusData::preprocess()
{
    if( recipientFunction != BaseData::DF_NOELEMENT )
        neighbourMarker = enclosingParser->createFrom<QMarkerData>( this, "recipient" );

    maxRate.preprocess( true );
    neighboursW.preprocess( true );

    if( maxRate.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( maxRate.marker );
    if( neighboursW.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( neighboursW.marker );
    if( rateMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( rateMarker );
}

void Parser::PilusData::loadFirst( World* world )
{
    const cg::QMarker* neighbourMarkerElem = name2elem<cg::QMarker>( neighbourMarker, world->getGeneticCollection(), name, false );
    const cg::QMarker* rateMarkerElem = name2elem<cg::QMarker>( rateMarker, world->getGeneticCollection(), name, false );
    const cg::QMarker* maxRateMarkerElem = name2elem<cg::QMarker>( maxRate.marker, world->getGeneticCollection(), name, false );
    const cg::QMarker* neighboursWMarkerElem = name2elem<cg::QMarker>( neighboursW.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::Pilus>( name, nullptr, nullptr, nullptr, neighbourMarkerElem
    , typename ClassType::ParamDataType( nullptr, nullptr, maxRateMarkerElem, maxRate.params )
    , typename ClassType::ParamDataType( nullptr, nullptr, neighboursWMarkerElem, neighboursW.params )
    , rateMarkerElem, bAutoVolScale );
}

void Parser::PilusData::loadLast( World* world )
{
    const cg::GeneticElement* donorGateElem = name2elemGeneric<cg::GeneticElement>( donorGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* recipientGateElem = name2elemGeneric<cg::GeneticElement>( recipientGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* donorFunctionElem = name2elemGeneric<cg::GeneticElement>( donorFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* recipientFunctionElem = name2elemGeneric<cg::GeneticElement>( recipientFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* maxRateRndElem = name2elemGeneric<cg::GeneticElement>( maxRate.rnd, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* neighboursWRndElem = name2elemGeneric<cg::GeneticElement>( neighboursW.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::Pilus>( name )->setAll( donorGateElem, recipientGateElem, donorFunctionElem, recipientFunctionElem, maxRateRndElem, neighboursWRndElem );
}


//-------------------------------------------------------------------------------------------------- OriT
void Parser::OriTData::preprocess()
{
  //markers
    if( donorEventMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<EventMarkerData>( donorEventMarker );
    if( recipientEventMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<EventMarkerData>( recipientEventMarker );
    
    rateW.preprocess( true );

    if( rateW.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( rateW.marker );
}

void Parser::OriTData::loadFirst( World* world )
{
    const cg::Pilus* pilusElem = name2elem<cg::Pilus>( pilus, world->getGeneticCollection(), name );
    const cg::EventMarker* donorEventMarkerElem = name2elem<cg::EventMarker>( donorEventMarker, world->getGeneticCollection(), name, false );
    const cg::EventMarker* recipientEventMarkerElem = name2elem<cg::EventMarker>( recipientEventMarker, world->getGeneticCollection(), name, false );
    const cg::QMarker* rateWMarkerElem = name2elem<cg::QMarker>( rateW.marker, world->getGeneticCollection(), name, false );
    cg::OriT::Markers markerElems( donorEventMarkerElem, recipientEventMarkerElem );

    world->getGeneticCollectionEdit()->create<cg::OriT>( name, pilusElem, nullptr, nullptr
    , typename ClassType::ParamDataType( nullptr, nullptr, rateWMarkerElem, rateW.params )
    , markerElems, bCopyDependent, bEclipse, ut::castVector<size_t>( mutationKeys ) );
}

void Parser::OriTData::loadLast( World* world )
{
    const cg::GeneticElement* donorGateElem = name2elemGeneric<cg::GeneticElement>( donorGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* recipientGateElem = name2elemGeneric<cg::GeneticElement>( recipientGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* rateWFunElem = name2elemGeneric<cg::GeneticElement>( rateWFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* rateWRndElem = name2elemGeneric<cg::GeneticElement>( rateW.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::OriT>( name )->setAll( donorGateElem, recipientGateElem, rateWFunElem, rateWRndElem );
}




//=============================================================================================================================================================== HOLDERS

//-------------------------------------------------------------------------------------------------- PlasmidBase
void Parser::PlasmidData::preprocess()
{
	while( repliMutationKeys.size() < repliMutations.size() )
	    repliMutationKeys.push_back( 1 );
}

void Parser::PlasmidData::loadLast( World* world )
{
    const std::vector< const cg::MutationProcess*> repliMutElems = names2elems<cg::MutationProcess>( repliMutations, world->getGeneticCollection(), name );
    world->getGeneticCollectionEdit()->getByNameEdit<cg::PlasmidBase>( name )->setRepliMutations( repliMutElems );
}


//-------------------------------------------------------------------------------------------------- BPlasmid
void Parser::PlasmidBooleanData::preprocess()
{
	PlasmidData::preprocess();
	loss.preprocess( true );

	if( loss.marker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( loss.marker );
}

void Parser::PlasmidBooleanData::loadFirst( World* world )
{
    std::vector<const cg::Operon*> operonElems = names2elems<cg::Operon>( operons, world->getGeneticCollection(), name );
    std::vector<const cg::OriT*> oriTElems = names2elems<cg::OriT>( oriTs, world->getGeneticCollection(), name );
    const cg::QMarker* lossMarkerElem = name2elem<cg::QMarker>( loss.marker, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->create<cg::PlasmidBoolean>( name, operonElems, oriTElems, std::vector<const cg::MutationProcess*>({}), ut::castVector<size_t>( repliMutationKeys )
    , typename ClassType::ParamDataType( nullptr, nullptr, lossMarkerElem, loss.params ) );
}

void Parser::PlasmidBooleanData::loadLast( World* world )
{
    PlasmidData::loadLast( world );
    const cg::GeneticElement* lossFunctionElem = name2elemGeneric<cg::GeneticElement>( lossFunction, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* lossRndElem = name2elemGeneric<cg::GeneticElement>( loss.rnd, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::PlasmidBoolean>( name )->setAll( lossFunctionElem, lossRndElem );
}

//-------------------------------------------------------------------------------------------------- QPlasmid
void Parser::PlasmidQuantitativeData::preprocess()
{
	PlasmidData::preprocess();
	if( eclipseMarker != DF_NOELEMENT )
        enclosingParser->createAndPreprocess<QMarkerData>( eclipseMarker );

    copyNum.preprocess( enclosingParser ); 
    eclipse.preprocess( enclosingParser ); 
}

void Parser::PlasmidQuantitativeData::loadFirst( World* world )
{
    const std::vector<const cg::Operon*>& operonElems = names2elems<cg::Operon>( operons, world->getGeneticCollection(), name );
    const std::vector<const cg::OriT*>& oriTElems = names2elems<cg::OriT>( oriTs, world->getGeneticCollection(), name );
    const std::vector<const cg::OriV*>& oriVElems = names2elems<cg::OriV>( oriVs, world->getGeneticCollection(), name );
    const std::vector<const cg::CopyControl*>& copyControlElems = names2elems<cg::CopyControl>( copyControls, world->getGeneticCollection(), name );
    const cg::PartitionSystem* partitionSystemElem = name2elem<cg::PartitionSystem>( partitionSystem, world->getGeneticCollection(), name );

    const cg::QMarker* eclipseMarkerElem = name2elem<cg::QMarker>( eclipseMarker, world->getGeneticCollection(), name, false );

    ut::ContinuousDist::DistributionType copyNumDistTypeVal = name2val<ut::ContinuousDistFactory>( copyNum.distType, name );
    ut::ContinuousDist::DistributionType eclipseDistTypeVal = name2val<ut::ContinuousDistFactory>( eclipse.distType, name );

    world->getGeneticCollectionEdit()->create<cg::PlasmidQuantitative>( name, operonElems, oriTElems, oriVElems, copyControlElems, partitionSystemElem, std::vector<const cg::MutationProcess*>({}), nullptr, eclipseMarkerElem
    , ut::ContinuousDistData( copyNumDistTypeVal, copyNum.params ), ut::ContinuousDistData( eclipseDistTypeVal, eclipse.params ), ut::castVector<size_t>( repliMutationKeys ) );
}

void Parser::PlasmidQuantitativeData::loadLast( World* world )
{
	PlasmidData::loadLast( world );
	const cg::GeneticElement* eclipseFunctionElem = name2elemGeneric<cg::GeneticElement>( eclipseFunction, world->getGeneticCollection(), name, false );

    world->getGeneticCollectionEdit()->getByNameEdit<cg::PlasmidQuantitative>( name )->setAll( eclipseFunctionElem );
}



//-------------------------------------------------------------------------------------------------- Strain
void Parser::StrainData::preprocess()
{
	growthRate.preprocess( true ); 

    iniVol.preprocess( enclosingParser );
    divisionVol.preprocess( enclosingParser );
    divisionFraction.preprocess( enclosingParser );
    divisionRotation.preprocess( enclosingParser );
}

void Parser::StrainData::loadFirst( World* world )
{
  //distributions
    cg::Strain::DistributionsDataDS distsData = {{
        ut::ContinuousDistData( name2val<ut::ContinuousDistFactory>( iniVol.distType, name ), iniVol.params )
        , ut::ContinuousDistData( name2val<ut::ContinuousDistFactory>( divisionVol.distType, name ), divisionVol.params )
        , ut::ContinuousDistData( name2val<ut::ContinuousDistFactory>( divisionFraction.distType, name ), divisionFraction.params )
        , ut::ContinuousDistData( name2val<ut::ContinuousDistFactory>( divisionRotation.distType, name ), divisionRotation.params )
    }};

    world->getGeneticCollectionEdit()->create<cg::Strain>( name, nullptr, nullptr, nullptr, nullptr, growthRate.params, distsData, deathThreshold, bNegativeGrowthAllowed, bGrowthCorrection );
}

void Parser::StrainData::loadLast( World* world )
{   
	const cg::GeneticElement* divisionGateElem = name2elemGeneric<cg::GeneticElement>( divisionGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* deathGateElem = name2elemGeneric<cg::GeneticElement>( deathGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* grFuntionElem = name2elemGeneric<cg::GeneticElement>( grCustomFun, world->getGeneticCollection(), name, false );
    const cg::GeneticElement* baseGrowthRateRandomnessElem = name2elemGeneric<cg::GeneticElement>( growthRate.rnd, world->getGeneticCollection(), name, false );
    
    world->getGeneticCollectionEdit()->getByNameEdit<cg::Strain>( name )->setAll( divisionGateElem, deathGateElem, grFuntionElem, baseGrowthRateRandomnessElem );
}

//-------------------------------------------------------------------------------------------------- CellType
void Parser::CellTypeData::preprocess()
{
    while( qPlasmidDists.size() < qPlasmids.size() )
        qPlasmidDists.push_back( DF_NOELEMENT );

    while( qPlasmidAmountVars.size() < qPlasmidAmounts.size() )
        qPlasmidAmountVars.push_back( 0.0 );
}

void Parser::CellTypeData::loadFirst( World* world )
{
    const cg::Strain* strainElem = name2elem<cg::Strain>( strain, world->getGeneticCollection(), name );
    std::vector<const cg::Molecule*> moleculeElems = names2elems<cg::Molecule>( molecules, world->getGeneticCollection(), name );

    std::vector<const cg::PlasmidBase*> bPlasmidElems = names2elems<cg::PlasmidBase>( bPlasmids, world->getGeneticCollection(), name );
    checkSubtypes( bPlasmidElems, &cg::PlasmidBase::getPlasmidType, cg::PlasmidBase::PlasmidType::BPLASMID );
    std::vector<const cg::PlasmidBase*> qPlasmidElems = names2elems<cg::PlasmidBase>( qPlasmids, world->getGeneticCollection(), name );
    checkSubtypes( qPlasmidElems, &cg::PlasmidBase::getPlasmidType, cg::PlasmidBase::PlasmidType::QPLASMID );

    std::vector<ut::ContinuousDistData> dists;
    for( size_t qp = 0; qp < qPlasmidElems.size(); qp++ )
    {
        if( qPlasmidDists[qp] != DF_NOELEMENT ) //custom distribution by name
        {
            DistributionData* distData = const_cast<DistributionData*>( static_cast<const DistributionData*>( enclosingParser->getParsedDataEdit()[ AllElementIdx::DISTRIBUTION ].find( qPlasmidDists[qp] )->second.get() ) );
            dists.emplace_back( name2val<ut::ContinuousDistFactory>( distData->type, name ), ut::castVector<TReal>( distData->params ) );
        }
        else if( qPlasmidAmounts.size() > qp ) //if no custom dist, use the params as normal distribution
            dists.emplace_back( ut::ContinuousDistData::DF_DIST_TYPE, ut::ContinuousDistInterface::ParamsType( { qPlasmidAmounts[qp], qPlasmidAmountVars[qp] } ) ); 
        else //if no params, use the QPlasmid fall-back 
            dists.emplace_back( static_cast<const cg::PlasmidQuantitative*>( qPlasmidElems[qp] )->getCopyNumber() );
    }
    
    world->getGeneticCollectionEdit()->create<cg::CellType>( name, strainElem, bPlasmidElems, moleculeElems, qPlasmidElems, dists, bQPlasmidsAsConc );
}

















//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// MEDIUM SIGNALS ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------- Signal
void Parser::SignalData::preprocess()
{
	enclosingParser->createAndPreprocess<QMarkerData>( CONC_PREFIX + name );
}

void Parser::SignalData::loadFirst( World* world )
{
  //signal
    world->getMediumCollectionEdit()->create<mg::Signal>( name, ut::RgbColour( rgbColour ), colourSaturationConc, kdiff, kdeg );
  //marker
    const cg::QMarker* concMarkerElem = name2elem<cg::QMarker>( CONC_PREFIX + name, world->getGeneticCollection(), name );
  //metabolite
    world->getGeneticCollectionEdit()->create<cg::Metabolite>( name, concMarkerElem, false );
}


//-------------------------------------------------------------------------------------------------- Grid
void Parser::GridData::loadFirst( World* world )
{
	if( enclosingParser->getParsedData( AllElementIdx::GRID ).size() == 1 || name != AUTO_NAME )
    	world->getMediumCollectionEdit()->create<mg::Grid>( name, unitSize, iniGridSize, deltaGridSize );
}











//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// SIMULATION ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//=============================================================================================================================================================== TIMING

//-------------------------------------------------------------------------------------------------- Timer
void Parser::TimerData::preprocess()
{
    if( end < 0.0 ) //negative end times are considered +infinite
        end = std::numeric_limits<TReal>::max();
}

void Parser::TimerData::loadFirst( World* world )
{
    const PopulationGate* gateElem = name2elem<PopulationGate>( gate, world->getGroCollection(), name );
    Timer::Mode modeVal = name2val( mode, ClassType::modeNM, name );
    world->getGroCollectionEdit()->create<Timer>( name, gateElem, world->getTime(), modeVal, start, end, period, bRnd, periodMin, bIgnoreStart );
}

//-------------------------------------------------------------------------------------------------- Checkpoint
void Parser::CheckpointData::loadFirst( World* world )
{
    const Timer* timerElem = name2elem<Timer>( timer, world->getGroCollection(), name );
    world->getGroCollectionEdit()->create<Checkpoint>( name, timerElem, message, bSound );
}





//=============================================================================================================================================================== LOGIC

void Parser::PopQMarkerData::loadFirst( World* world )
{
    world->getGroCollectionEdit()->create<QGroMarker>( name );
}

//-------------------------------------------------------------------------------------------------- PopStat
void Parser::PopulationStatData::loadFirst( World* world )
{
    const cg::GeneticElement* filterGateElem = name2elemGeneric<cg::GeneticElement>( filterGate, world->getGeneticCollection(), name );
    const cg::GeneticElement* fieldElem = name2elemGeneric< cg::GeneticElement >( field, world->getGeneticCollection(), name, false, false );
    
  //unpack stats
    std::vector<std::string> tempStats;
    if( stats == DF_STATS ) //no custom stats = use statPack
        tempStats = ut::StatBase::statTypeNM.subtypes2Names( ut::StatPack::pack2stats( name2val( statsPack, ut::StatPack::typeNM, name ) ) );
    else
        tempStats = stats;

   //others
    ClassType::AutoVolMode autoVolModeVal = name2val( autoVolMode, ClassType::autoVolModeNM, name );

  //create a marker for each stat
    std::vector<const QGroMarker*> markersElems;
    for( const auto& stat : tempStats )
    {
        std::string markerName = name + "_" + stat;
        world->getGroCollectionEdit()->create<QGroMarker>( markerName );
        markersElems.push_back( world->getGroCollectionEdit()->getByName<QGroMarker>( markerName ) );
    }

    if( fieldElem ) //a cg element
    {
        if( stats != DF_STATS ) //custom stats
        {
            std::vector< ut::StatBase::StatType > statsVals = names2vals( stats, ut::StatBase::statTypeNM, name );
            world->getGroCollectionEdit()->create<PopulationStatElem>( name, filterGateElem, fieldElem, statsVals, markersElems, autoVolModeVal, bFree );

        }
        else //from stat pack
        {
            ut::StatPack::Type statsPackVal = name2val( statsPack, ut::StatPack::typeNM, name );
            world->getGroCollectionEdit()->create<PopulationStatElem>( name, filterGateElem, fieldElem, statsPackVal, markersElems, autoVolModeVal, bFree );
        }
    }
    else //a special variable 
    {
        PopulationStatSpecial::Type specialTypeVal = name2val( field, PopulationStatSpecial::typeNM, name );

        if( stats != DF_STATS ) //custom stats
        {
            std::vector< ut::StatBase::StatType > statsVals = names2vals( stats, ut::StatBase::statTypeNM, name );
            world->getGroCollectionEdit()->create<PopulationStatSpecial>( name, filterGateElem, specialTypeVal, statsVals, markersElems, autoVolModeVal, bFree );

        }
        else //from stat pack
        {
            ut::StatPack::Type statsPackVal = name2val( statsPack, ut::StatPack::typeNM, name );
            world->getGroCollectionEdit()->create<PopulationStatSpecial>( name, filterGateElem, specialTypeVal, statsPackVal, markersElems, autoVolModeVal, bFree );
        }
    }
}


//-------------------------------------------------------------------------------------------------- PopFunction
void Parser::PopulationFunctionData::loadFirst( World* world )
{
    ClassType::Type typeVal = name2val<ClassType::MathFunctionType::FactoryType>( type, name );
    world->getGroCollectionEdit()->create<PopulationFunction>( name, std::vector<const GroElement*>( {} ), params, typeVal, min, max, roundPlaces ); //empty inputs because they may be of a type that is loaded after
}

void Parser::PopulationFunctionData::loadLast( World* world )
{
  //set the inputs
    std::vector<const GroElement*> inputElems = names2elemsGeneric<GroElement>( input, world->getGroCollection(), name );
    static_cast<PopulationFunction*>( world->getGroCollectionEdit()->getByNameEdit( name ) )->setInput( inputElems );
}


//-------------------------------------------------------------------------------------------------- PopBGate
void Parser::BPopulationGateData::preprocess()
{
  //process raw names
    inputNames.clear();
    inputPresence.clear();
    for( auto inputName : inputNamesRaw )
    {
        if( inputName.at(0) == '-' )
        {
            inputName.erase( inputName.begin() );
            inputPresence.push_back( false );
        }
        else
            inputPresence.push_back( true );

        inputNames.push_back( inputName );
    }
}

void Parser::BPopulationGateData::loadFirst( World* world )
{
    ut::toUpper( gateFunction );
    world->getGroCollectionEdit()->create<PopulationGateBoolean>( name, std::vector<const GroElement*>( {} ), inputPresence, name2val( gateFunction, ut::GateBoolean::functionNM, name, false, true ) );
}

void Parser::BPopulationGateData::loadLast( World* world )
{
  //set inputs
    std::vector<const GroElement*> inputElems = names2elemsGeneric<GroElement>( inputNames, world->getGroCollection(), name );
    static_cast<PopulationGateBoolean*>( world->getGroCollectionEdit()->getByNameEdit( name ) )->setInput( inputElems );
}


//-------------------------------------------------------------------------------------------------- PopQGate
void Parser::QPopulationGateData::loadFirst( World* world )
{
    world->getGroCollectionEdit()->create<PopulationGateQuantitative>( name, nullptr, value, name2val( compOperator, ut::GateQuantitative::compOperatorNM, name ) ); //nullptr input elem 
}

void Parser::QPopulationGateData::loadLast( World* world )
{
  //set the input elem
    const GroElement* elemElem = name2elemGeneric<GroElement>( elem, world->getGroCollection(), name );
    static_cast<PopulationGateQuantitative*>( world->getGroCollectionEdit()->getByNameEdit( name ) )->setElem( elemElem );
}





//=============================================================================================================================================================== PLACERS

//-------------------------------------------------------------------------------------------------- CellPlacer
void Parser::CellPlacerData::preprocess()
{
    while( cellProbs.size() < cellTypes.size() )
        cellProbs.push_back( DF_AUTOCOMPLETE_PROB );
    cellProbs.resize( cellTypes.size() );
    cellProbs = ut::normalizeVector( cellProbs );

    amount.preprocess( enclosingParser );
}

void Parser::CellPlacerData::loadFirst( World* world )
{
    std::vector<const cg::CellType*> cellTypeElems = names2elems<cg::CellType>( cellTypes, world->getGeneticCollection(), name );
    Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );
    ut::Circle coords = ut::Circle( coordsData.x, coordsData.y, coordsData.r );
    
    ut::ContinuousDist::DistributionType amountDistTypeVal = name2val<ut::ContinuousDistFactory>( amount.distType, name );

    std::transform( coordsData.mode.begin(), coordsData.mode.end(), coordsData.mode.begin(), ::tolower );
    world->getGroCollectionEdit()->create<CellPlacer>( name, cellTypeElems, timerElem, coords, coordsData.mode == "polar" ? CoordsSelectorBase::CoordsMode::POLAR : CoordsSelectorBase::CoordsMode::CARTESIAN, bMixed
    , ut::castVector<TReal>( cellProbs ), ut::ContinuousDistData( amountDistTypeVal, amount.params ) );
}


//-------------------------------------------------------------------------------------------------- CellPlating
void Parser::CellPlatingData::loadFirst( World* world )
{
    const cg::GeneticElement* gateElem = name2elemGeneric<cg::GeneticElement>( gate, world->getGeneticCollection(), name );
    Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );

    ut::Circle oldCoords = ut::Circle( oldCoordsData.x, oldCoordsData.y, ut::isPositive( oldCoordsData.r ) ? oldCoordsData.r : std::numeric_limits<double>::max() );
    ut::Circle coords = ut::Circle( coordsData.x, coordsData.y, coordsData.r );

    ut::ContinuousDist::DistributionType takeDistTypeVal = name2val<ut::ContinuousDistFactory>( takeFraction.distType, name );
    ut::ContinuousDist::DistributionType putDistTypeVal = name2val<ut::ContinuousDistFactory>( putFraction.distType, name );

    std::transform( coordsData.mode.begin(), coordsData.mode.end(), coordsData.mode.begin(), ::tolower );
    world->getGroCollectionEdit()->create<CellPlating>( name, gateElem, timerElem, coords, coordsData.mode == "polar" ? CoordsSelectorBase::CoordsMode::POLAR : CoordsSelectorBase::CoordsMode::CARTESIAN, oldCoords
    , ut::ContinuousDistData( takeDistTypeVal, takeFraction.params ), ut::ContinuousDistData( putDistTypeVal, putFraction.params ) );
}


//-------------------------------------------------------------------------------------------------- SignalPlacer
void Parser::SignalPlacerData::preprocess()
{
    while( signalProbs.size() < msignals.size() )
        signalProbs.push_back( DF_AUTOCOMPLETE_PROB );
    signalProbs.resize( msignals.size() );
    signalProbs = ut::normalizeVector( signalProbs );

    amount.preprocess( enclosingParser );
    sourceNum.preprocess( enclosingParser );
    size.preprocess( enclosingParser );
}

void Parser::SignalPlacerData::loadFirst( World* world )
{
    std::vector<const mg::Signal*> msignalElems = names2elems<mg::Signal>( msignals, world->getMediumCollection(), name );
    Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );
    
    ut::Circle coords = ut::Circle( coordsData.x, coordsData.y, coordsData.r );

    ut::ContinuousDist::DistributionType amountDistTypeVal = name2val<ut::ContinuousDistFactory>( amount.distType, name );
    ut::ContinuousDist::DistributionType sourceNumDistTypeVal = name2val<ut::ContinuousDistFactory>( sourceNum.distType, name );
    ut::ContinuousDist::DistributionType sizeDistTypeVal = name2val<ut::ContinuousDistFactory>( size.distType, name );

    std::transform( coordsData.mode.begin(), coordsData.mode.end(), coordsData.mode.begin(), ::tolower );
    //std::transform( mode.begin(), mode.end(), mode.begin(), ::tolower );
    SignalPlacer::Mode modeVal = name2val( mode, ClassType::modeNM, name );

    ut::Rectangle constArea = ut::Rectangle( constAreaData.x1, constAreaData.x2, constAreaData.y1, constAreaData.y2 );

    world->getGroCollectionEdit()->create<SignalPlacer>( name, msignalElems, timerElem, coords, coordsData.mode == "polar" ? CoordsSelectorBase::CoordsMode::POLAR : CoordsSelectorBase::CoordsMode::CARTESIAN, modeVal, bMixed
    , ut::castVector<TReal>( signalProbs ), ut::ContinuousDistData( amountDistTypeVal, amount.params ), ut::ContinuousDistData( sourceNumDistTypeVal, sourceNum.params ), ut::ContinuousDistData( sizeDistTypeVal, size.params ), constArea );
}





//=============================================================================================================================================================== PLACERS
//-------------------------------------------------------------------------------------------------- Snapshot
void Parser::SnapshotData::loadFirst( World* world )
{
    Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );
    world->getGroCollectionEdit()->create<Snapshot>( name, timerElem, path, fileName, imgFormat );
}


//-------------------------------------------------------------------------------------------------- CellsFile

void Parser::CellsFileData::preprocess() 
{ 
    path = basePath + "/" + folderName;

    std::transform( includeElemsMode.begin(), includeElemsMode.end(), includeElemsMode.begin(), ::tolower );
    
    if( includeElemsMode == "all" ) //all the types of elements in cg
    { 
        addElemTypes( ALL_ELEMS );
    }
    else
    {
        if( includeElemsMode == "classic" ) //cell type, molecules and plasmids
        { 
            addElemTypes( CLASSIC_ELEMS );
        }
        for( const auto& elem : fields ) //custom cg elements 
            addElem( elem );
    }
}

void Parser::CellsFileData::loadFirst( World* world )
{
    Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );
    const cg::GeneticElement* individualGateElem = name2elemGeneric<cg::GeneticElement>( filterGate, world->getGeneticCollection(), name );
    
    CellsFile::Mode modeVal = name2val( mode, ClassType::modeNM, name );

    for( const auto& popField : populationFields )
    {
        const GroElement* groElem = name2elemGeneric<GroElement>( popField, world->getGroCollection(), name );
        if( groElem->getElemType() == GroElementIdx::POP_STAT )
            statFields.push_back( popField );
        else
            otherFields.push_back( popField );
    }

    std::vector<const PopulationStat*> statFieldsElems = names2elems<PopulationStat>( statFields, world->getGroCollection(), name );
    std::vector<const GroElement*> otherFieldsElems = names2elemsGeneric<GroElement>( otherFields, world->getGroCollection(), name );
    
    world->getGroCollectionEdit()->create<CellsFile>( name, timerElem, individualGateElem, statFieldsElems, otherFieldsElems, modeVal, bIncludeAncestry, path, fileName, format, precision );
}

void Parser::CellsFileData::addElem( const std::string& elemName )
{
    std::string popStatName = "_" + PopulationStat::metadata.key + "_" + name + "_" + elemName;

    if( stats.size() > 0 ) //custom stats
        enclosingParser->createAndPreprocess<PopulationStatData>( popStatName, elemName, filterGate, stats, autoVolMode, false );
    else //from stat pack
        enclosingParser->createAndPreprocess<PopulationStatData>( popStatName, elemName, filterGate, statsPack, autoVolMode, false );

    statFields.push_back( popStatName );
}

void Parser::CellsFileData::addElemType( AllElementIdx elemTypeIdx )
{
    for( auto& it : enclosingParser->getParsedData()[ elemTypeIdx ] )
        addElem( it.first );
}


//-------------------------------------------------------------------------------------------------- CellsPlot
void Parser::CellsPlotData::preprocess() 
{ 
    path = basePath + "/" + folderName;

    for( const auto& elem : fields )
    {
        std::string popStatName = "_" + PopulationStat::metadata.key + "_" + name + "_" + elem;

        if( stats.size() > 0 )
            enclosingParser->createAndPreprocess<PopulationStatData>( popStatName, elem, filterGate, stats, autoVolMode, false );
        else
            enclosingParser->createAndPreprocess<PopulationStatData>( popStatName, elem, filterGate, statsPack, autoVolMode, false );

        statFields.push_back( popStatName );
    }

    for( size_t c = 0; c < CellsPlot::PRESET_HEX_COLOURS.size(); c++ )
        hexColours.push_back( CellsPlot::PRESET_HEX_COLOURS[c] );
}

void Parser::CellsPlotData::loadFirst( World* world )
{
    const Timer* timerElem = const_cast<Timer*>( name2elem<Timer>( timer, world->getGroCollection(), name ) );

    for( const auto& popField : populationFields )
    {
        const GroElement* groElem = name2elemGeneric<GroElement>( popField, world->getGroCollection(), name );
        if( groElem->getElemType() == GroElementIdx::POP_STAT )
            statFields.push_back( popField );
        else
            otherFields.push_back( popField );
    }

    std::vector<const PopulationStat*> statFieldsElems = names2elems<PopulationStat>( statFields, world->getGroCollection(), name );
    std::vector<const GroElement*> otherFieldsElems = names2elemsGeneric<GroElement>( otherFields, world->getGroCollection(), name );

    world->getGroCollectionEdit()->create<CellsPlot>( name, timerElem, statFieldsElems, otherFieldsElems, hexColours, path, fileName, format, historicSize, precision );
}




























//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////// *PARSER* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Parser::createAutoFirst()
{
//---CG
  //markers
    createAndPreprocess<QMarkerData>( QMarkerData::AUTO_VOL_NAME );
    createAndPreprocess<QMarkerData>( QMarkerData::AUTO_DVOL_NAME );
    createAndPreprocess<QMarkerData>( QMarkerData::AUTO_GR_NAME );
    createAndPreprocess<BMarkerData>( BMarkerData::AUTO_B_DIVISION_VOL_NAME );
    createAndPreprocess<QMarkerData>( QMarkerData::AUTO_NEIGH_DISTANCE_NAME );
    createAndPreprocess<EventMarkerData>( EventMarkerData::AUTO_DIVISION_NAME );
    createAndPreprocess<QMarkerData>( QMarkerData::AUTO_POLE_COUNT_NAME );
    createAndPreprocess<BMarkerData>( BMarkerData::AUTO_POLE_POS_NAME );
  //gates
    createAndPreprocess<BGateData>( BGateData::AUTO_TRUE_NAME, "OPEN", std::vector<std::string>( {} ) );
    createAndPreprocess<BGateData>( BGateData::AUTO_FALSE_NAME, "CLOSED", std::vector<std::string>( {} ) );
  //bio misc
    createAndPreprocess<PartitionSystemData>( PartitionSystemData::AUTO_RND_NAME, BGateData::AUTO_FALSE_NAME );
    createAndPreprocess<MetaboliteData>( MetaboliteData::AUTO_BIOMASS_NAME, true );
  //strains
    createAndPreprocess<StrainData>( StrainData::AUTO_WT_NAME, BaseData::DF_NOELEMENT, BaseData::DF_NOELEMENT, StrainData::AUTO_WT_GR );
    createAndPreprocess<StrainData>( StrainData::AUTO_SLOW_NAME, BaseData::DF_NOELEMENT, BaseData::DF_NOELEMENT, StrainData::AUTO_SLOW_GR );
    createAndPreprocess<StrainData>( StrainData::AUTO_NOGROWTH_NAME, BaseData::DF_NOELEMENT, BaseData::DF_NOELEMENT, std::vector<double>( { 0.0, 0.0 } ) );

//---MEDIUM
    createAndPreprocess<GridData>( GridData::AUTO_NAME );

//---SIM
  //pop
    createAndPreprocess<PopQMarkerData>( PopQMarkerData::AUTO_CELLNUM_NAME );
    //createAndPreprocess<PopulationStatData>( PopulationStatData::AUTO_CELLNUM_NAME, BGateData::AUTO_TRUE_NAME, BGateData::AUTO_TRUE_NAME, std::vector<std::string>( {"sum"} ), "none", true );
    createAndPreprocess<BPopulationGateData>( BPopulationGateData::AUTO_TRUE_NAME, "OPEN" );
    createAndPreprocess<BPopulationGateData>( BPopulationGateData::AUTO_FALSE_NAME, "CLOSED" );
  //timers
    createAndPreprocess<TimerData>( TimerData::AUTO_START_NAME, BPopulationGateData::AUTO_TRUE_NAME, "always", 0.0, 0.0, 1.0, false );
    createAndPreprocess<TimerData>( TimerData::AUTO_EVERY_STEP_NAME, BPopulationGateData::AUTO_TRUE_NAME, "always", 0.0, std::numeric_limits<TReal>::max(), 0.1, false );
    createAndPreprocess<TimerData>( TimerData::AUTO_EVERY_MIN_NAME, BPopulationGateData::AUTO_TRUE_NAME, "always", 0.0, std::numeric_limits<TReal>::max(), 1.0, false );
    createAndPreprocess<TimerData>( TimerData::AUTO_NEVER_NAME, BPopulationGateData::AUTO_TRUE_NAME, "always", 1.0, 0.0, 1.0, false );
  //population
}



void Parser::loadAll( World* world, uint seed )
{
//---load global params
    const GlobalParamsData* globalParams = static_cast<GlobalParamsData*>( parsedData[GLOBAL_PARAMS].cbegin()->second.get() );
    world->precompute( globalParams->stepSize, seed > 0 ? seed + globalParams->seedOffset : globalParams->seed );
  //theme
    const GlobalParamsData::ThemeData& themeData = globalParams->theme;
    Theme theme( themeData.backgroundCol, themeData.cellOutlineCol, themeData.cellOutlineSelectedCol, themeData.messageCol, themeData.mouseCol );
    world->getCallingThread()->getGraphicsHandler().setTheme( theme );
  //boptions
    world->getCallingThread()->setBAutoZoom( globalParams->bAutoZoom );
    world->getCallingThread()->setBShowPlotsOnStartup( globalParams->bShowPlots );
    world->getCallingThread()->getGraphicsHandler().setBRenderMediumGrid( globalParams->bRenderSignals );
    world->getCallingThread()->setBBatchMode( globalParams->bBatchMode );
    world->getCallingThread()->setBShowPerformance( globalParams->bShowPerformance );
  //misc options
    world->getGeneticCollectionEdit()->setSensitivity( globalParams->sensitivity );
    world->setCellsPerThread( globalParams->cellsPerThread );
    world->setGridRowsPerThread( globalParams->gridRowsPerThread );
    world->setMaxThreads( globalParams->maxThreads );
  //common
    world->getGeneticCollectionEdit()->create<cg::HorizontalCommon>( "horizontal_common" );
    
  //load elements
    for( auto& dataMap : parsedData )
    {
        for( auto& it : dataMap )
            it.second->loadFirst( world );
        
        for( auto& it : dataMap )
            it.second->loadSecond( world );
    }
    for( auto& dataMap : parsedData )
    {
        for( auto& it : dataMap )
            it.second->loadLast( world );
    }

  //set global cg markers
    world->getGeneticCollectionEdit()->setVolMarker( world->getGeneticCollection()->getByName<cg::QMarker>( QMarkerData::AUTO_VOL_NAME ) );
    world->getGeneticCollectionEdit()->setDvolMarker( world->getGeneticCollection()->getByName<cg::QMarker>( QMarkerData::AUTO_DVOL_NAME ) );
    world->getGeneticCollectionEdit()->setGrMarker( world->getGeneticCollection()->getByName<cg::QMarker>( QMarkerData::AUTO_GR_NAME ) );
    world->getGeneticCollectionEdit()->setDivisionVolBMarker( world->getGeneticCollection()->getByName<cg::BMarker>( BMarkerData::AUTO_B_DIVISION_VOL_NAME ) );
    world->getGeneticCollectionEdit()->setNeighDistanceMarker( world->getGeneticCollection()->getByName<cg::QMarker>( QMarkerData::AUTO_NEIGH_DISTANCE_NAME ) );
    world->getGeneticCollectionEdit()->setDivisionEventMarker( world->getGeneticCollection()->getByName<cg::EventMarker>( EventMarkerData::AUTO_DIVISION_NAME ) );
    world->getGeneticCollectionEdit()->setPoleCountMarker( world->getGeneticCollection()->getByName<cg::QMarker>( QMarkerData::AUTO_POLE_COUNT_NAME ) );
    world->getGeneticCollectionEdit()->setPolePositionMarker( world->getGeneticCollection()->getByName<cg::BMarker>( BMarkerData::AUTO_POLE_POS_NAME ) );

  //set global gro markers
	world->getGroCollectionEdit()->setCellNumMarker( world->getGroCollection()->getByName<QGroMarker>( PopQMarkerData::AUTO_CELLNUM_NAME ) );

  //init
    world->initCollections();
    world->createDish();
}
