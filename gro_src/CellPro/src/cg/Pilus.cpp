#include "cg/Pilus.hpp"
#include "ut/SeedGenerator.hpp" //nextSeed()
#include "cg/GeneticCollection.hpp" //getSeedGenerator()
#include "cg/Marker.hpp" //loadDirectElemLink()
#include "cg/Randomness.hpp" //Randomness::getScalerType()

using namespace cg;


Pilus::Pilus( const GeneticCollection* geneticCollection, size_t id, const std::string& name
, const GeneticElement* donorGate, const GeneticElement* recipientGate, const GeneticElement* recipientFunction, const QMarker* neighbourMarker
, const ParamDataType& maxRateData, const ParamDataType& neighboursWData, const QMarker* rateMarker, bool bAutoVolScale )

: GeneticElement( id, name, GeneticElement::ElementIndex::PILUS, false, geneticCollection, ElemCategory::FUNCTIONAL )
, donorGate( donorGate ), recipientGate( recipientGate ), donorFunction( maxRateData.fun ), recipientFunction( recipientFunction ), neighbourMarker( neighbourMarker ), rateMarker( rateMarker )
, maxRateRnd( maxRateData.rnd ), neighboursWRnd( neighboursWData.rnd ), maxRateMarker( maxRateData.marker ), neighboursWMarker( neighboursWData.marker )
, bAutoVolScale( bAutoVolScale ), bCopyDependent( DF_BCOPYDEPENDENT ), bDistanceCriterium( false )

, maxRateScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( maxRateData.rnd ), maxRateData.params ) ), maxRateParams( maxRateData.params )
, neighboursWScaler( ut::DistributionScalerFactory::create( Randomness::getScalerType( neighboursWData.rnd ), neighboursWData.params ) ), neighboursWParams( neighboursWData.params )
, conjugationOrNotDist( { 0.0, 1.0 }, geneticCollection->getSeedGenerator()->nextSeed() )
, neighbourSelectionUniDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() )
, neighbourSelectionDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() )
, oriTSelectionDist( {}, {}, geneticCollection->getSeedGenerator()->nextSeed() ) {;}

void Pilus::setAll( const GeneticElement* xDonorGate, const GeneticElement* xRecipientGate, const GeneticElement* xDonorFunction, const GeneticElement* xRecipientFunction, const GeneticElement* xMaxRateRnd, const GeneticElement* xNeighboursWRnd )
{
	setDonorGate( xDonorGate );
	setRecipientGate( xRecipientGate );
	setDonorFunction( xDonorFunction );
	setRecipientFunction( xRecipientFunction );
	setMaxRateRnd( xMaxRateRnd );	
	setNeighboursWRnd( xNeighboursWRnd );
}

void Pilus::setMaxRateRnd( const GeneticElement* xMaxRateRnd )
{
	maxRateRnd = xMaxRateRnd;
	maxRateScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( maxRateRnd ), maxRateParams );
}

void Pilus::setNeighboursWRnd( const GeneticElement* xNeighboursWRnd )
{
	neighboursWRnd = xNeighboursWRnd;
	neighboursWScaler = ut::DistributionScalerFactory::create( Randomness::getScalerType( neighboursWRnd ), neighboursWParams );
}


//---------------------------------------API-precompute

void Pilus::loadDirectElemLink()
{ 
	this->addLink( donorGate, LinkDirection::BACKWARD ); 
	this->addLink( recipientGate, LinkDirection::BACKWARD ); 
	this->addLink( donorFunction, LinkDirection::BACKWARD ); 
	this->addLink( recipientFunction, LinkDirection::BACKWARD ); 
	this->addLink( maxRateRnd, LinkDirection::BACKWARD ); 
}

void Pilus::precompute() 
{
	GeneticElement::precompute();
  //find of if the distance is used and it is necessary to calculate and pass it
	bDistanceCriterium = recipientFunction ? recipientFunction->findElemInLogic( static_cast<const GeneticCollection*>( collection )->getNeighDistanceMarker() ) : false;
	
  //bCopyDependent if all its OriTs are 
	const auto& oriTs = getBackwardLink().elemsByType[ ElemTypeIdx::ORI_T ];
	bCopyDependent = true;
	for( auto& t : oriTs )
	{
		if( ! static_cast<const OriT*>( t )->getIsCopyDependent() )
		{
			bCopyDependent = false;
			break;
		}
	}
}
