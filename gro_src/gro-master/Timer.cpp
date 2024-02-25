#include "Timer.h"
#include "ut/SeedGenerator.hpp" //nextSeed()

#include "PopulationGate.h" //gate, const cast in passUsedForTimer()
#include "GroCollection.h" //ctor, getSeedGenerator()

////////////////////////////////////////////////////////// NESTED TIME ADVANCERS /////////////////////////////////////////////////////////////////

//========================================================================================= NextTimeRnd
Timer::NextTimeRnd::NextTimeRnd( GroCollection* groCollection, TReal period, TReal periodMin )
: NextTimeBase(), periodMin( periodMin )
, eventsDist( { ut::isPositive( period ) ? 1.0 / period : 0.0 }, groCollection->getSeedGenerator()->nextSeed() ) {;}



////////////////////////////////////////////////////////// TIMER /////////////////////////////////////////////////////////////////

Timer::Timer( GroCollection* groCollection, size_t id, const std::string& name
, const PopulationGate* gate, const ut::Time& time, Mode mode, TReal start, TReal end, TReal period, bool bRnd, TReal periodMin, bool bIgnoreStart )

: GroElement( groCollection, id, name, GroElement::ElemTypeIdx::TIMER )
, gate( gate), mode( mode )
, start( time.time2step( ut::fitL( start ) ) ), end( time.time2step(  ut::fitL( end ) ) )
, bIgnoreStart( bIgnoreStart )
{
	if( bRnd )
		timeAdvancer = std::make_shared<NextTimeRnd>( groCollection, ut::fitL( period ), ut::fitL( periodMin ) );
	else
		timeAdvancer = std::make_shared<NextTimeDeterministic>( time.time2step( ut::fitL( period ) ) );
}

//-----------API-precompute

void Timer::loadDirectElemLink()
{ 
	this->addLink( gate, LinkDirection::BACKWARD ); 
}

void Timer::passUsedForTimers()
{ 
	if( gate ) const_cast<PopulationGate*>( gate )->passUsedForTimers(); 
} 