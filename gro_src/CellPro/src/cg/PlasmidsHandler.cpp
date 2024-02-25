#include "cg/PlasmidsHandler.hpp"
#include "ut/DistributionInterface.hpp" //ut::DiscreteDist::RealParamsType, ut::DiscreteDist::ParamsType params;
#include "ut/Time.hpp" //ut::Time::applyEpsilon() when creating events, time.time

#include "cg/Genome.hpp" //owner
#include "cg/GrowthHandler.hpp" //getLastDivisionFraction() to split QPlasmids on division
#include "cg/CopyControl.hpp"
#include "cg/OriV.hpp" //model
#include "cg/PartitionSystem.hpp" //model
#include "cg/PlasmidBase.hpp" //model
#include "cg/PlasmidQuantitative.hpp"
#include "cg/Mutation.hpp" //applyMutation()
#include "cg/MutationProcess.hpp" //model

/*PRECOMPILED
#include <algorithm> //std::count() for mutation keys */

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////

//======================================================================================================================================================================QPlasmid

//---------------------------------------------------------------------common
bool QPlasmidReport::update( const EventType* event )
/* responds to events created by self (end of eclipse times). Call to update eclipse time if the event matches */
{
    ownerHandler->getOwner()->removeChange( model );

    if( event && event->eventMode == Genome::EventType::ELEM_CHANGE && event->emitter == model && eclipseTimes.size() > 0 ) //ELEM_CHANGE, emitter and element self, positive quantity. No need to check all
    {
        updateEndOfEclipse();
        return true;
    }
    return false;
}

bool QPlasmidReport::divisionOld( QPlasmidReport* daughter )
/* a filter to call the division method only for BPlasmids; QPlasmids are handled by their PartitionSystems instead */
{
    if( model->getPlasmidType() == PlasmidBase::PlasmidType::BPLASMID )
    {
        bPlasmidDivisionOld( daughter );
        return true;
    }
    return false;
}

void QPlasmidReport::exposeParams( bool bInit )
{
	if( model->getPlasmidType() == PlasmidBase::PlasmidType::BPLASMID )
	{
		const PlasmidBoolean* plasmid = static_cast<const PlasmidBoolean*>( model );
		if( plasmid->getLossMarker() && ( plasmid->getLossRandomness() || bInit ) )
    		ownerHandler->getOwner()->setQState( plasmid->getLossMarker(), plasmid->sampleLossProb( ownerHandler->getOwner()->checkQState( plasmid->getLossRandomness() ) ) );
	}
}


void QPlasmidReport::addEclipseMarkers( int amount )
{
	const PlasmidQuantitative* plasmid = static_cast< const PlasmidQuantitative* >( model );
  //plasmid marker
	if( plasmid->getHasEclipseMarker() )
		ownerHandler->getOwner()->addAndForward( plasmid->getEclipseMarker(), amount );
  //partition marker
	ownerHandler->getOwner()->addAndForward( plasmid->getPartitionSystem()->getEclipseMarker(), amount );
  //orivs
	const std::vector<const OriV*>& orivs = plasmid->getOriVs();
    for( size_t ov = 0; ov < orivs.size(); ov++ )
    {
    	if( orivs[ov]->getEclipseMarker() )
    		ownerHandler->getOwner()->addAndForward( orivs[ov]->getEclipseMarker(), amount );
    }
}

//------------------------------------------------------------------------------------------------------private

bool QPlasmidReport::updateAsSelected( const OriVReport* emitter )
/* called when a QPlasmid is selected for replication from a OriV
it decides whether a mutation occurs and updates the genome state directly:
  a) if no eclipse, the QPlasmid addition directly
  b) if eclipse, the removal of the old copy. The eclipse times and events are then created when this first event occurs 
return true if repli mutation */
{
  //sample and set the replication mutation
	ut::DiscreteDist::ParamsType params;
	const auto& keys = emitter->model->getMutationKeys();
    const MutationProcess* mutation = model->selectMutation( keys, ownerHandler->getOwner()->getBState(), ownerHandler->getOwner()->getQState(), params );

  //expose mutation param markers
    const std::vector<const MutationProcess*>& repliMutations = model->getRepliMutations();
    for( size_t m = 0; m < repliMutations.size(); m++ )
	{
	    if( repliMutations[m]->getRateMarker() && std::count( keys.begin(), keys.end(), model->getRepliMutationKeys()[m] ) ) //if in emitter keys
	    {
	    	ownerHandler->getOwner()->setAndForward( repliMutations[m]->getRateMarker(), params[m] );
	    }
	}

    if( mutation )
    {
    	ownerHandler->getMutationProcessReportsEdit()[ mutation->getRelativeId() ].updateAsSelected(); //the MutationProcessReport handles it
        return true;
    }
	else //if normal replication, make the elem change or create eclipse events, depending on whether it has eclipse period
	{
		if( static_cast< const PlasmidQuantitative* >( model )->getHasEclipse() && emitter->model->getHasEclipse() ) //known that this is a QPlasmid (called from OriV). If eclipse, remove 1 plasmid and create 2 eclipses
		{
			addEclipseTimes( 2 );
			ownerHandler->getOwner()->addAndForwardContent( model, -1 );
		}
		else //without eclipse, just add one
			ownerHandler->getOwner()->addAndForwardContent( model, 1 );
		return false;
	}
}

