
#include "mg/GridReport.hpp"

using namespace mg;


//=====================================================================* INIT and CLEAR *==========================================================================
void GridReport::fillGrid( size_t idx, size_t signalNum )
{
    GridMatrix& matrix = matrixes[idx];

    for( size_t i = 0; i < gridSize; i++ )
    {
        GridRow tempRow = {};
        for( size_t j = 0; j < gridSize; j++ )
            tempRow.emplace_back( signalNum );
        matrix.push_back(tempRow);
    }
}

void GridReport::createMatrix( GridMatrix& matrix, size_t signalNum, size_t rowNum, size_t colNum )
{
    matrix.reserve( rowNum );
    for( size_t i = 0; i < rowNum; i++ )
        matrix.push_back( createRow( signalNum, colNum ) );
}

GridReport::GridMatrix GridReport::createMatrix( size_t signalNum, size_t rowNum, size_t colNum )
{
    GridMatrix matrix;
    createMatrix( matrix, signalNum, rowNum, colNum );
    return matrix;
}


GridReport::GridRow GridReport::createRow( size_t signalNum, size_t colNum )
{
    GridRow row;
    row.reserve( colNum );

    for( size_t j = 0; j < colNum; j++ )
        row.emplace_back( signalNum );
    return row;
}

void GridReport::clearGrid( size_t )
{
    //GridMatrix& matrix = matrixes[idx];
    /*for( size_t i = 0; i < matrix.size(); i++ )
    {
        for( size_t j = 0; j < matrix[i].size(); j++ )
            delete matrix[i][j];
    }*/
}

//=====================================================================* UPDATE *==========================================================================
void GridReport::resizePhysical()
{ 
    /*for( auto& matrix : matrixes )
    {
        for( size_t i = 0; i < gridSize; i++ )
        {
            auto leftUnits = createRow( getConcsSize(), grid->getDeltaGridSize() );
            matrix[i].insert( matrix[i].begin(), leftUnits.begin(), leftUnits.end() );

            for( size_t j = 0; j < grid->getDeltaGridSize(); j++ )
                matrix[i].emplace_back( getConcsSize() );
        }

        auto upRows = createMatrix( getConcsSize(), grid->getDeltaGridSize(), gridSize + 2 * grid->getDeltaGridSize() );
        matrix.insert( matrix.begin(), upRows.begin(), upRows.end() );

        for( size_t i = 0; i < grid->getDeltaGridSize(); i++ )
            matrix.push_back( createRow( getConcsSize(), gridSize + 2 * grid->getDeltaGridSize() ) );

        //ut::print( "new matrix size: ", matrix.size() );
    }
    setGridSize( gridSize + 2 * grid->getDeltaGridSize() ); */ //TODO
} 


void GridReport::setConcsRectangleUnit( const ConcsDS& concs, int row1, int row2, int col1, int col2 )
{
	for( int r = row1; r < row2; r++ )
	{
		for( int c = col1; c < col2; c++ )
			setConcsAtGridUnit( concs, r, c );
	}
}

void GridReport::setConcRectangleUnit( size_t id, TReal conc, int row1, int row2, int col1, int col2 )
{
	for( int r = row1; r < row2; r++ )
	{
		for( int c = col1; c < col2; c++ )
			setConcAtGridUnit( id, conc, r, c );
	}
}