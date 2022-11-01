import("stdfaust.lib");

// two parameters as horizontal sliders
gain  = hslider("Gain",0, 0, 1, 0.01);
delay = hslider("Delay",0, 0, 10000, 1);

// source signal is a saw
sig = os.lf_imptrain(1);
 
// the processing function
process = sig : + ~ (gain * (_ ,delay : @)) ; 