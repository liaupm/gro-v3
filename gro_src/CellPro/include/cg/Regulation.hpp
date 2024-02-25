#ifndef CG_REGULATION_HPP
#define CG_REGULATION_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/SubtypeRegister.hpp" //dosageDivisorNM
#include "ut/DistributionScaler.hpp" //onScaler and offScaler of NoisyDistribution

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in  ctor
#include <memory> //SP<> */


namespace cg
{
    class GeneticCollection;
    class QMarker;
    class Regulation : public GeneticElement
    /* Virtual base class for the exact and the noisy regualtions */
    {
        REGISTER_CLASS( "Regulation", "reg", GeneticElement::ElemTypeIdx::REGULATION, true )

        public:
            inline static const GeneticElement* DF_DOSAGE_FUN = nullptr;
            inline static const GeneticElement* DF_ON_FUN = nullptr;
            inline static const GeneticElement* DF_OFF_FUN = nullptr;
            inline static const QMarker* DF_ACTI_MARKER = nullptr;
            inline static const QMarker* DF_RND = nullptr;

            inline static const TReal DF_ON_ACTI_MEAN = 1.0;
            inline static const TReal DF_ON_ACTI_STDDEV = 0.0;
            inline static const std::vector<TReal> DF_ON_ACTI_PARAMS = { DF_ON_ACTI_MEAN, DF_ON_ACTI_STDDEV };

            inline static const TReal DF_OFF_ACTI_MEAN = 0.0;
            inline static const TReal DF_OFF_ACTI_STDDEV = 0.0;
            inline static const std::vector<TReal> DF_OFF_ACTI_PARAMS = { DF_OFF_ACTI_MEAN, DF_OFF_ACTI_STDDEV };

            static constexpr bool DF_B_AUTO_DIVISION = false; 


        //---ctor, dtor
            Regulation( const GeneticCollection* geneticCollection, size_t id, const std::string& name
			, const GeneticElement* gate, const GeneticElement* randomness, const ut::DistributionScaler::ParamsType& onParams, const ut::DistributionScaler::ParamsType& offParams
 			, const GeneticElement* dosageFun = DF_DOSAGE_FUN, const GeneticElement* onFunction = DF_ON_FUN, const GeneticElement* offFunction = DF_OFF_FUN, const QMarker* actiMarker = DF_ACTI_MARKER, bool bAutoDivision = DF_B_AUTO_DIVISION );

            virtual ~Regulation() = default;

        //---get
          //resources
            inline const GeneticElement* getGate() const { return gate; }
            inline const GeneticElement* getDosageFun() const { return dosageFun; }
            inline bool getBDosageEffect() const { return dosageFun; }
            inline bool getHasDosageEffect() const { return dosageFun; }
            inline const GeneticElement* getOnFunction() const { return onFunction; }
            inline const GeneticElement* getOffFunction() const { return offFunction; }
            inline const QMarker* getActiMarker() const { return actiMarker; }
            inline const GeneticElement* getRandomness() const { return randomness; }
          //options
            inline bool getBAutoDivision() const { return bAutoDivision; }  

        //---set
            void setAll( const GeneticElement* xGate, const GeneticElement* xDosageFun, const GeneticElement* xOnFunction, const GeneticElement* xOffFunction, const GeneticElement* xRandomness );
            inline void setGate( const GeneticElement* xGate ) { gate = xGate; }
            inline void setDosageFun( const GeneticElement* xDosageFun ) { dosageFun = xDosageFun; }
            inline void setOnFunction( const GeneticElement* xOnFunction ) { onFunction = xOnFunction; }
            inline void setOffFunction( const GeneticElement* xOffFunction ) { offFunction = xOffFunction; }
            void setRandomness( const GeneticElement* xRandomness );

        //---API
          //preprocess
            void loadDirectElemLink() override;
          //run
            TReal applyDosageEffect( TReal input, TReal copyNumber, TReal dosageFunValue = 1.0 ) const; //scale the raw input by dosageFun and bAutoDivision
            TReal sampleActivation( TReal copyNumber, TReal rawRndVal, TReal onFunVal, TReal offFunVal, bool gateState, TReal dosageFunValue = 1.0 ) const;


        protected:
          //resources
            const GeneticElement* gate; //the extended condition
            const GeneticElement* dosageFun; //how the dosage is applied. The activation is multiplied by this
            const GeneticElement* onFunction;
            const GeneticElement* offFunction;
            const QMarker* actiMarker; //exposes the activation, optional
            const GeneticElement* randomness; //provides the random numbers for the noisy behaviou
          //options
            bool bAutoDivision; //whether to automatically divide the activation by the copy number to get the unit activation or not (because it is already included in the dosageFun )
          //dists
            SP<ut::DistributionScaler> onScaler; //scales the randomness values to generate the activation when on (gate true)
            ut::DistributionScaler::ParamsType onParams; //stored to be passed to onScaler later
            SP<ut::DistributionScaler> offScaler; //scales the randomness values to generate the activation when off (gate false)
            ut::DistributionScaler::ParamsType offParams; //stored to be passed to offScaler later
    };
}

#endif //CG_REGULATION_HPP
