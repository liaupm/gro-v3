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

#include "Gro.h"


void registerGroFunctions() 
{
  //common
    register_ccl_function( "distribution", newElement<Parser::DistributionData> );
  
  //cell logic
    register_ccl_function( "randomness", newElement<Parser::RandomnessData> );
    register_ccl_function( "math_function", newElement<Parser::FunctionData> );
    register_ccl_function( "bgate", newElement<Parser::BGateData> );
    register_ccl_function( "qgate", newElement<Parser::QGateData> );
    register_ccl_function( "historic", newElement<Parser::HistoricData> );
    register_ccl_function( "ode", newElement<Parser::OdeData> );
    register_ccl_function( "cell_colour", newElement<Parser::CellColourData> );

    register_ccl_function ( "molecule", newElement<Parser::MoleculeData> );
    register_ccl_function ( "regulation", newElement<Parser::RegulationData> );
    register_ccl_function ( "operon", newElement<Parser::OperonData> );
    register_ccl_function ( "flux", newElement<Parser::FluxData> );

    register_ccl_function ( "pilus", newElement<Parser::PilusData> );
    register_ccl_function ( "orit", newElement<Parser::OriTData> );
    
    register_ccl_function ( "copy_control", newElement<Parser::CopyControlData> );
    register_ccl_function ( "oriv", newElement<Parser::OriVData> );
    register_ccl_function ( "partition", newElement<Parser::PartitionSystemData> );
    register_ccl_function ( "mutation", newElement<Parser::MutationData> );
    register_ccl_function ( "mutation_process", newElement<Parser::MutationProcessData> );

    register_ccl_function ( "bplasmid", newElement<Parser::PlasmidBooleanData> );
    register_ccl_function ( "qplasmid", newElement<Parser::PlasmidQuantitativeData> );
    register_ccl_function ( "strain", newElement<Parser::StrainData> );
    register_ccl_function ( "cell_type", newElement<Parser::CellTypeData> );

//signals
    register_ccl_function ( "signal", newElement<Parser::SignalData> );
    register_ccl_function ( "grid", newElement<Parser::GridData> );

//sim
    register_ccl_function ( "global_params", newElement<Parser::GlobalParamsData> );
    register_ccl_function ( "timer", newElement<Parser::TimerData> );
    register_ccl_function ( "checkpoint", newElement<Parser::CheckpointData> );
    register_ccl_function ( "population_stat", newElement<Parser::PopulationStatData> );
    register_ccl_function ( "population_function", newElement<Parser::PopulationFunctionData> );
    register_ccl_function ( "population_bgate", newElement<Parser::BPopulationGateData> );
    register_ccl_function ( "population_qgate", newElement<Parser::QPopulationGateData> );

    register_ccl_function ( "cell_placer", newElement<Parser::CellPlacerData> );
    register_ccl_function ( "cell_plating", newElement<Parser::CellPlatingData> );
    register_ccl_function ( "signal_placer", newElement<Parser::SignalPlacerData> );

    register_ccl_function ( "snapshot", newElement<Parser::SnapshotData> );
    register_ccl_function ( "cells_file", newElement<Parser::CellsFileData> );
    register_ccl_function ( "cells_plot", newElement<Parser::CellsPlotData> );
}
