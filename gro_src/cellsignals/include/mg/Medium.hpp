#ifndef MG_MEDIUM_HPP
#define MG_MEDIUM_HPP

#include "mg/defines.hpp"
#include "mg/MediumCollection.hpp"
#include "mg/DynamicBorder.hpp"
#include "mg/SignalReport.hpp"
#include "mg/Grid.hpp"
#include "mg/GridReport.hpp"
#include "mg/Diffuser.hpp"

namespace mg
{
	class Medium
	{
		public:
			using DiffuserType = DiffuserBase::DiffuserType;
            using SignalReportsDS = std::vector<SP<SignalReport>>;
           	inline static constexpr DiffuserType DF_DIFFUSER = DiffuserType::ORIGINAL;
           	using ConcsDS = Grid::ConcsDS;
           	using GlobalBorderDS = DynamicBorder::GlobalBorderDS;
           	using CoordIdx = DynamicBorder::CoordIdx;

		//---ctor, dtor
			Medium( const MediumCollection* mediumCollection, const Grid* grid, DiffuserType diffuserType = DF_DIFFUSER ) 
			: mediumCollection(mediumCollection), grid(grid)
			, signalReports( {} ), gridReport( nullptr ), diffuser( DiffuserFactory::makeDiffuser( diffuserType ) )
			, globalBorder{ { DynamicBorder::SideL::RESET_VALUE, DynamicBorder::SideR::RESET_VALUE, DynamicBorder::SideL::RESET_VALUE, DynamicBorder::SideR::RESET_VALUE } } {;} //ÑAPA

			virtual ~Medium() = default;
			Medium( const Medium& rhs ) = default;
			Medium( Medium&& rhs ) noexcept = default;
			Medium& operator=( const Medium& rhs ) = default;
			Medium& operator=( Medium&& rhs ) noexcept = default;

		//---get
			//inline const ConcsDS& getConcs( double x, double y ) const { return gridReport->checkPhysicalLimitsSpace( x, y ) ? gridReport->getConcsAtSpace( x, y ) : zeroConcs; }
			inline const ConcsDS& getConcs( double x, double y ) { /*resizeIfNeeded( zeroConcs, x, y );*/ return gridReport->getConcsAtSpace( x, y ); } //ÑAPA
			inline TReal getConc( size_t id, double x, double y ) const { return gridReport->checkPhysicalLimitsSpace( x, y ) ? gridReport->getConcAtSpace( id, x, y ) : 0.0; }
			inline const GlobalBorderDS& getGlobalBorder() const { return globalBorder; } 
			inline const GridReport* getGridReport() const { return gridReport.get(); }
			inline const SignalReport* getSignalReport( size_t id ) const { return signalReports[id].get(); }

		//---set
			inline void setConcs( const ConcsDS& concs, double x, double y ) { resizeIfNeeded( concs, x, y ); gridReport->setConcsAtSpace( concs, x, y ); }
			inline void setConc( size_t id, TReal amount, double x, double y ) { resizeIfNeeded( id, amount, x, y ); gridReport->setConcAtSpace( id, amount, x, y ); }
			inline void addConcs( const ConcsDS& concs, double x, double y ) { resizeIfNeeded( concs, x, y ); gridReport->changeConcsAtSpace( concs, x, y ); }
			inline void addConc( size_t id, TReal amount, double x, double y ) { resizeIfNeeded( id, amount, x, y ); gridReport->changeConcAtSpace( id, amount, x, y ); }

            inline void setConcsRectangle( const ConcsDS& concs, double x1, double x2, double y1, double y2 ) { resizeIfNeededCorners( concs, x1, x2, y1, y2 ); gridReport->setConcsRectangleSpace( concs, x1, x2, y1, y2 ); }
            inline void setConcRectangle( size_t id, TReal conc, double x1, double x2, double y1, double y2 ) { resizeIfNeededCorners( id, conc, x1, x2, y1, y2 ); gridReport->setConcRectangleSpace( id, conc, x1, x2, y1, y2 ); }

			inline void resizeIfNeeded( const ConcsDS& concs, double x, double y );
			inline void resizeIfNeeded( size_t id, TReal amount, double x, double y );
			inline void resizeIfNeededVirtual( const ConcsDS& concs, int row, int col );
			inline void resizeIfNeededVirtual( size_t id, TReal amount, int row, int col );
			inline void resizeIfNeededPhysical( int row, int col );

