#ifndef UT_ROUTINE_HPP
#define UT_ROUTINE_HPP

#include "ut/defines.hpp"
#include "ut/Bitset.hpp" //for SelectiveUpdateRoutine
#include "ut/MultiBitset.hpp" //for SelectiveUpdateRoutine


namespace ut
{
	struct Routine
	{
		public:
		//---ctor, dtor
			Routine() = default;
			virtual ~Routine() = default;
			Routine( const Routine& rhs ) = default;
			Routine( Routine&& rhs ) noexcept = default;
			Routine& operator=( const Routine& rhs ) = default;
			Routine& operator=( Routine&& rhs ) noexcept = default;

		private:
	};


/* used by Handlers (called from Agent) to call a routine for all the Handlers of the tuple
 and Reports (called from Handler) to call a rutine for all the reports of a tuple of vectors */
	
//////////////////////////////////////////////////// ROUTINES //////////////////////////////////////////////////////////////

//-------------------------------------------------preprocess

	struct OwnershipRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.setOwner( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->setOwner( params... ); }

		template< typename T, typename... TParams > 
		inline static void call( T& elem, TParams... params ) { elem.setOwner( params... ); }
	};

	struct PopulateRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.populate( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->populate( params... ); }

		template< typename T, typename... TParams > 
		inline static void call( T& elem, TParams... params ) { elem.populate( params... ); }
	};

	struct LinkRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.link( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->link( params... ); }

		template< typename T, typename... TParams > 
		inline static void call( T& elem, TParams... params ) { elem.link( params... ); }
	};



//------------------------------------------------------------run

	struct InitRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.init( params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->init( params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T& elem, TParams... params ) { elem.init( params... ); }
	};

	struct SecondInitRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.secondInit( params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->secondInit( params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T& elem, TParams... params ) { elem.secondInit( params... ); }
	};

	struct SelectiveSecondInitRoutine : public Routine
	{
		template< typename T, typename... TParams >
        inline static void call( std::vector<T>& vect, MultiBitset* mask, Bitset*, TParams&&... params )
		{ 
			size_t idx = T::ModelType::metadata.idx;

			//if( ! typeSummaryMask->checkBit( idx ) ) //TODO
				//return;
			//typeSummaryMask->setOff( idx );
            for( size_t i = mask->findFirstRel( idx ); i != mask->npos(); i = mask->findNextRel( idx, i + 1 ) )
				vect[i].secondInit( params... );
		}

		template< typename T, typename... TParams >
		inline static void call( T* elem, MultiBitset& mask, Bitset& typeSummaryMask, TParams... params ) { elem->secondInit( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T& elem, MultiBitset& mask, Bitset& typeSummaryMask, TParams... params ) { elem.secondInit( params... ); }	
	};

	struct UpdateRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.update( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->update( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T& elem, TParams... params ) { elem.update( params... ); }
	};

	struct SelectiveUpdateRoutine : public Routine
	{
		template< typename T, typename... TParams >
        inline static void call( std::vector<T>& vect, MultiBitset* mask, Bitset*, TParams&&... params )
		{ 
			if( mask->none() )
				return;

			size_t idx = T::ModelType::metadata.idx;

			//if( ! typeSummaryMask->checkBit( idx ) ) //TODO
				//return;
			//typeSummaryMask->setOff( idx );
            for( size_t i = mask->findFirstRel( idx ); i != mask->npos(); i = mask->findNextRel( idx, i + 1 ) )
				vect[i].update( params... );
		}

		template< typename T, typename... TParams >
		inline static void call( T* elem, MultiBitset& mask, Bitset& typeSummaryMask, TParams... params ) { elem->update( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T& elem, MultiBitset& mask, Bitset& typeSummaryMask, TParams... params ) { elem.update( params... ); }	
	};

	struct TickRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, TParams... params ) { for( auto& elem : vect ) elem.tick( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T* elem, TParams... params ) { elem->tick( params... ); }

		template< typename T, typename... TParams >
		inline static void call( T& elem, TParams... params ) { elem.tick( params... ); }
	};

	struct DivisionOldRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, std::vector<T>& daughterVect, TParams... params ) { for( size_t e = 0; e < vect.size(); e++ ) vect[e].divisionOld( &daughterVect[e], params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T* elem, T* daughterElem, TParams... params ) { elem->divisionOld( daughterElem, params... ); }

		template< typename T, typename... TParams >
		inline static void call( T& elem, T& daughterElem, TParams... params ) { elem.divisionOld( &daughterElem, params... ); }
	};

	struct DivisionNewRoutine : public Routine
	{
		template< typename T, typename... TParams >
		inline static void call( std::vector<T>& vect, std::vector<T>& motherVect, TParams... params ) { for( size_t e = 0; e < vect.size(); e++ ) vect[e].divisionNew( &motherVect[e], params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T* elem, T* motherElem, TParams... params ) { elem->divisionNew( motherElem, params... ); }
		
		template< typename T, typename... TParams >
		inline static void call( T& elem, T& motherElem, TParams... params ) { elem.divisionNew( &motherElem, params... ); }
	};





//////////////////////////////////////////////////////////// API ///////////////////////////////////////////////////////////////////////

    template< typename TRoutine, typename TTuple, typename... TParams >
    void applyRoutine( TTuple& tpl, TParams&&... params )
    {
    	if constexpr( std::tuple_size<TTuple>{} > 0 )
        	applyRoutine< std::tuple_size<TTuple>{} - 1, TRoutine >( tpl, std::forward<TParams>(params)... );
    }

    template< size_t I, typename TRoutine, typename TTuple, typename... TParams >
    void applyRoutine( TTuple& tpl, TParams&&... params )
    {
    	TRoutine::call( std::get<I>( tpl ), params... );
        
        if constexpr( I > 0 )
            applyRoutine< I -1, TRoutine >( tpl, std::forward<TParams>(params)... );
    }

    template< typename TRoutine, typename TTuple, typename... TParams >
    void applyRoutineDouble( TTuple& tpl1, TTuple& tpl2, TParams&&... params )
    {
    	if constexpr( std::tuple_size<TTuple>{} > 0 )
        	applyRoutineDouble< std::tuple_size<TTuple>{} - 1, TRoutine >( tpl1, tpl2, std::forward<TParams>( params )... );
    }

    template< size_t I, typename TRoutine, typename TTuple, typename... TParams >
    void applyRoutineDouble( TTuple& tpl1, TTuple& tpl2, TParams&&... params )
    {
    	TRoutine::call( std::get<I>( tpl1 ), std::get<I>( tpl2 ), params... );
        
        if constexpr( I > 0 )
            applyRoutineDouble< I -1, TRoutine >( tpl1, tpl2, std::forward<TParams>( params )... );
    }
}

#endif //UT_ROUTINE_HPP
