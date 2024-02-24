#ifndef UT_LINKEDELEMENT_HPP
#define UT_LINKEDELEMENT_HPP

#include "defines.hpp"
#include "Metadata.hpp"
#include "Identifiable.hpp" //base class
#include "Bitset.hpp" 
#include "MultiBitset.hpp" 
#include "ElementIndex.hpp" 
#include "ElementCounter.hpp" 
#include "ExtendedVector.hpp" 
#include "CollectionManager.hpp"

/*PRECOMPILED
#include <array>
#include <vector>
#include <memory>
#include <utility> //forward */


namespace ut
{
	template< typename TBase, typename TIdx >
	class LinkedElement : public Identifiable
	{
		public:
			using BaseType = TBase;                
			using BasePtr = const BaseType*;
            using ElemVector = std::vector< BasePtr >;
            using ElemCounter = ElementCounter< BasePtr >;
            using ElemCounterRaw = ElementCounter< class ElemWrapper >;

            using ElemTypeIdx = typename TIdx::ElemTypeIdx;
            using ElementIndex = ElemTypeIdx;
            using BElementIndex = ElemTypeIdx;
            //using CollectionType = TCollection;
            using CollectionType = CollectionManagerBase;
            using TCollection = CollectionManagerBase;

            using Mask = MultiBitset;
            using IdType = size_t;
            using ValType = TReal; //needs to be TReal to allow direct sumation with qState of Agent
            using ValVector = ut::ExtendedVector< ValType >;
            
            template< typename T >
            using ByTypeArray = std::array< T, ElemTypeIdx::COUNT >;

            template< typename T >
            using ElemArrayOfVectors = ByTypeArray< std::vector<T> >;

            using ElemArrayOfMasks = ByTypeArray< Mask >;
            using ElemArrayOfIds = ElemArrayOfVectors<IdType>;

            template< typename T >
            using ByElemTypeDS = std::array<T, ElemTypeIdx::COUNT >;

            enum LinkDirection : bool
            {
                FORWARD, BACKWARD
            };


    //---nested classes
            class LinkWrapper
            {
                public:
                    LinkWrapper( BasePtr elem, LinkDirection direction ) : elem( elem ), direction( direction ) {;}

                    inline bool isNull() const { return elem == nullptr; }

                    inline friend bool operator==( const LinkWrapper& lhs, const LinkWrapper& rhs ) { return ( lhs.elem == rhs.elem && lhs.direction == rhs.direction ); }
                    inline friend bool operator!=( const LinkWrapper& lhs, const LinkWrapper& rhs ) { return !( lhs == rhs ); }
                
                    BasePtr elem;
                    LinkDirection direction;   
            };

            class ContentWrapper
            {
                public:
                    ContentWrapper( BasePtr elem ) : elem( elem ) {;}

                    inline bool isNull() const { return elem == nullptr; }

                    inline friend bool operator==( const ContentWrapper& lhs, const ContentWrapper& rhs ) { return ( lhs.elem == rhs.elem ); }
                    inline friend bool operator!=( const ContentWrapper& lhs, const ContentWrapper& rhs ) { return !( lhs == rhs ); }
                
                    BasePtr elem;
            };


            struct LinkBase
            {
            public:
                    LinkBase() : typeSummary( ElemTypeIdx::COUNT ) {;}
                    virtual void init( const CollectionType* collection ) { vals = ValVector( collection->getTotalElementNum(), 0) ; mask = Mask( collection->getMaskCommon() ); }
                    virtual bool addElem( BasePtr elem, ValType = 1 ) = 0;

                    ValVector vals;
                    Mask mask;
                    Bitset typeSummary;
            };

            struct BLink : public LinkBase
            {
                    BLink() : LinkBase::LinkBase() {;}
                    //void init( const CollectionType* collection ) override { LinkBase::init( collection ); }
                    bool addElem( BasePtr elem, ValType = 1 ) override;
                    void accumulate( const BLink& rhs );
                    void accumulate( BasePtr elem ) { accumulate( elem->getFwLink() ); }

                    ByElemTypeDS< ElemVector > elemsByType;
                    ElemVector elems;
            };

            struct QLink : public LinkBase
            {
                    QLink() : LinkBase::LinkBase() {;}
                    //void init( const CollectionType* collection ) override { LinkBase::init( collection ); }
                    bool addElem( BasePtr elem, ValType val ) override;
                    bool addElemOnly( BasePtr elem, ValType val );
                    void accumulate( const QLink& rhs, int multiplier );
                    void accumulate( BasePtr elem, int multiplier ) { accumulate( elem->getContent(), multiplier ); }

