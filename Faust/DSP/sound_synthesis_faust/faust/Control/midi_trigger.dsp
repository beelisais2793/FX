// midi-trigger.dsp
//
// Henrik von Coler
// 2020-05-17

import("stdfaust.lib");
freq    = nentry("freq",200,40,2000,0.01) : si.polySmooth(gate,0.999,2);
gain   = nentry("gain",1,0,1,0.01) : si.polySmooth(gate,0.999,2);
gate   = button("gate") : si.smoo; 

process = vgroup("synth",os.sawtooth(freq)*gain*gate <: _,_);



// import("stdfaust.lib");

// freq =100;
// hslider("frequency[midi:ctrl 48]",100,20,1000,0.1) : si.smoo;

// trigger = 1 when MIDI key pressed 
//               =  0 when released
// trig = button("trigger[midi:key 0,1]");

// process = os.osc(freq) * en.arfe(0.01, 1, 0,trig) <: _,_ ;