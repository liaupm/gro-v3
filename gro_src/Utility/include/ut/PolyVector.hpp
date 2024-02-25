#ifndef UT_POLYVECTOR_HPP
#define UT_POLYVECTOR_HPP

#include "ut/defines.hpp"

/*PRECOMPILED
#include <vector> //UnderContainer
#include <string> //name in getByName
#include <memory> //SP, static_pointer_cast */


namespace ut
{
	template< typename TBase >
	class PolyVectorBase
	{
		public:
		//---ctor, dtor
			virtual ~PolyVectorBase() = default;

        //---get
		  //single element
            virtual const TBase* getElement( size_t idx ) const  = 0;
            virtual TBase* getElementEdit( size_t idx ) = 0;
            virtual const SP<TBase> getElementPtr( size_t idx ) const = 0;
            virtual SP<TBase> getElementPtrEdit( size_t idx ) = 0;
            virtual TBase* operator[] ( size_t idx ) = 0;
            virtual const TBase* getElementByName( const std::string& name ) const = 0;
            virtual const TBase* getElementByNameEdit( const std::string& name ) = 0;
          //wrapper
            virtual size_t size() const = 0;
            virtual const TBase* back() = 0;

        //---API
          //wrapper
            virtual void push_back( SP<TBase> ) = 0;
            virtual void clear() = 0;
            virtual void eraseAt( size_t idx ) = 0;
            

		protected:
			PolyVectorBase() = default;
	};


	template< typename T, typename TBase >
	class PolyVector : public PolyVectorBase<TBase>
	{
		public:
			using Type = T;
            using UnderType = SP<T>;
            using UnderContainer = std::vector<UnderType>;

            
		//---ctor, dtor
			PolyVector() = default;
			virtual ~PolyVector() = default;

        //---get
		  //all
            const UnderContainer& getElements() const { return elements; }
          //single element
            const T* getElement( size_t idx ) const override { return elements[idx].get(); }
            T* getElementEdit( size_t idx ) override { return elements[idx].get(); }
            const SP<TBase> getElementPtr( size_t idx ) const override { return elements[idx]; }
            SP<TBase> getElementPtrEdit( size_t idx ) override { return elements[idx]; }
            T* operator[] ( size_t idx ) override { return getElementEdit( idx ); }
            const T* getElementByName( const std::string& name ) const override;
            T* getElementByNameEdit( const std::string& name ) override { return const_cast<T*>( getElementByName( name ) ); }
          //wrapper
            size_t size() const override { return elements.size(); }
            T* back() override { return elements.back().get(); }
            inline typename UnderContainer::iterator begin() { return elements.begin(); }
            inline typename UnderContainer::iterator end() { return elements.end(); }
            inline typename UnderContainer::const_iterator cbegin() const { return elements.cbegin(); }
            inline typename UnderContainer::const_iterator cend() const { return elements.cend(); }

        //---set
            inline void setElements( const UnderContainer& xElements ) { elements = xElements; }
           
        //---API
          //common
            void push_back( SP<TBase> newElement ) override { elements.push_back( std::static_pointer_cast<T>( newElement ) ); }
            void clear() override { elements.clear(); }
            void eraseAt( size_t idx ) override { elements.erase( elements.begin() + idx ); }

          //special
            template< typename TReturn, typename... TArgs >
            auto call( TReturn ( UnderContainer::* funct )( TArgs... ), TArgs... args ) { return elements->funct( args... ); }


		private:
			UnderContainer elements;
	};




//////////////////////////////////////////////////////////// METHOD DEFINITIONS ////////////////////////////////////////////////////////////
	
	template< typename T, typename TBase >
	const T* PolyVector<T, TBase >::getElementByName( const std::string& name ) const 
	{ 
		for( const auto& e : elements )
		{
			if( e->getName() == name )
				return e.get();
		}
		return nullptr;
	}
}

#endif //UT_POLYVECTOR_HPP