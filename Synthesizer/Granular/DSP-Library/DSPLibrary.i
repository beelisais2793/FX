%module DSPLibrary
%{
#include "DSPLibraryInclude.h"
%}

%include "SinOsc.h"
%include "Noise.h"
%include "InterpolatingDelayLine.h"
%include "DelayLine.h"
%include "Recorder.h"
%include "GranularLine.h"
%include "DeadZone.h"
%include "SoftClip.h"
%include "Biquad.h"
%include "LevelEstimator.h"
%include "Compressor.h"
%include "Utilities.h"
