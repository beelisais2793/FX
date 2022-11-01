declare author "Bart Brouns";
declare license "GPLv3";
declare name "fof";
import("stdfaust.lib");
import("CZ.lib");

// process =
// fof;
// adapted from:
// https://ccrma.stanford.edu/~mjolsen/pdfs/smc2016_MOlsenFOF.pdf



/////////////////////////////////////////////////////////////////////////////
//                     Hard-Syncing Wavetable Oscillator                   //
/////////////////////////////////////////////////////////////////////////////
// resettable phasor, clock val > 0 resets phase to 0

ph(f0,c)=
  inc
  :(+:d)~
  (-(_<:(_,*(_,clk))))
  :*(pl.tablesize)

with {
  clk = c>0;
  d = ma.decimal;
  inc = f0/float(ma.SR);
};
// sin lookup table with resettable phase
oscpr(f0,c) =
  rdtable(pl.tablesize
         , os.sinwaveform(pl.tablesize)
         , int(ph(f0,c)));

oscpr2(f0,c) =
  fund(f0,0,c)
  : basicCZosc(oscType,index,res);
// sinwaveform 	= float(time)*(2.0*PI)/float(tablesize) : sin;
// sinwaveform(tablesize) = float(ba.time)*(2.0*ma.PI)/float(tablesize) : sin;
///////////////////////////////////////////////////////////////////////////////
//                            FOF Generation System                          //
///////////////////////////////////////////////////////////////////////////////

// function to generate a single Formant-Wave-Function
fof(fc,bw,a,g) =
  _ <: (_',_)
  : (f * s)
with {
  T
  = 1/ma.SR;
  // sampling period
  pi = ma.PI;
  u1 = exp(-a*pi*T);
  u2 = exp(-bw*pi*T);
  a1 = -1*(u1+u2);
  a2 = u1*u2;
  G0 = 1/(1+a1+a2);
  // dc magnitude response
  b0 = g/G0;
  // normalized filter gain
  s
  = oscpr2(fc);
  // wavetable oscillator
  f
  = fi.tf2(b0,0,0,a1,a2); // biquad filter
};

///////////////////////////////////////////////////////////////////////////////
//                        Cyclic Impulse Train Streams                       //
///////////////////////////////////////////////////////////////////////////////

// import the oscillator library
ol = library("oscillator.lib");
// impulse train at frequency f0
clk(f0) = (1-1')+ol.lf_imptrain(f0)';
// impulse train at frequency f0 split into n cycles
clkCycle(n,f0) = clk(f0) <: par(i,n,resetCtr(n,(i+1)));
// function that lets through the mth impulse out of
// each consecutive group of n impulses
resetCtr(n,m) = _ <: (_,ctr(n)) : (_,(_==m)) : *;
                                               // function to count nonzero inputs and reset after
                                               // receiving x of them
                                               ctr(x) = (+(_)~(negSub(x)));
// function that subtracts value x from
// input stream value if input stream value >= x
negSub(x)= _<: (_>=x,_,_):((-1*_),_,_):((_*_),_):(_+_);

///////////////////////////////////////////////////////////////////////////////
//                          FOF with Sample-and-Hold                         //
///////////////////////////////////////////////////////////////////////////////

// sample and hold filter coefficients
curbw = (_,bw) : ba.latch;
cura = (_,a) : ba.latch;
// FOF sample and hold mechanism
fofSH =
  _ <: (curbw,cura,_) : (fc,_,_,g,_') : fof ;

///////////////////////////////////////////////////////////////////////////////
//                              Example Program                              //
///////////////////////////////////////////////////////////////////////////////

/************** parameters/GUI controls **************/
// fundamental freq (in Hz)
f0 = hslider("f0",220,0,2000,0.01):si.smoo;
// formant center freq (in Hz)
fc = hslider("Fc",800,100,6000,0.01):si.smoo;
// FOF filter gain (in dB)
g = ba.db2linear(hslider("Gain",0,-40,40,0.01)):si.smoo;
// FOF bandwidth (in Hz)
bw = hslider("BW",80,1,10000,1):si.smoo;
// FOF attack value (in Hz)
a = hslider("A",90,1,10000,1):si.smoo;


// main process
process =
  // fof
  // rdtable(pl.tablesize, (ba.time:os.sinwaveform), int(ph(f0,c)))


  clkCycle(multi,f0)
  <:
  par(i,multi,fofSH*OctMuliply(i))
  :> _
;

OctMuliply(i) =
  (OctMuliplyPart(i,floor(octaviation+1))*    ma.decimal(octaviation)) +
  (OctMuliplyPart(i,floor(octaviation  ))* (1-ma.decimal(octaviation)));
OctMuliplyPart(i,oct) = select2(i>0,1,
                                (((i % int(2:pow(oct)))):min(1)*-1)+1
                                 );

multi = 2:pow(maxOctavation);
maxOctavation             = 4;
octaviation = hslider("octaviation",0,0,maxOctavation,0.001):si.smoo;
