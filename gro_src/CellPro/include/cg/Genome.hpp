#ifndef CG_GENOME_HPP
#define CG_GENOME_HPP

#include "ut/Time.hpp" //.time
#include "ut/Agent.hpp" //base

#include "cg/defines.hpp"
#include "cg/CellType.hpp" //getStrain()
#include "cg/GeneticCollection.hpp" //ctor, ElemTypeIdx
#include "cg/GeneticHandler.hpp" //HandlerTypeIdx
#include "cg/EventScheduler.hpp" //EventType, access to events in update(), wrap of addEvent() and removeEvent()

#include "cg/RandomnessHandler.hpp"
#include "cg/LogicHandler.hpp"
#include "cg/ColourHandler.hpp"
#include "cg/MetabolismHandler.hpp"
#include "cg/ExpressionHandler.hpp"
#include "cg/GrowthHandler.hpp"
#include "cg/HorizontalHandler.hpp"
#include "cg/PlasmidsHandler.hpp"

/*PRECOMPILED
#include <vector> //metabolism medium concs */


namespace ut { class RgbColour; } //getColour()
namespace cg
{
    class GeneticElement; //param of refreshLogicFrom()
    class Strain; 
    struct GenomeMembersWrapper
    {
        GenomeMembersWrapper( const CellType* cellType ) : cellType(cellType) {;}
        const CellType* cellType;
    };

