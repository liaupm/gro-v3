#ifndef UT_AGENT_HPP
#define UT_AGENT_HPP

#include "ut/defines.hpp"
#include "ut/ExtendedVector.hpp" //QType
#include "ut/ElementCounter.hpp" //QInitializerType
#include "ut/MultiBitset.hpp" //BType, BInitializerType
#include "ut/Bitset.hpp" //BTypeSimple
#include "ut/CollectionManager.hpp" //base of TCollection
#include "ut/Handler.hpp" //handlers
#include "ut/Time.hpp" //time
#include "ut/Routine.hpp" //handler routines
#include "ut/AgentState.hpp"

/*PRECOMPILED
#include <vector> //multiple get for states and changes
#include <tuple> //handlers
#include <utility> //std::move */


namespace ut
{
	template< typename TDerived, typename TScheduler, typename TCollection, typename THandlerIdx, typename... THandlers >
	class Agent
	{
		public:
			using DerivedType = TDerived;
			using SchedulerType = TScheduler;
			using EventType = typename SchedulerType::EventType;
			using CollectionType = TCollection;
			using HandlerIdx = THandlerIdx;
			using ElementBaseType = typename CollectionType::ElementBaseType;
			using ElemTypeIdx = typename CollectionType::ElemTypeIdx;

			using QInitializerType = ElementCounter< ElementBaseType >;
			using BInitializerType = MultiBitset;
			using BType = AgentState::BType;
			using QType = AgentState::QType;

			static constexpr size_t HANDLER_NUM = sizeof...( THandlers );


		//---ctor, dtor
			Agent( const CollectionType* collection, const Time& time ) 
			: collection(collection) 
            , handlers( ( std::move( THandlers( static_cast<DerivedType* const>(this), collection ) ) )... ), scheduler()
			, bState( collection->getMaskCommon() ), qState( collection->getTotalElementNum(), 0 )
			, changes( collection->getMaskCommon() ), changesSummary( ElemTypeIdx::COUNT )
			, time( time.stepSize, time.time ) 
            { populateHandlers(); linkHandlers(); }

			Agent( const Agent* mother )  //not a copy ctor
			: collection( mother->collection )
            , handlers( mother->handlers ), scheduler( mother->scheduler )
			, bState( mother->bState ), qState( mother->qState )
			, changes( collection->getMaskCommon() ), changesSummary( ElemTypeIdx::COUNT )
			, time( mother->time.stepSize, mother->time.time ) 
            { setOwnershipHandlers(); linkHandlers(); }
			

			virtual ~Agent() = default;
            Agent( const Agent& rhs ) = delete;
            Agent& operator=( const Agent& rhs ) = delete;
			/*Agent( const Agent& rhs ) = default;
			Agent( Agent&& rhs ) noexcept = default;
			Agent& operator=( Agent&& rhs ) noexcept = default;*/

		//---get
          //recources
			inline const CollectionType* getCollection() const { return collection; }
			
            template<THandlerIdx I>
			const auto& getHandler() const { return std::get<I>( handlers ); }
			template<typename THandler>
			const THandler& getHandler() const { return std::get<THandler::metadata.idx>( handlers ); }
			template<THandlerIdx I>
			auto& getHandlerEdit() { return std::get<I>( handlers ); }
			template<typename THandler>
			THandler& getHandlerEdit() { return std::get<THandler::metadata.idx>( handlers ); }

			inline const SchedulerType& getScheduler() const { return scheduler; }
			inline SchedulerType& getSchedulerEdit() { return scheduler; }

          //state
			inline const BType& getBState() const { return bState; }
			inline BType& getBStateEdit() { return bState; }
            inline bool checkBState( const ElementBaseType* elem ) const { return elem ? bState.checkBit( elem->getAbsoluteId() ) : false; } 
            inline bool checkBState( size_t absoluteId ) const { return bState.checkBit( absoluteId ); } 

            inline const QType& getQState() const { return qState; }
            inline QType& getQStateEdit() { return qState; }
            inline TReal checkQState( const ElementBaseType* elem ) const { return elem ? qState[ elem->getAbsoluteId() ] : 0.0; } 
            inline TReal checkQState( size_t absoluteId ) const { return qState[ absoluteId ]; } 
            template< typename TVal = TReal >
            std::vector<TVal> checkQState( const std::vector<const ElementBaseType*>& elems );

