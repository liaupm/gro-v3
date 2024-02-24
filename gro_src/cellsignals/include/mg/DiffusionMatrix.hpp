#ifndef MG_DIFFUSIONMATRIX_HPP
#define MG_DIFFUSIONMATRIX_HPP

#include "mg/defines.hpp"

#include<vector> //DiffMatrixDS


namespace mg
{
	using DiffMatrixDS = std::vector<std::vector<TReal>>;

	class DiffusionMatrix : public DiffMatrixDS
	{
		public:
			friend class Signal;
			
			enum class Neighborhood
            {
                NEUMANN_4, MOORE_8, FREE
            };

            static const DiffMatrixDS MATRIX_NEUMANN_4;
            static const DiffMatrixDS MATRIX_MOORE_8;

		//---ctor, dtor
			DiffusionMatrix( Neighborhood neighborhood, const DiffMatrixDS& xMatrix = {} );
			virtual ~DiffusionMatrix() = default;
			
			DiffusionMatrix( const DiffusionMatrix& rhs ) = default;
			DiffusionMatrix( DiffusionMatrix&& rhs ) noexcept = default;
			DiffusionMatrix& operator=( const DiffusionMatrix& rhs ) = default;
			DiffusionMatrix& operator=( DiffusionMatrix&& rhs ) noexcept = default;

		//---get
			inline Neighborhood getNeighborhood() const { return neighborhood; }
			inline uint getHalfSize() const { return halfSize; }
			inline uint getPaddingUnits() const { return paddingUnits; /*return 10;*/ } 

		//---set

		//---API

		private:
			Neighborhood neighborhood;

            uint halfSize;
            uint paddingUnits;

        //---aux
            DiffMatrixDS selectMatrix( Neighborhood neighborhood, const DiffMatrixDS& matrix = {} );
	};
}

#endif //MG_DIFFUSIONMATRIX_HPP