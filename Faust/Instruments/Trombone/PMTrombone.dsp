import("stdfaust.lib");

//volume / vibrato / smoothing
vol = hslider("vol",35,0,100,0.01);
vib = hslider("vib",0,-1,1,0.01);
smoo = hslider("smoo",0.1,0,1,0.1);
//brass model parameters
tubeLength = hslider("length",3.3,0,5,0.01);
lipsTension = hslider("lipsTension",0.62,0,1,0.01);
mute = hslider("mute",0,0,1,0.01);
pressure = hslider("pressure",0.05,0,1,0.01);

//non-player vibrato
autoVibHz = hslider("autoVibHz", 4, 0, 10, 0.01);
autoVibWidth = hslider("autoVibWidth", 0.05, 0, 1, 0.01);
//phasor
phasor(freq) = (+(freq/ma.SR) ~ ma.decimal);
//sine
osc(freq, amp) = sin(phasor(freq)*2*ma.PI)*amp; 
//autovib
autoVib = 1+(osc(autoVibHz, autoVibWidth));

//envelopes
toneGate = button("toneGate");
toneEnv = en.adsr(0.01,0.01,0.7,0.15,toneGate); 
pressEnv = en.adsr(0.01,0.01,0.9,0.15,toneGate)+0.1; 

tbLen=(tubeLength+(vib*0.1)) : si.smooth(ba.tau2pole(smoo));
tbTens=(lipsTension) : si.smooth(ba.tau2pole(smoo));
tbMute=(mute) : si.smooth(ba.tau2pole(smoo));
tbPress=((pressure+(vib*0.02))*autoVib)*pressEnv : si.smooth(ba.tau2pole(smoo));

//brass PM
trombone = pm.brassModel(tbLen,tbTens,tbMute,tbPress);
process = trombone*autoVib*vol*toneEnv;



