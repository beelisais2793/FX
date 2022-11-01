import("stdfaust.lib");

smthFactor = hslider("smthFactor",0.2,0,2,0.01);
volSmth = hslider("volSmth",0.2,0,2,0.01);
fireWater = hslider("fireWater",0,0,1,0.01);//:si.smooth(ba.tau2pole(smthFactor));
vol = hslider("vol", 0, 0,1,0.01):si.smooth(ba.tau2pole(volSmth));
waterSmth = hslider("waterSmth",0.2,0,4,0.01);
waterVol = (0.33-(fireWater)) : max(0) *3 : si.smooth(ba.tau2pole(waterSmth));
maxWater = hslider("maxWater", 300, 0,900,0.01);
waterflow = waterVol*waterVol*maxWater +1;
fireAmt = (-0.3+(fireWater)) : max(0.1);

//Volumes
phutBalance=hslider("phut",0.4,0,1,0.01);
popBalance=hslider("pop",0.4,0,1,0.01 );
maxPop=hslider("maxPop",30,20,200,0.01);
hissBalance=hslider("hiss",0.05,0,1,0.01);
woofBalance=hslider("woof",0.1,0,1,0.01);
waterBalance=hslider("water",0.2,0,1,0.01);

//Utilities-----------
lfNoise(freq) = no.lfnoise0(freq):si.smooth(ba.tau2pole(1/freq));
randRange0(freq, lo, hi) = no.lfnoise0(freq)*((hi-lo)*0.5)+(lo+((hi-lo)*0.5));
randRange1(freq, lo, hi) = lfNoise(freq)*((hi-lo)*0.5)+(hi-(lo));
dust(freq, thresh) = no.lfnoise0(freq) >(thresh);
clip(in, thresh) = in : min(thresh) : max(thresh * -1);

//WATER
trigs = dust(waterflow, 0.7);
env = en.ar(0.015,1,trigs);
freq = ba.midikey2hz(randRange0(waterflow, 70, 98)) + (lfNoise(20)*300) + (env*17);
someWater = os.osc(freq)*0.3*env:fi.fi.lowpass3e(800);
stream = (someWater+someWater)*0.2*waterVol;

//FIRE
hiss(freq) = no.noise * lfNoise(freq)*lfNoise(freq): fi.resonhp(randRange1(freq*0.4,2500,10000),3,0.5)*hissBalance;
phut(freq) = en.ar(0.00001, randRange1(5, 0.0001,0.02), dust(freq, 0.7))*no.noise : fi.resonlp(randRange1(freq,900,3000),4,0.5) * lfNoise(freq) * phutBalance;
pop(freq) = en.ar(0.000001,0.00001,dust(freq, 0.7)):fi.resonlp(randRange1(freq*10,600,15000),1,1) * lfNoise(freq*0.2)*lfNoise(freq*0.5) * popBalance;
woof(freq) = no.pink_noise * lfNoise(freq)*lfNoise(freq)*100: fi.resonlp(randRange1(freq*0.4,80,150),3,0.5) : clip(_, 1): fi.resonlp(2000,1,1)*woofBalance;

fire = woof(1) + woof(1 + fireAmt*20) + hiss(fireAmt) + phut(1 + fireAmt*(maxPop*0.1)) + pop(fireAmt*maxPop) : clip(_, 1): co.limiter_1176_R4_mono*fireWater;


process = (stream+(fire))*vol;
// process = (stream+(fire))*vol;