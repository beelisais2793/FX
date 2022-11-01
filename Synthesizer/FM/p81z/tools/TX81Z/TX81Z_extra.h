#pragma once
#include "TX81Z_common.h"

namespace TX81Z {
// Based on information found at
//    https://github.com/eclab/edisyn/blob/master/edisyn/synth/yamahatx81z/YamahaTX81Z.java
float computeRatio(unsigned coarse, unsigned fine);
float computeFrequency(unsigned range, unsigned coarse, unsigned fine);

// Based on information https://github.com/ngeiswei/deicsonze
//    dubious in the the lowest values (manual 0.007 Hz vs formula 0.070 Hz)
float lfoSpeedToFrequency(unsigned speed);
}
