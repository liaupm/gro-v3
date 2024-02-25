#ifndef CG_GENETIC_ELEMENT_HPP
#define CG_GENETIC_ELEMENT_HPP

#include "ut/Metadata.hpp"
#include "ut/Param.hpp"
#include "ut/LinkedElement.hpp" //base class
#include "ut/ElementIndex.hpp" //template arg

#include "cg/defines.hpp"


namespace cg
{
    class GeneticElementIdx : ut::ElementIndex
    {
        public:
            enum ElemTypeIdx : size_t
            {
                BMARKER, QMARKER, EVENT_MARKER, RANDOMNESS, FUNCTION, GATE, ODE, HISTORIC, CELL_COLOUR
                , MOLECULE, REGULATION, OPERON, METABOLITE, FLUX
                , PILUS, ORI_T, PARTITION_SYSTEM, COPY_CONTROL, ORI_V
                , PLASMID, MUTATION, MUTATION_PROCESS, STRAIN, CELL_TYPE
                , HORIZONTAL_COMMON, COUNT
            };
    };

    class GeneticCollection;
    class GeneticElement : public ut::LinkedElement<GeneticElement, GeneticElementIdx>
    {
        public:
        	using ParamType = ut::Param<GeneticElement>;
        	using ParamDataType = ut::ParamData<GeneticElement>;

            enum class ElemCategory
            {
                DNA_CONTAINER, DNA_CONTENT, DNA_BOTH, FREE_ELEM, FUNCTIONAL, LOGIC, INITIALIZER, HANDLER_COMMON, COUNT
            };

            static constexpr bool DF_B_FORWARD_LOGIC = false; 

        //---ctor, dtor
            virtual ~GeneticElement() = default;

        //---get
            inline ElemCategory getElemCategory() const { return elemCategory; }
            inline bool getBPhysical() const { return bPhysical; }
            inline bool getBForwardLogic() const { return bForwardLogic; }
            inline bool getHasForwardLogic() const { return bForwardLogic; }
            inline bool getBForwardLogicContent() const { return bForwardLogicContent; }
            inline bool getHasForwardLogicContent() const { return bForwardLogicContent; }

            inline bool getActiveForCellType( size_t cellTypeIdx ) const { return activeForCellTypes[ cellTypeIdx ]; }

        //---API
          //precompute
            virtual void precompute() override { calculateBForwardLogic(); calculateBForwardLogicContent(); calculateActiveForCellTypes(); }

            bool calculateBForwardLogic();
            void calculateBForwardLogicContent();
            void calculateActiveForCellTypes();

            virtual bool findElemInLogic( const GeneticElement* elem ) const  { return this == elem; }


        protected:
            ElemCategory elemCategory;
            bool bPhysical;
            bool bForwardLogic; //whether it contains gates or functions among its direct forward elements
            bool bForwardLogicContent; //whether it contains gates or functions among its forward elements or those of its contents
            bool bForwardLogicCalculated;

            std::vector<bool> activeForCellTypes; //cell types where the element can be active. Curretly, only for RANDOMNESS elements. For efficiency, prevents re-sampling of Randomness in cells that won't use them


            GeneticElement( size_t relativeId, const std::string& name, GeneticElementIdx::ElemTypeIdx elementType, bool bQuantitative, const GeneticCollection* collection, ElemCategory elemCategory );  
    };
}

#endif //CG_GENETIC_ELEMENT_HPP