#ifndef CG_GATE_QUANTITATIVE_HPP
#define CG_GATE_QUANTITATIVE_HPP

#include "ut/AgentState.hpp" //check()
#include "ut/GateQuantitative.hpp" //wrapped Utility class

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //input
#include "cg/GateBase.hpp" //base class


namespace cg
{
    class GeneticCollection;
    class GateQuantitative : public GateBase
    /* Gate that evaluates a quantitative condition (inequation), wrapper for ut::GateQuantitative */
    {
        public:
            using CompOperator = ut::GateQuantitative::CompOperator;

            static constexpr TReal DF_VALUE = ut::GateQuantitative::DF_VALUE;
            static constexpr CompOperator DF_COMP_OPERATOR = ut::GateQuantitative::DF_COMP_OPERATOR;


        //---ctor, dtor
            GateQuantitative( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const GeneticElement* input, TReal value = DF_VALUE, CompOperator compOperator = DF_COMP_OPERATOR )
            : GateBase( geneticCollection, id, name, GateBase::GateType::QGATE )
            , gate( value, compOperator ), input( input ) {;}
            
            virtual ~GateQuantitative() = default;

        //---get
            inline const GeneticElement* getInput() const { return input; }
            inline TReal getValue() const { return gate.getValue(); }
            inline CompOperator getCompOperator() const { return gate.getCompOperator(); }

        //---set
            inline void setInput( const GeneticElement* xInput ) { input = xInput; } 
            inline void setAll( const GeneticElement* xInput ) { setInput( xInput ); }
            
        //---API
          //precompute
            void loadDirectElemLink() override { this->addLink( input, LinkDirection::BACKWARD ); }
            void precompute() override { gate.setElemIdx( input->getAbsoluteId() ); } //set the absolute id of the input once it is final (after ranking and sorting of the collection)
            uint calculateRank() override; //used to short Gates so that they are evaluated after other gates that act as their inputs. Just for efficiency, not neccessary for correct behaviour 
          //fun
            bool check( const ut::AgentState::BTypeSimple&, const ut::AgentState::QType& qState ) const override { return gate.check( qState ); } 
            bool findElemInLogic( const GeneticElement* elem ) const override { return this == elem || input->findElemInLogic( elem ); } //whether the given element is an input to this gate, directly or as inputs to its inputs. Recursive, checks the whole hierarchy in a transitive way


        private:
          //resources
            ut::GateQuantitative gate;
          //raw
            const GeneticElement* input; //when constructed, the input elements do not have their final absolute ids. Stored to pass the id to the wrapped gate afterwards
    };

    using QGate = GateQuantitative;
}

#endif //CG_GATE_QUANTITATIVE_HPP
