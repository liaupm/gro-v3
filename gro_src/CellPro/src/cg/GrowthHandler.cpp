#include "cg/GrowthHandler.hpp"
#include "cg/Genome.hpp"
#include "cg/Strain.hpp" //model

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////
//================================================ GrowthReport

GrowthReport::GrowthReport( const Strain* model, GrowthHandler* ownerHandler )
: GeneticReport( model, ownerHandler ), delta( INI_BASE_GR ), lastVal( INI_LAST_VAL )
, lastUpdateTime( ownerHandler ? ownerHandler->getOwner()->getTime().time : INI_LAST_UPDATE_TIME ), lastReportedVal( INI_LAST_VAL ) {;}


//------------------------------------------------------------- common
bool GrowthReport::init()
{ 
    ownerHandler->getOwner()->removeChange( model );
    calculate();
    ownerHandler->getOwner()->setAndForward( ownerHandler->getOwner()->getCollection()->getGrMarker(), lastVal );
    return true;
}

void GrowthReport::reset()
{ 
    delta = INI_BASE_GR; 
    lastVal = INI_LAST_VAL; 
    lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

void GrowthReport::partialReset()
{ 
    delta = INI_BASE_GR; 
    lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

bool GrowthReport::update( const EventType* )
{ 
    //do not remove changes of Strain here because it may be used for changes other than the growth rate (like the death and division conditions). 
    //All the Strain changes are removed at GrowthHandler level
    
    accumulateDelta(); //using the previous lastVal
    calculate(); //update lastVal
  //if change greater than sensitivity, forward
    if( ut::relativeChange( lastReportedVal, lastVal, ownerHandler->getCollection()->getSensitivity() ) )
    {
        ownerHandler->getOwner()->setAndForward( ownerHandler->getOwner()->getCollection()->getGrMarker(), lastVal );
        lastReportedVal = lastVal;
        return true;
    }
  //if not, just set but don't update its dependent elements
    ownerHandler->getOwner()->justSet( ownerHandler->getOwner()->getCollection()->getGrMarker(), lastVal );
    return false;
}


//------------------------------------------------------------- special
void GrowthReport::calculate()
{
	if( ownerHandler->bDivisionVolReached )
		lastVal = 0.0;
	else
	{
		if( model->getGrCustomFun() ) //custom calculation
			lastVal = ownerHandler->getOwner()->checkQState( model->getGrCustomFun() );
		else  //canonic calculation
			lastVal = model->justSampleGrowthRate( ownerHandler->getOwner()->checkQState( model->getBaseGrRandomness() ) ) + ownerHandler->getOwner()->checkQState( ownerHandler->getOwner()->getCollection()->getBiomass() );
			//lastVal = model->sampleGrowthRate( ownerHandler->getOwner()->checkQState( model->getBaseGrRandomness() ), ownerHandler->getOwner()->getTime().stepSize ) + ownerHandler->getOwner()->checkQState( ownerHandler->getOwner()->getCollection()->getBiomass() );

		if( ! model->getBNegativeGrowthAllowed() ) //restrict sign
			lastVal = ut::fitL( lastVal );

		if( model->getBGrowthCorrection() ) //restrict sign
			lastVal = model->correctGrowthRate( lastVal, ownerHandler->getOwner()->getTime().stepSize );
	}
}

void GrowthReport::accumulateDelta()
{
    delta += lastVal * ( ownerHandler->getOwner()->getTime().time - lastUpdateTime );
    lastUpdateTime = ownerHandler->getOwner()->getTime().time;
}








//////////////////////////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////////////////////////////////////

GrowthHandler::GrowthHandler( OwnerType* owner, const GeneticCollection* collection )
: GeneticHandler( owner, collection )
, strain( owner->getCellType()->getStrain() ), growthReport( strain, nullptr )

, volume( INI_VOLUME ), iniVolume( INI_VOLUME ), dVolume( INI_D_VOLUME )
, lastReportedVol( INI_VOLUME ), lastReportedDVol( INI_D_VOLUME ), divisionVolume( INI_DIVISION_VOLUME )
, length( INI_LENGTH ), dLength( INI_D_LENGTH ), totalLength( INI_LENGTH + Strain::DF_DIAMETER )
, bMustDie( false ), bMustDivide( false ), bDivisionVolReached( false )
, lastDivisionFraction( INI_LAST_DIV_FRANCTION ), totalLengthFraction( INI_LAST_DIV_FRANCTION ) {;}


//------------------------------------------ common

bool GrowthHandler::init()
{
    growthReport.init();
    //volume is already sampled by preInitVol()
    checkDeath();
	recalculateLength();
	divisionVolume = strain->sampleDivisionVol();

	owner->setAndForward( owner->getCollection()->getPoleCountMarker(), 1.0 );
	owner->setBState( owner->getCollection()->getPolePositionMarker(), owner->getCollection()->samplePolePosDist() );

	ut::print( "$$$$$$$$$$$$$$$$$$$$$$$$$$$ init cell poles. Count: ", owner->checkQState( owner->getCollection()->getPoleCountMarker() ), "   sign: ", owner->checkBState( owner->getCollection()->getPolePositionMarker() ) );
	owner->removeChanges( ElemTypeIdx::STRAIN ); 
    return true;
}

bool GrowthHandler::divisionOld( HandlerBaseType* daughter )
{
    bMustDivide = false;
    bDivisionVolReached = false;
    owner->setAndForward( owner->getCollection()->getDivisionVolBMarker(), false );

  //---modify old and new cells
    TReal totalVolume = volume;
    TReal oldTotalLength = totalLength;
    updateVolumeDivision( lastDivisionFraction * totalVolume );
    totalLengthFraction = totalLength / oldTotalLength;

    static_cast<GrowthHandler*>( daughter )->updateVolumeDivision( ( 1.0 - lastDivisionFraction ) * totalVolume );
    static_cast<GrowthHandler*>( daughter )->lastDivisionFraction = 1.0 - lastDivisionFraction;
    static_cast<GrowthHandler*>( daughter )->totalLengthFraction = static_cast<GrowthHandler*>( daughter )->totalLength / oldTotalLength;
  //---

    divisionVolume = strain->sampleDivisionVol();
    growthReport.update( nullptr ); //to restart growth (the growth rate was 0 because the division volume was reached)

  //---pole track
    ut::print( "$$$$$$$$$$$$$$$$$$$$$$$$$$$ div previous. Count: ", owner->checkQState( owner->getCollection()->getPoleCountMarker() ), "   sign: ", owner->checkBState( owner->getCollection()->getPolePositionMarker() ) );
    TReal oldPoleCount = owner->checkQState( owner->getCollection()->getPoleCountMarker() );

    if( owner->checkBState( owner->getCollection()->getPolePositionMarker() ) )
    {
    	owner->setBState( owner->getCollection()->getPolePositionMarker(), false );
    	owner->setAndForward( owner->getCollection()->getPoleCountMarker(), 1.0 );
    	static_cast<GrowthHandler*>( daughter )->getOwner()->setAndForward( owner->getCollection()->getPoleCountMarker(), ut::round<uint>( oldPoleCount + 1.0 ) );
    }
    else
    {
    	//owner->addAndForward( owner->getCollection()->getPoleCountMarker(), 1.0 );
    	owner->setAndForward( owner->getCollection()->getPoleCountMarker(), ut::round<uint>( oldPoleCount + 1.0 ) );
   		static_cast<GrowthHandler*>( daughter )->getOwner()->setAndForward( owner->getCollection()->getPoleCountMarker(), 1.0 );
    }
    ut::print( " mother count: ", owner->checkQState( owner->getCollection()->getPoleCountMarker() ), "   sign: ", owner->checkBState( owner->getCollection()->getPolePositionMarker() ) );
    ut::print(  "daughter  count: ", static_cast<GrowthHandler*>( daughter )->getOwner()->checkQState( owner->getCollection()->getPoleCountMarker() ), "   sign: ", static_cast<GrowthHandler*>( daughter )->getOwner()->checkBState( owner->getCollection()->getPolePositionMarker() ) );


    return true;
}

bool GrowthHandler::divisionNew( HandlerBaseType* )
{
    bMustDivide = false;
    bDivisionVolReached = false;
    owner->setAndForward( owner->getCollection()->getDivisionVolBMarker(), false );

    divisionVolume = strain->sampleDivisionVol();
    growthReport.update( nullptr ); //to restart growth (the growth rate was 0 because the division volume was reached)

    owner->setBState( owner->getCollection()->getPolePositionMarker(), true );
    return true;
}


bool GrowthHandler::update( ut::MultiBitset*, ut::Bitset*, const GeneticHandler<>::EventType* event ) 
{
    if( owner->checkChange( strain ) ) //change in custom function, biomass metabolite, randomness or gates
    {
        growthReport.update( event );
        checkDeath(); //check even if the gorwth rate doesn't change because the change may be in the death gate
    }
    owner->removeChanges( ElemTypeIdx::STRAIN ); //remove also the Strains that do not match that of the cell
    return true;
}

void GrowthHandler::tick()
{
    grow(); //calculate dVol, increase vol, expose, recalculate length

    if( volume >= divisionVolume && ! bMustDivide ) //division size
    {
        if( owner->checkBState( strain->getDivisionGate() ) ) //all division conditions met
        {
            bMustDivide = true;
            owner->setAndForward( owner->getCollection()->getDivisionEventMarker(), true );
        }

        if( ! bDivisionVolReached ) //first time reaching the division value
        {
            bDivisionVolReached = true; //control to do this part only once
            growthReport.update( nullptr ); //to set the growth rate to 0.0
            owner->setAndForward( owner->getCollection()->getDivisionVolBMarker(), true );

          //calculate division fraction because the volume won't change more
            TReal minimumFraction = strain->getMinimunVol() / volume;
            lastDivisionFraction = ut::fit( strain->sampleDivisionFraction(), minimumFraction, 1.0 - minimumFraction );
        }
    }
}



//-------------------------------------------------- special

void GrowthHandler::preInitVol()
{
    volume = iniVolume = strain->sampleIniVol();
    owner->justSet( owner->getCollection()->getVolMarker(), volume );
}



//-------------------------------------------------- private

void GrowthHandler::updateVolumeDivision( TReal xVolume )
{ 
  //set and forward new volume
    volume = iniVolume = xVolume; 
    owner->setAndForward( owner->getCollection()->getVolMarker(), volume );

  //set and forward new dVolume. Important to do after volume
    dVolume = ut::fitL( growthReport.getDelta() * volume ) / owner->getTime().stepSize;
    owner->setAndForward( owner->getCollection()->getDvolMarker(), dVolume );

    recalculateLength(); 
}

void GrowthHandler::grow()
{
  //calculate dVolume and forward if enough change
    growthReport.accumulateDelta();
    dVolume = ut::fitL( growthReport.getDelta() * volume, 0.0 ) / owner->getTime().stepSize; //derivative, always positive
    growthReport.partialReset();

    if( ut::relativeChange( lastReportedDVol, dVolume, collection->getSensitivity() ) )
    {
    	owner->setAndForward( owner->getCollection()->getDvolMarker(), dVolume );
    	lastReportedDVol = volume;
    }
    else
    	owner->justSet( owner->getCollection()->getDvolMarker(), dVolume );
   
  //increase volume and forward if enough change
    if( volume < divisionVolume )
    {
        volume = ut::fitU( volume + dVolume * owner->getTime().stepSize, divisionVolume );
        if( ut::relativeChange( lastReportedVol, volume, collection->getSensitivity() ) )
        {
        	owner->setAndForward( owner->getCollection()->getVolMarker(), volume );
        	lastReportedVol = volume;
        }
		else
			owner->justSet( owner->getCollection()->getVolMarker(), volume );
		recalculateLength();
    }
}

void GrowthHandler::checkDeath()
{
    if( growthReport.getLastVal() < strain->getDeathThreshold() && owner->checkBState( strain->getDeathGate() ) )
        bMustDie = true;
}

void GrowthHandler::recalculateLength()
{
	if( DF_BTRACK_LENGTH )
	{
		length = strain->vol2length( volume );
		dLength = strain->dvol2dlength( dVolume );
        totalLength = length + Strain::DF_DIAMETER;
	}
}