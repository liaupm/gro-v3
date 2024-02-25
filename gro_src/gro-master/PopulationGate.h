#ifndef POPULATIONGATE_H
#define POPULATIONGATE_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/AgentState.hpp" //AgentState in check() params
#include "ut/GateBoolean.hpp" //implementation
#include "ut/GateQuantitative.hpp" //implementation

#include "defines.h"
#include "GroElement.h" //base

//PRECOMPILED
/* #include <vector> //input, inputSigns
#include <string> //name in constructor */


namespace ut { class CollectionManagerBase; class Bitset; } //calculateMasks(), gateMask, presence of PopulationGateBoolean
class GroColleciton; //ctor
class CellsFile; //passUsedForOutput()
class PopulationGate : public GroElement
/* virtual base class */
{
	REGISTER_CLASS( "PopulationGate", "ga", GroElement::ElemTypeIdx::POP_GATE, false )

	public:
		static constexpr uint INI_RANK = 0;

	    enum class GateType
	    {
	        BGATE, QGATE, COUNT
	    };
	    using SubtypeIdx = GateType;
	    static constexpr GateType DF_TYPE = GateType::BGATE;

    //---ctor, dtor
	    virtual ~PopulationGate() = default;

    //---get
	    inline GateType getGateType() const { return gateType; }
	    inline uint getRank() const { return rank; }

	//---API
        virtual bool check( const ut::AgentState::BTypeSimple& rawInput, const ut::AgentState::QType& ) const = 0;
        virtual uint calculateRank() = 0;


	protected:
	    GateType gateType;
        uint rank; //to sort the PopulationGates so that dependencies are evaluated before (Q before B and within B by rank)
        bool bRankCalculated; //to avoid calculating the rank twice

    	PopulationGate( const GroCollection* groCollection, size_t id, const std::string& name, GateType gateType = GateType::BGATE );
};


class PopulationGateBoolean : public PopulationGate
{
    public:
        using GateFunction = ut::GateBoolean::GateFunction;

        static constexpr GateFunction DF_FUNCTION = ut::GateBoolean::DF_FUNCTION;
        inline static std::vector<const GroElement*> DF_INPUT = {};
        inline static std::vector<bool> DF_INPUT_SIGNS = {};

    //---ctor, dtor
        PopulationGateBoolean( const GroCollection* groCollection, size_t id, const std::string& name
        , const std::vector<const GroElement*>& input, const std::vector<bool>& inputSigns, GateFunction gateFunction = DF_FUNCTION )
        : PopulationGate( groCollection, id, name, PopulationGate::GateType::BGATE )
        , gate( gateFunction ), input( input ), inputSigns( inputSigns ) {;}

        virtual ~PopulationGateBoolean() = default;

    //---get
        inline GateFunction getGateFunction() const { return gate.getGateFunction(); }
        inline const std::vector<const GroElement*>& getInput() const { return input; }
        inline const std::vector<bool>& getInputSigns() const { return inputSigns; }
        inline const ut::Bitset& getGateMask() const { return gate.getGateMask(); }
        inline const ut::Bitset& getPresence() const { return gate.getPresence(); }

    //---set
        inline void setInput( const std::vector<const GroElement*>& xInput ) { input = xInput; } 

    //---API
      //precompute
        void loadDirectElemLink() override { this->addLinks( input, LinkDirection::BACKWARD ); }
        void calculateMasks( ut::CollectionManagerBase* groCollection ) override; //translates the inputs into masks and creates the ut::Gate
        uint calculateRank() override; //called manually from GroCollection on init
        void passUsedForTimers() override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForTimers(); }
        void passUsedForPlots() override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForPlots(); }
        void passUsedForOutput( const CellsFile* cellsFile ) override { for( auto inElem : input ) const_cast<GroElement*>( inElem )->passUsedForOutput( cellsFile ); }
      //run
        bool check( const ut::AgentState::BTypeSimple& rawInput, const ut::AgentState::QType& ) const override { return gate.check( rawInput ); } //wrapper
        

    protected:
      //resources
        ut::GateBoolean gate; //implementation
        std::vector<const GroElement*> input; //used when creating gate 
      //options
        std::vector<bool> inputSigns; //used when creating gate    
};


class PopulationGateQuantitative : public PopulationGate
{
    public:
        using CompOperator = ut::GateQuantitative::CompOperator;

        static constexpr TReal DF_VALUE = ut::GateQuantitative::DF_VALUE;
        static constexpr CompOperator DF_COMP_OPERATOR = ut::GateQuantitative::DF_COMP_OPERATOR;

    //---ctor, dtor
        PopulationGateQuantitative( const GroCollection* groCollection, size_t id, const std::string& name
        , const GroElement* elem, TReal value = DF_VALUE, CompOperator compOperator = DF_COMP_OPERATOR )
        : PopulationGate( groCollection, id, name, PopulationGate::GateType::QGATE )
        , gate( value, compOperator ), elem( elem ) {;}
        
        virtual ~PopulationGateQuantitative() = default;

    //---get
        inline const GroElement* getElem() const { return elem; }
        inline TReal getValue() const { return gate.getValue(); }
        inline CompOperator getCompOperator() const { return gate.getCompOperator(); }

    //---set
        inline void setElem( const GroElement* xElem ) { elem = xElem; } 

    //---API
      //precommpute
        void loadDirectElemLink() override { this->addLink( elem, LinkDirection::BACKWARD ); }
        uint calculateRank() override { return 0; } //no rank used (it is assumed that other gates are not used as input)
        void calculateMasks( ut::CollectionManagerBase* ) override { gate.setElemIdx( elem->getAbsoluteId() ); } //automatically called from GroCollection

        void passUsedForTimers() override { const_cast<GroElement*>( elem )->passUsedForTimers(); } 
        void passUsedForPlots() override { const_cast<GroElement*>( elem )->passUsedForPlots(); } 
        void passUsedForOutput( const CellsFile* cellsFile ) override { const_cast<GroElement*>( elem )->passUsedForOutput( cellsFile ); }
      //fun
        bool check( const ut::AgentState::BTypeSimple&, const ut::AgentState::QType& qState ) const override { return gate.check( qState ); } 


    protected:
      //resources
        ut::GateQuantitative gate; //implementation
        const GroElement* elem; //used when creating gate 
};

#endif // POPULATIONGATE_H
