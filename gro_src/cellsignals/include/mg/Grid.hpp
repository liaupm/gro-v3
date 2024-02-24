#ifndef MG_GRID_HPP
#define MG_GRID_HPP

#include "mg/defines.hpp"
#include "mg/MediumElement.hpp" //base class

#include <vector>

namespace mg
{
	class MediumCollection;
	class GridUnit;
	class Grid : public MediumElement
	{
		REGISTER_CLASS( "Grid", "grid", MediumElement::ElemTypeIdx::GRID );

		public:
			using ConcsDS = std::vector<TReal>;
            using GridRow = std::vector< GridUnit >;
            using GridMatrix = std::vector< GridRow >;
            using MatrixesDS = std::vector<GridMatrix>;

            inline static constexpr uint DF_UNIT_SIZE = 5;
            inline static constexpr uint DF_GRID_SIZE = 1000;
            inline static constexpr uint DF_D_GRID_SIZE = 100;
            inline static constexpr size_t DF_MATRIX_NUM = 2;

		//---ctor, dtor
			Grid( const MediumCollection* collection, size_t relativeId, const std::string& name
			, uint unitSize = DF_UNIT_SIZE, uint iniGridSize = DF_GRID_SIZE, uint deltaGridSize = DF_D_GRID_SIZE, size_t matrixNum = DF_MATRIX_NUM )
			: MediumElement::MediumElement( relativeId, name, MediumElement::ElemTypeIdx::GRID, false, collection ) 
			, unitSize(unitSize), iniGridSize(iniGridSize), deltaGridSize(deltaGridSize), matrixNum(matrixNum) {;}

			virtual ~Grid() = default;
			Grid( const Grid& rhs ) = delete;
            Grid& operator=( const Grid& rhs ) = delete;

		//---get
			inline uint getUnitSize() const { return unitSize; } 
			inline uint getIniGridSize() const { return iniGridSize; } 
			inline uint getDeltaGridSize() const { return deltaGridSize; } 
			inline size_t getMatrixNum() const { return matrixNum; } 

		//---set

		//---API

		private:
			uint unitSize; //number of pixels per grid unit
			uint iniGridSize;
			uint deltaGridSize;
			size_t matrixNum;
	};
}

#endif //MG_GRID_HPP