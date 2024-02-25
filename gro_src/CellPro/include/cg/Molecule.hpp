#ifndef CG_MOLECULE_HPP
#define CG_MOLECULE_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/DistributionCombi.hpp" //ut::ContinuousDistData in TimeDistribution ctor
#include "ut/DistributionScaler.hpp" //TimeDistribution::scaler
#include "ut/ThresholdVector.hpp" //ut::DistributionsVector expressionDistributions
#include "ut/Time.hpp" //applyEpsilon() at scaleExpressionTime()

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
    class GeneticCollection;
    class Molecule : public GeneticElement
    /* Any simple (without components) molecule with digital representation, usually a protein, but also mRNA, cofactors, virions, etc. 
    Produced by Operons and Fluxes, uses the digital step expression model */
    {
        REGISTER_CLASS( "Molecule", "mol", GeneticElement::ElemTypeIdx::MOLECULE, false )
        
        public:
            enum ExpressionDirection : int
            {
                DIR_OFF = -1, DIR_ON = 1, DIR_NONE = 0
            };


///////////////////////////////////////////// NESTED CLASSES (TIME DISTRIBUTION ) /////////////////////////////////////////////////////////////
            
            struct TimeDistribution
            {
            	const GeneticElement* fun;
                SP<ut::DistributionScaler> scaler;
                ExpressionDirection direction;

                TimeDistribution( const GeneticElement* fun, const SP<ut::DistributionScaler> scaler, ExpressionDirection direction ) : fun( fun ), scaler( scaler ), direction( direction ) {;}
                TimeDistribution( const GeneticElement* fun, const GeneticElement* rnd, const ut::ContinuousDistData& distData, ExpressionDirection direction );
            };



////////////////////////////////////////////////////////////////// MOLECULE /////////////////////////////////////////////////////////////

            using DistVectorDS = ut::ThresholdVector< TReal, TimeDistribution >;

            inline static const std::vector<TReal> DF_MEAN_TIMES = { -20.0, 10.0 };
            static constexpr TReal DF_VAR_TIME = 0.0;
            inline static const std::vector<TReal> DF_VAR_TIMES = { DF_VAR_TIME, DF_VAR_TIME };

            inline static const TReal DF_FIRST_THRESHOLD = 0.5;
            inline static const TReal DF_THRESHOLD_INCREASE = 1.0;
            inline static const std::vector<TReal> DF_THRESHOLDS = { DF_FIRST_THRESHOLD };
            
            inline static const GeneticElement* DF_CUSTOM_RND_SYMMETRY = nullptr;
            static constexpr bool DF_SYMMETRIC_RND = true;


        //---ctor, dtor
            Molecule( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const GeneticElement* randomness, const DistVectorDS& timeDistributions, const GeneticElement* customRndSymmetry = DF_CUSTOM_RND_SYMMETRY, bool bSymmetricRnd = DF_SYMMETRIC_RND );
            
            virtual ~Molecule() = default;

        //---get
            inline const GeneticElement* getRandomness() const { return randomness; }
            inline const GeneticElement* getCustomRndSymmetry() const { return customRndSymmetry; }
            inline bool getbSymmetricRnd() const { return bSymmetricRnd; }

        //---set
            void setAll( const GeneticElement* xRandomness, const GeneticElement* xCustomRndSymmetry, const DistVectorDS& xTimeDistributions );
            inline void setRandomness( const GeneticElement* xRandomness ) { randomness = xRandomness; calculateBSymmetricRnd(); }
            inline void setCustomRndSymmetry( const GeneticElement* xCustomRndSymmetry ) { customRndSymmetry = xCustomRndSymmetry; calculateBSymmetricRnd(); }
            inline void setTimeDistributions( const DistVectorDS& xTimeDistributions ) { timeDistributions = xTimeDistributions; calculateBCustomFunctions(); }
            
        //---API
          //precompute
            void loadDirectElemLink() override;
          //run
            inline size_t inputIndex( TReal activation ) const { return timeDistributions.input2index( activation ); } //convert input activation to the index of the time distribution to use, using the thresholds
            size_t inputIndex( ExpressionDirection& direction, TReal activation ) const; //same as the function with the same name, but provides the direction directly
            inline ExpressionDirection getDirectionAtIdx( size_t idx ) { return timeDistributions.retrieveAtIndex( idx ).direction; } //get the direction (activation or degradation)
            inline TReal scaleExpressionTime( TReal rawNewTime, TReal fraction, bool bSameDirection ) const { return bSameDirection ? ut::Time::applyEpsilon( rawNewTime * fraction ) : ut::Time::applyEpsilon( rawNewTime ); } //used when there is an ongoing time the direction doesn't change
          //sampling
            TReal sampleExpressionTimeAtIndex( size_t idx, TReal funVal, TReal rawRndVal, TReal symmetricValue ) const; //"sample" (scale) using the distribution at given index
            const GeneticElement* getFunctionAtIndex( size_t idx ) const { return bCustomFunctions ? timeDistributions.retrieveAtIndex( idx ).fun : nullptr; }
            

        private:
          //resources
            const GeneticElement* randomness; //stochasticity in the expression times
            const GeneticElement* customRndSymmetry; //optional custom function to calculate the symmetric values when randomness is not a cannonical distribution type
            std::vector<const GeneticElement*> timeFunctions;
          //options
            DistVectorDS timeDistributions; //expression time distributions with their directions (activation / degradation) and activation thresholds
            bool bSymmetricRndParam; //whether the symmetric of the random values produced by randomness must be used when the direction is degradation (longer activations = shorter degradations). Raw
            bool bSymmetricRnd; //precomputed bSymmetricRndParam && other neccessary conditions
            bool bCustomFunctions;

          //fun
            inline void calculateBSymmetricRnd() { bSymmetricRnd = bSymmetricRndParam && randomness && randomness->getElemType() == ElemTypeIdx::RANDOMNESS && ! customRndSymmetry; }
            void calculateBCustomFunctions();
    };
}

#endif //CG_MOLECULE_HPP
