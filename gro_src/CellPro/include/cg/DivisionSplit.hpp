#ifndef CG_DIVISION_SPLIT_HPP
#define CG_DIVISION_SPLIT_HPP

#include "cg/defines.hpp"

namespace cg
{
	class DivisionSplit
	{
		public:
			static int splitExtremeCases( TReal fraction, TReal copyNum, bool daughter );
			static TReal scaleSplitFraction( TReal fraction, TReal volFraction, bool daughter );
	};
}

#endif //CG_DIVISION_SPLIT_HPP