			inline BType& getChanges() const { return changes; }
            inline bool checkChange( const ElementBaseType* elem ) const { return elem ? changes.checkBit( elem->getAbsoluteId() ) : false; } 
            inline bool checkChange( size_t absoluteId ) const { return changes.checkBit( absoluteId ); } 

			inline const Time& getTime() const { return time; }


		//---API
          //precompute
            void populateHandlers() { ut::applyRoutine<PopulateRoutine>( handlers ); }
            template< size_t I = HANDLER_NUM - 1 >           
            void setOwnershipHandlers();
            void linkHandlers() { ut::applyRoutine<LinkRoutine>( handlers ); }
          //run
            virtual void initCycle();
            virtual void updateCycle( const EventType* event );
              
          //State: BState, QState and content
            inline void setState( const ElementBaseType* elem, TReal val = 1.0, bool bNotNegative = false ) { setQState( elem, val, bNotNegative ); setBState( elem, isPositive( val ) ); } 
            inline void addState( const ElementBaseType* elem, TReal val = 1.0, bool bNotNegative = false ) { addQState( elem, val, bNotNegative ); setBStateFromQ( elem ); } 
            inline void setBState( const ElementBaseType* elem, bool val = true ) { bState.setBit( elem->getAbsoluteId(), val ); } 
            inline void setBState( size_t absoluteId, bool val = true ) { bState.setBit( absoluteId, val );  } 
            inline void setBStateFromQ( const ElementBaseType* elem ) {  bState.setBit( elem->getAbsoluteId(), qState[ elem->getAbsoluteId() ] > 0  ); }
            inline void setBStateFromQ( size_t absoluteId ) {  bState.setBit( absoluteId, qState[ absoluteId ] > 0  ); }
            inline void setBStateFromQ() { for( size_t e = 0; e < qState.size(); e++ ) bState.setBit( e, qState[ e ] > 0  ); }
            inline void setBStateFromQ( const Bitset& mask ) { for( size_t e  = mask.first(); e != mask.npos(); e = mask.next( e + 1 ) ) bState.setBit( e, qState[ e ] > 0  ); }

            inline void setQState( const ElementBaseType* elem, TReal val = 1.0, bool bNotNegative = false ) { qState[ elem->getAbsoluteId() ] = bNotNegative ? ut::fitL( val ) : val; } 
            inline void setQState( size_t absoluteId, TReal val = 1.0, bool bNotNegative = false ) { qState[ absoluteId ] = bNotNegative ? ut::fitL( val ) : val; } 
            inline void addQState( const ElementBaseType* elem, TReal val = 1.0, bool bNotNegative = false ) { qState[ elem->getAbsoluteId() ] = bNotNegative ? ut::fitL( qState[ elem->getAbsoluteId() ] + val ) : qState[ elem->getAbsoluteId() ] + val; } 

            template< typename TAmount >
            inline void addContentFrom( const ElementBaseType* elem, TAmount amount = 1.0, bool bNotNegative = false ) { elem->isQuantitative() ? addContentFromQuantitative( elem, amount, bNotNegative ) : addContentFromBoolean( elem, amount ); }
            void addContentFromBoolean( const ElementBaseType* elem, int amount = 1 );
            void addContentFromQuantitative( const ElementBaseType* elem, TReal amount = 1.0, bool bNotNegative = false );
            template< typename TAmount >
            inline void setContentFrom( const ElementBaseType* elem, TAmount amount = 1.0, bool bNotNegative = false ) { elem->isQuantitative() ? setContentFromQuantitative( elem, amount, bNotNegative ) : setContentFromBoolean( elem, amount ); }
            void setContentFromBoolean( const ElementBaseType* elem, int amount );
            void setContentFromQuantitative( const ElementBaseType* elem, TReal amount = 1.0, bool bNotNegative = false );
          //changes
            inline void addChange( const ElementBaseType* elem ) { changes.setOn( elem->getAbsoluteId() ); }
            inline void addChangesFrom( const ElementBaseType* elem ) { changes |= elem->getForwardLink().mask; }
            inline void addChangesFrom( const ElementBaseType* elem, ElemTypeIdx typeIdx ) { changes |= elem->getForwardLink().mask.getMasked( typeIdx ); }
            inline void addChangesFromContent( const ElementBaseType* elem ) { changes |= elem->getForwardOfContent().mask; }
            inline void removeChange( const ElementBaseType* elem ) { changes.setOff( elem->getAbsoluteId() ); }
            inline void removeChange( size_t absoluteIdx ) { changes.setOff( absoluteIdx ); }
            inline void removeChanges( ElemTypeIdx typeIdx ) { changes.reset( typeIdx ); }
            inline void resetChangesToZero() { changes.reset(); changesSummary.reset(); }

