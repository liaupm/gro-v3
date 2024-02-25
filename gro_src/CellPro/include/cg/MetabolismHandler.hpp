#ifndef CG_METABOLISMHANDLER_HPP
#define CG_METABOLISMHANDLER_HPP

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports */


namespace cg
{
	struct MetabolismHReportIdx
    {
    	enum ReportTypeIdx : size_t { METABOLITE, FLUX, COUNT };
    };

    class Genome;
    class Metabolite; //model
    class Flux; //model
    class MetabolismHandler;
	class FluxReport : public GeneticReport< Flux, MetabolismHandler >
	{
	    public:
	    	friend MetabolismHandler;
	    	friend class MetaboliteReport;
	    	static constexpr bool INI_B_EXTERNAL_CONDITION = false;
	    	static constexpr bool INI_B_ACTIVE = false;
	        static constexpr TReal INI_VALUE = 0.0;

	        
	    //---ctor, dtor
	        FluxReport( const Flux* model, MetabolismHandler* ownerHandler );
	        virtual ~FluxReport() = default;

	    //---get
	        inline bool getBActive() const { return bActive; }

	    //---API
	      //common
	    	void link() {;}
	    	bool init();
	    	inline bool secondInit() { return update( nullptr ); }
	    	inline void reset() { bExternalCondition = INI_B_EXTERNAL_CONDITION; bActive = INI_B_ACTIVE; rawValue = INI_VALUE; value = INI_VALUE; } 
	    	inline bool divisionOld( FluxReport* ) { return true; }
	        inline bool divisionNew( FluxReport* ) { return true; }
	        bool update( const EventType* );
	        inline bool tick() { return true; }
	        void exposeParams( bool bInit );

	        
	    private:
	    	bool bExternalCondition; //simple threshold AND sgate
	        bool bActive; //bExternalCondition AND internalGate
	        TReal rawValue; //flux amount before restriction for negative fluxes
	        TReal value; //final flux amount after restriction of negative fluxes to available amount

	        void fullCheck( bool bForward ); //update bExternalCondition, bActive and rawValue
        	bool checkExternalState( TReal conc ); //update bExternalCondition
        	bool checkState(); //update bActive
        	bool calculateFlux(); //update rawValue
        	void updateValue( TReal newVal ); //set value to an external restricted value (usually calculated from rawValue)
        	void commitValue(); //just set value to rawValue (no restriction)
	};


	class MetaboliteReport : public GeneticReport< Metabolite, MetabolismHandler >
	{
	    public:
	    	friend class MetabolismHandler;

	    	static constexpr bool INI_B_LIMITED = false;
	    	static constexpr TReal INI_DELTA = 0.0;
            static constexpr TReal INI_LAST_VAL = 0.0;
            static constexpr TReal INI_LAST_UPDATE_TIME = 0.0;

	    //---ctor, dtor
	        MetaboliteReport( const Metabolite* model, MetabolismHandler* ownerHandler );
	        virtual ~MetaboliteReport() = default;

	    //---API
	      //common
	    	void link();
	    	bool init();
	    	inline bool secondInit() { return update( nullptr ); }
	    	void reset();
	    	void partialReset();
	    	inline bool divisionOld( MetaboliteReport* ) { return true; }
	        inline bool divisionNew( MetaboliteReport* ) { return true; }
	        bool update( const EventType* );
	        inline bool tick() { return true; }

	      //special
	        inline bool updateConc( const std::vector<TReal>& concs ) { return updateConc( concs[ model->getRelativeId() ] ); }
	        bool updateConc( TReal conc ); //set the conc marker to the current medium conc

	        
	    private:
	    	std::vector<FluxReport*> fluxReports;

	    	bool bLimited; //whether it is currently negative and restricted by the available amount during a fraction of time step, so that it requires update even if the medium concentration doesn't change
	    	bool bExternalChange; //whether the external concentration of signal changed from the previous update
	    	TReal rawValue; //sum of the raw values of all its fluxes, before restriction

	    	TReal delta; //accumulated exchanged amount during the current time step (in case there are changes in the amount within the time step)
        	TReal lastVal; //current value (rawValue after restrictions), derivative
        	TReal lastUpdateTime; //to average the potential changes in amount within the time step

			bool collectFluxes(); //update rawValue
			void limitToMedium(); //restriction of negative fluxes to the available amount
			void accumulateDelta(); //add lastVal multiplied by elapsed time to delta and reset lastUpdateTime
			bool integrate(); //add the final restricted value to the totalFluxes vector for medium modification
	};


	class MetabolismHandler : public GeneticHandler< ut::ReportsDS<MetaboliteReport>, ut::ReportsDS<FluxReport> >
	{
		REGISTER_CLASS( "MetabolismHandler", "metH", HandlerTypeIdx::H_METABOLISM )

		public:
			friend FluxReport;
			friend MetaboliteReport;
			static constexpr TReal INI_FLUX = 0.0;
			
		//---ctor, dtor
			MetabolismHandler( OwnerType* owner, const GeneticCollection* collection )
			: GeneticHandler( owner, collection ) 
			, totalFluxes( collection->getElementNum<Metabolite>() - 1, INI_FLUX ) {;}

			virtual ~MetabolismHandler() = default;
			MetabolismHandler( const MetabolismHandler& rhs ) = default;
			MetabolismHandler& operator=( const MetabolismHandler& rhs ) = default;
			MetabolismHandler( MetabolismHandler&& rhs ) noexcept = default;
			MetabolismHandler& operator=( MetabolismHandler&& rhs ) noexcept = default;

		//---get
			inline const std::vector<FluxReport>& getFluxReports() const { return std::get< MetabolismHReportIdx::FLUX >(reports); }
			inline std::vector<FluxReport>& getFluxReportsEdit() { return std::get< MetabolismHReportIdx::FLUX >(reports); }
			inline const std::vector<MetaboliteReport>& getMetaboliteReports() const { return std::get< MetabolismHReportIdx::METABOLITE >(reports); }
			inline std::vector<MetaboliteReport>& getMetaboliteReportsEdit() { return std::get< MetabolismHReportIdx::METABOLITE >(reports); }

			inline const std::vector<TReal>& getTotalFluxes() const { return totalFluxes; }
			inline std::vector<TReal>& getTotalFluxesEdit() { return totalFluxes; }
			TReal getBiomassFlux() const;

		//---API
		  //common
			void tick() override;
		  //special
			inline void initExternal( const std::vector<TReal>& concs ) { updateConcs( concs ); } //initial update before the init() of handlers
            inline void updateConcs( const std::vector<TReal>& concs ) { for( size_t m = 0; m < collection->getElementNum<Metabolite>() - 1; m++ ) getMetaboliteReportsEdit()[ m ].updateConc( concs[ m ] ); }
            void applyLysisFluxes(); //called on cell death
            
            
		private:
			std::vector<TReal> totalFluxes;	

			inline void resetTotalFluxes() { std::fill( totalFluxes.begin(), totalFluxes.end(), INI_FLUX ); } 
	};
}

#endif //CG_METABOLISMHANDLER_HPP