    class Genome : private GenomeMembersWrapper
                , public ut::Agent< Genome, EventScheduler, GeneticCollection, GeneticHandler<>::HandlerTypeIdx
                                    , ColourHandler, PlasmidsHandler, HorizontalHandler, GrowthHandler, ExpressionHandler, MetabolismHandler, LogicHandler, RandomnessHandler >
    {
        public:
            using AgentBaseType = Genome::Agent;
            using AgentBaseType::Agent;
            using HandlerBaseType = GeneticHandler<>::HandlerBaseType;
            using ElemTypeIdx = GeneticCollection::ElemTypeIdx;
            using EventType = EventScheduler::EventType;

    //---ctor, dtor
            Genome( GeneticCollection* geneticCollection, const CellType* cellType, const ut::Time& time, const std::vector<TReal>& mediumConcs ); //called for newly created cells
            Genome( const Genome& original ) = delete;
            Genome( Genome* mother ); //not a copy ctor, called to create daughter cells from existing ones
            virtual ~Genome() = default;

	//---get
            const CellType* getCellType() const { return cellType; }
            const Strain* getStrain() const { return cellType->getStrain(); }
          //handlers
            inline const RandomnessHandler& getRandomnessHandler() const { return getHandler<HandlerIdx::H_RANDOMNESS>(); }
            inline const LogicHandler& getLogicHandler() const { return getHandler<HandlerIdx::H_LOGIC>(); }
            inline const ColourHandler& getColourHandler() const { return getHandler<HandlerIdx::H_COLOUR>(); }
            inline const MetabolismHandler& getMetabolismHandler() const { return getHandler<HandlerIdx::H_METABOLISM>(); }
            inline const ExpressionHandler& getExpressionHandler() const { return getHandler<HandlerIdx::H_EXPRESSION>(); }
            inline const GrowthHandler& getGrowthHandler() const { return getHandler<HandlerIdx::H_GROWTH>(); }
            inline const HorizontalHandler& getHorizontalHandler() const { return getHandler<HandlerIdx::H_HORIZONTAL>(); }
            inline const PlasmidsHandler& getReplicationHandler() const { return getHandler<HandlerIdx::H_PLASMIDS>(); }
          //handlers edit
            inline RandomnessHandler& getRandomnessHandlerEdit() { return getHandlerEdit<HandlerIdx::H_RANDOMNESS>(); }
            inline LogicHandler& getLogicHandlerEdit() { return getHandlerEdit<HandlerIdx::H_LOGIC>(); }
            inline ColourHandler& getColourHandlerEit() { return getHandlerEdit<HandlerIdx::H_COLOUR>(); }
            inline MetabolismHandler& getMetabolismHandlerEdit() { return getHandlerEdit<HandlerIdx::H_METABOLISM>(); }
            inline ExpressionHandler& getExpressionHandlerEdit() { return getHandlerEdit<HandlerIdx::H_EXPRESSION>(); }
            inline GrowthHandler& getGrowthHandlerEdit() { return getHandlerEdit<HandlerIdx::H_GROWTH>(); }
            inline HorizontalHandler& getHorizontalHandlerEdit() { return getHandlerEdit<HandlerIdx::H_HORIZONTAL>(); }
            inline PlasmidsHandler& getReplicationHandlerEdit() { return getHandlerEdit<HandlerIdx::H_PLASMIDS>(); }
          //colourH wrapper
            inline const ut::RgbColour& getColour() const { return getHandler<HandlerIdx::H_COLOUR>().getTotalColour(); }
          //growthH wrapper
            inline TReal getGrowthRate() const { return getGrowthHandler().getGrowthRate(); }
            inline TReal getGenerationTime() const { return getGrowthHandler().getGenerationTime(); }
            inline TReal getTotalLength() const { return getGrowthHandler().getTotalLength(); }
            inline TReal getTotalLengthFraction() const { return getGrowthHandler().getTotalLengthFraction(); }
            inline TReal getDLength() const { return getGrowthHandler().getDLength(); }
            inline TReal getVolume() const { return getGrowthHandler().getVolume(); }
            inline TReal getDVolume() const { return getGrowthHandler().getDVolume(); }
            inline bool getBMustDie() const { return getGrowthHandler().getBMustDie(); }
            inline bool getBMustDivide() const { return getGrowthHandler().getBMustDivide(); }
            inline bool getBMustGrow() const { return getGrowthHandler().getBMustGrow(); }
            inline bool getDivisionFraction() const { return getGrowthHandler().getLastDivisionFraction(); }
          //metabolismH wrapper
            inline const std::vector<TReal>& getMetFluxes() const { return getHandler<HandlerIdx::H_METABOLISM>().getTotalFluxes(); }

    //---API
          //common
            template< bool VbInitialUpdate = true, bool VbAsynchronousUpdate = true, bool VbTick = true >
            bool update( TReal time );
            //void death();

            inline bool tick() { setEventMarkersOff(); tickHandlers(); updateCycle( nullptr ); return true; }
          //special
            inline bool initialUpdate() { updateCycle( nullptr ); return true; }
            bool asynchronousUpdate( TReal time );
            

          //agent extension
            template< typename TAmount >
            inline void justAdd( const ElementBaseType* elem, TAmount amount ) { addState( elem, amount, elem->getBPhysical() ); }
            template< typename TAmount >
            inline void justSet( const ElementBaseType* elem, TAmount amount ) { setState( elem, amount, elem->getBPhysical() ); }
            template< typename TAmount >
            inline void justAddContent( const ElementBaseType* elem, TAmount amount ) { addContentFrom( elem, amount, elem->getBPhysical() ); }
            template< typename TAmount >
            inline void justSetContent( const ElementBaseType* elem, TAmount amount ) { setContentFrom( elem, amount, elem->getBPhysical() ); }


            inline void forwardFrom( const ElementBaseType* elem ) { addChangesFrom( elem ); refreshLogicFrom( elem ); }
            inline void forwardFromExcludeSelf( const ElementBaseType* elem ) { addChangesFrom( elem ); removeChange( elem ); refreshLogicFrom( elem ); }
            inline void forwardFromContent( const ElementBaseType* elem ) { addChangesFromContent( elem ); refreshLogicFromContent( elem ); }
            inline void forwardFromContentExcludeSelf( const ElementBaseType* elem ) { addChangesFromContent( elem ); removeChange( elem ); refreshLogicFromContent( elem ); }

            template< typename TAmount >
            inline void addAndForward( const ElementBaseType* elem, TAmount amount ) { justAdd( elem, amount ); forwardFrom( elem ); }
            template< typename TAmount >
            inline void addAndForwardExcludeSelf( const ElementBaseType* elem, TAmount amount ) { justAdd( elem, amount ); forwardFromExcludeSelf( elem ); }
            template< typename TAmount >
            inline void setAndForward( const ElementBaseType* elem, TAmount amount ) { justSet( elem, amount ); forwardFrom( elem ); }
            template< typename TAmount >
            inline void setAndForwardExcludeSelf( const ElementBaseType* elem, TAmount amount ) { justSet( elem, amount ); forwardFromExcludeSelf( elem ); }

            template< typename TAmount >
            inline void addAndForwardContent( const ElementBaseType* elem, TAmount amount ) { justAddContent( elem, amount ); forwardFromContent( elem ); }
            template< typename TAmount >
            inline void setAndForwardContent( const ElementBaseType* elem, TAmount amount ) { justSetContent( elem, amount ); forwardFromContent( elem ); }
            template< typename TAmount >
            inline void addAndForwardContentExcludeSelf( const ElementBaseType* elem, TAmount amount ) { justAddContent( elem, amount ); forwardFromContentExcludeSelf( elem ); }


          //eventsH wrapper
            inline void addEvent( EventType::EventMode eventType, const GeneticElement* emitter, const GeneticElement* elem, TReal eventTime, int quantity = 0 ) { if( eventTime > time.time ) scheduler.addEvent( eventType, emitter, elem, eventTime, quantity ); }
            inline void removeEvent( EventType::EventMode eventType, const GeneticElement* emitter, const GeneticElement* elem, TReal eventTime, int quantity = 0 ) { if( eventTime > time.time ) scheduler.removeEvent( eventType, emitter, elem, eventTime, quantity ); }
           	inline void addFakeEvent( const GeneticElement* evElem, int amount = 1 ) { addAndForward( evElem, amount ); addEvent( EventType::ELEM_CHANGE, nullptr, evElem, time.time + time.stepSize, - amount ); }
          //logicH wrapper
            inline void refreshLogic() { getLogicHandlerEdit().refresh(); }
            inline void refreshLogicFrom( const GeneticElement* elem ) { getLogicHandlerEdit().refreshFrom( elem ); } 
            inline void refreshLogicFromContent( const GeneticElement* elem ) { getLogicHandlerEdit().refreshFromContent( elem ); } 
          //metabolismH wrapper
            inline void initMetExternal( const std::vector<TReal>& mediumConcs ) { getMetabolismHandlerEdit().initExternal( mediumConcs ); }
            inline void updateMetExternal( const std::vector<TReal>& mediumConcs ) { getMetabolismHandlerEdit().updateConcs( mediumConcs ); /*updateCycle( nullptr );*/ }
          //horizontalH wrapper
            inline bool checkActivePili() const { return getHorizontalHandler().checkActivePili(); }
            inline bool checkActivePiliDistance() const { return getHorizontalHandler().checkActivePiliDistance(); }
            inline void updateConjExternal( const std::vector<Genome*>& neighbours, const std::vector<TReal>& distances ) { getHorizontalHandlerEdit().updateExternal( neighbours, distances );  }
			inline void applyConjugations() { getHorizontalHandlerEdit().selectAndApplyConjugations(); updateCycle( nullptr ); } 


        private:
            void addQPlasmids(); //sample the initial copy numbers of QPlasmids
            void setEventMarkersOff(); //set off the true tick event markers
    };


//====================================== TEMPALTE METHOD DEFINITIONS
    
    template< bool VbInitialUpdate, bool VbAsynchronousUpdate, bool VbTick >
    bool Genome::update( TReal time )
    {
        if constexpr( VbInitialUpdate )
            initialUpdate();
        if constexpr( VbAsynchronousUpdate )
            asynchronousUpdate( time );
        if constexpr( VbTick )
            tick();
        return true;
    }
}


#endif //CG_GENOME_HPP
