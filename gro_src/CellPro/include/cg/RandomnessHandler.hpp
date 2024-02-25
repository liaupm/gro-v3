#ifndef CG_RANDOMNESSHANDLER_HPP
#define CG_RANDOMNESSHANDLER_HPP

#include "cg/defines.hpp"
#include "cg/GeneticReport.hpp" //base
#include "cg/GeneticHandler.hpp" //base

/*PRECOMPILED
#include <vector> //reports
#include <tuple> //get<> */


namespace cg
{
	struct RandomnessHReportIdx
    {
    	enum ReportTypeIdx : size_t { RANDOMNESS, COUNT };
    };

	class Genome;
	class Randomness; //model
    class RandomnessHandler;
	class RandomnessReport : public GeneticReport< Randomness, RandomnessHandler >
    { 
    	public:
    		friend RandomnessHandler;
        	static constexpr TReal INI_SAMPLING_TIME = -1.0; //no re-sampling
        	
		//---ctor, dtor
            RandomnessReport( const Randomness* model, RandomnessHandler* ownerHandler ) : GeneticReport( model, ownerHandler ), nextSamplingTime( INI_SAMPLING_TIME ) {;}
            virtual ~RandomnessReport() = default;

    	//---API
          //common
            void link() {;}
            bool init();
            inline bool secondInit() { return true; }
            inline void reset() { nextSamplingTime = INI_SAMPLING_TIME; }
            bool divisionOld( RandomnessReport* );
            bool divisionNew( RandomnessReport* );
            bool update( const EventType* event );
            inline bool tick() { return true; }

        private:
            TReal nextSamplingTime; //time when the value will be re-sampled. Matches the time of the event
    };


	class RandomnessHandler : public GeneticHandler< ut::ReportsDS<RandomnessReport> >
	{
		REGISTER_CLASS( "RandomnessHandler", "rndH", HandlerTypeIdx::H_RANDOMNESS )

		public:
			using GeneticHandlerBaseType = RandomnessHandler::GeneticHandler;
			using GeneticHandlerBaseType::HandlerBaseType;

		//---ctor, dtor
			RandomnessHandler( Genome* owner, const GeneticCollection* collection ) : GeneticHandler( owner, collection ) {;}
			virtual ~RandomnessHandler() = default;
			RandomnessHandler( const RandomnessHandler& rhs ) = default;
			RandomnessHandler& operator=( const RandomnessHandler& rhs ) = default;
			RandomnessHandler( RandomnessHandler&& rhs ) noexcept = default;
			RandomnessHandler& operator=( RandomnessHandler&& rhs ) noexcept = default;

		//---get
			inline const std::vector<RandomnessReport>& getRandomnessReports() const { return std::get<RandomnessHReportIdx::RANDOMNESS>( reports ); }
			inline std::vector<RandomnessReport>& getRandomnessReportsEdit() { return std::get<RandomnessHReportIdx::RANDOMNESS>( reports ); }
		
		//---set

		//---API
	};
}

#endif //CG_RANDOMNESSHANDLER_HPP