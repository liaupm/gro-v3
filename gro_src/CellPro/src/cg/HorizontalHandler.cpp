#include "cg/HorizontalHandler.hpp"
#include "ut/Time.hpp" //apply epsilon to event times
#include "cg/Genome.hpp" //owner
#include "cg/Pilus.hpp" //model
#include "cg/OriT.hpp" //model
#include "cg/HorizontalCommon.hpp" //getShuffleRndEngine()

/*PRECOMPILED
#include <algorithm> //all_of */

using namespace cg;


/////////////////////////////////////////////////////////////////////////////////// REPORTS ////////////////////////////////////////////////////////////////////////////////////////////////

//======================================================================================================================================================================OriT

//----------------------------------------------common
bool OriTReport::init()
{
	exposeParams( true );
    return bActive = ownerHandler->getOwner()->checkBState( model ) && ownerHandler->getOwner()->checkBState( model->getDonorGate() );
}

bool OriTReport::update( const EventType* )
{
    ownerHandler->getOwner()->removeChange( model );
    
    bool bActiveOld = bActive;
    bActive = ownerHandler->getOwner()->checkBState( model ) && ownerHandler->getOwner()->checkBState( model->getDonorGate() );
    if( bActive != bActiveOld ) 
    {
        ownerHandler->getOwner()->addChangesFrom( model, GeneticCollection::ElemTypeIdx::PILUS );
        //no logic refresh as the activation state of the OriT is private. 
        //The logic that relays on the copy number is refreshed when the copy nmber change is made
        return true;
    }
    return false;
}

