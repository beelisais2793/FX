import("stdfaust.lib");
origSR = 44100;
freeverb(fb1,fb2,damp,spread) = _ <: par(i,8,lbcf(combtuningL(i)+spread,fb1,damp)) :> 
	seq(i,4,fi.allpass_comb(1024,allpasstuningL(i)+spread,fb2))
with{
	dLall = (556,441,341,225);
	allpasstuningL(i) = ba.take(i+1,dLall)*ma.SR/origSR;
	
	dLcomb = (1116,1188,1277,1356,1422,1491,1557,1617);
	combtuningL(i) = ba.take(i+1,dLcomb)*ma.SR/origSR;
	
	lbcf(dt,fb,damp) = (+:@(dt)) ~ (*(1-damp) : (+~*(damp)) : *(fb));
};
stereo_freeverb(fb1,fb2,damp,spread) = + <: freeverb(fb1,fb2,damp,0),
	freeverb(fb1,fb2,damp,spread);
scaleroom = 0.28;
offsetroom = 0.7;
allpassfeed = 0.5;
scaledamp = 0.4;
fixedgain = 0.1;
combfeed = hslider("Room Size",0.5,0,1,0.025)*scaleroom*ma.SR/origSR + offsetroom;
damping = hslider("Damping",0.5,0,1,0.025)*scaledamp*ma.SR/origSR;
spatSpread = hslider("Stereo Spread",0.5,0,1,0.01)*46*ma.SR/origSR;
g = hslider("Dry/Wet",0.333,0,1,0.01);
process = _,_ <: 
	(*(g)*fixedgain,*(g)*fixedgain : stereo_freeverb(combfeed,allpassfeed,damping,spatSpread)),
	*(1-g),*(1-g) :> _,_;