#include "cg/GeneticElement.hpp"
#include "cg/GeneticCollection.hpp" //implicit cast to base in ctor, getElementNum()
#include "cg/Strain.hpp" //calculateActiveForCellTypes() getStrain()->getAbsoluteId()
#include "cg/CellType.hpp" //calculateActiveForCellTypes()

using namespace cg;


//has to be in the cpp for the conversion of collection
GeneticElement::GeneticElement( size_t relativeId, const std::string& name, GeneticElementIdx::ElemTypeIdx elementType, bool bQuantitative, const GeneticCollection* collection, ElemCategory elemCategory )
: ut::LinkedElement<GeneticElement, GeneticElementIdx>( relativeId, name, elementType
, bQuantitative, elemCategory == ElemCategory::DNA_CONTAINER || elemCategory == ElemCategory::DNA_CONTENT || elemCategory == ElemCategory::DNA_BOTH, collection ), elemCategory( elemCategory )
, bPhysical( elemCategory == ElemCategory::DNA_CONTAINER || elemCategory == ElemCategory::DNA_CONTENT || elemCategory == ElemCategory::DNA_BOTH || elemCategory == ElemCategory::FREE_ELEM )
, bForwardLogic( DF_B_FORWARD_LOGIC ), bForwardLogicContent( DF_B_FORWARD_LOGIC ), bForwardLogicCalculated( false ) {;}


bool GeneticElement::calculateBForwardLogic() 
{ 
	if( ! bForwardLogicCalculated )
	{
		bForwardLogic = getForwardLink().elemsByType[ ElemTypeIdx::FUNCTION ].size() > 0 || getForwardLink().elemsByType[ ElemTypeIdx::GATE ].size() > 0; 
		bForwardLogicCalculated = true;	
	}
	return bForwardLogic;
} 

void GeneticElement::calculateBForwardLogicContent()
{ 
	if( bForwardLogic )
	{
		bForwardLogicContent = true; //self is content
		return;
	}
	for( const auto& pair : getContent().elems )
	{
		if( const_cast< GeneticElement* >( pair.first )->calculateBForwardLogic() ) //recursive
		{
			bForwardLogicContent = true;
			break;
		}
	}
} 

void GeneticElement::calculateActiveForCellTypes()
{
	size_t cellTypeNum = static_cast<const GeneticCollection*>( collection )->getElementNum<CellType>();
	activeForCellTypes = std::vector<bool>( cellTypeNum, true );

	if( elementType != ElemTypeIdx::RANDOMNESS )
		return;

	//if( getForwardLink().elems.size() == getForwardLink().elemsByType[ ElemTypeIdx::STRAIN ].size() + 1 ) //+1 is self
	if( getForwardLink().elems.size() == 2 && getForwardLink().elemsByType[ ElemTypeIdx::STRAIN ].size() == 1 ) //+1 is self
	{
		for( size_t c = 0; c < cellTypeNum; c++ )
			activeForCellTypes[c] = getForwardLink().mask.checkBit( static_cast<const GeneticCollection*>( collection )->getById<CellType>( c )->getStrain()->getAbsoluteId() );
	}
}
