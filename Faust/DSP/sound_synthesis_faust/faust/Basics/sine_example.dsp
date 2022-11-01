// sine_example.dsp
//
// Henrik von Coler
// 2020-04-21

import("stdfaust.lib");

// input parameters with GUI elements
freq  = hslider("frequency",100, 10, 1000, 0.001);
gain  = hslider("gain[style:knob]",0.5, 0, 1, 0.001);

// a sine oscillator with controllable freuency and aplitude:
process = os.osc(freq)*gain;