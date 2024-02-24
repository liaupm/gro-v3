#ifndef MG_MEDIUMCOLLECTION_HPP
#define MG_MEDIUMCOLLECTION_HPP

#include "defines.hpp"
#include "ut/CollectionManager.hpp" //base class

#include "MediumElement.hpp"
#include "Signal.hpp"
#include "Grid.hpp"

namespace mg
{
	class MediumCollection : public ut::CollectionManager<MediumCollection, MediumElement, Signal, Grid>
	{
		public:
		//---ctor, dtor
			MediumCollection() = default;
			virtual ~MediumCollection() = default;
			MediumCollection( const MediumCollection& rhs ) = delete;
			MediumCollection& operator=( const MediumCollection& rhs ) = delete;

		//---get

		//---set

		//---API

		private:
	};
}

#endif //MG_MEDIUMCOLLECTION_HPP