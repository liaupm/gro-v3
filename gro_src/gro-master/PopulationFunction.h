#ifndef POPULATIONFUNCTION_H
#define POPULATIONFUNCTION_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/AgentState.hpp" //ut::AgentState::QType at evaluate()
#include "ut/MathFunction.hpp" //implementation, ::Type

#include "defines.h"
#include "GroElement.h" //base class

//PRECOMPILED
/* #include <vector> //input, params
#include <string> //name in constructor */


class GroCollection; //ctor
class CellsFile; //passUsedForOutput()
class PopulationFunction : public GroElement
{
	REGISTER_CLASS( "PopulationFunction", "fun", GroElement::ElemTypeIdx::POP_FUNCTION, true )

	public:
        using MathFunctionType = ut::MathFunction<>;
        using Type = MathFunctionType::Type;
        static constexpr MathFunctionType::Type DF_FUNTION_TYPE = MathFunctionType::Type::SUM;
        static constexpr TReal DF_MIN = ut::MathFun::DF_MIN;
        static constexpr TReal DF_MAX = ut::MathFun::DF_MAX;
        static constexpr int DF_ROUND_PLACES = ut::MathFun::DF_ROUND_PLACES;
        static constexpr uint INI_RANK = 0;


    //---ctor, dtor
        PopulationFunction( const GroCollection* groCollection, size_t id, const std::string& name
        , const std::vector<const GroElement*>& input, const std::vector<TReal>& params, MathFunctionType::Type funType = DF_FUNTION_TYPE, TReal min = DF_MIN, TReal max = DF_MAX, int roundPlaces = DF_ROUND_PLACES );

    	virtual ~PopulationFunction() = default;

    //---get
    	const std::vector<const GroElement*>& getInput() const { return input; }
        uint getRank() const { return rank; }

    //---set
        inline void setInput( const std::vector<const GroElement*>& xInput ) { input = xInput; }

    //---API
      //precompute
        void loadDirectElemLink() override { this->addLinks( input, LinkDirection::BACKWARD ); }
        void precompute() override { createMathFunction(); } //automatically called by GroCollection on initialization
	    uint calculateRank(); //has to be called manually from GroCollection
	    
	    void passUsedForTimers() override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForTimers(); } 
        void passUsedForPlots() override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForPlots(); }
        void passUsedForOutput( const CellsFile* cellsFile ) override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForOutput( cellsFile ); }
	  //run
        inline TReal evaluate( const ut::AgentState::QType& quantitativeElements ) const { return mathFun->calculate( quantitativeElements ); } //a wrapper


	private:
      //resources
        SP<MathFunctionType> mathFun; //implementation
        std::vector<const GroElement*> input; //used when creating mathFun
      //state
        uint rank; //to sort the PopulationFunctions so that dependencies are evaluated before
        bool bRankCalculated; //to avoid calculating the rank multiple times


        void createMathFunction(); //called when the input elements have the right absolute id
};

#endif // POPULATIONFUNCTION_H
