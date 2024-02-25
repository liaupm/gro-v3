#ifndef CG_GATE_BOOLEAN_HPP
#define CG_GATE_BOOLEAN_HPP

#include "ut/AgentState.hpp" //check()
#include "ut/GateBoolean.hpp" //wrapped Utility class

#include "cg/defines.hpp"
#include "cg/GateBase.hpp" //base class


namespace ut { class Bitset; } //getGateMask(), getPresence() wrapper methods

namespace cg
{
    class GeneticCollection;
    class GateBoolean : public GateBase
    /* Gate that evaluates a logic condition on digital inputs, wrapper for ut::GateBoolean */
    {
        public:
            using GateFunction = ut::GateBoolean::GateFunction;

            static constexpr GateFunction DF_FUNCTION = ut::GateBoolean::DF_FUNCTION;
            inline static std::vector<const GeneticElement*> DF_INPUT = {};
            inline static std::vector<bool> DF_INPUT_SIGNS = {};

        //---ctor, dtor
            GateBoolean( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const std::vector<const GeneticElement*>& input, const std::vector<bool>& inputSigns, GateFunction gateFunction = DF_FUNCTION )
            : GateBase( geneticCollection, id, name, GateBase::GateType::BGATE )
            , gate( gateFunction ), input( input ), inputSigns( inputSigns ) {;}

            virtual ~GateBoolean() = default;

        //---get
            inline GateFunction getGateFunction() const { return gate.getGateFunction(); }
            inline const std::vector<const GeneticElement*>& getInput() const { return input; }
            inline const std::vector<bool>& getInputSigns() const { return inputSigns; }
            inline const ut::Bitset& getGateMask() const { return gate.getGateMask(); }
            inline const ut::Bitset& getPresence() const { return gate.getPresence(); }
    
        //---set
            inline void setInput( const std::vector<const GeneticElement*>& xInput ) { input = xInput; } 
            inline void setAll( const std::vector<const GeneticElement*>& xInput ) { setInput( xInput ); }

        //---API
          //preprocess
            void loadDirectElemLink() override { this->addLinks( input, LinkDirection::BACKWARD ); }
            void calculateMasks( ut::CollectionManagerBase* geneticCollection ) override; //called once all the elements in the collection are loaded, linked and sorted, to convert the raw input into masks and
            uint calculateRank() override; //used to short Gates so that they are evaluated after other gates that act as their inputs. Just for efficiency, not neccessary for correct behaviour 
            bool findElemInLogic( const GeneticElement* elem ) const override; //whether the given element is an input to this gate, directly or as inputs to its inputs. Recursive, checks the whole hierarchy in a transitive way
          //run
            bool check( const ut::AgentState::BTypeSimple& rawInput, const ut::AgentState::QType& ) const override { return gate.check( rawInput ); }


        private:
          //resources
            ut::GateBoolean gate;
          //raw
            std::vector<const GeneticElement*> input; //when constructed, the input elements do not have their final absolute ids. Stored to create the masks afterwards
            std::vector<bool> inputSigns; //presence or absence
    };

    using BGate = GateBoolean;
}

#endif //CG_GATE_BOOLEAN_HPP
