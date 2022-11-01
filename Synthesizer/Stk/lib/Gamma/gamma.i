%module gamma
%{
#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"
using namespace gam;
using namespace gam::phsInc;
%}

/*
%include "Gamma/arr.h"
%include "Gamma/gen.h"
%include "Gamma/ipl.h"
%include "Gamma/mem.h"
%include "Gamma/rnd.h"
%include "Gamma/scl.h"
%include "Gamma/tbl.h"
*/
%include "Gamma/Timer.h"
%include "Gamma/Types.h"

%include "modules/std_gamma.i"
%include "modules/access.i"
%include "modules/envfollow.i"
%include "modules/zerocross.i"
%include "modules/pcounter.i"
%include "modules/maxabs.i"
%include "modules/silencedetect.i"
%include "modules/threshold.i"
%include "modules/zerocrossrate.i"
%include "modules/audioio.i"
%include "modules/containers.i"
%include "modules/conversion.i"
%include "modules/dft.i"
%include "modules/delay.i"
%include "modules/domain.i"
%include "modules/effects.i"
%include "modules/filter.i"
%include "modules/formant.i"
%include "modules/noise.i"
%include "modules/oscillator.i"
%include "modules/recorder.i"
%include "modules/samples.i"
%include "modules/sampleplayer.i"
%include "modules/soundfile.i"
%include "modules/spatial.i"
%include "modules/transferfunc.i"

