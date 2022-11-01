import("stdfaust.lib");
//Utilities-----------
lfNoise(freq) = no.lfnoise0(freq):si.smooth(ba.tau2pole(1/freq));
randRange(freq, lo, hi) = lfNoise(freq)*((hi-lo)*0.5)+(hi-(lo));
dust(freq, thresh) = no.lfnoise(freq) >(thresh);
clip(in, thresh) = in : min(thresh) : max(thresh * -1);

//FIRE------------------
phutBalance=0.4;
popBalance=1;
hissBalance=0.05;
woofBalance=0.3;

hiss(freq) = no.noise * lfNoise(freq)*lfNoise(freq): fi.resonhp(randRange(freq*0.4,2500,10000),3,0.5)*hissBalance;
phut(freq) = en.ar(0.00001, randRange(5, 0.0,0.02), dust(freq, 0.7))*no.noise : fi.resonlp(randRange(freq,900,3000),4,0.5) * phutBalance * lfNoise(freq);
pop(freq) = en.ar(0.000001,0.00001,dust(freq, 0.7)):fi.resonlp(randRange(freq*10,600,15000),1,1) * popBalance * lfNoise(freq*0.2)*lfNoise(freq*0.5);
woof(freq) = no.pink_noise * lfNoise(freq)*lfNoise(freq)*100: fi.resonlp(randRange(freq*0.4,80,150),3,0.5) : clip(_, 1): fi.resonlp(2000,1,1);

fire(dens) = woof(1) + woof(1 + dens*0.4) + hiss(dens*0.05 ) + phut(dens*0.9) + pop(1 +(dens*100) ) : clip(_, 1): co.limiter_1176_R4_mono;

fireDens = hslider("firedens", 1, 0.5, 100, 0.01);

vol = hslider("vol", 0, 0,1,0.01);

// process = vol*hiss;
// process = phut(0.9)+pop(10)+hiss(1)+hiss(0.7),phut(0.9)+pop(10)+hiss(1)+hiss(0.7): _*vol,_*vol;
process = fire(fireDens)*vol;