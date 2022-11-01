///
// towards a hard-synced oscillator
//
// based on:
// Synchronous Programming in Audio Processing:
// A Lookup Table Oscillator Case Study
//
// no anti-aliasing
//
// HvC
// 2020-08-29

//import( "stdfaust.lib" ) ;
import( "all.lib" ) ;

// gate and single sample impulse 
gater = button ("gater");
trig =  pm.impulseExcitation(gater);

// some basic stuff
sr = SR;
twopi = 2.0*PI;


ts =  1<<16 ;
time = (+(1) ~ _ ) , 1 : - ;

// define the waveform
sawwave =  ((float(time) / float(ts)) *2 -1)*-1;

pulsewidth = hslider("pulsewidth", 0, 0, 1, 0.01);

sqaurewave = sawwave : >(0.0);


dec ( x ) = x - floor (x) ;

// from the paper:
// phase ( freq ) = freq / float ( sr ) : (+ : dec ) ~ _ : * ( float (ts) ) ;

phase = os.hs_phasor(ts,f,trig);

saw_osc( freq) = rdtable ( ts , sawwave , int ( phase  ) ) ;
square_osc( freq) = rdtable ( ts , sqaurewave , int ( phase  ) ) ;

f = hslider("f", 440, 2, 20000, 1);

 
 mix = hslider("mix", 0, 0, 1, 0.01);


process = saw_osc(f) *mix + square_osc(f)*(1-mix) <: _,_ ;