bool QPlasmidReport::updateNoMutation( bool bEmitterHasEclipse, bool bRecipient )
/* called from OriT on conjugation, already decided that there is no mutation.
bRecipient true for recipient and false for donor */
{
	if( model->getPlasmidType() == PlasmidBase::PlasmidType::QPLASMID && static_cast< const PlasmidQuantitative* >( model )->getHasEclipse() && bEmitterHasEclipse ) //with eclipse
	{
		if( ! bRecipient )
			ownerHandler->getOwner()->addAndForwardContent( model, -1 ); //if donor, the copy that undergoes eclipse is first removed; not for recipient
		addEclipseTimes( 1 );
	}
	else if( bRecipient ) //without eclipse, recipient increases the plasmid by 1 and nothing happens to donor
		ownerHandler->getOwner()->addAndForwardContent( model, 1 );
	return false;
}


//---eclipse
void QPlasmidReport::addEclipseTimes( size_t num )
/* called on the starting event that removes 1 QPlasmid copy (if no mutation) or the mutation one
only called for QPlasmids and (OriVs, OriTs or MutationProcesses) that have eclipse period */
{
	const PlasmidQuantitative* plasmid = static_cast< const PlasmidQuantitative* >( model );
  //increase markers
	//if( plasmid->getHasEclipseMarker() )
		//ownerHandler->getOwner()->addAndForward( plasmid->getEclipseMarker(), num );
	//ownerHandler->getOwner()->addAndForward( plasmid->getPartitionSystem()->getEclipseMarker(), num );

	addEclipseMarkers( num );

  //create num eclipse times
    bool bSame = false; //to detect the rare case of the new eclipse times matching the current first. This would require an update to change the amount
    for( size_t i = 0; i < num; i++ )
    {

        TReal newTime = ownerHandler->getOwner()->getTime().time + ut::Time::applyEpsilon( plasmid->calculateEclipse( ownerHandler->getOwner()->checkQState( plasmid->getEclipseFunction() ) ) );
        eclipseTimes.add( newTime );
        if( newTime == nextEclipseTime )
            bSame = true;
    }

  //check if new eclipse times have to replace the old first
    if( eclipseTimes[0] < nextEclipseTime || bSame )
        removeEclipseEvents();
    
    if( eclipseTimes.size() == num || eclipseTimes[0] < nextEclipseTime || bSame  )
        createEclipseEvents();
}

void QPlasmidReport::updateEndOfEclipse()
/* called when a eclipse event happens, from update() */ 
{
  //decrease markers
	//if( static_cast< const PlasmidQuantitative* >( model )->getHasEclipseMarker() )
		//ownerHandler->getOwner()->addAndForward( static_cast< const PlasmidQuantitative* >( model )->getEclipseMarker(), - (long)nextEclipseNum ); //cast to signed required to get a negative number (nextEclipseNum is unsigned)
	//ownerHandler->getOwner()->addAndForward( static_cast< const PlasmidQuantitative* >( model )->getPartitionSystem()->getEclipseMarker(), - (long)nextEclipseNum );

	addEclipseMarkers( - (int)nextEclipseNum );
  //remove times
    eclipseTimes.erase( eclipseTimes.begin(), eclipseTimes.begin() + nextEclipseNum );
  //create the next events 
    createEclipseEvents();
}

void QPlasmidReport::createEclipseEvents()
/* selects the first (one or several with same time) times from eclipseTimes to create events */
{
    if( ! eclipseTimes.empty() )
    {
	    nextEclipseTime = eclipseTimes[0];

	    for( nextEclipseNum = 1; nextEclipseNum < eclipseTimes.size(); nextEclipseNum++ ) //nextEclipseNum counts the number of simultaneous eclipses with exact same time
	    {
	    	if( eclipseTimes[ nextEclipseNum ] != nextEclipseTime )
	    		break;
	    } 
        ownerHandler->getOwner()->addEvent( Genome::EventType::ELEM_CHANGE, model, model, nextEclipseTime, nextEclipseNum ); //ELEM_CHANGE so that the plasmids are added at Genome when handling the event
    }
}

void QPlasmidReport::removeEclipseEvents()
/* removes the existing event. Used when:
  a) it has to be replaced by a sorter time 
  b) on division, that time went to the other cell. In that case, the new size of eclipseTimes may be zero */
{
	ownerHandler->getOwner()->removeEvent( Genome::EventType::ELEM_CHANGE, model, model, nextEclipseTime, nextEclipseNum );
}

void QPlasmidReport::updateEclipseEvents()
/* checks if the current eclipse event is the first time and modifies it if not */
{
    //deliberately != and not < because it may also be > if the first time was lost on division
    if( eclipseTimes.size() > 0 && nextEclipseTime != eclipseTimes[0] )
    {
        removeEclipseEvents();
        createEclipseEvents();
    }
}

