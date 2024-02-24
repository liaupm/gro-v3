#ifndef UT_HANDLER_HPP
#define UT_HANDLER_HPP

#include "ut/defines.hpp"
#include "ut/Routine.hpp" //apply routines to reports

/*PRECOMPILED 
#include<tuple> //reports */


namespace ut
{
	class Bitset; //update() changes
	class MultiBitset; //update() changesSummary

	template< typename TReport >
	using ReportsDS = std::vector<TReport>; //DS used to store reports by default, accessed from the class definitions of concrete Handlers

	class HandlerBase //common base type of all the handlers
	{};

	template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
	class Handler : public HandlerBase
	{
		public: 
		///////////////////////////////////////////////////////////// NESTED CLASSES //////////////////////////////////////////
		  //to get the type of the model of a Report
			template< typename T >
			struct ModelTypeExtractor //for objects
			{
				using Type = typename T::ModelType;
			};

			template< typename T >
			struct ModelTypeExtractor< std::vector<T> > //fir vectors of objects
			{
				using Type = typename std::vector<T>::value_type::ModelType;
			};

			template< typename T >
			struct ModelTypeExtractor< T* > //for pointers
			{
				using Type = typename T::ModelType;
			};

			template< typename T >
			using model_t = typename ModelTypeExtractor<T>::Type;


		///////////////////////////////////////////////////////////// HANDLER //////////////////////////////////////////
			using CollectionType = TCollection;
			using OwnerType = TOwner;
			using EventType = TEvent;
			using ElemTypeIdx = typename CollectionType::ElemTypeIdx;
			using ReportsTuple = std::tuple< TReports... >;

			static constexpr size_t REPORT_NUM = sizeof...( TReports );

		//---ctor, dtor
			virtual ~Handler() = default;
			Handler( const Handler& rhs ) = default;
			Handler& operator=( const Handler& rhs ) = default;

		//---get
			const CollectionType* getCollection() const { return collection; }
			inline OwnerType* getOwner() { return owner; }
			inline const ReportsTuple& getReports() const { return reports; }
			inline ReportsTuple& getReportsEdit() { return reports; }

		//---set
			inline void setOwner( OwnerType* xOwner ) { owner = xOwner; }

		//---API
		  //precompute
			virtual void populate() { fillReports(); }
			virtual void setOwnershipReports() { applyRoutine<OwnershipRoutine>( reports, this ); } //assign this as the owner of its Reports
			
		  //common
			virtual void fillSpecialReports() {;} //overrided by Handlers that do not use the default Report DS
			virtual void link() { applyRoutine<LinkRoutine>( reports ); } //create links between Reports, potentially from other Handlers
			virtual bool init() { applyRoutine<InitRoutine>( reports ); return false; } //called for newly externally created cells, not from cell division
			virtual bool secondInit( MultiBitset* mask, Bitset* typeSummaryMask ) { applyRoutine<SelectiveSecondInitRoutine>( reports, mask, typeSummaryMask ); return false; } //called for newly externally created cells, not from cell division
			virtual void tick() { applyRoutine<TickRoutine>( reports ); } //called once every time step
			virtual bool update( MultiBitset* mask, Bitset* typeSummaryMask, const TEvent* event ) { applyRoutine<SelectiveUpdateRoutine>( reports, mask, typeSummaryMask, event ); return false; } //called whenever there are changes affecting the report
			virtual bool divisionOld( Handler* daughter ) { applyRoutineDouble<DivisionOldRoutine>( reports, daughter->getReportsEdit() ); return false; } //called for the mother cell on division
			virtual bool divisionNew( Handler* mother ) { applyRoutineDouble<DivisionNewRoutine>( reports, mother->getReportsEdit() ); return false; } //called for the daughter cell on division


		protected:
			const CollectionType* collection; //CollectionManager-derived
			OwnerType* owner; //Agent-derived
			ReportsTuple reports;


			Handler( OwnerType* owner, const CollectionType* collection ) : collection(collection), owner(owner) {;}
			//Handler( const Handler& mother, OwnerType* owner ) : owner(owner), collection( mother.collection ) {;}

		  //report handling
			void fillReports();
			template< size_t I >
            void fillReports();
            
			template< typename ModelType, typename ReportType >
            void fillReportContainer( const typename CollectionType::template ElementContainer<ModelType>& modelContainer, std::vector<ReportType>& reportContainer );
            template< typename ReportType >
            void copyReportContainer( const std::vector<ReportType>& originalContainer, std::vector<ReportType>& reportContainer );
           	template<typename ReportType>
    		void linkReportContainer( std::vector<ReportType>& reportContainer );
	};




///////////////////////////////////////////////// DEFINITION OF METHODS ////////////////////////////////////////////////////////////////////////////////////////////
	template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
	void Handler<TCollection, TOwner, TEvent, TReports...>::fillReports()
	{
		fillSpecialReports();
		if constexpr( REPORT_NUM > 0 )
			fillReports<REPORT_NUM - 1>();
	}

	template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
	 template< size_t I >
    void Handler<TCollection, TOwner, TEvent, TReports...>::fillReports()
    {
    	using ReportContainerType = typename std::tuple_element_t< I, ReportsTuple >;
    	using ModelType = model_t<ReportContainerType>;

    	const auto& modelContainer = collection->template getAll< ModelType >();
    	fillReportContainer( modelContainer, std::get<I>( reports ) );
    	if constexpr( I > 0 )
    		fillReports< I - 1 >();
    }


	template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
	 template<typename ModelType, typename ReportType>
    void Handler<TCollection, TOwner, TEvent, TReports...>::fillReportContainer( const typename CollectionType::template ElementContainer<ModelType>& modelContainer, std::vector<ReportType>& reportContainer )
    {
        for( size_t e = 0; e < modelContainer.size(); e++ )
            reportContainer.emplace_back( modelContainer.getElement( e ), static_cast< typename ReportType::HandlerType* >( this ) );
    }

	template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
     template< typename ReportType >
    void Handler<TCollection, TOwner, TEvent,TReports...>::copyReportContainer( const std::vector<ReportType>& originalContainer, std::vector<ReportType>& reportContainer )
    {
    	for( size_t e = 0; e < originalContainer.size(); e++ )
            reportContainer.emplace_back( originalContainer[e], static_cast< typename ReportType::HandlerType* >( this ) );
    }

    template< typename TCollection, typename TOwner, typename TEvent, typename... TReports >
	 template<typename ReportType>
    void Handler<TCollection, TOwner, TEvent, TReports...>::linkReportContainer( std::vector<ReportType>& reportContainer )
    {
        for( auto& report : reportContainer )
        {
            report.linkForward( owner );
            report.linkBackward( owner );
        }
    }
}

#endif //UT_HANDLER_HPP
