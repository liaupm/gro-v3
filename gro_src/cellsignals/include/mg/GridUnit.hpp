#ifndef GRID_UNIT_H
#define GRID_UNIT_H

#include "defines.hpp"
#include "mg/Grid.hpp"

#include <vector> //ConcsDS
#include <algorithm> //std::max

namespace mg
{
	class GridUnit
	{
		public:
			using ConcsDS = Grid::ConcsDS;

		//---ctor, dtor
			GridUnit( size_t signalNum ): concs( signalNum, 0.0 ) {;}
			//GridUnit( const ConcsDS& concs ): concs(concs) {;}
			virtual ~GridUnit() = default;

			GridUnit( const GridUnit& rhs ) = default;
			GridUnit( GridUnit&& rhs ) noexcept = default;
			GridUnit& operator=( const GridUnit& rhs ) = default;
			GridUnit& operator=( GridUnit&& rhs ) noexcept = default;

		//---get
			inline const ConcsDS& getConcs() const { return concs; }
			inline ConcsDS& getConcsEdit() { return concs; }
			inline TReal getConc( size_t id ) const { return concs[id]; }
			inline size_t getSignalNum() const { return concs.size(); }
		//---set
			inline void setConcs( const ConcsDS& xConcs ) { concs = xConcs; }
			inline void setConc( size_t id, TReal xConc ) { concs[id] = xConc; }
		//---API
			inline void changeConcs( const ConcsDS& changes ) { for( size_t id = 0; id < changes.size(); id++ ) changeConc( id, changes[id] ); }
			inline void changeConc( size_t id, TReal change ) { concs[id] = std::max( concs[id] + change, 0.0 ); }

		private:
			ConcsDS concs;
	};
}

#endif //GRID_UNIT_H