//---
void QPlasmidReport::bPlasmidDivisionOld( QPlasmidReport* daughter )
/* randomnly removes a BPlasmid from one of the daughter cells if loss
refhreshes the logic and creates changes */
{
	const PlasmidBoolean* plasmid = static_cast<const PlasmidBoolean*>( model );
    uint lossVal = plasmid->sampleLoss( ownerHandler->getOwner()->getQState() );
    switch( lossVal )
    {
        case 1: //current cell losses the plasmid
            ownerHandler->getOwner()->addAndForwardContent( model, -1 );
            break;
        case 2: //daughter losses the plasmid
            daughter->ownerHandler->getOwner()->addAndForwardContent( model, -1 );
            break;
        default : break; //case 0: nobody lossses the plasmid  
    }

  //expose loss param marker
	exposeParams( false );
}

void QPlasmidReport::qPlasmidIndependentDivisionOld( QPlasmidReport* daughter )
/* called from the PartitionSystemReport if false (random) or not shared
Only called for QPlasmids as only QPlasmids have PartitionSystems */
{
	const PlasmidQuantitative* plasmid = static_cast< const PlasmidQuantitative* >( model );
    const PartitionSystem* partition = plasmid->getPartitionSystem();

	removeEclipseEvents();
    TReal total = ownerHandler->getOwner()->checkQState( model ) + eclipseTimes.size();

  //get the number of copies that stay in the current cell (half). total - half go to daughter
    uint half;
    if( ownerHandler->getOwner()->checkBState( partition->getGate() ) ) //exact partition
    	half = partition->sampleExact( total, ownerHandler->getOwner()->getGrowthHandler().getLastDivisionFraction(), ownerHandler->getOwner()->checkQState( partition->getFractionFunction() ), ownerHandler->getOwner()->checkQState( partition->getFractionRandomness() ) );
    else //binomial
    	half = partition->sampleRandom( total, ownerHandler->getOwner()->getGrowthHandler().getLastDivisionFraction(), ownerHandler->getOwner()->checkQState( partition->getPFunction() ), ownerHandler->getOwner()->checkQState( partition->getPRandomness() ) );
  
  //simpler case, just modify the QState, add changes and refhresh logic
    if( eclipseTimes.size() == 0 )
    {
        ownerHandler->getOwner()->setAndForwardContent( model, half );
        daughter->ownerHandler->getOwner()->setAndForwardContent( model, total - half );
    }
    else
    {
      //encode the eclipse times and the active copies as ids
        uint totalQState = ownerHandler->getOwner()->checkQState( model );
        std::vector<size_t> idxs;
        idxs.reserve( total );

        for( size_t et = 0; et < eclipseTimes.size(); et++ )
            idxs.push_back( et );
        for( size_t q = eclipseTimes.size(); q < total; q++ )
            idxs.push_back( eclipseTimes.size() );

      //shuffle the eclipse times and the copies of qState to then split them in 2 
        std::shuffle( idxs.begin(), idxs.end(), partition->getShuffleRndEngine() );

      //clear the eclipse times of both cells to add the selected half of them. It will be more efficient erasing, but less clear
        auto eclipseTimesBak = eclipseTimes;
        eclipseTimes.clear();
        daughter->eclipseTimes.clear(); //not efficient but consistent

      //decode the ids
        uint qState = 0; //the resulting qState in the current cell. The daughter gets totalQState - qState
        for( uint i = 0; i < half; i++ ) //current cell
        {
            if( idxs[i] < eclipseTimesBak.size() )
                eclipseTimes.add( eclipseTimesBak[ idxs[i] ] );
            else
                qState++;
        }
        for( uint i = half; i < total; i++ ) //daughter
        {
            if( idxs[i] < eclipseTimesBak.size() )
                daughter->eclipseTimes.add( eclipseTimesBak[ idxs[i] ] );
        }
      //apply the changes
        ownerHandler->getOwner()->setAndForwardContent( model, qState );
        daughter->ownerHandler->getOwner()->setAndForwardContent( model, totalQState - qState );

      //update markers
        ownerHandler->storeEclipseMarkers( plasmid, half - qState );
        daughter->ownerHandler->storeEclipseMarkers( plasmid, ( total - half ) - ( totalQState - qState ) );

	  //create the event for the first eclipse time of each cell
        daughter->createEclipseEvents(); //has to create them from scartch as events are not copied
        createEclipseEvents();
    }
}




//======================================================================================================================================================================OriV

//----------------------------------common
void OriVReport::link()
{
    qplasmidReports.clear();
    for( const auto& qp : model->getBackwardContent().elemsByType[ GeneticElement::ElemTypeIdx::PLASMID ] )
        qplasmidReports.push_back( const_cast<QPlasmidReport*>( &ownerHandler->getQPlasmidReports()[ qp->getRelativeId() ] ) );
}

bool OriVReport::update( const EventType* event )
/* responds only to events created by self */
{
    if( event && event->emitter == model )
    {
    	updateAsSelected();
    	return true;
    }
    return false;
}

