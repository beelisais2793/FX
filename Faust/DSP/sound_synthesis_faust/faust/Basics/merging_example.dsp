import("stdfaust.lib");

// create four sine waves
// with arbitrary frequencies
s1 = 0.2*os.osc(120);
s2 = 0.2*os.osc(340);
s3 = 0.2*os.osc(1560);
s4 = 0.2*os.osc(780);

// merge them to two signals
process = s1,s2,s3,s4 :> _;
