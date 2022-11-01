// midi-example.dsp
//
// Control a sine wave frequency with a MIDI controller.
//
// Henrik von Coler
// 2020-05-17

import("stdfaust.lib");

freq = hslider("frequency[midi:ctrl 48]",100,20,1000,0.1) : si.smoo;

process = os.osc(freq) <: _,_ ;