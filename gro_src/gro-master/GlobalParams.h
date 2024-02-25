#ifndef GLOBALPARAMS_H
#define GLOBALPARAMS_H

#include "Defines.h"


struct GlobalParams
/*just as a namespace, to keep the default values
there is no need for instantiation */ 
{
	public:
		static constexpr uint DF_SEED = 1234;
		static constexpr uint DF_SEED_OFFSET = 11;
		static constexpr double DF_STEP_SIZE = 0.1;
		static constexpr double DF_SENSITIVITY = 0.01;
		static constexpr size_t DF_CELLS_PER_THREAD = 10'000;
		static constexpr size_t DF_GRID_ROWS_PER_THREAD = 100;
		static constexpr size_t DF_MAX_THREADS = 10;

		static constexpr bool DF_B_AUTO_ZOOM = false;
		static constexpr bool DF_B_SHOW_PLOTS = false;
		static constexpr bool DF_B_RENDER_SIGNALS = true;
		static constexpr bool DF_B_BATCH_MODE = false;
		static constexpr bool DF_B_SHOW_PERFORMANCE = false;
};

#endif // GLOBALPARAMS_H