void OriVReport::exposeParams( bool bInit )
{
    if( ! model->getCustomFunction() )
    {
    	if( model->getVmaxMarker() && ( model->getVmaxRandomness() || bInit ) )
    		ownerHandler->getOwner()->setAndForward( model->getVmaxMarker(), model->sampleVmax( ownerHandler->getOwner()->checkQState( model->getVmaxRandomness() ) ) );
    	if( model->getWMarker() && ( model->getWRandomness() || bInit ) )
    		ownerHandler->getOwner()->setAndForward( model->getWMarker(), model->sampleW( ownerHandler->getOwner()->checkQState( model->getWRandomness() ) ) );

    	const std::vector<const CopyControl*>& copyControls = model->getCopyControls();
    	for( const auto* cc : copyControls )
    	{
    		if( cc->getWMarker() && ( cc->getWRandomness() || bInit ) )
    			ownerHandler->getOwner()->setAndForward( cc->getWMarker(), cc->sampleW( ownerHandler->getOwner()->checkQState( cc->getWRandomness() ) ) );
    	}
    }
}

//--------------------------------------special

void OriVReport::exposeEclipseMarkers()
/*update the qstate of fake event markers */
{
	if( model->getEclipseMarker() )
	{
		if( eclipseMarkers != ut::round<uint>( ownerHandler->getOwner()->checkQState( model->getEclipseMarker() ) ) )
			ownerHandler->getOwner()->setAndForward( model->getEclipseMarker(), eclipseMarkers );
		eclipseMarkers = 0;
	}
}


//--------------------------------------private
void OriVReport::calculateRate()
/* value stored in newRate until it is accessed */
{
  //calculate
    newRate = model->calculateRate( ownerHandler->getOwner()->getBState(), ownerHandler->getOwner()->getQState(), ownerHandler->getOwner()->getVolume() );

    if( ! ut::isPositive( newRate ) ) //negative rates and very small ones are set to zero
        newRate = 0.0;

  //expose param markers
    exposeParams( false );
}

bool OriVReport::updateRate() 
/* called frowm Gillespie algorithm and when checking for changes
if custom, just returning the qstate of the Function is cheap  */
{
	if( ownerHandler->getOwner()->checkChange( model ) ) //only update if in changes
	{
		ownerHandler->getOwner()->removeChange( model );
    	calculateRate();

	    if( ut::relativeChange( rate, newRate, ownerHandler->getCollection()->getSensitivity() ) || ut::isPositive( rate ) != ut::isPositive( newRate ) ) //if enough change (sensitivity)
	    	return true; //tells the caller that the Gillespie event needs resampling due to a change in rates
	}
    return false;
}

bool OriVReport::updateAsSelected()
/* response to replication events created by self */
{
  //add event marker
	if( model->getRepliEventMarker() )
		ownerHandler->getOwner()->addFakeEvent( model->getRepliEventMarker() );
  //select plasmid
    const auto& plasmids = model->getBackwardContent().elemsByType[ GeneticElement::ElemTypeIdx::PLASMID ];
    ut::DiscreteDist::RealParamsType plasmidParams;
    plasmidParams.reserve( plasmids.size() );
    for( size_t p = 0; p < plasmids.size(); p++ )
    	plasmidParams.push_back( ownerHandler->getOwner()->checkQState( plasmids[p] ) * plasmids[p]->getContent().vals[ model->getAbsoluteId() ] ); //the wight of each plasmid is its copy number multiplied by the multiplicity of the OriV 

    size_t selectedPlasmidIdx = model->samplePlasmid( plasmidParams );
  //update the selected plasmid ( select whether mutation and create replication event )
    return qplasmidReports[ selectedPlasmidIdx ]->updateAsSelected( this ); //returns true if repli mutation
}





//======================================================================================================================================================================Partition

//--------------------------------------common
void PartitionSystemReport::link()
{
    qplasmidReports.clear();
    for( const auto& qp : model->getBackwardContent().elemsByType[ GeneticElement::ElemTypeIdx::PLASMID ] )
        qplasmidReports.push_back( const_cast<QPlasmidReport*>( &ownerHandler->getQPlasmidReports()[ qp->getRelativeId() ] ) );
}

bool PartitionSystemReport::update( const EventType* )
/* nothing to do, just remove the chage */
{ 
    ownerHandler->getOwner()->removeChange( model ); 
    return true;
}

