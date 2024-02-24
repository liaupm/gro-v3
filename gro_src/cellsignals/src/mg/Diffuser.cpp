#include "mg/Diffuser.hpp"

using namespace mg;



void DiffuserBase::update( GridReport* gridReport, SignalReport* sReport, TReal dt, size_t, size_t ) 
{ 

	calculate( gridReport, dt, sReport->getSignal()->getId(), sReport->getSignal()->getKdiff(), sReport->getSignal()->getKdeg()
	, sReport->getBorder().getX1(), sReport->getBorder().getX2(), sReport->getBorder().getY1(), sReport->getBorder().getY2() );

	//size_t chunkSize = 100; //number of rows per chunck
	
	/*if( sReport->getBorder().getY2() <= sReport->getBorder().getY1() )
		return;

	size_t rowNum = sReport->getBorder().getY2() - sReport->getBorder().getY1();
	size_t chunkNum = rowNum / chunkSize + 1; //number of chuncks

	if( chunkNum > maxThreads )
    {
    	chunkNum = maxThreads;
    	chunkSize = rowNum / ( chunkNum - 1 );
    }

  //create and store futures
    std::vector< std::future<void> > futures;
    futures.reserve( chunkNum );
    for( size_t c = 0; c < chunkNum; c++ )
    {
        futures.emplace_back( std::async( &DiffuserBase::calculate, this
        , gridReport, dt, sReport->getSignal()->getId(), sReport->getSignal()->getKdiff(), sReport->getSignal()->getKdeg()
        , sReport->getBorder().getX1(), sReport->getBorder().getX2(), sReport->getBorder().getY1() + c * chunkSize, std::min( sReport->getBorder().getY1() + (c + 1) * chunkSize, (size_t)sReport->getBorder().getY2() ) ) );
    }
  
  //concurrent execution. All must finish for the function to return
    for( auto& fut : futures )
        fut.wait();*/
} 



/*void DiffuserOriginal::calculate( GridReport* gridReport, TReal dt, size_t id, TReal kdiff, TReal kdeg, size_t x1, size_t x2, size_t y1, size_t y2, const DiffusionMatrix& )
{
  //interior
    for( int row = border.getY1() + 1; row < border.getY2(); row++ ) 
    {
        for( int col = border.getX1() + 1; col < border.getX2(); col++ ) 
        {
			TReal concChange = - ( 4.0 * kdiff + kdeg ) * gridReport->getConcAtGridUnit( id, row, col ) + kdiff * (
            gridReport->getConcAtGridUnit( id, row-1, col ) +  gridReport->getConcAtGridUnit( id, row, col-1 ) +  gridReport->getConcAtGridUnit( id, row, col+1 ) + gridReport->getConcAtGridUnit( id, row+1, col ) );

            gridReport->getTempMatrix()[row][col].setConc( id, gridReport->getConcAtGridUnit( id, row, col ) + dt * concChange );
        }
    }
  //border pad
    for( int row = border.getY1(); row < border.getY2() + 1; row++ ) 
    {
    	gridReport->getTempMatrix()[row][border.getX1()].setConc( id, gridReport->getConcAtGridUnit( id, row, border.getX1() ) );
    	gridReport->getTempMatrix()[row][border.getX2()].setConc( id, gridReport->getConcAtGridUnit( id, row, border.getX2() ) );
    }
    for( int col = border.getX1() + 1; col < border.getX2(); col++ )
    {
    	gridReport->getTempMatrix()[border.getY1()][col].setConc( id, gridReport->getConcAtGridUnit( id, border.getY1(), col ) );
    	gridReport->getTempMatrix()[border.getY2()][col].setConc( id, gridReport->getConcAtGridUnit( id, border.getY2(), col ) );
    }
}*/



