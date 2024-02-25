#ifndef CG_OPERON_HPP
#define CG_OPERON_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <vector> //std::vector<const Molecule*> output
#include <string> //name in ctor */


namespace cg
{
    class GeneticCollection;
    class QMarker; //actiMarker
    class Regulation; //regulation
    class Molecule; //output
    class Operon : public GeneticElement
    /* A series of Molecules under the control of a Regulation. The activation produced by the Regulation is transmitted to all of them.
    Typically an operon, but may represent any other regulation point */
    {
        REGISTER_CLASS( "Operon", "op", GeneticElement::ElemTypeIdx::OPERON, true )

        public:
            inline static const std::vector<const Molecule*>& DF_OUTPUT = {};
            inline static const QMarker* DF_ACTI_MARKER = nullptr;
            static constexpr bool DF_BDOSAGE = false;


        //---ctor, dtor
            Operon( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const Regulation* regulation, const std::vector<const Molecule*>& output = DF_OUTPUT, const QMarker* actiMarker = DF_ACTI_MARKER, bool bDosageEffect = DF_BDOSAGE );
            
            virtual ~Operon() = default;

        //---get
            inline const Regulation* getRegulation() const { return regulation; }
            inline const std::vector<const Molecule*>& getOutput() const { return output; }
            inline const Molecule* getOutputByIndex( size_t idx ) const { return output[idx]; }
            inline const QMarker* getActiMarker() const { return actiMarker; }
            inline bool getBDosageEffect() const { return bDosageEffect; }

        //---API
          //precompute
            void loadDirectElemLink() override;
          //run
            inline TReal scaleActivation( TReal rawActivation, TReal copyNum ) const { return bDosageEffect ? rawActivation * copyNum : rawActivation; } //multiplies the unit activation by the copy number if bDosageEffect


        private:
            const Regulation* regulation; //promoter
            std::vector<const Molecule*> output; //genees
            const QMarker* actiMarker; //exposes the activation value, optional
            bool bDosageEffect; //whether the activation is affected by dosage effect (multiplied by the copy number). Always true if the regulation uses dosage effect
    };
}

#endif //CG_OPERON_HPP