void OriTReport::exposeParams( bool bInit )
{
	if( model->getRateWMarker() && ( model->getRateWRandomness() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getRateWMarker(), model->sampleRateW( ownerHandler->getOwner()->checkQState( model->getRateWRandomness() ) ) );
}


//----------------------------------------- special
const PlasmidBase* OriTReport::selectPlasmid()
{
    const auto& plasmids = model->getBackwardContent().elemsByType[ GeneticElement::ElemTypeIdx::PLASMID ];
    ut::DiscreteDist::ParamsType params;
    params.reserve( plasmids.size() );
    
    if( model->getIsCopyDependent() )
    {
        for( const auto& pla : plasmids )
            params.push_back( ownerHandler->getOwner()->checkQState( pla ) * pla->getContent().vals[ model->getAbsoluteId() ] );
    }
    else
    {
        for( const auto& pla : plasmids )
            params.push_back( ownerHandler->getOwner()->checkBState( pla ) ? pla->getContent().vals[ model->getAbsoluteId() ] : 0.0 );
    }
    return static_cast<const PlasmidBase*>( plasmids[ model->samplePlasmid( params ) ] );
}



//======================================================================================================================================================================Pilus

//-------------------------------common
void PilusReport::link()
{ 
    oriTReports.clear();
    for( const auto& ot : model->getBackwardLink().elemsByType[ GeneticElement::ElemTypeIdx::ORI_T ] )
        oriTReports.push_back( const_cast<OriTReport*>( &ownerHandler->getOriTReports()[ ot->getRelativeId() ] ) );
}

bool PilusReport::init()
{
	exposeParams( true );
    if( checkState() )
    {
        ownerHandler->getOwner()->setAndForward( model, true ); //the state of the Pilus may be an input
        return true;
    }
    return false;
}

bool PilusReport::update( const EventType* )
{
    ownerHandler->getOwner()->removeChange( model );
    bool bActiveNew = checkState();
    if( bActiveNew != ownerHandler->getOwner()->checkBState( model ) )
    {
        ownerHandler->getOwner()->setAndForward( model, bActiveNew ); //the state of the Pilus may be an input
        return true;
    }
    return false; 
}

void PilusReport::exposeParams( bool bInit )
{
	if( model->getMaxRateMarker() && ( model->getMaxRateRnd() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getMaxRateMarker(), model->sampleMaxRate( ownerHandler->getOwner()->checkQState( model->getMaxRateRnd() ) ) );

	if( model->getNeighboursWMarker() && ( model->getNeighboursWRnd() || bInit ) )
		ownerHandler->getOwner()->setAndForward( model->getNeighboursWMarker(), model->sampleNeighboursW( ownerHandler->getOwner()->checkQState( model->getNeighboursWRnd() ) ) );
}


//-------------------------------special

bool PilusReport::updateExternal( const std::vector<Genome*>& neighbours, const std::vector<TReal>& distances )
{
    if( ownerHandler->getOwner()->checkBState( model ) )
    {
        TReal donorRate = calculateDonorRate();
        std::vector<size_t> validNeighbourIdxs = filterNeighbours( neighbours );
        exposeParams( false );
        
      //if custom recipient weights
        if( model->getRecipientFunction() )
        {
            ut::DiscreteDist::ParamsType params = calculateRecipientWeights( neighbours, validNeighbourIdxs, distances );
            TReal conjugationProb = model->calculateConjugationProb( donorRate * ownerHandler->getOwner()->getTime().stepSize, params, ownerHandler->getOwner()->checkQState( model->getNeighboursWRnd() ) );

            if( model->getRateMarker() )
            	ownerHandler->getOwner()->setAndForward( model->getRateMarker(), conjugationProb );
            if( model->sampleConjugationOrNot( conjugationProb ) )
            {
                size_t neighIdx = model->sampleNeighbour( params );
                transmitPlasmid( neighbours[ validNeighbourIdxs[ neighIdx ] ] );
                return true;
            }
        }
      //if homogeneous recipients
        else 
        {
       		TReal conjugationProb = model->calculateConjugationProb( donorRate * ownerHandler->getOwner()->getTime().stepSize, validNeighbourIdxs.size(), ownerHandler->getOwner()->checkQState( model->getNeighboursWRnd() ) );
            
            if( model->getRateMarker() )
            	ownerHandler->getOwner()->setAndForward( model->getRateMarker(), conjugationProb );

            if( model->sampleConjugationOrNot( conjugationProb ) )
            {
                size_t neighIdx = model->sampleNeighbour( validNeighbourIdxs.size() - 1 );
                transmitPlasmid( neighbours[ validNeighbourIdxs[ neighIdx ] ] );
                return true;
           }
        }
    }
    return false;
}


//-----------------------------------private

bool PilusReport::checkState() const
{
    if( ownerHandler->getOwner()->checkBState( model->getDonorGate() ) )
    {
        for( const auto& otR : oriTReports )
        {
            if( otR->getBActive() )
                return true;
        }
    }
    return false;
}

TReal PilusReport::calculateDonorRate()
{
    TReal donorRate;
    if( model->getDonorFunction() )
        donorRate = ut::fitL( ownerHandler->getOwner()->checkQState( model->getDonorFunction() ) ); //always positive
    else
		donorRate = model->sampleMaxRate( ownerHandler->getOwner()->checkQState( model->getMaxRateRnd() ) ); //sign already restricted
	
    if( model->getBAutoVolScale() )
        donorRate *= ownerHandler->getOwner()->getVolume();
    return donorRate;
}

std::vector<size_t> PilusReport::filterNeighbours( const std::vector<Genome*>& neighbours )
{
    std::vector<size_t> validNeighbourIdxs;
    for( size_t n = 0; n < neighbours.size(); n++ )
    {
        if( neighbours[n]->checkBState( model->getRecipientGate() ) )
            validNeighbourIdxs.push_back( n );
    }
    return validNeighbourIdxs;
}

ut::DiscreteDist::ParamsType PilusReport::calculateRecipientWeights( const std::vector<Genome*>& neighbours, const std::vector<size_t>& validNeighbourIdxs, const std::vector<TReal>& distances )
{
    ut::DiscreteDist::ParamsType params;
    params.reserve( validNeighbourIdxs.size() );

    for( size_t idx : validNeighbourIdxs )
    {
        if( model->getBDistanceCriterium() )
            neighbours[ idx ]->setQState( ownerHandler->getOwner()->getCollection()->getNeighDistanceMarker(), distances[ idx ] ); 
        neighbours[ idx ]->getHandlerEdit<OwnerType::HandlerIdx::H_LOGIC>().deepCheck( model->getRecipientFunction() );
        params.push_back( neighbours[ idx ]->checkQState( model->getRecipientFunction() ) );
    }
    return params;
}


void PilusReport::transmitPlasmid( Genome* recipient )
//select plasmid and transmit plasmid to recipient
{
  //select oriT and plasmid
    OriTReport* selectedOriT = selectOriT();

  //entry exclusion, the conjugation atempt is lost
    if( ! recipient->checkBState( selectedOriT->getModel()->getRecipientGate() ) )
    	return;
  //select specific plasmid
    const PlasmidBase* selectedPlasmid = selectedOriT->selectPlasmid();

  //select whether repli mutation
    ut::DiscreteDist::ParamsType params;
    const auto& keys = selectedOriT->model->getMutationKeys();
    const MutationProcess* mutation = selectedPlasmid->selectMutation( keys, ownerHandler->getOwner()->getBState(), ownerHandler->getOwner()->getQState(), params );

  //expose mutation param markers
    const std::vector<const MutationProcess*>& repliMutations = selectedPlasmid->getRepliMutations();
    for( size_t m = 0; m < repliMutations.size(); m++ )
	{
	    if( repliMutations[m]->getRateMarker() && std::count( keys.begin(), keys.end(), selectedPlasmid->getRepliMutationKeys()[m] ) ) //if in emitter keys
	    {
	    	ownerHandler->getOwner()->setAndForward( repliMutations[m]->getRateMarker(), params[m] );
	    }
	}

  //add the conjugation proposal to the recipient, not commited yet
    recipient->getHorizontalHandlerEdit().addConjugation( ownerHandler->getOwner(), model, selectedOriT->model, selectedPlasmid, mutation );
}

OriTReport* PilusReport::selectOriT()
{
    const auto& oriTs = model->getBackwardLink().elemsByType[ GeneticElement::ElemTypeIdx::ORI_T ];
    ut::DiscreteDist::ParamsType params;
    params.reserve( oriTs.size() );
    
    if( model->getIsCopyDependent() )
    {
        for( size_t t = 0; t < oriTs.size(); t++ )
        {
        	const OriT* oriT = static_cast< const OriT*>( oriTs[t] );
            params.push_back(  oriTReports[ t ]->getIsActive() ? ownerHandler->getOwner()->checkQState( oriTs[t] ) 
            * oriT->sampleRateW( ownerHandler->getOwner()->checkQState( oriT->getRateWFunction() ), ownerHandler->getOwner()->checkQState( oriT->getRateWRandomness() ) ) : 0.0 );

            ownerHandler->getOriTReportsEdit()[ oriT->getRelativeId() ].exposeParams( false );
        }
    }
    else
    {
        for( size_t t = 0; t < oriTs.size(); t++ )
        {
        	const OriT* oriT = static_cast< const OriT*>( oriTs[t] );
            params.push_back( oriTReports[ t ]->getIsActive() ? oriT->sampleRateW( ownerHandler->getOwner()->checkQState( oriT->getRateWFunction() ), ownerHandler->getOwner()->checkQState( oriT->getRateWRandomness() ) ) : 0.0 );

            ownerHandler->getOriTReportsEdit()[ oriT->getRelativeId() ].exposeParams( false );
        }
    }
    return oriTReports[ model->sampleOriT( params ) ];
}






/////////////////////////////////////////////////////////////////////////////////// HANDLER ////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------- special

bool HorizontalHandler::checkActivePili() const
{
    for( const auto& pilusReport : getPilusReports() )
    {
        if( owner->checkBState( pilusReport.model ) )
            return true;
    }
    return false;
}

bool HorizontalHandler::checkActivePiliDistance() const
{
    for( const auto& pilusReport : getPilusReports() )
    {
        if( owner->checkBState( pilusReport.model ) && pilusReport.model->getBDistanceCriterium() )
            return true;
    }
    return false;
}


void HorizontalHandler::applyConjugation( const ConjugationData& conj )
{
  //recipient
	if( conj.mutationProcess && conj.mutationProcess->getMutations().size() >= 1 && conj.mutationProcess->getMutations()[0] )
		owner->getReplicationHandlerEdit().applyMutation( conj.mutationProcess->getMutations()[0], conj.mutationProcess->getHasEclipse() );
	else
		owner->getReplicationHandlerEdit().applyPlasmid( conj.plasmid, conj.oriT->getHasEclipse(), true );
  //donor
	if( conj.mutationProcess && conj.mutationProcess->getMutations().size() >= 2 && conj.mutationProcess->getMutations()[1] )
		conj.donor->getReplicationHandlerEdit().applyMutation( conj.mutationProcess->getMutations()[1], conj.mutationProcess->getHasEclipse() );
	else
		conj.donor->getReplicationHandlerEdit().applyPlasmid( conj.plasmid, conj.oriT->getHasEclipse(), false );

  //recipient (this) markers
    if( conj.oriT->getRecipientEventMarker() )
        owner->addAndForward( conj.oriT->getRecipientEventMarker(), true ); 

  //donor markers
	if( conj.oriT->getDonorEventMarker() )
	{
		conj.donor->addAndForward( conj.oriT->getDonorEventMarker(), true ); 
		conj.donor->updateCycle( nullptr );
	}
}

void HorizontalHandler::selectAndApplyConjugations()
{
	if( pendingConjugations.size() == 0 )
		return;
	if( pendingConjugations.size() == 1 )
		applyConjugation( pendingConjugations[0] );
	else
	{
		std::shuffle( pendingConjugations.begin(), pendingConjugations.end(), owner->getCollection()->getById<HorizontalCommon>( 0 )->getShuffleRndEngine() );

		applyConjugation( pendingConjugations[0] );

		for( size_t c = 1; c < pendingConjugations.size(); c++ )
		{
			if( owner->checkBState( pendingConjugations[c].pilus->getRecipientGate() ) && owner->checkBState( pendingConjugations[c].oriT->getRecipientGate() ) )
				applyConjugation( pendingConjugations[c] );
		}
	}
	pendingConjugations.clear();
}