bool PartitionSystemReport::divisionOld( PartitionSystemReport* daughter )
/* divide the copies of the affected QPlasmids and the potential eclipse times
similar to qPlasmidIndependentDivisionOld(), but whit several plasmids that share the PartitionSystem at the same time */
{
    //nothing to do if the Partition (and its QPlasmids) are not present
    if( ownerHandler->getOwner()->checkBState( model ) || ownerHandler->getOwner()->checkBState( model->getEclipseMarker() )  )
    {
    	exposeParams( false );
      //perfect partition if active
        if( ownerHandler->getOwner()->checkBState( model->getGate() ) && qplasmidReports.size() > 1 )
        {
            uint totalEclipse = 0; //number of eclipse times of all the QPlasmids together
            size_t currentSize = 0; //key used to crete the idxs to shuffle
            std::vector<size_t> idxs;
            idxs.reserve( ownerHandler->getOwner()->checkQState( model ) + ownerHandler->getOwner()->checkQState( model->getEclipseMarker() ) ); //the total size
            
            std::vector<EclipseTimesDS> eclipseTimesBaks; //bak of the eclipse times of all the QPlasmids to get them back from here after clearing
            eclipseTimesBaks.reserve( qplasmidReports.size() );

            for( size_t q = 0; q < qplasmidReports.size(); q++ )
            {
                QPlasmidReport* qpR = qplasmidReports[q];
                qpR->removeEclipseEvents();
              //add keys for the eclipse times and count
                for( size_t et = 0; et < qpR->eclipseTimes.size(); et++ )
                    idxs.push_back( currentSize + et );
                currentSize += qpR->eclipseTimes.size();
                totalEclipse += qpR->eclipseTimes.size();
              //bak and clear
                eclipseTimesBaks.push_back( qpR->eclipseTimes );
                qpR->eclipseTimes.clear();
                daughter->qplasmidReports[q]->eclipseTimes.clear(); //inefficient
              //add keys for the active copies
                for( size_t q = 0; q < ownerHandler->getOwner()->checkQState( qpR->model ); q++ )
                    idxs.push_back( currentSize );
                currentSize += 1;
            }

          //count the total number of active copies (qstate) and the ongoing eclipse times
            uint totalQ = ut::round<uint>( ownerHandler->getOwner()->checkQState( model ) );
            uint total = totalQ + totalEclipse;
          
          //sample the exact half (potentially noisy and scaled by the difference in volume between the tho cells)
            uint half = model->sampleExact( total, ownerHandler->getOwner()->getGrowthHandler().getLastDivisionFraction(), ownerHandler->getOwner()->checkQState( model->getFractionFunction() ), ownerHandler->getOwner()->checkQState( model->getFractionRandomness() ) );
          
          //shuffle all the keys to split them randomly
            std::shuffle( idxs.begin(), idxs.end(), model->getShuffleRndEngine() );
          
          //apply changes
            apply( idxs, eclipseTimesBaks, this, 0, half );
            apply( idxs, eclipseTimesBaks, daughter, half, total );

            return true;
        }
      //independent random split of each QPlasmid if inactive
        else
        {
        	ownerHandler->getOwner()->justSet( model->getEclipseMarker(), 0 ); //then every plasmid adds its eclipses in qPlasmidIndependentDivisionOld()
            for( size_t p = 0; p < qplasmidReports.size(); p++ )
                qplasmidReports[p]->qPlasmidIndependentDivisionOld( daughter->qplasmidReports[p] );
            return false;
        }
    }
    return false;
}

