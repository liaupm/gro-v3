#ifndef UT_COLLECTIONMANAGER_HPP
#define UT_COLLECTIONMANAGER_HPP

#include "ut/defines.hpp"
#include "ut/Metadata.hpp"
#include "ut/NameMap.hpp" //name2TypeMap
#include "ut/PolyVector.hpp" //DS to contain the elements
#include "ut/MultiBitset.hpp"
#include "ut/TypeCollection.hpp"

/*PRECOMPILED
#include <vector> //elements, startingIds, getAllNames()
#include <string> //names, get elements by name
#include <memory> //SP<>, std::make_shared, std::static_pointer_cast */


namespace ut
{
	class CollectionManagerBase
	/* common base for all the speciallizations of CollectionManager 
	with the members tht do not depend on template arguments */
	{
		public:
			using Mask = MultiBitset;

		//---ctor, dtor
			virtual ~CollectionManagerBase() = default;
		//---get
			inline size_t getTotalElementNum() const { return totalElementNum; }
			inline const std::vector< size_t >& getStartingIds() const { return startingIds; }
			inline const MultiBitsetCommon* getMaskCommon() const { return maskCommon.get(); }


		protected:
			size_t totalElementNum; //total number of elements, including all the types
			std::vector< size_t > startingIds; //first absolute id of each type of element. Same size as the elements DS
			SP<MultiBitsetCommon> maskCommon; //common part of the Multibitsets based on this collection. Used to initialize the state masks of Agent

			CollectionManagerBase() : totalElementNum( 0 ), maskCommon( nullptr ) {;}
	};


	template< typename TActual, typename TBase, typename... TsDerived >
	//TActual: derived class, TBase: base type of elements, TsDerived: all the element types
	class CollectionManager : public CollectionManagerBase
	{
		public:
			using ActualType = TActual; //derived Class
		    using ElementBaseType = TBase;
	        using ElementIndex = typename ElementBaseType::ElementIndex;
			using ElemTypeIdx = ElementIndex;
			using ElementIdx = ElementIndex;

			template< typename T >
			using EnableForElementTypes = ut::EnableForDerived< ElementBaseType, T >; //for SFINAE
			using ElementContainerBase = ut::PolyVectorBase< ElementBaseType >; //base class of the element containers
			template< typename T >
			using ElementContainer = ut::PolyVector< T, ElementBaseType >; //type of each specific container
			using MyTypes = ut::TypeCollection< TsDerived... >; //element types

		//---ctor, dtor
			CollectionManager() { addElementContainers<>(); }
			virtual ~CollectionManager() = default;
			CollectionManager( const CollectionManager& rhs ) = default;
			CollectionManager( CollectionManager&& rhs ) noexcept = default;
			CollectionManager& operator=( const CollectionManager& rhs ) = default;
			CollectionManager& operator=( CollectionManager&& rhs ) noexcept = default;

		//---get
			//elements can be accessed by type statically (template parameter) or dynamically (normal parameter)
		  //all
			template< typename T, typename=EnableForElementTypes<T> > 
			inline const ElementContainer<T>& getAll() const { return *static_cast< const ElementContainer<T>* >( elements[ T::metadata.idx ].get() ); }
			inline const ElementContainerBase& getAll( ElemTypeIdx typeIdx ) const { return *elements[ typeIdx ]; }
			template< typename T, typename=EnableForElementTypes<T> > 
			inline ElementContainer<T>& getAllEdit( ) { return *static_cast< ElementContainer<T>* >( elements[ T::metadata.idx ].get() ); }
			inline ElementContainerBase& getAllEdit( ElemTypeIdx typeIdx ) { return *elements[ typeIdx ]; }

			template< typename T, typename=EnableForElementTypes<T> >
			inline size_t getElementNum() const { return getAll<T>().size(); }
			inline size_t getElementNum( ElemTypeIdx typeIdx ) const { return elements[ typeIdx ].size(); }

			template< typename T, typename=EnableForElementTypes<T> >
			inline bool getAny() const { return getElementNum<T>() > 0; }
			inline bool getAny( ElemTypeIdx typeIdx ) const { return getElementNum( typeIdx ) > 0; }

		  //individual elements by idx (just id is assumed to be relative)
			template< typename T, typename=EnableForElementTypes<T> >
			const T* getById( size_t id ) const { return getAll<T>().getElement( id ); }
			const ElementBaseType* getById( size_t id, ElemTypeIdx typeIdx ) const { return elements[ typeIdx ]->getElement( id ); }
			const ElementBaseType* getByAbsId( size_t absId ) const { return elements[ getTypeOfAbsId( absId ) ]->getElement( abs2relId( absId ) ); }
			template< typename T, typename=EnableForElementTypes<T> >
			T* getByIdEdit( size_t id ) { return const_cast<T*>( getAllEdit<T>().getElement( id ) ); }
			ElementBaseType* getByIdEdit( size_t id, ElemTypeIdx typeIdx ) const { return const_cast<ElementBaseType*>( elements[typeIdx]->getElement( id ) ); }
			ElementBaseType* getByAbsIdEdit( size_t absId ) { return elements[ getTypeOfAbsId( absId ) ]->getElement( abs2relId( absId ) ); }

