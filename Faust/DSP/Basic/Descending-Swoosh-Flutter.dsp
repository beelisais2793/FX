import("stdfaust.lib");

//____________________________________________________________________________________SubtractiveSwoosh/Perc

darkness = hslider("darkness", 1500, 100, 15000, 1);
trig = button("trig");
freqDelta = hslider("freqDeltaMult",-1400,-14000,14000,1);
att = hslider("att", 0.01, 0, 15, 0.01);
rel = hslider("rel", 0.01, 0, 15, 0.01);

rampT = att+rel;

switchRampT = rampT*trig;
line = darkness+(freqDelta*trig):si.smooth(ba.tau2pole(switchRampT));
env = en.ar(att, rel, trig);

process = no.noise:fi.resonlp(line,5,0.5)*env;