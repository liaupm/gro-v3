#ifndef CG_FUNCTION_HPP
#define CG_FUNCTION_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp" //autoVolModeNM
#include "ut/AgentState.hpp" //evaluate()
#include "ut/MathFunction.hpp" //wrapped utility class
#include "ut/DistributionScaler.hpp" //rnd params

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class

/*PRECOMPILED
#include <string> //name in constructor
#include <memory> //SP<MathFunctionType> mathFun */


namespace cg
{
    class GeneticCollection;
    class Function : public GeneticElement
    /* Math function on a series of input elements, wrapper for the utility MathFunction, extended with auto scaling by volume and randomness */
    {
        REGISTER_CLASS( "Function", "fun", GeneticElement::ElemTypeIdx::FUNCTION, true )

        public:
            enum AutoVolMode
            {
                NONE, DIVISION, PRODUCT, COUNT
            };

            using MathFunctionType = ut::MathFunction<>;
            using Type = MathFunctionType::Type;

            static constexpr MathFunctionType::Type DF_FUNTION_TYPE = MathFunctionType::Type::SUM;
            inline static const GeneticElement* const DF_RND = nullptr;
 
            static constexpr AutoVolMode DF_AUTO_VOL_MODE = AutoVolMode::NONE;
            static constexpr TReal DF_MIN = ut::MathFun::DF_MIN;
            static constexpr TReal DF_MAX = ut::MathFun::DF_MAX;
            static constexpr int DF_ROUND_PLACES = ut::MathFun::DF_ROUND_PLACES;
            static constexpr uint INI_RANK = 0;

            static ut::NameMap<AutoVolMode> autoVolModeNM;


        //---ctor, dtor
             Function( const GeneticCollection* geneticCollection, size_t id, const std::string& name
            , const std::vector<const GeneticElement*>& input, const std::vector<TReal>& params, MathFunctionType::Type funType = DF_FUNTION_TYPE, TReal min = DF_MIN, TReal max = DF_MAX
            , const GeneticElement* randomness = DF_RND, const typename ut::DistributionScaler::ParamsType& rndScalerParams = {}, AutoVolMode autoVolMode = DF_AUTO_VOL_MODE, int roundPlaces = DF_ROUND_PLACES );

            virtual ~Function() = default;

        //---get
            inline const std::vector<const GeneticElement*>& getInput() const { return input; }
            inline const GeneticElement* getRandomness() const { return randomness; }
            inline AutoVolMode getAutoVolMode() const { return autoVolMode; } 
            inline uint getRank() const { return rank; }
            
        //---set
            inline void setInput( const std::vector<const GeneticElement*>& xInput ) { input = xInput; }

        //---API
          //precompute
            void loadDirectElemLink() override;
            void precompute() override; //passes the final input ids to the wrapped mathFun, once they have been calculated by the collection
            uint calculateRank(); //used to short Function so that they are evaluated after other gates that act as their inputs. Just for efficiency, not neccessary for correct behaviour 
            bool findElemInLogic( const GeneticElement* elem ) const override; //whether the given element is an input to this gate, directly or as inputs to its inputs. Recursive, checks the whole hierarchy in a transitive way

          //run
            TReal evaluate( const ut::AgentState::QType& qState, TReal vol, TReal rawRndVal ) const; //evaluates, scaling by vol if autoVolMode and using rawRndVal if randomness


        private:
          //resources
            SP<MathFunctionType> mathFun;
            const GeneticElement* randomness;
          //raw
            std::vector<const GeneticElement*> input;
          //options
            AutoVolMode autoVolMode;
          //dists         
            SP<ut::DistributionScaler> rndScaler;
          //state
            uint rank;
            bool bRankCalculated;  

          //fun
            inline TReal scaleRnd( TReal rawRndVal ) const { return rndScaler->scale( rawRndVal ); }
    };
}


#endif //CG_FUNCTION_HPP
