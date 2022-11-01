import("stdfaust.lib");

//____________________________________________________________________________________KICK / DRONE-GLISS
trig = button("trig");
rampT = hslider("rampT", 0.1, 0, 20, 0.01);
freq = hslider("freq", 370, 10, 600, 1);
freqDelta = hslider("freqDelta", -300, -1000,1000,1);
att = hslider("att", 0.01, 0, 15, 0.01);
rel = hslider("rel", 0.01, 0, 15, 0.01);

switchRampT = rampT*trig;
line = freq+(freqDelta*trig):si.smooth(ba.tau2pole(switchRampT));
env = en.ar(att, rel, trig);

krone = os.triangle(line)*env;

process = krone;

