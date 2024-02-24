#ifndef MG_DIFFUSER_HPP
#define MG_DIFFUSER_HPP

#include "mg/defines.hpp"
#include "mg/DiffusionMatrix.hpp"
#include "mg/DynamicBorder.hpp"
#include "mg/SignalReport.hpp"
#include "mg/Grid.hpp"
#include "mg/GridReport.hpp"


namespace mg
{
	class DiffuserBase
	{
		public:
            enum class DiffuserType
            {
                ORIGINAL, MATRIX
            };

            using GridRow = std::vector< GridUnit* >;
            using GridMatrix = std::vector< GridRow >;
            using Border = DynamicBorder;

		//---ctor, dtor
			DiffuserBase( DiffuserType diffuserType ) : diffuserType(diffuserType) {;}
			virtual ~DiffuserBase() = default;

		//---get

		//---set

		//---API
			virtual void calculate( GridReport* gridReport, TReal dt, size_t id, TReal kdiff, TReal kdeg, int x1, int x2, int y1, int y2 ) = 0;
			void update( GridReport* gridReport, SignalReport* sReport, TReal dt, size_t, size_t );

		private:
			DiffuserType diffuserType;
	};

	using Diffuser = DiffuserBase;

	class DiffuserOriginal : public DiffuserBase
	{
		public:
		//---ctor, dtor
			DiffuserOriginal() : DiffuserBase::DiffuserBase( DiffuserType::ORIGINAL ) {;}
			virtual ~DiffuserOriginal() = default;

			void calculate( GridReport* gridReport, TReal dt, size_t id, TReal kdiff, TReal kdeg, int x1, int x2, int y1, int y2 ) override;
	};

	class DiffuserMatrix : public DiffuserBase
	{
		public:
		//---ctor, dtor
			DiffuserMatrix() : DiffuserBase::DiffuserBase( DiffuserType::MATRIX ) {;}
			virtual ~DiffuserMatrix() = default;

			void calculate( GridReport*, TReal, size_t, TReal, TReal, int, int, int, int ) override;
	};


	class DiffuserFactory
	{
		public:
			static UP<DiffuserBase> makeDiffuser( Diffuser::DiffuserType diffuserType )
			{
				if( diffuserType == Diffuser::DiffuserType::MATRIX )
					return std::make_unique<DiffuserMatrix>();
				else
					return std::make_unique<DiffuserOriginal>();
			}
	};
}

#endif //MG_DIFFUSER_HPP