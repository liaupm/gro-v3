#ifndef CG_GROWTHHANDLER_HPP
#define CG_GROWTHHANDLER_HPP

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <limits> */


namespace cg
{
	struct GrowthHReportIdx
    {
    	enum ReportTypeIdx : size_t { GROWTH, COUNT };
    };

    class Genome; //owner
    class Strain; //model
    class GrowthHandler;
	class GrowthReport : public GeneticReport< Strain, GrowthHandler >
	{
    	public: 
    		friend GrowthHandler;
            static constexpr TReal INI_BASE_GR = 0.0;
            static constexpr TReal INI_LAST_VAL = 0.0;
            static constexpr TReal INI_LAST_UPDATE_TIME = 0.0;
        
        //---ctor, dtor
			GrowthReport( const Strain* model, GrowthHandler* ownerHandler );
    		virtual ~GrowthReport() = default;

    	//---get
    		inline TReal getDelta() const { return delta; } 
    		inline TReal getLastVal() const { return lastVal; }
            inline TReal getLastUpdateTime() const { return lastUpdateTime; }

    	//---API
          //common
    		void link() {;}
            bool init();
            inline bool secondInit() { return update( nullptr ); }
			void reset();
            void partialReset();
            inline bool divisionOld( GrowthReport* ) { return true; }
            inline bool divisionNew( GrowthReport* ) { return true; }
            bool update( const EventType* );
			bool tick();


        private:
            TReal delta; //change in volume accumulated during the time step. To account for changes in the growth rate within a time step
            TReal lastVal; //current growth rate
        	TReal lastUpdateTime; //to average the value in case it changes within a time step
        	TReal lastReportedVal; //the last forwarded value. Used to update forward elements when the change is enough, according to the sensitivity param (efficiency)

        	void calculate(); //calculate current growth rate
			void accumulateDelta(); //multiply the lastVal by the time interval and add to delta
	};


	class GrowthHandler : public GeneticHandler<>
	{
		REGISTER_CLASS( "GrowthHandler", "grH", GeneticHandler<>::HandlerTypeIdx::H_GROWTH )

		public:
			friend GrowthReport;
			using HandlerBaseType = GrowthHandler::Handler;

			static constexpr TReal INI_LENGTH = 0.0;
			static constexpr TReal INI_VOLUME = 0.0;
			static constexpr TReal INI_D_LENGTH = 0.0;
			static constexpr TReal INI_D_VOLUME = 0.0;
			static constexpr TReal INI_DIVISION_VOLUME = std::numeric_limits<TReal>::max();
			static constexpr TReal INI_LAST_DIV_FRANCTION = 0.0;
			static constexpr bool DF_BTRACK_LENGTH = true;


		//---ctor, dtor
			GrowthHandler( OwnerType* owner, const GeneticCollection* collection );
			//GrowthHandler( const GrowthHandler& original, OwnerType* owner );
			virtual ~GrowthHandler() = default;
			GrowthHandler( const GrowthHandler& rhs ) = default;
			GrowthHandler& operator=( const GrowthHandler& rhs ) = default; 
			GrowthHandler( GrowthHandler&& rhs ) noexcept = default;
			GrowthHandler& operator=( GrowthHandler&& rhs ) noexcept = default; 

		//---get
			inline TReal getGrowthRate() const { return growthReport.getLastVal(); }
			inline TReal getGenerationTime() const { return strain->calculateGenerationTime( iniVolume, divisionVolume, getGrowthRate() ); }

			inline TReal getVolume() const { return volume; } 
        	inline TReal getDVolume() const { return dVolume; }
        	inline TReal getDivisionVolume() const { return divisionVolume; }
        	inline TReal getLength() const { return DF_BTRACK_LENGTH ? length : strain->vol2length( volume ); }
        	inline TReal getTotalLength() const { return DF_BTRACK_LENGTH ? totalLength : strain->vol2totalLength( volume ); }
        	inline TReal getDLength() const { return DF_BTRACK_LENGTH ? dLength : strain->vol2length( dVolume ); }
        	
        	inline bool getDivSizeReached() const { return volume >= divisionVolume; } 
			inline bool getBMustDie() const { return bMustDie; }
			inline bool getBMustDivide() const { return bMustDivide; }
			inline bool getBMustGrow() const { return volume < divisionVolume; }

			inline TReal getLastDivisionFraction() const { return lastDivisionFraction; }
			inline TReal getTotalLengthFraction() const { return totalLengthFraction; }

		//---API
		  //common
			void fillSpecialReports() override { growthReport = GrowthReport( strain, this ); } 
			void setOwnershipReports() override { growthReport.setOwner( this ); }
			void link() override { growthReport.link(); }
			bool init() override;
			bool secondInit( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask ) override { return update( mask, typeSummaryMask, nullptr ); } 
			bool update( ut::MultiBitset* mask, ut::Bitset* typeSummaryMask, const GeneticHandler<>::EventType* event ) override;
			void tick() override; //grow and check for division conditions and set bMustDivide
			bool divisionOld( HandlerBaseType* daughter ) override;
      		bool divisionNew( HandlerBaseType* ) override;
          //special
			void preInitVol(); //called by Genome before the init() of handlers. Required to add the right initial amounts of qplasmids when copy numbers defined in concentration
			inline bool checkDivisionSizeReached() const { return volume >= divisionVolume; } 

			
		private: 
		  //resources
			const Strain* strain;
			GrowthReport growthReport;
		  //state
			TReal volume, iniVolume, dVolume;
			TReal lastReportedVol, lastReportedDVol; //the last forwarded values, to forward only when the change is enough (sensitivity param), for efficiency
			TReal divisionVolume; //future 
			TReal length, dLength, totalLength; //length = only the cilinder, total = all, including tips. CellEngine uses dLenght to grow body and indirectly totalLength for totalLengthFraction calculation (used to divide body)
			
			bool bMustDie;
			bool bMustDivide;
			bool bDivisionVolReached;
	
			TReal lastDivisionFraction; //in volume, used by split functions in LogicHandler (Odes) and PlasmidsHandler (plasmids) and Horizontalhandler (conjugation counters)
			TReal totalLengthFraction; //used by CellEngine

			void updateVolumeDivision( TReal xVolume ); //apply divisionFraction to set the new volume of daughter cells on division and update all the dependent state vars
			void grow(); //calculate dvolume from current volume and gr, increase the volume and update dependent vars
			void checkDeath(); //check death conditions and set bMustDie
			void recalculateLength(); //calculate length, dLength and totalLength from volume, dVolume
	};
}

#endif //CG_GROWTHHANDLER_HPP