                    ByElemTypeDS< ElemCounter > elemsByType;
                    ElemCounter elems;
            };


		//---ctor, dtor
            LinkedElement( size_t relativeId, const std::string& name, ElemTypeIdx elementType, bool bQuantitative, bool bInteger, const CollectionType* collection ) 
            : ut::Identifiable::Identifiable( relativeId, name )
            /*, caller(nullptr)*/, collection(collection)
            , elementType( elementType ), bQuantitative( bQuantitative), bInteger( bInteger ), bNegativeContent( false )
            , bDone(false), bAccumulated(false) {;}


			virtual ~LinkedElement() = default;
			LinkedElement( const LinkedElement& rhs ) = default;
			LinkedElement( LinkedElement&& rhs ) noexcept = default;
			LinkedElement& operator=( const LinkedElement& rhs ) = default;
			LinkedElement& operator=( LinkedElement&& rhs ) noexcept = default;

		//---get
            inline const CollectionType* getCollection() const { return collection; }
            inline ElemTypeIdx getElemType() const { return elementType; }
            inline bool isQuantitative() const { return bQuantitative; }
            inline bool getIsInteger() const { return bInteger; }
            inline bool getHasNegativeContent() const { return bNegativeContent; }

            inline const BLink& getBLink( LinkDirection dir ) const { return dir == LinkDirection::FORWARD ? forwardLink : backwardLink; }
            inline BLink& getBLinkEdit( LinkDirection dir ) { return dir == LinkDirection::FORWARD ? forwardLink : backwardLink; }
            inline const BLink& getForwardLink() const { return forwardLink; }
            inline const BLink& getFwLink() const { return forwardLink; }
            inline const BLink& getBackwardLink() const { return backwardLink; }
            inline const BLink& getBwLink() const { return backwardLink; }

            inline const QLink& getContent() const { return content; }
            inline QLink& getContentEdit() { return content; }
            inline const BLink& getForwardOfContent() const { return forwardOfContent; }
            inline const BLink& getBackwardContent() const { return backwardContent; }

            inline bool getBDone() const { return bDone; }
         
		//---set
			
		//---API
            bool addLink( BasePtr elem, LinkDirection direction ) { return elem ? rawLink[ elem->getElemType() ].add( LinkWrapper( elem, direction ) ) : false; }

            template< typename T >
            void addLinks( const std::vector< const T* >& elems, LinkDirection direction ) { for( const T* elem : elems ) addLink( elem, direction ); }


            bool addContent( BasePtr elem, int amount = 1 ) { return elem ? rawContent[ elem->getElemType() ].add( ContentWrapper( elem ), amount ) : false; }

            template< typename T >
            void addContents( const std::vector< const T* >& elems, int amount = 1 ) { for( const T* elem : elems ) addContent( elem, amount ); }

            template< typename T, typename TAmount >
            void addContents( const std::vector< const T* >& elems, const std::vector<TAmount>& amounts ) { for( size_t i = 0; i < elems.size(); i++ ) addContent( elems[i], amounts[i] ); }

            void initLinkMasks( CollectionType* collection );
            void makeBidirectional( CollectionType* collection, ElemTypeIdx typeIdx );
            void makeBidirectional( CollectionType* collection );
            void accumulateForwardOfContent();
            

            virtual void init( CollectionType* collection ) { makeBidirectional( collection ); }
            //virtual void makeForwardLink( CollectionType* collection = nullptr ) {;}
            virtual void calculateMasks( CollectionType* ) {;}
            virtual void loadDirectElemLink() {;}
            virtual void precompute() {;}


		protected:
			const CollectionType* collection;
            ElemTypeIdx elementType;
            bool bQuantitative;
            bool bInteger;
            bool bNegativeContent;

            BLink forwardLink;
            BLink backwardLink;
            QLink content;
            BLink forwardOfContent;
            BLink backwardContent;

            ByElemTypeDS< ElementCounter<LinkWrapper> > rawLink;
            ByElemTypeDS< ElementCounter<ContentWrapper> > rawContent;
            bool bDone;
            bool bAccumulated;

            void calculateNegativeContent();
	};




//////////////////////////////////////// FUNCTION DEFINITIONS ///////////////////////////////////////////////////////////////

//============================================================================================= NESTED CLASSES
 