          //handlers
            inline void initHandlers() { ut::applyRoutine<InitRoutine>( handlers ); }
            inline void secondInitHandlers() { ut::applyRoutine<SecondInitRoutine>( handlers, &changes, &changesSummary ); }
            inline void updateHandlers( const EventType* event ) { ut::applyRoutine<UpdateRoutine>( handlers, &changes, &changesSummary, event ); }
            inline void tickHandlers() { ut::applyRoutine<TickRoutine>( handlers ); }
            inline void divisionOldHandlers( Agent* daughter ) { ut::applyRoutineDouble<DivisionOldRoutine>( handlers, daughter->handlers ); }
            inline void divisionNewHandlers( Agent* mother ) { ut::applyRoutineDouble<DivisionNewRoutine>( handlers, mother->handlers ); }

          //TODO
            inline Bitset& getChangesSummary() const { return changesSummary; }
            //inline bool checkTypeChanges( ElemTypeIdx elemTypeIdx ) const { return changesSummary.checkBit( elemTypeIdx ); } 
            inline bool checkTypeChanges( ElemTypeIdx elemTypeIdx ) const { return changes.any( elemTypeIdx ); } 
            void refreshChangesSummary( ElemTypeIdx elemType ) { changesSummary.setBit( elemType, changes.any( elemType ) ); }


		protected:
          //resources
			const CollectionType* collection;
			std::tuple<THandlers...> handlers;
			SchedulerType scheduler;


          //state
			BType bState;
			QType qState;
			mutable BType changes;
			mutable Bitset changesSummary;

			Time time;
	};






/////////////////////////////////////////////////////// METHOD DEFINITIONS ////////////////////////////////////////////////////////

//========================================== SET 
	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	template< typename TVal >
	std::vector<TVal> Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::checkQState( const std::vector<const ElementBaseType*>& elems )
	{ 
	    std::vector<TVal> vals; 
	    vals.reserve( elems.size() ); 
	    for( const ElementBaseType* elem : elems )
	        vals.push_back( checkQState( elem ) );
	    return vals;
	}

	//============================================ API

	//------------------------------------------------------precompute
	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	template< size_t I >           
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::setOwnershipHandlers()
	{
	    std::get<I>( handlers ).setOwner( static_cast<DerivedType* const>(this) );
	    std::get<I>( handlers ).setOwnershipReports();

	    if constexpr( I > 0 )
	        setOwnershipHandlers< I -1 >();
	}

