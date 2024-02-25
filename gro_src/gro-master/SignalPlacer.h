#ifndef SIGNALPLACER_H
#define SIGNALPLACER_H

#include "ut/Metadata.hpp" //REGISTER_CLASS
#include "ut/NameMap.hpp"  //ut::NameMap
#include "ut/DistributionCombi.hpp" //all the dists
#include "ut/Shape.hpp" //rectangle constArea

#include "Defines.h"
#include "GroElement.h" //base class
#include "CoordsSelector.h" //coordsSelector, CoordsSelectorBase::CoordsMode

//PRECOMPILED
/* #include <memory> //SP for coordsSelector
#include <vector> //msignals */


namespace mg { class Signal; } //signals
class Timer; //timer
class GroCollection; //ctor
class SignalPlacer : public GroElement
{
    REGISTER_CLASS( "SignalPlacer", "sp", ElemTypeIdx::SIGNAL_PLACER )
	
	public:
		using CoordsMode = CoordsSelectorBase::CoordsMode;

		enum class Mode
		{
			SET, ADD, FULL, COUNT
		};

		static constexpr Mode DF_MODE = Mode::ADD;
		static constexpr bool DF_BMIXED = false;
		inline static const ut::Rectangle DF_CONST_AREA = ut::Rectangle();
		inline static const std::vector<double> DF_SIGNAL_PROBS = { 1.0 };

		static constexpr double DF_AMOUNT_MEAN = 1'000.0;
		static constexpr double DF_AMOUNT_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_AMOUNT_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_AMOUNT_PARAMS = { DF_AMOUNT_MEAN, DF_AMOUNT_VAR };
		inline static const ut::ContinuousDistData DF_AMOUNT_DIST = ut::ContinuousDistData( DF_AMOUNT_DISTTYPE, DF_AMOUNT_PARAMS );

		static constexpr double DF_SOURCE_NUM_MEAN = 1.0;
		static constexpr double DF_SOURCE_NUM_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_SOURCE_NUM_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_SOURCE_NUM_PARAMS = { DF_SOURCE_NUM_MEAN, DF_SOURCE_NUM_VAR };
		inline static const ut::ContinuousDistData DF_SOURCE_NUM_DIST = ut::ContinuousDistData( DF_SOURCE_NUM_DISTTYPE, DF_SOURCE_NUM_PARAMS );

		static constexpr double DF_SIZE_MEAN = 0.0;
		static constexpr double DF_SIZE_VAR = 0.0;
		static constexpr ut::ContinuousDist::DistributionType DF_SIZE_DISTTYPE = ut::ContinuousDist::DistributionType::NORMAL;
		inline static const std::vector<TReal> DF_SIZE_PARAMS = { DF_SIZE_MEAN, DF_SIZE_VAR };
		inline static const ut::ContinuousDistData DF_SIZE_DIST = ut::ContinuousDistData( DF_SIZE_DISTTYPE, DF_SIZE_PARAMS );


        inline static const ut::NameMap<Mode> modeNM = ut::NameMap<Mode>( {
                                  { "set", Mode::SET }
                                , { "add", Mode::ADD }
                                , { "full", Mode::FULL } } );


	//---ctor, dtor
		SignalPlacer( GroCollection* groCollection, size_t id, const std::string& name
	    , const std::vector<const mg::Signal*>& msignals, const Timer* timer, const CoordsCircular& coords, CoordsMode coordsMode, Mode mode, bool bMixed, const std::vector<TReal>& signalProbs
	    , const ut::ContinuousDistData& amountDist = DF_AMOUNT_DIST, const ut::ContinuousDistData& sourceNumDist = DF_SOURCE_NUM_DIST, const ut::ContinuousDistData& sizeDist = DF_SIZE_DIST, const ut::Rectangle& constArea = DF_CONST_AREA );

	    virtual ~SignalPlacer() = default; 

	//---get
	    //inline const mg::Signal* getSignal() const{ return msignals[0]; }
	    inline const std::vector<const mg::Signal*>& getSignals() const{ return msignals; }
	    inline const Timer* getTimer() const { return timer; }
	    inline const CoordsSelectorBase* getCoordsSelector() const { return coordsSelector.get(); }
	    inline Mode getMode() const { return mode; }
	    inline bool getBMixed() const { return bMixed; }
		inline bool getIsMixed() const { return bMixed; }
		inline const ut::Rectangle& getConstArea() const { return constArea; }

	//---API
	  //precompute
		void loadDirectElemLink() override;
	  //sampling
	    inline const mg::Signal* sampleSignal() const { return msignals[ signalSelectDist.sample() ]; }
	    //inline size_t sampleSignalId() const { return sampleSignal()->getId(); }
	    inline uint sampleSourceNum() const { return ut::round<uint>( ut::fitL( sourceNumDist.sample() ) ); }
	    inline TReal sampleAmount() const { return ut::fitL( amountDist.sample() ); }
	    inline TReal sampleSize() const { return ut::fitL( sizeDist.sample() ); }
	   	inline void samplePos( TReal& x, TReal& y ) const { coordsSelector->sampleCoords( x, y ); } 
	    

	private:
	  //resources
		std::vector<const mg::Signal*> msignals; //alternative signals
		const Timer* timer;
		SP<CoordsSelectorBase> coordsSelector;
	  //options
		Mode mode; //set or add
		bool bMixed; //whether to place different signals in the same event
		ut::Rectangle constArea;
	  //dists
		mutable ut::DiscreteDistCombi< ut::DiscreteDist > signalSelectDist;
		mutable ut::DistributionCombi sourceNumDist; //number of source spots
        mutable ut::DistributionCombi amountDist; //amount of signal per spot
        mutable ut::DistributionCombi sizeDist; //radius of spot
};

#endif // SIGNALPLACER_H