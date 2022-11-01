declare author "Bart Brouns";
declare license "GPLv3";
declare name "formantOscs";
import("stdfaust.lib");

process =
  fund(freq,phase,reset)<:
  (
    oscChooser(oscTypeL)
  , oscChooser(oscTypeR)
  );


oscTypeL    = hslider ("[0] oscillator type left [scale:int][style:menu{'PAF':0;'FM':1;'FOF':2}]", 0, 0, 2, 1);
oscTypeR    = hslider ("[1] oscillator type right[scale:int][style:menu{'PAF':0;'FM':1;'FOF':2}]", 1, 0, 2, 1);
reset       = button  ("[2]reset");
freq        = hslider ("[3]freq", 440, 27.5, 3520, 0.1)              :si.smoo;
phase       = hslider ("[4]phase", 0, -1, 1, 0.001)                  :si.smoo;
formantFreq = hslider ("[5]formant frequency", 440, 27.5, 3520, 0.1) :si.smoo;
index       = hslider ("[6]index",	1, 0, 1, 0.001)                  :si.smoo;
attack      = hslider ("[7]FOF attack", 2, 0.001, multi, 0.001)      :si.smoo;
release     = hslider ("[8]FOF release", 2, 0.001, multi, 0.001)     :si.smoo;
octavation  = hslider ("[9]FOF octavation",0,0,maxOctavation,0.001)  :si.smoo;

///////////////////////////////////////////////////////////////////////////////
//                                  general                                  //
///////////////////////////////////////////////////////////////////////////////

fund(freq,phase,reset) = lf_sawpos_phase_reset(freq,phase,reset);
lf_sawpos_phase_reset(freq,phase,reset) = lf_sawpos_reset(freq,reset) + phase : ma.frac;
lf_sawpos_reset(freq,reset) = ma.frac * (reset == 0) ~ +(freq/ma.SR);

oscChooser(type) =
  select3(type
         , (PAF(center,index*1000,1))
         , (FMformant(index*40,formantFreq)*0.0625)
         , (octavedFOF(octavation)));