  //---BLink
    template< typename TBase, typename TIdx >
    bool LinkedElement<TBase, TIdx>::BLink::addElem( BasePtr elem, ValType )
    { 
        this->typeSummary.setOn( elem->getElemType() );
        this->mask.setOn( elem->getAbsoluteId() );
        if( this->vals[ elem->getAbsoluteId() ] == 1 )
            return false;

        this->vals[ elem->getAbsoluteId() ] = 1;
        elems.push_back( elem ); 
        elemsByType[ elem->getElemType() ].push_back( elem ); 
        return true;  
    }

    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::BLink::accumulate( const BLink& rhs ) 
    { 
        this->mask |= rhs.mask; 
        this->vals += rhs.vals; 
        this->vals.fitU( 1.0 );
        ut::unionToVector( elems, rhs.elems ); 

        for( size_t t = 0; t < ElemTypeIdx::COUNT; t++ )
            ut::unionToVector( elemsByType[t], rhs.elemsByType[t] ); 
    }


  //---QLink
    template< typename TBase, typename TIdx >
    bool LinkedElement<TBase, TIdx>::QLink::addElem( BasePtr elem, ValType val )
    { 
        this->typeSummary.setOn( elem->getElemType() ); 
        this->typeSummary |= elem->getContent().typeSummary; 
        accumulate( elem->getContent(), val ); 

        return true; 
    }


    template< typename TBase, typename TIdx >
    bool LinkedElement<TBase, TIdx>::QLink::addElemOnly( BasePtr elem, ValType val ) 
    { 
        this->typeSummary.setOn( elem->getElemType() ); 
        this->mask.setOn( elem->getAbsoluteId() );
        this->vals[ elem->getAbsoluteId() ] += val;

        elems.add( elem, val ); 
        elemsByType[ elem->getElemType() ].add( elem, val ); 

        return true; 
    }


    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::QLink::accumulate( const QLink& rhs, int multiplier ) 
    { 
        this->mask |= rhs.mask; 
        this->vals += ( rhs.vals * multiplier ); 
        elems += rhs.elems * multiplier; 

        for( size_t t = 0; t < ElemTypeIdx::COUNT; t++ ) 
            this->elemsByType[t] += rhs.elemsByType[t] * multiplier; 
    }


//============================================================================================= MAIN CLASS
    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::makeBidirectional( CollectionType* collection, ElemTypeIdx typeIdx )
    {
        for( const auto& elemPair : rawLink[ typeIdx ] )
        {
            if( ! elemPair.first.isNull() )
            {
                getBLinkEdit( elemPair.first.direction ).addElem( elemPair.first.elem );
                const_cast<BaseType*>( elemPair.first.elem )->getBLinkEdit( static_cast<LinkDirection>( ! elemPair.first.direction ) ).addElem( static_cast<BaseType* const>(this) );
            }
        }

        for( const auto& elemPair : rawContent[ typeIdx ] )
        {
            if( ! elemPair.first.isNull() )
            {
                const_cast<BaseType*>( elemPair.first.elem )->makeBidirectional( collection );
                content.addElem( elemPair.first.elem, elemPair.second );
                //forwardOfContent.accumulate( elemPair.first.elem->getFwLink() );
                //forwardOfContent.accumulate( elemPair.first.elem->getForwardOfContent() );
                const_cast<BaseType*>( elemPair.first.elem )->backwardContent.addElem( static_cast<BaseType* const>(this) );
            }
        }       
    }


    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::makeBidirectional( CollectionType* collection )
    {
        if( bDone )
            return; 

        content.addElemOnly( static_cast<BaseType* const>( this ), 1 );
        //forwardLink.addElem( static_cast<BaseType* const>( this ) );

        for( size_t t = 0; t < ElemTypeIdx::COUNT; t++ )
            makeBidirectional( collection, static_cast<ElemTypeIdx>( t ) );

        calculateNegativeContent();
        bDone = true;
    }

    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::accumulateForwardOfContent()
    {
    	if( bAccumulated )
    		return;

    	forwardOfContent = forwardLink;
    	for( auto& elemPair : content.elems )
    	{
    		if( elemPair.first != this )
    		{
    			const_cast<BaseType*>( elemPair.first )->accumulateForwardOfContent();
				forwardOfContent.accumulate( elemPair.first->getForwardOfContent() );
    		}
    	}
    	bAccumulated = true;
    }


    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::initLinkMasks( CollectionType* collection )
    {
        forwardLink.init( collection );
        backwardLink.init( collection );

        content.init( collection );
        forwardOfContent.init( collection );
        backwardContent.init( collection );
    }


//-----private
    template< typename TBase, typename TIdx >
    void LinkedElement<TBase, TIdx>::calculateNegativeContent()
    {
    	for( const auto& val : content.vals )
    	{
    		if( isNegative( val ) )
    		{
    			bNegativeContent = true;
    			return;
    		}
    	}
    }

}

#endif //UT_LINKEDELEMENT_HPP