	//------------------------------------------------------run
	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::addContentFromBoolean( const ElementBaseType* elem, int amount )
	{
		if( amount > 0 && ! bState.checkBit( elem->getAbsoluteId() ) )
		{
			qState += elem->getContent().vals;
			
			if( elem->getHasNegativeContent() )
				setBStateFromQ( elem->getContent().mask );
			else
		    	bState |= elem->getContent().mask;
		}
		else if( amount < 0 && bState.checkBit( elem->getAbsoluteId() ) )
		{
			qState -= elem->getContent().vals;
			setBStateFromQ( elem->getContent().mask );
		}
	    if( elem->getIsInteger() )
	    {
	        for( const auto& pair : elem->getContent().elems )
	            qState[ pair.first->getAbsoluteId() ] = round<uint>( qState[ pair.first->getAbsoluteId() ] );   
	    }
		
	    /*if( amount == 0 )
	          return;
	    if( amount > 0 )
	          bState.setOn( elem->getAbsoluteId() );
	    else
	          bState.setOff(  elem->getAbsoluteId() );

	    qState[ elem->getAbsoluteId() ] = round<uint>( fit( qState[ elem->getAbsoluteId() ] + amount ) );*/
	}

	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::addContentFromQuantitative( const ElementBaseType* elem, TReal amount, bool bNotNegative )
	{
		if( bNotNegative )
			amount = fitL( amount, - qState[ elem->getAbsoluteId() ] );
	    qState += elem->getContent().vals * amount;

	    if( isPositive( amount ) && ! elem->getHasNegativeContent() )
	        bState |= elem->getContent().mask;
	    else if( notZero( amount ) || elem->getHasNegativeContent() )
	        setBStateFromQ( elem->getContent().mask );

	    if( elem->getIsInteger() )
	    {
	        for( const auto& pair : elem->getContent().elems )
	           qState[ pair.first->getAbsoluteId() ] = round<uint>( qState[ pair.first->getAbsoluteId() ] );   
	    }
	}

	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::setContentFromBoolean( const ElementBaseType* elem, int amount )
	{
		if( amount > 0 && ! bState.checkBit( elem->getAbsoluteId() ) )
		{
			qState += elem->getContent().vals;

		   	if( elem->getHasNegativeContent() )
				setBStateFromQ( elem->getContent().mask );
			else
		    	bState |= elem->getContent().mask;
		}
		else if( amount <= 0 &&bState.checkBit( elem->getAbsoluteId() ) )
		{
			qState -= elem->getContent().vals;
			setBStateFromQ( elem->getContent().mask );
		}

	   	if( elem->getIsInteger() )
	    {
	        for( const auto& pair : elem->getContent().elems )
	           qState[ pair.first->getAbsoluteId() ] = round<uint>( qState[ pair.first->getAbsoluteId() ] );   
	    }

	    /*if( amount > 0 )
	        bState.setOn( elem->getAbsoluteId() ); 
	    else
	        bState.setOff(  elem->getAbsoluteId() );

	    qState[ elem->getAbsoluteId() ] = amount > 0 ? 1.0 : 0.0;*/
	}

	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers > 
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::setContentFromQuantitative( const ElementBaseType* elem, TReal amount, bool bNotNegative )
	{
		if( bNotNegative )
			amount = fitL( amount );

	    TReal current = checkQState( elem );

	    qState += elem->getContent().vals * ( amount - current );

		if( isZero( current ) && isPositive( amount ) && ! elem->getHasNegativeContent() )
			bState |= elem->getContent().mask;
		else if( ! isPositive( amount - current ) || elem->getHasNegativeContent() )
			setBStateFromQ( elem->getContent().mask );

		if( elem->getIsInteger() )
		{
			for( const auto& pair : elem->getContent().elems )
				qState[ pair.first->getAbsoluteId() ] = round<uint>( qState[ pair.first->getAbsoluteId() ] );   
		}
	}



	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::initCycle()
	{
	    //ut::print( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!???????????????????????????? t: ", time ); 
	    //ut::printVector( changes.getWords() );
	    while( changes.any() )
	        secondInitHandlers();
	    //ut::printVector( changes.getWords() );
	    /*while( changes.any() )
	    {
	          ut::printVector( changes.getWords() );
	          updateHandlers( event );
	          //resetChangesToZero();
	          //setBState( ElemTypeIdx::EVENT_MARKER, false );
	          //resetChanges();
	    }*/
	}

	template< typename TDerived, typename TEvent, typename TCollection, typename THandlerIdx, typename... THandlers >
	void Agent< TDerived, TEvent, TCollection, THandlerIdx, THandlers... >::updateCycle( const EventType* event )
	{
	    //ut::print( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!???????????????????????????? t: ", time ); 
	    //ut::printVector( changes.getWords() );
	    if( changes.any() )
	    	updateHandlers( event );
	    while( changes.any() )
	        updateHandlers( nullptr );
	    //ut::printVector( changes.getWords() );
	/*while( changes.any() )
	{
	          ut::printVector( changes.getWords() );
		updateHandlers( event );
	          //resetChangesToZero();
	          //setBState( ElemTypeIdx::EVENT_MARKER, false );
		//resetChanges();
	}*/
	}
}

#endif //UT_AGENT_HPP