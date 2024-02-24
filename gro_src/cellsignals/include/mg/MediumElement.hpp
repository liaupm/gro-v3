#ifndef MG_MEDIUMELEMENT_HPP
#define MG_MEDIUMELEMENT_HPP

#include "mg/defines.hpp"
#include "ut/Metadata.hpp" //used by all the derived elements
#include "ut/ElementIndex.hpp" //base class for idx
#include "ut/LinkedElement.hpp" //base class

namespace mg
{
	class MediumCollection;
	class MediumElementIdx : ut::ElementIndex
    {
    public:
        enum ElemTypeIdx : size_t
        {
            SIGNAL, GRID, COUNT
        };
    };

	class MediumElement : public ut::LinkedElement<MediumElement, MediumElementIdx>
	{
		public:
		//---ctor, dtor
			MediumElement( size_t relativeId, const std::string& name, MediumElementIdx::ElemTypeIdx elementType, bool bQuantitative, const MediumCollection* collection );

			virtual ~MediumElement() = default;
			MediumElement( const MediumElement& rhs ) = delete;
			MediumElement& operator=( const MediumElement& rhs ) = delete;
		//---get

		//---set

		//---API

		private:
	};
}

#endif //MG_MEDIUMELEMENT_HPP