void DiffuserOriginal::calculate( GridReport* gridReport, TReal dt, size_t id, TReal kdiff, TReal kdeg, int x1, int x2, int y1, int y2 )
{
    for( int row = y1; row < y2 + 1; row++ ) 
    {
        for( int col = x1; col < x2 + 1; col++ ) 
        {
            if( row > y1 && row < y2 && col > x1 && col < x2 )
            {
            	TReal concChange = - ( 6.0 * kdiff + kdeg) * gridReport->getConcAtGridUnit( id, row, col ) + kdiff * (
                0.5 * gridReport->getConcAtGridUnit( id, row-1, col-1 ) +  gridReport->getConcAtGridUnit( id, row-1, col ) + 0.5 * gridReport->getConcAtGridUnit( id, row-1, col+1 )
                +  gridReport->getConcAtGridUnit( id, row, col-1 ) +  gridReport->getConcAtGridUnit( id, row, col+1 )
                + 0.5 * gridReport->getConcAtGridUnit( id, row+1, col-1 ) +  gridReport->getConcAtGridUnit( id, row+1, col ) + 0.5 *  gridReport->getConcAtGridUnit( id, row+1, col+1 ) );

				/*TReal concChange = - ( 4.0 * kdiff + kdeg ) * gridReport->getConcAtGridUnit( id, row, col ) + kdiff * (
	            gridReport->getConcAtGridUnit( id, row-1, col ) +  gridReport->getConcAtGridUnit( id, row, col-1 ) +  gridReport->getConcAtGridUnit( id, row, col+1 ) + gridReport->getConcAtGridUnit( id, row+1, col ) );*/
                
                gridReport->getTempMatrix()[row][col].setConc( id, gridReport->getConcAtGridUnit( id, row, col ) + dt * concChange );
            }
            else
                gridReport->getTempMatrix()[row][col].setConc( id, gridReport->getConcAtGridUnit( id, row, col ) );
        }
    }
}


/*void DiffuserOriginal::calculate( GridReport* gridReport, TReal dt, size_t id, TReal kdiff, TReal kdeg, size_t x1, size_t x2, size_t y1, size_t y2, const DiffusionMatrix& )
{
    for( int row = border.getY1(); row < border.getY2() + 1; row++ ) 
    {
        for( int col = border.getX1(); col < border.getX2() + 1; col++ ) 
        {
            if( row > border.getY1() && row < border.getY2() && col > border.getX1() && col < border.getX2() )
            {
                //ut::print( "diff center ", col, row );
                //TReal previousConc = gridReport->getConcAtGridUnit( id, row, col );
				
                TReal concChange = - ( 6.0 * kdiff + kdeg) * gridReport->getConcAtGridUnit( id, row, col ) + kdiff * (
                0.5 * gridReport->getConcAtGridUnit( id, row-1, col-1 ) +  gridReport->getConcAtGridUnit( id, row-1, col ) + 0.5 * gridReport->getConcAtGridUnit( id, row-1, col+1 )
                +  gridReport->getConcAtGridUnit( id, row, col-1 ) +  gridReport->getConcAtGridUnit( id, row, col+1 )
                + 0.5 * gridReport->getConcAtGridUnit( id, row+1, col-1 ) +  gridReport->getConcAtGridUnit( id, row+1, col ) + 0.5 *  gridReport->getConcAtGridUnit( id, row+1, col+1 ) );

                gridReport->getTempMatrix()[row][col].setConc( id, gridReport->getConcAtGridUnit( id, row, col ) + dt * concChange );
                //grids[tempGridIndex][row][col]->setConc( id, gridReport->getConcAtGridUnit( id, row, col ) + dt * concChange ); //TODO
            }
            else
                gridReport->getTempMatrix()[row][col].setConc( id, gridReport->getConcAtGridUnit( id, row, col ) );
        }
    }
}*/


void DiffuserMatrix::calculate( GridReport*, TReal, size_t, TReal, TReal, int, int, int, int )
{
    /*for ( int row = border.getY1() + M.getHalfSize(); row < border.getY2() + 1 - M.getHalfSize(); row++ ) 
    {
        for ( int col = border.getX1() + M.getHalfSize(); col < border.getX2() + 1 - M.getHalfSize(); col++ )
        {
            TReal concChange = - ( M[1][1] * kdiff + kdeg) * gridReport->getConcAtGridUnit( id, row, col ) + kdiff * (
            M[0][0] * gridReport->getConcAtGridUnit( id, row-1, col-1 ) +  M[0][1] * gridReport->getConcAtGridUnit( id, row-1, col ) + M[0][2] * gridReport->getConcAtGridUnit( id, row-1, col+1 )
            + M[1][0] * gridReport->getConcAtGridUnit( id, row, col-1 ) +  M[1][2] * gridReport->getConcAtGridUnit( id, row, col+1 )
            + M[2][0] * gridReport->getConcAtGridUnit( id, row+1, col-1 ) + M[2][1] * gridReport->getConcAtGridUnit( id, row+1, col ) + M[2][2] * gridReport->getConcAtGridUnit( id, row+1, col+1 ) );

            //grids[tempGridIndex][row][col]->setConc( id, gridReport->getConcAtGridUnit( id, row, col ) + dt * concChange ); //TODO
        }
    }*/
}
