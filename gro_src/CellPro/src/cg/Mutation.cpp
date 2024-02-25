#include "cg/Mutation.hpp"
#include "cg/PlasmidBase.hpp" //::PlasmidType
#include "cg/PlasmidBoolean.hpp" //cast
#include "cg/PlasmidQuantitative.hpp" //cast

using namespace cg;


Mutation::Mutation( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const std::vector<const PlasmidBase*>& plasmids, const std::vector<int>& amounts, const QMarker* eventMarker )
: GeneticElement( id, name, ElemTypeIdx::MUTATION, true, geneticCollection, ElemCategory::FUNCTIONAL )
, plasmids( plasmids ), eventMarker( eventMarker ), amounts( amounts ), bNull( true )
{
  //classify the plasmids by type in bplasmids and qplasmids to treat them differently
	for( size_t p = 0; p < plasmids.size(); p++ )
	{
		if( plasmids[p]->getPlasmidType() == PlasmidBase::PlasmidType::BPLASMID ) //bplasmids
		{
		  //restrict the stoichiometric coefficients of bplasmids
			if( this->amounts[p] > 0 )
				this->amounts[p] = 1;
			else if( this->amounts[p] < 0 )
				this->amounts[p] = -1;

			bplasmids.push_back( static_cast<const PlasmidBoolean*>( plasmids[p] ) );
			bamounts.push_back( amounts[p] );
		}
		else //qplasmids
		{
			qplasmids.push_back( static_cast<const PlasmidQuantitative*>( plasmids[p] ) );
			qamounts.push_back( amounts[p] );
		}
			
		if( amounts[p] != 0 )
			bNull = false;
	}
}