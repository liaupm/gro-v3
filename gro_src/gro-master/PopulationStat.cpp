#include "PopulationStat.h"
#include "GroCollection.h"


////////////////////////////////////////// VIRTUAL BASE POPULATION STAT ///////////////////////////////////////////////////////////////////////////////////////

PopulationStat::PopulationStat( GroCollection* groCollection, size_t id, const std::string& name
, const cg::GeneticElement* filterGate, const std::string& header, ut::StatPack::Type statsType, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode, bool bFree )
: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::POP_STAT ) 
, filterGate( filterGate ), markers( markers ), header( header ), statsType( statsType ), statTypes( ut::StatPack::STATS_BY_TYPE[ static_cast<size_t>( statsType ) ] ), autoVolMode( autoVolMode ), bFree( bFree )
, bUsedForTimers( DF_B_USED_FOR_TIMERS ), bUsedForPlots( DF_B_USED_FOR_PLOTS ) {;}


PopulationStat::PopulationStat( GroCollection* groCollection, size_t id, const std::string& name
, const cg::GeneticElement* filterGate, const std::string& header, const std::vector< ut::StatBase::StatType >& statTypes, const std::vector<const QGroMarker*>& markers, AutoVolMode autoVolMode, bool bFree )
: GroElement::GroElement( groCollection, id, name, ElementTypeIdx::POP_STAT ) 
, filterGate( filterGate ), markers( markers ), header( header ), statsType( ut::StatPack::Type::CUSTOM ), statTypes( statTypes ), autoVolMode( autoVolMode ), bFree( bFree )
, bUsedForTimers( DF_B_USED_FOR_TIMERS ), bUsedForPlots( DF_B_USED_FOR_PLOTS ) {;}



////////////////////////////////////////// POPULATION STAT ELEM ///////////////////////////////////////////////////////////////////////////////////////

TReal PopulationStatElem::getField( const Cell* cell ) const 
{ 
	TReal raw = cell->getGenome().checkQState( elem ); 
	if( autoVolMode == AutoVolMode::DIVISION )
		return raw / cell->getGenome().getVolume();
	else if( autoVolMode == AutoVolMode::PRODUCT )
		return raw * cell->getGenome().getVolume();
	else
		return raw;
}



////////////////////////////////////////// POPULATION STAT SPECIAL ///////////////////////////////////////////////////////////////////////////////////////

ut::NameMap< PopulationStatSpecial::Type > PopulationStatSpecial::typeNM ( { 
                                        { "_x", PopulationStatSpecial::Type::POS_X }
										, { "_y", PopulationStatSpecial::Type::POS_Y }
										, { "_gr", PopulationStatSpecial::Type::GR }
										, { "_gt", PopulationStatSpecial::Type::GT }
                                        , { "_length", PopulationStatSpecial::Type::LENGTH }
                                        , { "_d_length", PopulationStatSpecial::Type::D_LENGTH }
                                        , { "_volume", PopulationStatSpecial::Type::VOLUME }
                                        , { "_d_volume", PopulationStatSpecial::Type::D_VOLUME }
                                        } );
