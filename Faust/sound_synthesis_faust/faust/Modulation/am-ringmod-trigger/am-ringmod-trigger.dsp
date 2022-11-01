// am-ringmod.dsp
//
//
//
// Henrik von Coler
// 2020-05-11

import("stdfaust.lib");
 
f_x = hslider("Signal Frequency",100,0.01,1000,0.1);
f_m = hslider("Modulator Frequency",100,0.01,1000,0.1);

m_off = hslider("Modulator Offset",0,0,0.5,0.01);


am(fx, fm) = os.osc(fx) * ((1-m_off) * os.osc(fm) + m_off);
 
// generate a single sine and apply the arfe envelope
// the attack time is set to 0.01
process =  am(f_x,f_m) <: _,_;