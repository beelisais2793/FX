%module fluidsynth
%{

#include "fluidsynth.h"
%}
#define FLUIDSYNTH_API
#define FLUID_DEPRECATED 
%ignore fluid_cmd_set_handler_set_synth;
%ignore fluid_cmd_handler_set_synth;
%include "fluidsynth/types.h"
%include "fluidsynth/settings.h"
%include "fluidsynth/synth.h"
%include "fluidsynth/shell.h"
%include "fluidsynth/sfont.h"
//%include "fluidsynth/ramsfont.h"
%include "fluidsynth/audio.h"
%include "fluidsynth/event.h"
%include "fluidsynth/midi.h"
%include "fluidsynth/seq.h"
%include "fluidsynth/seqbind.h"
%include "fluidsynth/log.h"