void PartitionSystemReport::exposeParams( bool bInit )
{
	if( model->getFractionMarker() && ownerHandler->getOwner()->checkBState( model->getGate() ) && ( model->getFractionRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getFractionMarker(), model->sampleExactFraction( ownerHandler->getOwner()->checkQState( model->getFractionRandomness() ) ) );
	else if( model->getPMarker() && ! ownerHandler->getOwner()->checkBState( model->getGate() ) && ( model->getPRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getPMarker(), model->sampleP( ownerHandler->getOwner()->checkQState( model->getPRandomness() ) ) );
}

//--------------------------------------special

void PartitionSystemReport::exposeEclipseMarkers()
{
	if( eclipseMarkers != ut::round<uint>( ownerHandler->getOwner()->checkQState( model->getEclipseMarker() ) ) )
	{
		ownerHandler->getOwner()->setAndForward( model->getEclipseMarker(), eclipseMarkers );
	}
	eclipseMarkers = 0;
}


//------------------------------------------private

void PartitionSystemReport::apply( const std::vector<size_t>& idxs, const std::vector<EclipseTimesDS>& eclipseTimesBaks, PartitionSystemReport* daughter, size_t start, size_t end )
/* called from divisionOld() to commit the changes in the plasmids
the keys are translated into changes in the qstates and eclipse times 
  idx: the shuffled keys
  daughter : determines which cell to modify: daughter or self
  start-end: which subset of the keys are assigned to the cell */ 
{
	uint totalQ = 0;
	std::vector<size_t> qStates( eclipseTimesBaks.size(), 0 ); //counts the qstate of each plasmid

    for( size_t i = start; i < end; i++ )
    {
        size_t idx = idxs[i];
        for( size_t p = 0; p < eclipseTimesBaks.size(); p++ ) //iterate the plasmdids
        {
            if( idx < eclipseTimesBaks[p].size() ) //eclipse time
            {
                daughter->qplasmidReports[p]->eclipseTimes.add( eclipseTimesBaks[p][ idx ] );
                break;
            }
            else if( idx == eclipseTimesBaks[p].size() ) //qstate
            {
                qStates[p]++;
                totalQ++;
                break;
            }
            idx -= ( eclipseTimesBaks[p].size() + 1 ); //move to next plasmid
        }
    }
  //apply (content, changes and logic refresh)
	for( size_t p = 0; p < qplasmidReports.size(); p++ )
	{
		const PlasmidQuantitative* plasmid = static_cast< const PlasmidQuantitative* >( qplasmidReports[p]->model );

        daughter->ownerHandler->getOwner()->setAndForwardContent( qplasmidReports[p]->model, qStates[p] );
    	daughter->qplasmidReports[p]->createEclipseEvents();
    	daughter->ownerHandler->storeEclipseMarkers( plasmid, daughter->qplasmidReports[p]->eclipseTimes.size() );
	} 
}







//======================================================================================================================================================================Mutations

//--------------------------------------------------------------common
bool MutationProcessReport::update( const EventType* event )
/* responds only to events created by self */
{
    if( event && event->emitter == model )
    {
    	updateAsSelected();
    	return true;
    }
    return false;
}

void MutationProcessReport::exposeParams( bool bInit )
{
	if( model->getRateMarker() && ( model->getRateRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getRateMarker(), model->sampleRate( ownerHandler->getOwner()->checkQState( model->getRateRandomness() ) ) );
}

//--------------------------------------private

void MutationProcessReport::calculateRate()
/* values stored in newRate until it is accessed */
{
	if( model->getType() == MutationProcess::Type::SPONTANEOUS )
	{
		newRate = model->calculateRate( ownerHandler->getOwner()->getBState(), ownerHandler->getOwner()->getQState() );
    	if( ! ut::isPositive( newRate ) ) //negative and very little rates are set to zero
        	newRate = 0.0;

        exposeParams( false );
	}
}

bool MutationProcessReport::updateRate() 
/* called frowm Gillespie algorithm and when checking for changes
just returning the qstate of the Function is cheap  */
{
	ownerHandler->getOwner()->removeChange( model );
	if( model->getType() == MutationProcess::Type::SPONTANEOUS && ownerHandler->getOwner()->checkChange( model ) )
	{
    	calculateRate();

	    if( ut::relativeChange( rate, newRate, ownerHandler->getCollection()->getSensitivity() ) || ut::isPositive( rate ) != ut::isPositive( newRate ) )
	    	return true; //tells the caller that the Gillespie event needs resampling due to a change in rates
	}
    return false;
}

void MutationProcessReport::updateAsSelected()
/* selects the specific mutation to apply if more than one and creates/removes the plasmids and/or creates the eclipse times:
  a) a single mutation event if no eclipse
  b) independent events for each affected plasmid if eclipse */
{
  //event marker
	if( model->getEventMarker() )
		ownerHandler->getOwner()->addFakeEvent( model->getEventMarker() );
  //select mutation and apply
    const Mutation* mutation = model->sampleMutation();
    if( mutation )
    	ownerHandler->applyMutation( mutation, model->getHasEclipse() ); 
}









//////////////////////////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------common
bool PlasmidsHandler::init()
{ 
	GeneticHandlerType::init(); 
	return collection->getBGillespie() ? selectNextEvent( true ) : false; //true because this is the first event
}

bool PlasmidsHandler::secondInit( ut::MultiBitset*, ut::Bitset* )
{
	forawardCopyControls(); //potential changes in the CopyControl's gates
    //not updating reports as that is only needed when there is an event

  //select next event. The only posibility is that there are changes in the rates as there are no previous events or cell division
	if( checkChanges() ) 
	{
		selectNextEvent( false ); //false because this is only neccesary if the changes in rates are big enough (sensitivity)
		return true;
	}
	return false;
}

bool PlasmidsHandler::divisionOld( HandlerBaseType* daughter )
{
    GeneticHandlerType::divisionOld( daughter ); //divisionOld reports
    exposeEclipseMarkers();
    forawardCopyControls(); //potential changes in the CopyControl's gates or copy number after the partition of plasmids
    
    if( collection->getBGillespie() && checkChanges() ) 
		selectNextEvent( false ); //not neccessary if no change in rates because this daughter cell has the old event
    return true;
}

bool PlasmidsHandler::divisionNew( HandlerBaseType* mother )
{
    GeneticHandlerType::divisionNew( mother ); //divisionOld reports
    exposeEclipseMarkers();
	forawardCopyControls(); //potential changes in the CopyControl's gates or copy number after the partition of plasmids
	if( collection->getBGillespie() )
    	selectNextEvent( true ); // big changes expected (number of plasmids and orivs, volume ). Neccessary because the event is not copied
    return true;
}

bool PlasmidsHandler::update( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask, const EventType* event )
{
	forawardCopyControls();
  //update the reports. OriVReport or MutationProcessReport handle the event in case it is a Guillespie event
	//if( event ) //no, to allow for qplasmids with eclipse that are added by a source (OriV, OriT or MutationProcess) without eclipse to remove themselves from changes
	GeneticHandlerType::update( mask, typeSummaryMask, event ); //changes are not removed here but when updating the rates

  //select next event. The first event selection is called from init(), not here
	//case 1: as response to the previous event that reached its time, necessary (true).
	if( event && checkGillespieEvent( event ) && owner->getTime().time == nextEventTime ) //if previous Gillespie event time reached, time is unnecessary extra check
	{
		selectNextEvent( true ); 
		return true;
	}
	//case 2: change in the conditions (before the previous event is reached or when there are no events), recalculated only if the change is big enough (sensitivity)
	if( checkChanges() ) // if OriV or MutationProcess in Genome changes
	{
		selectNextEvent( false );
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------special

void PlasmidsHandler::applyMutation( const Mutation* mutation, bool bCallerHasEclipse )
/* make and forward the changes stated by a Mutation. Called from MutationProcessReport and from OriT on conjugation with mutation */
{
  //event marker
	if( mutation->getEventMarker() )
		owner->addFakeEvent( mutation->getEventMarker() );

  //changes
	//no eclipses, the simplest case
	if( ! bCallerHasEclipse ) //direct event with the Mutation as elem
    {
    	//owner->addAndForwardContent( mutation, 1 ); //the Mutation element is added to the qstate too (acts as a counter). qplasmids added here as part of content
    	for( size_t p = 0; p < mutation->getPlasmids().size(); p++ )
    		owner->addAndForwardContent( mutation->getPlasmids()[p], mutation->getAmounts()[p] ); //bplasmids have to be added one by one to ensure that the function for digital is called and their number is maintained in {0,1}
    }
    //different handling of each Plasmid as some may have eclipse while others don't 
    else 
    {
      //qplasmids, may have eclipse (first, in case custom eclipse function)
        for( size_t qp = 0; qp < mutation->getQPlasmids().size(); qp++ )
        {
            //creation of a plasmid with eclipse
            if( mutation->getQAmounts()[qp] > 0 && mutation->getQPlasmids()[qp]->getHasEclipse() )
            	getQPlasmidReportsEdit()[ mutation->getQPlasmids()[qp]->getRelativeId() ].addEclipseTimes( mutation->getQAmounts()[qp] );
        }

    	//owner->addAndForward( mutation, 1 ); //mutation counter
    	//bplasmids, no eclipse
    	for( size_t bp = 0; bp < mutation->getBPlasmids().size(); bp++ )
    		owner->addAndForwardContent( mutation->getBPlasmids()[bp], mutation->getBAmounts()[bp] );

        //qplasmids, may have eclipse
        for( size_t qp = 0; qp < mutation->getQPlasmids().size(); qp++ )
        {
        	//either removal or creation without eclipse
            if( mutation->getQAmounts()[qp] < 0 || ! mutation->getQPlasmids()[qp]->getHasEclipse() )
            	owner->addAndForwardContent( mutation->getQPlasmids()[qp], mutation->getQAmounts()[qp] );
        }
    }
}


//-----------------------------------------------------------------------private

bool PlasmidsHandler::selectNextEvent( bool bRequired )
/* If neccesary, removes the current Gillespie event if it exists and finds a new one
called when there are changes in the conditions (OriV, CopyControl, MUtationProcess) */
{
  //check if the update is neccesary (bRequired or changes in rate big enough)
	bool bControl = true;
	if( ! bRequired )
		bControl = checkRateChange();

	if( bControl )
	{
	  //remove current event
		if( nextEventTime > owner->getTime().time ) //NULL_EVENT_TIME is negative i.e. never greater
			removeCurrentEvent();

	  //find new event
	    if( findNextEventGillespie( ! bRequired ) ) //if ! bRequired, the rates are already updated by checkRateChange(); if bRequired, they are not
	    {
	    	if( selectedMutationProcessReport ) //MutationProcess selected, either spontaneous or repli
	        {
	            owner->addEvent( Genome::EventType::INTERNAL, selectedMutationProcessReport->model, selectedMutationProcessReport->model, nextEventTime, 0 ); 
	            return true;
	        }
	        else if( selectedOriVReport ) //OriV replication
	        {
	        	owner->addEvent( Genome::EventType::INTERNAL, selectedOriVReport->model, selectedOriVReport->model, nextEventTime, 0 ); 
	            return true;
	        }
		}
	}
    return false;
}

bool PlasmidsHandler::checkRateChange()
/* check whether the change in the rates is enough to justify re-sampling the event, according to sensitivity
if no used, the event can be potentially re-sampled every time step due to the change in volume (growth) as the built-in OriV rate uses concentration and custom Functions may too */
{
	bool bChange = false; 
	for( auto& vReport : getOriVReportsEdit() )
        bChange |= vReport.updateRate();
    for( auto& mReport : getMutationProcessReportsEdit() )
        bChange |= mReport.updateRate();
    return bChange;
}

bool PlasmidsHandler::findNextEventGillespie( bool bAlreadyUpdated )
/* Gillespie algorithm to find the next event and its type
it can be either a replication from a OriV (with or without repli mutation) or a MutationProcess */
{
    TReal totalRate = 0.0;
    ut::DiscreteDist::RealParamsType selectParams; //rates of all the OriVs and MutationProcesses
    selectParams.reserve( getOriVReports().size() + getMutationProcessReports().size() );

  //update (if not already done) and store the rates of the OriV and MutationProcess elements
    for( auto& vReport : getOriVReportsEdit() ) //OriVs
    {
    	if( ! bAlreadyUpdated )
    		vReport.updateRate();
        TReal rate = vReport.accessRate(); //commited when accessed
        selectParams.push_back( rate );
        totalRate += rate;
    }
    for( auto& mReport : getMutationProcessReportsEdit() ) //MutationProcesses
    {
    	if( ! bAlreadyUpdated )
    		mReport.calculateRate();
        TReal rate = mReport.accessRate(); //commited when accessed
        selectParams.push_back( rate );
        totalRate += rate;
    }

  //sample the time of the next event
    if( ! ut::isPositive( totalRate ) )
    {
    	nextEventTime = NULL_EVENT_TIME;
    	return false;
    }
    nextEventTime = owner->getTime().time + ut::Time::applyEpsilon( collection->sampleGillespieTimeDist( totalRate ) );

  //sample the identity (the emitter element) of the event
    partialReset(); //sets both ptrs to nullptr
    size_t eventIdx = collection->sampleGillespieSelectDist( selectParams );

    if( eventIdx < getOriVReports().size() )
        selectedOriVReport = &getOriVReportsEdit()[eventIdx];
    else
        selectedMutationProcessReport = &getMutationProcessReportsEdit()[ eventIdx - getOriVReports().size() ];
    
    return true;
}

bool PlasmidsHandler::removeCurrentEvent()
{
	if( selectedMutationProcessReport ) //mutation event
	{
		owner->removeEvent( Genome::EventType::INTERNAL, selectedMutationProcessReport->model, selectedMutationProcessReport->model, nextEventTime, 0 );
		return true;
	}
    if( selectedOriVReport ) //replication event
    {
        owner->removeEvent( Genome::EventType::INTERNAL, selectedOriVReport->model, selectedOriVReport->model, nextEventTime, 0 );
        return true;
    }
    return false;
}


//---
bool PlasmidsHandler::checkChanges()
{
    //there is no need to check the QPlasmid. The concrete plasmids are not selected until the event time is reached
    //there is no need to check the CopyControl as they are forwarded to OriVs at update() before calling this method
    return owner->checkTypeChanges( ElemTypeIdx::ORI_V ) || owner->checkTypeChanges( ElemTypeIdx::MUTATION_PROCESS );
}

void PlasmidsHandler::forawardCopyControls()
/* forward the CopyControl changes to their OriVs (canonical rate calculation) and updates active marker*/
{
	//iterate changes in CopyControls
	for( size_t cc = owner->getChanges().findFirstRel( ElemTypeIdx::COPY_CONTROL ); cc < owner->getChanges().npos(); cc = owner->getChanges().findNextRel( ElemTypeIdx::COPY_CONTROL, cc + 1 ) )
	{
	  //remove its change and add the affected OriVs instead
		const CopyControl* copyControl = owner->getCollection()->getById<CopyControl>( cc );
		owner->removeChange( copyControl );
		owner->addChangesFrom( copyControl, GeneticCollection::ElemTypeIdx::ORI_V );

	  //update active marker if it exists
		if( copyControl->getHasActiveMarker() )
		{
			if( owner->checkBState( copyControl->getGate() ) && owner->checkQState( copyControl->getActiveMarker() ) != owner->checkQState( copyControl ) ) //on
				owner->setAndForward( copyControl->getActiveMarker(), owner->checkQState( copyControl ) );
			else if( ! owner->checkBState( copyControl->getGate() ) && owner->checkBState( copyControl->getActiveMarker() ) ) //off
				owner->setAndForward( copyControl->getActiveMarker(), 0.0 );
		}
	}
}

//---
void PlasmidsHandler::storeEclipseMarkers( const PlasmidQuantitative* plasmid, int amount )
{
  //plasmid marker (directly set)
	if( plasmid->getHasEclipseMarker() )
		owner->setAndForward( plasmid->getEclipseMarker(), amount );
  //partition marker
	getPartitionSystemReportsEdit()[ plasmid->getPartitionSystem()->getRelativeId() ].addEclipseMarkers( amount );
  //orivs
	const std::vector<const OriV*>& orivs = plasmid->getOriVs();
    for( size_t ov = 0; ov < orivs.size(); ov++ )
    {
    	if( orivs[ov]->getEclipseMarker() )
    		getOriVReportsEdit()[ orivs[ov]->getRelativeId() ].addEclipseMarkers( amount );
    }
}

void PlasmidsHandler::exposeEclipseMarkers()
{
	for( OriVReport& orivR : getOriVReportsEdit() )
		orivR.exposeEclipseMarkers();
	
	for( PartitionSystemReport& partR : getPartitionSystemReportsEdit() )
		partR.exposeEclipseMarkers();
}