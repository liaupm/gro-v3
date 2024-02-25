#include "cg/GeneticCollection.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()

/*PRECOMPILED
#include <memory> */

using namespace cg;


GeneticCollection::GeneticCollection( SP<ut::SeedGeneratorBase> seedGen )
: seedGenerator( seedGen ), biomass( nullptr ), sensitivity( DF_SENSITIVITY ), bGillespie( false )
, volMarker( nullptr ), dvolMarker( nullptr ), grMarker( nullptr ), divisionVolBMarker( nullptr ), neighDistanceMarker( nullptr ), divisionEventMarker( nullptr )
, gillespieTimeDist( {}, seedGenerator->nextSeed() ), gillespieSelectDist( {}, {}, seedGenerator->nextSeed() ), iniPolePosDist( { 0, 1 }, seedGenerator->nextSeed() ) {;}


void GeneticCollection::calculateBGillespie()
{
	if( getElementNum<OriV>() > 0 )
	{
		bGillespie = true;
		return;
	}
	if( getElementNum<MutationProcess>() > 0 )
	{
		for( size_t mp = 0; mp < getElementNum<MutationProcess>(); mp++ )
		{
			if( getById<MutationProcess>( mp )->getType() == MutationProcess::Type::SPONTANEOUS )
			{
				bGillespie = true;
				return;
			}
		}
	}
}

void GeneticCollection::rankAndSort()
{
  //-------------------- BIOMASS: put the biomass Metabolite in the last place

  //locate the biomass metabolite, copy to the back and thn erase
	for( size_t m = 0; m < elements[ ElemTypeIdx::METABOLITE ]->size(); m++ )
	{
		if( getByIdEdit<Metabolite>( m )->getIsBiomass() )
		{
			elements[ ElemTypeIdx::METABOLITE ]->push_back( std::static_pointer_cast<GeneticElement>( elements[ ElemTypeIdx::METABOLITE ]->getElementPtr( m ) ) );
			elements[ ElemTypeIdx::METABOLITE ]->eraseAt( m );
		}
	}
  //update the relative ids of all the metabolites to match the new ordering
	for( size_t m = 0; m < elements[ ElemTypeIdx::METABOLITE ]->size(); m++ )
		getByIdEdit<Metabolite>( m )->setRelativeId( m );
  //set the direct access to biomass
	biomass = getById<Metabolite>( getElementNum<Metabolite>() - 1 );

  //-------------------------------- FUNCTIONS : rank and sort by rank
  //calculate ranks
	for( size_t f = 0; f < elements[ ElemTypeIdx::FUNCTION ]->size(); f++ )
		getByIdEdit<Function>( f )->calculateRank();
	
  //copy the functions container, sort it and delete the original one
	std::vector< SP<Function> > functions = std::static_pointer_cast< ut::PolyVector<Function, GeneticElement > >( elements[ ElemTypeIdx::FUNCTION ] )->getElements();
	std::sort( functions.begin(), functions.end(), []( SP<Function> a, SP<Function> b ) { return a->getRank() < b->getRank(); } );
	elements[ ElemTypeIdx::FUNCTION ]->clear();
  
  //load the functions in the new order and update their relative ids to match it
	for( size_t f = 0; f < functions.size(); f++ )
	{
		functions[ f ]->setRelativeId( f );
		elements[ ElemTypeIdx::FUNCTION ]->push_back( functions[ f ] );
	}

  //-------------------------------- GATES : rank and sort by rank
  //calculate ranks
	for( size_t ga = 0; ga < elements[ ElemTypeIdx::GATE ]->size(); ga++ )
		getByIdEdit<GateBase>( ga )->calculateRank();

  //copy the gates container, sort it and delete the original one
	std::vector< SP<GateBase> > gates = std::static_pointer_cast< ut::PolyVector<GateBase, GeneticElement > >( elements[ ElemTypeIdx::GATE ] )->getElements();
	std::sort( gates.begin(), gates.end(), []( SP<GateBase> a, SP<GateBase> b ) { return a->getRank() < b->getRank(); } );
	elements[ ElemTypeIdx::GATE ]->clear();

  //load the gates in the new order and update their relative ids to match it
	for( size_t ga = 0; ga < gates.size(); ga++ )
	{
		gates[ ga ]->setRelativeId( ga );
		elements[ ElemTypeIdx::GATE ]->push_back( gates[ ga ] );
	}
}