      	  //individual elements by name
			template< typename T, typename=EnableForElementTypes<T> > 
			inline const T* getByName( const std::string& name ) const { return getAll<T>().getElementByName( name ); }
			inline const ElementBaseType* getByName( const std::string& name, ElemTypeIdx typeIdx ) const { return elements[typeIdx]->getElementByName( name ); }
			inline const ElementBaseType* getByName( const std::string& name ) const { return checkName( name ) ? getByName( name, getTypeOfName( name ) ) : nullptr; }
			template< typename T, typename = EnableForElementTypes<T> > 
			inline T* getByNameEdit( const std::string& name ) { return const_cast<T*>( getAllEdit<T>().getElementByName( name ) ); }
			inline ElementBaseType* getByNameEdit( const std::string& name, ElemTypeIdx typeIdx ) { return const_cast<ElementBaseType*>( elements[typeIdx]->getElementByName( name ) ); }
			inline ElementBaseType* getByNameEdit( const std::string& name ) { return checkName( name ) ? getByNameEdit( name, getTypeOfName( name ) ) : nullptr; }


		//---API
		  //idx conversions
			template< typename T >
            inline size_t getStartingId() const { return startingIds[ T::metadata.idx ]; }
            inline size_t getStartingId( ElemTypeIdx elemType ) const { return startingIds[ elemType ]; }
            template< typename T >
            inline size_t getLastId() const { return startingIds[ T::metadata.idx ] + elements[ T::metadata.idx ].size(); }
            inline size_t getLastId( ElemTypeIdx elemType ) const { return startingIds[ elemType ] + elements[ elemType ].size(); }
			
			template< typename T >
			inline size_t rel2absId( size_t id ) const { return rel2absId( id, static_cast<ElemTypeIdx>( T::metadata.idx ) ); }
			inline size_t rel2absId( size_t id, ElemTypeIdx typeIdx ) const { return id + startingIds[ typeIdx ]; }
			template< typename T >
			inline size_t abs2relId( size_t id ) const { return abs2relId( id, static_cast<ElemTypeIdx>( T::metadata.idx ) ); }
			inline size_t abs2relId( size_t id, ElemTypeIdx typeIdx ) const { return id - startingIds[ typeIdx ]; }
			inline size_t abs2relId( size_t id ) const { return id - startingIds[ getTypeOfAbsId( id ) ]; }
		  //name-idx conversions
			template< typename T, typename=EnableForElementTypes<T> >
			inline size_t name2RelId( const std::string& name ) const { return getByName<T>( name )->getRelativeId(); }
			inline size_t name2RelId( const std::string& name, ElemTypeIdx  typeIdx ) const { return getByName( name, typeIdx )->getRelativeId(); }
			inline size_t name2RelId( const std::string& name ) const { return getByName( name )->getRelativeId(); }
			template< typename T, typename=EnableForElementTypes<T> >
			inline size_t name2AbsId( const std::string& name ) const { return getByName<T>( name )->getAbsoluteId(); }
			inline size_t name2AbsId( const std::string& name, ElemTypeIdx typeIdx ) const { return getByName( name, typeIdx )->getAbsoluteId(); }
			inline size_t name2AbsId( const std::string& name ) const { return getByName( name )->getAbsoluteId(); }
		  //misc 
			ElemTypeIdx getTypeOfAbsId( size_t id ) const;
			inline ElemTypeIdx getTypeOfName( const std::string& name ) const { return name2TypeMap.getSubtype( name ); }
			inline bool checkName( const std::string& name ) const { return name2TypeMap.underDS.find( name ) != name2TypeMap.underDS.end(); }
			inline std::vector<std::string> getAllNames() const { return getMapKeys( name2TypeMap.underDS ); }

		  //precompute
			template< typename T, typename... Ts, typename=EnableForElementTypes<T> >
			SP<T> create( Ts... args ); //create a new element

			virtual void init() { directLinkElems(); calculateIndexes(); linkElems(); calculateMasksElems(); precomputeElems(); } //all the steps required before run. May be overrided to add custom steps


		protected:
			std::vector< SP<ElementContainerBase> > elements;
			NameMap<ElemTypeIdx> name2TypeMap; //register of the type of each name

