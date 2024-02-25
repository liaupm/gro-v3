#include "cg/ColourHandler.hpp"
#include "cg/Genome.hpp" //owner
#include "cg/CellColour.hpp" //model

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------------------------- common
bool ColourReport::init()
{ 
	ownerHandler->getOwner()->removeChange( model );
	return calculateTargetValue() ? calculateDelta() : false; 
}

bool ColourReport::update( const EventType* )
{
	ownerHandler->getOwner()->removeChange( model );
	calculateTargetValue();
	return calculateDelta();
}

bool ColourReport::tick()
{
	if( bActive ) //DELTA mode and value != targetValue
	{
		if( targetValue > value ) //add delta
			value = ut::fit( value + delta * ownerHandler->getOwner()->getTime().stepSize, 0.0, targetValue );
		else // targetValue < value, substract delta
			value = ut::fitL( value - delta * ownerHandler->getOwner()->getTime().stepSize );
		
		ownerHandler->getOwner()->justSet( model, value ); //no forward because the colour is final, cannot be used as input
		bActive = ! ut::areSame( value, targetValue ); //here is already known that the mode is DELTA, no ned to check
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------------------------- special
bool ColourReport::calculateTargetValue()
{
	if( ownerHandler->getOwner()->checkBState( model->getGate() ) ) //on
	{
		targetValue = model->calculateMaxValue( ownerHandler->getOwner()->checkQState( model->getTarget() ), ownerHandler->getOwner()->checkQState( model->getMaxValueRandomness() ) ); //already fit in [0,1]
		return true;
	}
	targetValue = 0.0; //off
	return false;
}

bool ColourReport::calculateDelta()
{
	if( model->getMode() == CellColour::ColourMode::DELTA )
	{
		delta = model->sampleDelta( ownerHandler->getOwner()->checkQState( model->getDeltaRandomness() ) );
		bActive = true;
		return true;
	}
	value = targetValue; //ABSOLUTE mode, automatic update
	ownerHandler->getOwner()->justSet( model, value );
	return false;
}



/////////////////////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////////////////////////////////////////

void ColourHandler::calculateTotalColour()
{
	totalColour.reset();
	for( const auto& colourR : getColourReports() )
		colourR.addTo( totalColour );
	totalColour.normalize();
}