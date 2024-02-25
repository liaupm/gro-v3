#ifndef UT_FACTORY_HPP
#define UT_FACTORY_HPP

#include "ut/defines.hpp"
#include "ut/Metadata.hpp" //subtypeMeta
#include "ut/TypeCollection.hpp" //DerivedTypes with derived types

/*PRECOMPILED
#include <memory> //UP<> */


namespace ut
{
   	template< typename TBase, typename... TsDerived >
   	//TBase: common base type, with an idx enum, TsDerived: all the derived types, with metadata
	class Factory
	{
		public:
			using BaseType = TBase;
	        using SubtypeIdx = typename BaseType::SubtypeIdx;
			using DerivedTypes = ut::TypeCollection< TsDerived... >;

			template< typename T >
			using EnableForSubtypes = ut::EnableForDerived<BaseType, T >;

			inline static const std::string NO_NAME = "";


		//---ctor, dtor
			Factory() = default;
			virtual ~Factory() = default;

		//---API
		    template< size_t I = 0 > 
		    static const std::string& idx2name( SubtypeIdx idx ); //returns ::COUNT if invalid input

		    template< size_t I = 0 > 
		    static SubtypeIdx name2idx( const std::string& name ); //returns NO_NAME if invalid input

		    template< typename TExtractor, size_t I = 0 > 
            static const auto& extractTypeData( SubtypeIdx idx ); //gets the class from the enum idx. The input must ve valid

		    template< size_t I = 0, typename... Ts > 
		    static UP<TBase> create( SubtypeIdx idx, Ts&&... args ); //create by type idx (an enum given by the BaseType)
	};




//////////////////////////////////////////////// METHOD DEFINITIONS //////////////////////////////////////////////////////////////////////

	template<typename TBase, typename... TsDerived>
     template< size_t I >
    const std::string& Factory<TBase, TsDerived...>::idx2name( SubtypeIdx idx )
    {
        if( DerivedTypes::template TypeAt< I >::subtypeMeta.idx == idx )
        	return DerivedTypes::template TypeAt< I >::subtypeMeta.name;

        if constexpr( I < DerivedTypes::nSize - 1 )
            return idx2name< I + 1 >( idx );

        return NO_NAME;
    }

    template<typename TBase, typename... TsDerived>
     template< size_t I >
    typename Factory<TBase, TsDerived...>::SubtypeIdx Factory<TBase, TsDerived...>::name2idx( const std::string& name )
    {
        if( DerivedTypes::template TypeAt< I >::subtypeMeta.name == name )
                return static_cast<SubtypeIdx>( DerivedTypes::template TypeAt< I >::subtypeMeta.idx );

        if constexpr( I < DerivedTypes::nSize - 1 )
            return name2idx< I + 1 >( name );

        return SubtypeIdx::COUNT;
    }

    template<typename TBase, typename... TsDerived>
     template< typename TExtractor, size_t I >
    const auto& Factory<TBase, TsDerived...>::extractTypeData( SubtypeIdx idx )
    {
        if( DerivedTypes::template TypeAt< I >::subtypeMeta.idx == idx )
            return TExtractor::template call< typename DerivedTypes::template TypeAt< I > >();

        if constexpr( I < DerivedTypes::nSize - 1 )
            return extractTypeData< TExtractor, I + 1 >( idx );
        
        return TExtractor::template call< typename DerivedTypes::template TypeAt< 0 > >();
    }

    template<typename TBase, typename... TsDerived>
     template< size_t I, typename... Ts >
    UP<TBase> Factory<TBase, TsDerived...>::create( SubtypeIdx idx, Ts&&... args )
    {
        if( DerivedTypes::template TypeAt< I >::subtypeMeta.idx == idx )
        	return std::make_unique< typename DerivedTypes::template TypeAt< I > >( args... );

        if constexpr( I < DerivedTypes::nSize - 1 )
            return create< I + 1 >( idx, args... );

        return nullptr;
    }
}

#endif //UT_FACTORY_HPP
