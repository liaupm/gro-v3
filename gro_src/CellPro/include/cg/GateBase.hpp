#ifndef CG_GATE_BASE_HPP
#define CG_GATE_BASE_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/AgentState.hpp" //check()

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class


namespace cg
{
    class GeneticCollection;
    class GateBase : public GeneticElement
    /* Virtual base class for GateBoolean and GateQuantitative, as a common interface */
    {
        REGISTER_CLASS( "Gate", "ga", GeneticElement::ElemTypeIdx::GATE, false )
       
        public:
            enum class GateType
            {
                BGATE, QGATE, COUNT
            };
            using SubtypeIdx = GateType;

            static constexpr GateType DF_TYPE = GateType::BGATE;
            static constexpr uint INI_RANK = 0;


        //---ctor, dtor
            virtual ~GateBase() = default;

        //---get
            inline GateType getGateType() const { return gateType; }
            inline uint getRank() const { return rank; }

        //---API
          //precompute
            virtual uint calculateRank() = 0;
          //run
            virtual bool check( const ut::AgentState::BTypeSimple&, const ut::AgentState::QType& ) const { return false; }


        protected:
          //options
            GateType gateType;
          //state
            uint rank; //used to short Gates so that they are evaluated after other gates that act as their inputs. Just for efficiency, not neccessary for correct behaviour
            bool bRankCalculated;

          //fun
            GateBase( const GeneticCollection* geneticCollection, size_t id, const std::string& name, GateType gateType = DF_TYPE )
            : GeneticElement::GeneticElement( id, name, GeneticElement::BElementIndex::GATE, false, geneticCollection, ElemCategory::LOGIC )
            , gateType( gateType ), rank( INI_RANK ), bRankCalculated( false ) {;}
    };

    using Gate = GateBase;
}

#endif //CG_GATE_BASE_HPP