		  //precomputing steps
			void directLinkElems(); //load the raw link
			void calculateIndexes(); //store the starting idxs, calculate the total number, the maskCommon and the absolute idxs
			void linkElems(); //create the content and bidirectional links, which include masks
			void calculateMasksElems(); //create masks for the elements that use them
			void precomputeElems(); //call to the custom precompute() of each element

		  //initial creation of the element containers (recursive call)
			template< typename T >
			void addElementContainer();
			template< size_t idx = 0 >
			void addElementContainers();
	};



//////////////////////////////////////////////////////// METHOD DEFINITIONS ///////////////////////////////////////////////////////////////////////////////////

//---idx conversions
	template<typename TActual, typename TBase, typename... TsDerived>
	typename CollectionManager<TActual, TBase, TsDerived...>::ElemTypeIdx CollectionManager<TActual, TBase, TsDerived...>::getTypeOfAbsId( size_t id ) const
	{ 
		for( size_t result = 0; result < startingIds.size(); result++ ) 
		{ 
			if( startingIds[result] > id ) 
				return static_cast<ElemTypeIdx>( result - 1 );
		} 
		return ElemTypeIdx::COUNT;
	}

//---API
    template<typename TActual, typename TBase, typename... TsDerived>
     template< typename T, typename... Ts, typename >
    SP<T> CollectionManager<TActual, TBase, TsDerived...>::create( Ts... args )
    {
    	auto& container = elements[ T::metadata.idx ]; //get the right container for the type 
	    SP<T> newElement = std::make_shared<T>( static_cast<ActualType* const>( this ), container->size(), args... ); //create the element. container->size() is the relative idx
		container->push_back( std::static_pointer_cast< T >( newElement ) );
	    
		name2TypeMap.registerSubclass( container->back()->getName(), static_cast<ElementIndex>( T::metadata.idx ) ); //register the type of the name
		return newElement;
    }


//---precomputing steps
    template<typename TActual, typename TBase, typename... TsDerived>
	void CollectionManager<TActual, TBase, TsDerived...>::directLinkElems()
	{
	    for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->loadDirectElemLink();
	    }
	}

    template<typename TActual, typename TBase, typename... TsDerived>
	void CollectionManager<TActual, TBase, TsDerived...>::calculateIndexes()
	{
	    startingIds = std::vector<size_t>( MyTypes::nSize );
	    startingIds[0] = 0;
	    totalElementNum = elements[0]->size();

	    for( size_t i = 1; i < startingIds.size(); i++ )
	    {
	        startingIds[i] = startingIds[ i - 1 ] + elements[ i - 1 ]->size();
	        totalElementNum += elements[ i ]->size();

	        for( size_t e = 0; e < elements[i]->size(); e++ )
	            elements[i]->getElementEdit( e )->updateAbsoluteId( startingIds[i] );
	    }
	    maskCommon = std::make_shared<MultiBitsetCommon>( totalElementNum, startingIds );
	}

	template<typename TActual, typename TBase, typename... TsDerived>
	void CollectionManager<TActual, TBase, TsDerived...>::linkElems()
	{
		for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->initLinkMasks( this );
	    }
	    for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->makeBidirectional( this );
	    }
	   	for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->accumulateForwardOfContent();
	    }
	}

	template<typename TActual, typename TBase, typename... TsDerived>
	void CollectionManager<TActual, TBase, TsDerived...>::calculateMasksElems()
	{
	    for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->calculateMasks( this );
	    }
	}

	template<typename TActual, typename TBase, typename... TsDerived>
	void CollectionManager<TActual, TBase, TsDerived...>::precomputeElems()
	{
	    for( size_t i = 0; i < elements.size(); i++ )
	    {
	        for( size_t e = 0; e < elements[i]->size(); e++  )
	            elements[i]->getElementEdit( e )->precompute(); 
	    }
	}


//---initial creation of the element containers (recursive call)
	template<typename TActual, typename TBase, typename... TsDerived>
     template< typename T >
    void CollectionManager<TActual, TBase, TsDerived...>::addElementContainer()
    {
        while( T::metadata.idx >= elements.size() )
        	elements.push_back( std::make_shared< ElementContainer<ElementBaseType> >() );

        elements[ T::metadata.idx ] = std::make_shared< ElementContainer<T> >();
    }

	template<typename TActual, typename TBase, typename... TsDerived>
     template< size_t idx >
    void CollectionManager<TActual, TBase, TsDerived...>::addElementContainers()
    {
        addElementContainer< typename MyTypes::template TypeAt< idx > >();

        if constexpr( idx < MyTypes::nSize - 1 )
            addElementContainers< idx + 1 >();
    }
}


#endif //UT_COLLECTIONMANAGER_HPP