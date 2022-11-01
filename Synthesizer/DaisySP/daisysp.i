%module daisysp 
%{
#include <daisysp.h>
%}
%include "stdint.i"
%import "daisysp.h"

/** Control Modules */
%include "Control/adenv.h"
%include "Control/adsr.h"
%include "Control/line.h"
%include "Control/phasor.h"


/** Drum Modules */
%include "Drums/analogbassdrum.h"
%include "Drums/analogsnaredrum.h"
%include "Drums/hihat.h"
%include "Drums/synthbassdrum.h"
%include "Drums/synthsnaredrum.h"



/** Dynamics Modules */
%include "Dynamics/balance.h"
%include "Dynamics/compressor.h"
%include "Dynamics/crossfade.h"
%include "Dynamics/limiter.h"

/** Effects Modules */
%include "Effects/autowah.h"
%include "Effects/bitcrush.h"
%include "Effects/chorus.h"
%include "Effects/decimator.h"
%include "Effects/flanger.h"
%include "Effects/fold.h"
%include "Effects/overdrive.h"
%include "Effects/reverbsc.h"
%include "Effects/phaser.h"
%include "Effects/pitchshifter.h"
%include "Effects/sampleratereducer.h"
%include "Effects/tremolo.h"

/** Filter Modules */
%include "Filters/allpass.h"
%include "Filters/atone.h"
%include "Filters/biquad.h"
%include "Filters/comb.h"
%include "Filters/mode.h"
%include "Filters/moogladder.h"
%include "Filters/nlfilt.h"
%include "Filters/svf.h"
%include "Filters/tone.h"

//%include "Filters/fir.h"

/** Noise Modules */
%include "Noise/clockednoise.h"
%include "Noise/dust.h"
%include "Noise/fractal_noise.h"
%include "Noise/grainlet.h"
%include "Noise/particle.h"
%include "Noise/whitenoise.h"

/** Physical Modeling Modules */
%include "PhysicalModeling/drip.h"
%include "PhysicalModeling/modalvoice.h"
%include "PhysicalModeling/pluck.h"
%include "PhysicalModeling/PolyPluck.h"
%include "PhysicalModeling/resonator.h"
%include "PhysicalModeling/KarplusString.h"
%include "PhysicalModeling/stringvoice.h"

/** Synthesis Modules */
%include "Synthesis/blosc.h"
%include "Synthesis/fm2.h"
%include "Synthesis/formantosc.h"
%include "Synthesis/harmonic_osc.h"
%include "Synthesis/oscillator.h"
%include "Synthesis/oscillatorbank.h"
%include "Synthesis/variablesawosc.h"
%include "Synthesis/variableshapeosc.h"
%include "Synthesis/vosim.h"
%include "Synthesis/zoscillator.h"

/** Utility Modules */
%include "Utility/dcblock.h"
%include "Utility/delayline.h"
%include "Utility/dsp.h"
%include "Utility/jitter.h"
%include "Utility/looper.h"
%include "Utility/maytrig.h"
%include "Utility/metro.h"
%include "Utility/port.h"
%include "Utility/samplehold.h"
%include "Utility/smooth_random.h"