// TODO: make work with negative frequency
fund2freq(fund) = delta*ma.SR
with {
  absDelta = abs(fund-fund');
  delta = select2(absDelta>0.5,absDelta,1-absDelta);
};

///////////////////////////////////////////////////////////////////////////////
//                               PAF oscilator                               //
///////////////////////////////////////////////////////////////////////////////

// http://msp.ucsd.edu/techniques/v0.11/book-html/node96.html
//
center = formantFreq / abs(freq);
// deglitch adapted from Chris Chafe:
// http://chrischafe.net/glitch-free-fm-vocal-synthesis/
// center, fund, index, volume
PAF(c,i,v,f) =
  BasicPAF(ef,f,i,ea)+
  BasicPAF(of,f,i,oa)
with {
  ci     = int(floor(c));          // integer harmonic below center freq
  ci1    = ci+1;                   // and above
  isEven = ba.if(((ci%2)<1),1,0);     // below is even?
  ef     = ba.if(isEven,ci,ci1);      // then set even harmonic to lowest
  of     = ba.if(isEven,ci1,ci);      // and odd harmonic to highest
  frac   = c-ci;                   // fractional frequency remainder
  comp   = 1-frac;                 // and it's complement
  oa     = ba.if(isEven,frac,comp)*v; // odd harmonic amplitude
  ea     = ba.if(isEven,comp,frac)*v; // even harmonic amplitude
};


// center, fund, index, volume
BasicPAF(c,f,i,v)=
  (((cos12(c,f))*bell(f,i)) * v) * volumeCompensate
with {
  Nbellcurve(x) = curve(x*100);
  bellcurve(x) = rdtable(belltablesize + 1,ba.time:curve,int(x*(belltablesize/200)):max(0):min(belltablesize));
  belltablesize 	= 1 << 16;
  curve 	= (((_-(belltablesize*0.5))/(belltablesize*0.125))<:exp((_*_)*-1));
  // belltablesize 	= 200;
  // curve 	= ba.time:(((_-100)/25)<:exp((_*_)*-1));
  // curve 	= ba.time:(((x-100)/25)<:exp((w*w)*-1));
  volumeCompensate = 0.666+(i/300)*0.75:min(1)*0.25;
  // volumeCompensate = 1;
  wrap            = _<:(_>0,(_,1:fmod)+1,(_,1:fmod)):select2;
  centerWrap(c,f) = c:sampleAndHold(f)<:wrap;
  centerMin(c,f)  = c:sampleAndHold(f)-centerWrap(c,f);
  // different from library ba.sAndH because that one has ``sample`` instead of ``sample!=0``
  sampleAndHold(sample) = select2((sample!=0)) ~ _;
  cos12(c,f)      = (centerMin(c,f)*f<:(_*2*ma.PI:cos)<:(_,_((_,(_+f:(_*2*ma.PI:cos))):_-_:(_*centerWrap(c,f))) )):_+_;
  // bell(f,i)       = (((f*0.5)-0.25:(_*4*ma.PI:cos))*i)+100:bellcurve;
  bell(f,i)       = ((((f*0.5)-0.25)*2*ma.PI:cos)*i)+100:bellcurve;
};

///////////////////////////////////////////////////////////////////////////////
//                           FM formant oscilator                            //
///////////////////////////////////////////////////////////////////////////////

// declare name 		"FMVox";
// declare version		"1.0";
// declare author		"Chris Chafe";
// declare license		"BSD";
// declare copyright	"stk";


// ------ Program A of "Glitch Free FM Vocal Synthesis" (minor corrections from article)

// ------ table lookup oscillators -----//
ts 	= 1 << 16;			// table size
fs 	= float(ts);
ts1	= ts+1;
ct 	= (+(1)~_ ) - 1;		// incrementing counter from 0
fct	= float(ct);
sc 	= fct*(2*ma.PI)/fs:sin;		// sine table for carrier
sm	= fct*(2*ma.PI)/fs:sin:/(2*ma.PI);	// sine table for modulator
dec(x)	= x-floor(x);			// fractional remainder
pha(f)	= f/float(ma.SR):(+:dec) ~ _;	// generates a index signal
tbl(t,p)= s1
          // tbl(t,p)= s1+dec(f)*(s2-s1)		// looks up linearly interpolated table value
with {
  f = p*fs;
  i = int(f):min(ts);
  s1 = rdtable(ts1,t,i);
  s2 = rdtable(ts1,t,i+1);
};

// synced to an external phaser "fund"
// the parameter eo sets the proportion of even and odd harmonics.
FMformant(b,c,fund) = even+odd	// outputs the sum of bracketing harmonics
with {					// from f0, amp, bandwidth, center freq
  f0 = fund2freq(fund);
  cf 	= c/f0;
  ci	= int(floor(cf));			// integer harmonic below center freq
  ci1	= ci+1;				// and above
  isEven= ba.if(((ci%2)<1),1,0);	// below is even?
  ef 	= ba.if(isEven,ci,ci1);		// then set even harmonic to lowest
  of 	= ba.if(isEven,ci1,ci);		// and odd harmonic to highest
  frac	= cf-ci;			// fractional frequency remainder
  comp	= 1-frac;
  oa 	= ba.if(isEven,frac,comp);		// odd harmonic amplitude
  ea 	= ba.if(isEven,comp,frac);		// even harmonic amplitude
  m 	= tbl(sm,fund):*(b);		// modulator sine signal
  even 	= ea:*(tbl(sc,(dec(ef*fund+m)))); // even harmonic signal with phase modulation
  odd 	= oa:*(tbl(sc,(dec(of*fund+m))));	// odd harmonic signal
};

///////////////////////////////////////////////////////////////////////////////
//                                   FOF                                     //
///////////////////////////////////////////////////////////////////////////////

// declare author "Bart Brouns";
// declare license "GPLv3";
// declare name "fof";

// adapted from:
// https://ccrma.stanford.edu/~mjolsen/pdfs/smc2016_MOlsenFOF.pdf

//                        Cyclic Impulse Train Streams                       //
// impulse train at frequency of fund
// clk(fund) = (1-1')+(fund<:-(mem)<0)';
clk(fund) = (1-1') +(
              abs(fund-fund')>0.5
            )';
// impulse train at frequency f0 split into n cycles
clkCycle(n,fund) = clk(fund) <: par(i,n,resetCtr(n,(i+1)));
// function that lets through the mth impulse out of
// each consecutive group of n impulses
resetCtr(n,m) = _ <: (_,ctr(n)) : (_,(_==m)) : *;
                                               // function to count nonzero inputs and reset after
                                               // receiving x of them
                                               ctr(x) = (+(_)~(negSub(x)));
// function that subtracts value x from
// input stream value if input stream value >= x
negSub(x)= _<: (_>=x,_,_):((-1*_),_,_):((_*_),_):(_+_);


// main process
octavedFOF(octavation,fund) =
  (
    clkCycle(multi,fund)
    : par(i,multi,myFOF(fund)*OctMuliply(i))
      :> _
         * (octavation:max(1)/maxOctavation)
  )
;


myFOF(fund,fofTrig) =
  fofEnv(fund,fofTrig)*fofSlaveOsc(fofTrig);

fofEnv(fund,fofTrig) =
  curved_ar(fund,attack,release,fofTrig) ;

fofSlaveOsc(fofTrig) =
  // fund(formantFreq,0,fofTrig)
  // : basicCZosc(oscType,index,reset);
  rdtable(pl.tablesize
         , os.sinwaveform(pl.tablesize)
         , int(pl.tablesize * fund(formantFreq,0,fofTrig)));
// fund(formantFreq,0,fofTrig)
// *2*ma.PI:sin;

curved_ar(fund,a,r,gate) =
  FB~(_,_) // we need the previous state and value
     :!,_ // we only need the actual envelope output
with {
  FB(prevState, prevValue) =
    // states are :0 = release, 1= attack
    state
    // the actual value of the envelope
   ,envelope
  with { // so we can use prevState by name, without passing it as an argument
  state = (attackRamp < 1) | trigAttack;
  trigAttack= gate;
  attackRamp = ramp(attSamples,trigAttack);
  attSamples   = int( att * ma.SR / fund2freq(fund) );
  att = (a/scale);
  scale = ((a+r)/multi):max(1);
  envelope = it.interpolate_linear(theRamp:SINshaper,from,to(state));
  theRamp = ramp(samples,trig);
  SINshaper(x) = sin(ma.PI*(x-0.5))*0.5+0.5;
  samples = int( length(state) * ma.SR / fund2freq(fund) );
  length(state) = select2(state,r,a)/scale;
  trig = trigAttack, trigRelease :> _>0;
  trigRelease =
    (((prevState==1) & (attackRamp ==1)))  // doesn not auto-impulsify because prev will NOT increase
    | trigAttack & (attSamps==0);
  from = prevValue : ba.sAndH(trig);
  to(state) = select2(state,0,1); // release to 0, attack to 1, decay to sustain-level
  attSamps = int(a * ma.SR);
  // ramp from 1/n to 1 in n samples.  (don't start at 0 cause when the ramp restarts, the crossfade should start right away)
  // when reset == 1, go back to 1/n.
  // ramp(n,reset) = select2(reset,_+(1/n):min(1),0)~_;
  ramp(n,reset) = (select2(reset,_+(1/n):min(1),1/n)~_)+(n<1):min(1);
};
};


OctMuliply(i) =
  (
    (OctMuliplyPart(i,floor(octavation+1))*    ma.decimal(octavation)) +
    (OctMuliplyPart(i,floor(octavation  ))* (1-ma.decimal(octavation)))
  )
;
OctMuliplyPart(i,oct) =
  select2(i>0,1,
          (((i % int(2:pow(oct)))):min(1)*-1)+1) ;

multi = 2:pow(maxOctavation);
maxOctavation = 4;

