import("stdfaust.lib");

freq  = hslider("frequency",100, 10, 1000, 0.001) : si.smoo;

sig  = 0.5*os.square(50);
filt = fi.lowpass(5,freq);

process = sig:filt;
