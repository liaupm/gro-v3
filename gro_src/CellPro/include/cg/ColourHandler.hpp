#ifndef CG_COLOURHANDLER_HPP
#define CG_COLOURHANDLER_HPP

#include "cg/defines.hpp"

#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports */


namespace cg
{
	struct ColourHReportIdx
    {
    	enum ReportTypeIdx : size_t { COLOUR, COUNT };
    };

	class Genome;
	class CellColour;
    class ColourHandler;
    class ColourReport : public GeneticReport< CellColour, ColourHandler >
    {
        public:
        	friend ColourHandler;
        	static constexpr bool INI_B_ACTIVE = false;
            static constexpr TReal INI_VALUE = 0.0;
            static constexpr TReal INI_TARGET_VALUE = 0.0;
            static constexpr TReal INI_DELTA = 0.0;
		
		//---ctor, dtor
            ColourReport( const CellColour* model, ColourHandler* ownerHandler ) 
            : GeneticReport( model, ownerHandler )
            , bActive( INI_B_ACTIVE ), value( INI_VALUE ), targetValue( INI_TARGET_VALUE ), delta( INI_DELTA ) {;}
            
            ColourReport( const ColourReport& original, ColourHandler* ownerHandler ) 
            : GeneticReport( original.model, ownerHandler )
            , bActive( original.bActive ), value( original.value ), targetValue( original.targetValue ), delta( original.delta ) {;}
            
            virtual ~ColourReport() = default;

        //---get
            inline bool getBActive() const { return bActive; }
            inline bool getIsActive() const { return bActive; }
            inline TReal getValue() const { return value; }

        //---API
          //common
            void link() {;}
            bool init();
            inline bool secondInit() { return update( nullptr ); }
            inline void reset() { value = INI_VALUE; }
            inline bool divisionOld( ColourReport* ) { return true; }
            inline bool divisionNew( ColourReport* ) { return true; }
            bool update( const EventType* );
			bool tick();

		  //special
			bool calculateTargetValue();
			bool calculateDelta();
			inline void addTo( ut::RgbColour& totalColour ) const { totalColour.add( model->getRgbColour() * value ); }
            

        private:
        	bool bActive; //only for delta mode
        	TReal value;
        	TReal targetValue; //only for delta mode
        	TReal delta; //only for delta mode
    };


	class ColourHandler : public GeneticHandler< ut::ReportsDS<ColourReport> >
	{
		REGISTER_CLASS( "ColourHandler", "colH", HandlerTypeIdx::H_COLOUR )

		public:
		//---ctor, dtor
			ColourHandler( OwnerType* owner, const GeneticCollection* collection ): GeneticHandler(owner, collection ) {;}
			//ColourHandler( const ColourHandler& original, OwnerType* owner ) : GeneticHandler( original, owner ) {;}
			virtual ~ColourHandler() = default;
			ColourHandler( const ColourHandler& rhs ) = default;
			ColourHandler& operator=( const ColourHandler& rhs ) = delete;
			ColourHandler( ColourHandler&& rhs ) noexcept = default;
			ColourHandler& operator=( ColourHandler&& rhs ) noexcept = default;

		//---get
			inline const std::vector<ColourReport>& getColourReports() const { return std::get< ColourHReportIdx::COLOUR >( this->reports ); }
			inline std::vector<ColourReport>& getColourReportsEdit() { return std::get< ColourHReportIdx::COLOUR >( this->reports ); }
			inline const ut::RgbColour& getTotalColour() const { return totalColour; }

		//---set

		//---API
		  //common
			bool init() override { HandlerBaseType::init(); calculateTotalColour(); return true; } 
			void tick() override { HandlerBaseType::tick(); calculateTotalColour(); }
		  //special
			void calculateTotalColour();

			
		private:
			ut::RgbColour totalColour; //combination of all the Reports = the colour of the cell
	};
}

#endif //CG_COLOURHANDLER_HPP