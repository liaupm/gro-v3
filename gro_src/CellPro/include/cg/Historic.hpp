#ifndef CG_HISTORIC_HPP
#define CG_HISTORIC_HPP

#include "ut/Metadata.hpp" //REGISTER_CLASS

#include "cg/defines.hpp"
#include "cg/GeneticElement.hpp" //base class, iniValueElem and target


namespace cg
{
	class GeneticCollection; 
	class Historic : public GeneticElement
	/*Past state register of any element. May be used for delayed differential equations. */
	{
		REGISTER_CLASS( "Historic", "h", GeneticElement::ElemTypeIdx::HISTORIC, true )

		public:
			static constexpr TReal DF_DELAY = 0.1;
			static constexpr TReal DF_INI_VALUE = 0.0;
			inline static const GeneticElement* DF_INI_VALUE_ELEM = nullptr;


		//---ctor, dtor
			Historic( const GeneticCollection* geneticCollection, size_t id, const std::string& name                                  
			, const GeneticElement* target, const GeneticElement* iniValueElem = DF_INI_VALUE_ELEM, TReal delay = DF_DELAY )
			: GeneticElement( id, name, GeneticElement::ElemTypeIdx::HISTORIC, true, geneticCollection, ElemCategory::LOGIC ) 
			, target( target ), iniValueElem( iniValueElem ), delay( delay ) {;}

			virtual ~Historic() = default;

		//---get
			inline const GeneticElement* getTarget() const { return target; }
			inline const GeneticElement* getIniValueElem() const { return iniValueElem; }
			inline TReal getDelay() const { return delay; }
			inline bool getBCustomIniValue() const { return iniValueElem; }
			inline bool getHasCustomIniValue() const { return iniValueElem; }

		//---set
			inline void setTarget( const GeneticElement* xTarget ) { target = xTarget; }
			inline void setIniValueElem( const GeneticElement* xIniValueElem ) { iniValueElem = xIniValueElem; }
			inline void setAll( const GeneticElement* xTarget, const GeneticElement* xIniValueElem ) { setTarget( xTarget ); setIniValueElem( xIniValueElem ); } 

		//---API
		  //precompute
			void loadDirectElemLink() override { this->addLink( target, LinkDirection::BACKWARD ); this->addLink( iniValueElem, LinkDirection::BACKWARD ); }

		private:
			const GeneticElement* target; //the element to record its past states
			const GeneticElement* iniValueElem; //element to use as the initial value. If null, target is used
			TReal delay; //in min
	};
}

#endif //CG_HISTORIC_HPP