			inline void resizeIfNeededCorners( const ConcsDS& concs, double x1, double x2, double y1, double y2 ) { resizeIfNeeded( concs, x1, y1 ); resizeIfNeeded( concs, x1, y2 ); resizeIfNeeded( concs, x2, y1 ); resizeIfNeeded( concs, x2, y2 ); }
			inline void resizeIfNeededCorners( size_t id, TReal amount, double x1, double x2, double y1, double y2 ) { resizeIfNeeded( id, amount, x1, y1 ); resizeIfNeeded( id, amount, x1, y2 ); resizeIfNeeded( id, amount, x2, y1 ); resizeIfNeeded( id, amount, x2, y2 ); }

		//---API
			void init();
			inline void update( TReal dt, size_t chunkSize, size_t maxThreads ) { resize(); diffuse( dt, chunkSize, maxThreads ); updateGlobalBorder(); gridReport->update(); } 


		private:
			//links and resources
            const MediumCollection* mediumCollection;
            const Grid* grid;

            SignalReportsDS signalReports;
			SP<GridReport> gridReport;
            SP<Diffuser> diffuser;

            //ConcsDS zeroConcs;
			GlobalBorderDS globalBorder;


        //---init and clear
            void fillReports();
            void initGrid() { gridReport = std::make_shared<GridReport>( grid ); gridReport->init( mediumCollection->getElementNum<Signal>() ); } 

        //---update
			inline void resize() 
			{ 
				for( size_t s = 0; s < signalReports.size(); s++ ) 
				{
					if( signalReports[s]->getBPresent() )
					{
						signalReports[s]->update( gridReport.get() );
						//resizeIfNeededPhysical( signalReports[s]->getBorder().getX1(), signalReports[s]->getBorder().getY1() );
						//resizeIfNeededPhysical( signalReports[s]->getBorder().getX2(), signalReports[s]->getBorder().getY2() );
					}
				}
			} 


            inline void diffuse( TReal dt, size_t chunkSize, size_t maxThreads ) { for( size_t s = 0; s < signalReports.size(); s++ ) diffuse( dt, s, chunkSize, maxThreads ); }
            inline void diffuse( TReal dt, size_t id, size_t chunkSize, size_t maxThreads ) { diffuser->update( gridReport.get(), signalReports[id].get(), dt, chunkSize, maxThreads ); }
            inline void offsetBorders( uint offset ) { for( auto report : signalReports ) report->offsetBorder( offset ); }
            inline void updateGlobalBorder() { for( auto report : signalReports ) report->getBorder().registerToGlobal( globalBorder ); }
	};




//============================================ inline definitions ==========================================
	inline void Medium::resizeIfNeeded( const ConcsDS& concs, double x, double y )
	{
		resizeIfNeededPhysical( gridReport->groCoord2gridUnit( y ), gridReport->groCoord2gridUnit( x ) );
		//ut::print( "x: ", x, "  y: ", y );
		//ut::print( "concs size: ", concs.size(), "  report size: ", signalReports.size(), "  signal size: ", mediumCollection->getElementNum<Signal>() );
		resizeIfNeededVirtual( concs, gridReport->groCoord2gridUnit( y ), gridReport->groCoord2gridUnit( x ) );
	}

	inline void Medium::resizeIfNeeded( size_t id, TReal amount, double x, double y )
	{
		resizeIfNeededPhysical( gridReport->groCoord2gridUnit( y ), gridReport->groCoord2gridUnit( x ) );
		//ut::print( "x: ", x, "  y: ", y );
		//ut::print( "concs size: ", concs.size(), "  report size: ", signalReports.size(), "  signal size: ", mediumCollection->getElementNum<Signal>() );
		resizeIfNeededVirtual( id, amount, gridReport->groCoord2gridUnit( y ), gridReport->groCoord2gridUnit( x ) );
	}

	inline void Medium::resizeIfNeededVirtual( const ConcsDS& concs, int row, int col )
	{
		for( size_t c = 0; c < concs.size(); c++ )
		{
			if( concs[c] >= signalReports[c]->getSignal()->getResizeThresholdConc() )
				signalReports[c]->resizeTo( gridReport.get(), row, col, signalReports[c]->getSignal()->getPaddingUnits() );
		}
	}

	inline void Medium::resizeIfNeededVirtual( size_t id, TReal amount, int row, int col )
	{

		if( amount >= signalReports[id]->getSignal()->getResizeThresholdConc() )
			signalReports[id]->resizeTo( gridReport.get(), row, col, signalReports[id]->getSignal()->getPaddingUnits() );

	}

	inline void Medium::resizeIfNeededPhysical( int row, int col )
	{
		if( !gridReport->checkPhysicalLimitsUnit( row, col ) )
		{
			//ut::print( "resize physical needed", row, col );
			gridReport->resizePhysical();
			offsetBorders( grid->getDeltaGridSize() );
		}
	}
}

#endif //MG_MEDIUM_HPP