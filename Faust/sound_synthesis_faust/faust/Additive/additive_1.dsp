

import("music.lib");
import("filter.lib");

freq = hslider("freq",300,20,2000,0.01) : smooth(0.999);
gain = hslider("gain",0.3,0,1,0.01) : smooth(0.999);
t = hslider("attRel (s)",0.1,0.001,2,0.001);
gate = button("gate") : smooth(tau2pole(t));

process = osc(freq),osc(freq*2),osc(freq*3) :> /(3) : *(gain)*gate;
