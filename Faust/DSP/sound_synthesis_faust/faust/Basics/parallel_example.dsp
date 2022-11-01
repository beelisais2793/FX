import("stdfaust.lib");

freq  = hslider("Cutoff Frequency",100, 10, 1000, 0.001);

sig1  = os.square(50);
sig2  = os.square(70);

filt = fi.lowpass(5,freq); 
 
process = (sig1:filt),(sig2:filt); 