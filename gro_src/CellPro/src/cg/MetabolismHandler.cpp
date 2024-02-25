#include "cg/MetabolismHandler.hpp"
#include "cg/Genome.hpp" //owner
#include "cg/Strain.hpp" //getGrCustomFun()
#include "cg/Metabolite.hpp" //model
#include "cg/Flux.hpp" //model

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////

//======================================================================================================================================================================Flux
FluxReport::FluxReport( const Flux* model, MetabolismHandler* ownerHandler ) 
: GeneticReport( model, ownerHandler )
, bExternalCondition( INI_B_EXTERNAL_CONDITION ), bActive( INI_B_ACTIVE ), rawValue( INI_VALUE ), value( INI_VALUE ) {;}


//-------------------------------------------------------------------common
bool FluxReport::init()
{ 
	ownerHandler->getOwner()->removeChange( model );
	if( model->getTimingType() == Flux::TimingType::CONTINUOUS )
	{
		fullCheck( true );
		exposeParams( true );
		return true;
	}
	return false;
}

bool FluxReport::update( const EventType* )
{
	ownerHandler->getOwner()->removeChange( model );
	if( model->getTimingType() == Flux::TimingType::CONTINUOUS )
	{
		fullCheck( true );
		exposeParams( false );
		return true;
	}
	return false;
}

