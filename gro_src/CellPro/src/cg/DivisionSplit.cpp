#include "cg/DivisionSplit.hpp"

using namespace cg;


int DivisionSplit::splitExtremeCases( TReal fraction, TReal copyNum, bool daughter ) 
{
	if( ( fraction == 1.0 && daughter ) || ( fraction == 0.0 && ! daughter ) )
		return ut::round<uint>( copyNum ); 
	if( ( fraction == 0.0 && daughter ) || ( fraction == 1.0 && ! daughter ) )
		return 0; 
	return -1;
}

TReal DivisionSplit::scaleSplitFraction( TReal fraction, TReal volFraction, bool daughter )
{
	TReal ratio = volFraction / ( 1.0 - volFraction );
	if( daughter )
		ratio *= ( fraction / ( 1.0 - fraction ) );
	else
		ratio *= ( ( 1.0 - fraction ) / fraction );
	return ut::fit( ratio / ( ratio + 1.0 ) );
}