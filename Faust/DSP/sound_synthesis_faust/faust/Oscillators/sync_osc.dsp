// A simple oscillator with phase reset.
//
// Parameters
// freq: set the frequency in Hz
// reset: the phase is reset when going >= 0
// offset: the phase value on reset
//
// HvC
// 2020-08-30


import("stdfaust.lib");  

// some basic stuff
sr = SR;
twopi = 2.0*ma.PI;

// define the waveform in table
ts =  1<<16; // size
time = (+(1) ~ _ ) , 1 : - ; 
sinewave =  ((float(time) / float(ts)) * twopi) : sin;

// the wrapper function keeps a signal above 0 and below m (max)
wrapper(m,x) =  select2(x<=m, (x-m):max(0), x);

// phase is used with offset and wrap
phase = os.hs_phasor(ts,freq,trig) : +(off_trig*ts) : wrapper(ts);

// read from table
sin_osc( freq) = rdtable(ts ,sinewave , int(phase)) ;

// generate a one sample impulse from the gate
trig =  pm.impulseExcitation(reset);

// a number only changed when triggered 
// this avoids glitches when changing offset during play
off_trig = offset : ba.sAndH(trig);

// all control variables
reset = button ("reset");
freq = hslider("freq", 100, 0, 16000, 0.00001);
offset = hslider("offset", 0, 0, 1, 0.00001);

process = sin_osc(freq);