void FluxReport::exposeParams( bool bInit )
{
	if( model->getThresholdMarker() && ( model->getThresholdRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getThresholdMarker(), model->scaleThreshold( ownerHandler->getOwner()->checkQState( model->getThresholdRandomness() ) ) );
}

//---------------------------------------------------------------------------------------private
void FluxReport::fullCheck( bool bForward )
{
	//bForward true for CONTINUOUS fluxes and false for LYSIS
	checkExternalState( model->getMetabolite()->getIsBiomass() ? 0.0 : ownerHandler->getOwner()->checkQState( model->getMetabolite()->getSignalMarker() ) );

	if( checkState() && bForward )
		ownerHandler->getOwner()->addChangesFrom( model, GeneticCollection::ElemTypeIdx::MOLECULE );
	
	if( calculateFlux() && bForward )
		ownerHandler->getOwner()->addChange( model->getMetabolite() );
}

bool FluxReport::checkExternalState( TReal conc )
{
	bool oldExternalCondition = bExternalCondition;
	TReal scaledThreshold = model->scaleThreshold( ownerHandler->getOwner()->checkQState( model->getThresholdRandomness() ) ); 
	bExternalCondition = ownerHandler->getOwner()->checkBState( model->getSignalsGate() ) 
	&& ( model->getMetabolite()->getIsBiomass() || conc >= scaledThreshold );

	//if( model->getThresholdMarker() )
		//ownerHandler->getOwner()->setAndForward( model->getThresholdMarker(), scaledThreshold );
	
	return bExternalCondition != oldExternalCondition;
}

bool FluxReport::checkState()
{
	bool oldActive = bActive;
	bActive = ownerHandler->getOwner()->checkBState( model->getInternalGate() ) && bExternalCondition;
	return bActive != oldActive;
}

bool FluxReport::calculateFlux()
{
	TReal oldVal = rawValue;

	if( bActive )
	{ 
		if( model->getAmountFunction() ) ////custom flux Function
		{
			rawValue = ownerHandler->getOwner()->checkQState( model->getAmountFunction() );
			if( model->getIsInverted() )
				rawValue = - rawValue;
		}
		else  //independent param
			rawValue = model->calculate( ownerHandler->getOwner()->checkQState( model->getAmountRandomness() ), bActive );
	
	  //scaling by the volume and restricting the direction
		if( model->getIsRelative() )
			rawValue *= ownerHandler->getOwner()->getVolume();

		if( ( rawValue < 0.0 && model->getFluxDirection() == Flux::FluxDirection::EMISSION ) || ( rawValue > 0.0 && model->getFluxDirection() == Flux::FluxDirection::ABSORPTION ) )
			rawValue = 0.0;
	}
	else
		rawValue = 0.0;

	return ut::notSame( oldVal, rawValue );
}

void FluxReport::updateValue( TReal newVal )
{
	if( ut::notSame( newVal, value ) )
	{
		value = newVal;
		ownerHandler->getOwner()->setAndForward( model, newVal );
		ownerHandler->getOwner()->removeChange( model->getMetabolite() ); //the metabolite needs no update as this method is called by it
	}
}

void FluxReport::commitValue()
{
	if( ut::notSame( rawValue, value ) )
	{
		value = rawValue;
		ownerHandler->getOwner()->setAndForward( model, value );
		ownerHandler->getOwner()->removeChange( model->getMetabolite() ); //the metabolite needs no update as this method is called by it
	}
}



//======================================================================================================================================================================Metabolite

MetaboliteReport::MetaboliteReport( const Metabolite* model, MetabolismHandler* ownerHandler ) 
: GeneticReport( model, ownerHandler )
, bLimited( INI_B_LIMITED ), bExternalChange( false ), rawValue( INI_LAST_VAL ), delta( INI_DELTA ), lastVal( INI_LAST_VAL ), lastUpdateTime( ownerHandler->getOwner()->getTime().time ) {;}


//-------------------------------------------------------------------common
void MetaboliteReport::link()
{ 
	fluxReports.clear();
	for ( const auto& f : model->getBackwardLink().elemsByType[ GeneticCollection::ElemTypeIdx::FLUX ] )
		fluxReports.push_back( &ownerHandler->getFluxReportsEdit()[ f->getRelativeId() ] );
}

void MetaboliteReport::reset()
{ 
	bLimited = INI_B_LIMITED;
	rawValue = INI_LAST_VAL;
	lastVal = INI_LAST_VAL; 
	delta = INI_DELTA; 
	lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

void MetaboliteReport::partialReset()
{ 
	delta = INI_DELTA; 
	lastUpdateTime = ownerHandler->getOwner()->getTime().time; 
}

bool MetaboliteReport::init()
{
	ownerHandler->getOwner()->removeChange( model );
	collectFluxes(); //updates rawValue
	limitToMedium(); //updates lastVal
	if( ut::notSame( lastVal, INI_LAST_VAL ) )
	{
		ownerHandler->getOwner()->setAndForward( model, lastVal );
		return true;
	}
	return false;
}

bool MetaboliteReport::update( const EventType* )
{
	ownerHandler->getOwner()->removeChange( model );

	if( collectFluxes() || bExternalChange ) //change in rawValue or in the available concentration
	{
		bExternalChange = false;
		accumulateDelta();
		limitToMedium();
		ownerHandler->getOwner()->setAndForward( model, lastVal );
		return true;
	}
	limitToMedium(); //has to be called always, even if there is no restriction to do, to commit the rawValues of FluxReport
	return false;
}


//--------------------------------------------------------------------------special
bool MetaboliteReport::updateConc( TReal conc )
{
	//if negative flux limited by medium and ( change in concentration or partial step in the last restriction )
	if( ! ut::isPositive( lastVal ) && ( ut::notSame( conc, ownerHandler->getOwner()->checkQState( model->getSignalMarker() ) ) || bLimited ) ) 
	{
		bExternalChange = true;
		ownerHandler->getOwner()->addChange( model );
	}
  //forward marker if change in the concentration
	if( ut::notSame( conc, ownerHandler->getOwner()->checkQState( model->getSignalMarker() ) ) )
	{
		ownerHandler->getOwner()->setAndForward( model->getSignalMarker(), conc );
		return true;
	}
 	return false;
}


//-------------------------------------------------------------------private
bool MetaboliteReport::collectFluxes()
{
	TReal oldRaw = rawValue;
	rawValue = 0.0;
	for( auto& fluxR : fluxReports )
	{
		if( fluxR->model->getTimingType() == Flux::TimingType::CONTINUOUS )
			rawValue += fluxR->rawValue;
	}
	return ut::notSame( rawValue, oldRaw );
}

void MetaboliteReport::accumulateDelta()
{
	delta += lastVal * ( ownerHandler->getOwner()->getTime().time - lastUpdateTime );
	lastUpdateTime = ownerHandler->getOwner()->getTime().time;
}

bool MetaboliteReport::integrate()
{
	accumulateDelta();
	if( ! model->getIsBiomass() )
		ownerHandler->totalFluxes[ model->getRelativeId() ] += delta; //*= instead of + to add CONTINUOUS and LYSIS fluxes in case of cell death. totalFluxes is assumed to be reset before
	partialReset();
	return true;
}


void MetaboliteReport::limitToMedium()
{
//---biomass case: no restriction, just commit
	if( model->getIsBiomass() )
	{
		for( auto& fluxR : fluxReports )
			fluxR->commitValue();
		lastVal = rawValue;
		return;
	}

//---non-biomass: negative fluxes restricted by medium availability
	TReal available = ut::fitL( ownerHandler->getOwner()->checkQState( model->getSignalMarker() ) + delta ); //medium concentration + change during the current time step
	
	TReal remainingTime = ut::fitL( ownerHandler->getOwner()->getTime().getNextTime() - lastUpdateTime ); //remaining of the current time step
	if( ut::isZero( remainingTime ) )
		remainingTime = ownerHandler->getOwner()->getTime().stepSize; //avoid zero times (division by zero)
	bLimited = ut::notSame( remainingTime, ownerHandler->getOwner()->getTime().stepSize ); //true for partial time steps. Will require update the next time step even if the concs don't change
	
	TReal extraSignal = available + rawValue * remainingTime; //the concentration that would result if the current raw total flux was active for the remaining time

  //if negative, fluxes must be scaled down
	if( ut::isNegative( extraSignal ) )
	{
		lastVal = - available / remainingTime; //the total flux that would completely deplete the medium conc

	  //calculate scale
		TReal totalNeg = 0.0; //only negative fluxes are scaled down
		for( const auto& fluxR : fluxReports )
		{
			if( ut::isNegative( fluxR->rawValue ) && fluxR->model->getTimingType() == Flux::TimingType::CONTINUOUS )
				totalNeg += fluxR->rawValue;
		}
		TReal scale = 1.0 - extraSignal / ( totalNeg * remainingTime ); // neg / neg = pos

	  //apply scale
		for( auto& fluxR : fluxReports )
		{
			if( fluxR->model->getTimingType() != Flux::TimingType::CONTINUOUS )
				continue;

			if( ut::isNegative( fluxR->rawValue ) && fluxR->getBActive() )
				fluxR->updateValue( fluxR->rawValue * scale ); //neg - ( neg * pos)
			else
				fluxR->commitValue();
		}
	}
  //if positive, no restriction is needed. Just commit
	else
	{
		for( auto& fluxR : fluxReports )
		{
			if( fluxR->model->getTimingType() == Flux::TimingType::CONTINUOUS )
				fluxR->commitValue();
		}
		lastVal = rawValue;
	}
}



//////////////////////////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------get
TReal MetabolismHandler::getBiomassFlux() const 
{ 
	return owner->checkQState( getMetaboliteReports().back().model ); 
}

//---------------------------------------------------------------------------API special
void MetabolismHandler::tick()
{
	resetTotalFluxes();

	for( auto& metRep : getMetaboliteReportsEdit() )
		metRep.integrate();
	
	if( owner->getBMustDie() )
		applyLysisFluxes();
}

void MetabolismHandler::applyLysisFluxes()
{
	for( auto& fluxRep : getFluxReportsEdit() )
	{
		if( ! fluxRep.model->getMetabolite()->getIsBiomass() && fluxRep.model->getTimingType() == Flux::TimingType::LYSIS )
		{
			fluxRep.fullCheck( false ); //update bit don't forward. Lysis fluxes are applied directly and unrestricted (assumed to be positive)
			if( fluxRep.bActive )
				totalFluxes[ fluxRep.model->getMetabolite()->getRelativeId() ] += fluxRep.rawValue;
		}
	}
}