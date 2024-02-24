#include "mg/DiffusionMatrix.hpp"

using namespace mg;

//class consts
const DiffMatrixDS DiffusionMatrix::MATRIX_NEUMANN_4 ( { { 0.0, 1.0/4.0, 0.0 }, { 1.0/4.0, -1.0, 1.0/4.0 }, { 0.0, 1.0/4.0, 0.0 } } );
const DiffMatrixDS DiffusionMatrix::MATRIX_MOORE_8 ( { { 1.0/8.0, 1.0/8.0, 1.0/8.0 }, { 1.0/8.0, -1.0, 1.0/8.0 }, { 1.0/8.0, 1.0/8.0, 1.0/8.0 } } );


DiffusionMatrix::DiffusionMatrix( Neighborhood neighborhood, const DiffMatrixDS& matrix )
: DiffMatrixDS( selectMatrix( neighborhood, matrix ) )
, neighborhood(neighborhood)
{
	halfSize = size() / 2;
	paddingUnits = size() - 1;
}


DiffMatrixDS DiffusionMatrix::selectMatrix( Neighborhood neighborhood, const DiffMatrixDS& matrix )
{
	if( neighborhood == Neighborhood::FREE )
		return matrix;
	else
		return neighborhood == Neighborhood::MOORE_8 ? MATRIX_MOORE_8 : MATRIX_NEUMANN_4;
}