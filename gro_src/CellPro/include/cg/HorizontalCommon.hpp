#ifndef CG_HORIZONTAL_COMMON_HPP
#define CG_HORIZONTAL_COMMON_HPP

#include "ut/Metadata.hpp"
#include "ut/DistributionCombi.hpp" //shuffleRnEngine

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class


/*PRECOMPILED
#include <string> //name in constructor */


namespace cg
{
    class GeneticCollection;

    class HorizontalCommon : public GeneticElement
    {
    	REGISTER_CLASS( "HorizontalCommon", "cHorizontal", GeneticElement::ElemTypeIdx::HORIZONTAL_COMMON, false )

    	public: 
    		friend class HorizontalHandler;

    	//---ctor, dtor
    		HorizontalCommon( const GeneticCollection* geneticCollection, size_t id, const std::string& name = "" );

    		virtual ~HorizontalCommon() = default;

    	//---get
    		inline ut::DistributionCombiBase<>::RandomEngineType& getShuffleRndEngine() const { return shuffleRnEngine; }

    	private: 
			mutable ut::DistributionCombiBase<>::RandomEngineType shuffleRnEngine;
    };
}

#endif //CG_HORIZONTAL_COMMON_HPP