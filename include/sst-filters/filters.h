#ifndef _SST_CPP_FILTERS_H
#define _SST_CPP_FILTERS_H

/** Parent namespace for all Surge Synth Team code. */
namespace sst
{

/** DSP code for Surge filters */
namespace filters
{

/**
 * Utility code needed for Surge filters.
 * Note that this code may be moved to a submodule
 * at some point in the future.
 */
namespace utilities
{
}

} // namespace filters

} // namespace sst

#include "filters/VintageLadders.h"
#include "filters/OBXDFilter.h"
#include "filters/K35Filter.h"
#include "filters/DiodeLadder.h"
#include "filters/CutoffWarp.h"
#include "filters/ResonanceWarp.h"
#include "filters/TriPoleFilter.h"

#include "filters/QuadFilterUnit_Impl.h"
#include "filters/FilterCoefficientMaker_Impl.h"

#endif
