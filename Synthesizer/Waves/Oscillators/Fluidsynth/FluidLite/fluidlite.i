%module fluidlite
%{
#include "fluidlite.h"
%}
#define FLUIDSYNTH_API
%include "fluidsynth/types.h"
%include "fluidsynth/settings.h"
%include "fluidsynth/synth.h"
//%include "fluidsynth/shell.h"
%include "fluidsynth/sfont.h"
%include "fluidsynth/ramsfont.h"
//%include "fluidsynth/audio.h"
//%include "fluidsynth/event.h"
%include "fluidsynth/voice.h"
//%include "fluidsynth/midi.h"
//%include "fluidsynth/seq.h"
//%include "fluidsynth/seqbind.h"
%include "fluidsynth/log.h"
%include "fluidsynth/version.h"
