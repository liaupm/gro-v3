#include "mg/Medium.hpp"

using namespace mg;

void Medium::init()
{ 
	//ut::print( "medium init");
    //auto signalNum = mediumCollection->getElementNum<Signal>();
	//zeroConcs = ConcsDS( mediumCollection->getElementNum<Signal>(), 0.0 ); 
	initGrid(); 
	fillReports(); 
}

void Medium::fillReports() 
{
	//ut::print( "fill reports with ", mediumCollection->getElementNum<Signal>(), " signals " );
    for( size_t s = 0; s < mediumCollection->getElementNum<Signal>(); s++ )
    {
    	//ut::print( mediumCollection->getById<Signal>( s )->getId(), mediumCollection->getById<Signal>( s )->getName(), "grid size: ", grid->getIniGridSize() );
    	//ut::print( mediumCollection->getById<Signal>( s )->getKdiff(), mediumCollection->getById<Signal>( s )->getResizeThresholdConc(), mediumCollection->getById<Signal>( s )->getMatrixHalfSize() );

    	//SignalReport* newReport = new SignalReport( mediumCollection->getById<Signal>( s ), grid->getIniGridSize() );
    	signalReports.emplace_back( std::make_shared<SignalReport>( mediumCollection->getById<Signal>( s ) ) );
    }
}
