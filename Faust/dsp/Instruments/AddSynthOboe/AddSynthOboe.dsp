import("stdfaust.lib");

//OBOE ARGUMENTS
//choose note & define range (Bb3-Bb5)
fund = hslider("fundFreq", 60, 60, 72, 0.001):ba.midikey2hz;
vibratoHz = hslider("vibratoHz", 4, 0, 10, 0.01)+(randomize(0.7));
trigger = button("trigger");
legatoTrigger = button("legatoTrig");
loShelfFreq = hslider("loShelfFreq",1000,10,20000,1);
loShelfGain = hslider("loShelfGain",-36,-64,64,0.1);

randomize(amt)=no.sparse_noise(5)*amt+(amt)+(0.002);

//amp envs
vSlowEnv = en.adsr(2*randomize(2),0.01,0.8,0.03,trigger-legatoTrigger);
fastEnv1 = en.adsr(0.01*randomize(1.5),0.01,0.5,0.23,trigger);
fastEnv2 = en.adsr(0.02*randomize(1.5),0.02,0.8,0.09,trigger);
fastEnv3 = en.adsr(0.04*randomize(2.5),0.04,0.8,0.03,trigger);
fastEnv4 = en.adsr(0.08*randomize(2.5),0.08,0.8,0.03,trigger);
fastEnv7 = en.adsr(0.2*randomize(3.5),0.2,0.8,0.03,trigger);
medEnv = en.adsr(0.2*randomize(1.5),0.05,0.8,0.03,trigger);
//legato amp Envs
legatoEnv = en.adsr(0.1*randomize(1.5),0.05,1,0.1,legatoTrigger);
legatoDip(amt) = 1-(legatoEnv*amt);

vibratoWidth = hslider("vibratoWidth", 0.45, 0, 1, 0.01)*vSlowEnv*randomize(0.4);
vol = hslider("vol", 1, 0, 1, 0.001);

//phasor
phasor(freq) = (+(freq/ma.SR) ~ ma.decimal);
//sine
osc(freq, amp) = sin(phasor(freq)*2*ma.PI)*amp; 
//AM
am(freq, amt) = 1 - ((osc(freq,0.5)+0.5) * amt);
//FM
fm(freq, amt) = 1 + (osc(freq,amt));
//partial
partial(fund, ratio, amp) = sin(
        phasor(fund*ratio*fm(vibratoHz, 
        vibratoWidth*.0028*ratio)
    )*2*ma.PI)*amp;

pulse = os.pulsetrain(fund*fm(vibratoHz, 
        vibratoWidth*.0018),0.88):fi.bandpass(
            3,110,1720
        );
oboeComponents(fund, amp) = (
    no.noise*0.009*fastEnv1 +
    partial(fund, 1,  0.2)  *   fastEnv7*vol *legatoDip(0.8) +
    partial(fund, 2,  0.380)*   fastEnv2*vol *legatoDip(0.4) +
    partial(fund, 3,  0.780)*   fastEnv2*vol *legatoDip(0.3) +
    partial(fund, 4,  0.20) *   fastEnv3*vol *legatoDip(0.4) +
    partial(fund, 5,  0.090)*   fastEnv4*vol *legatoDip(0.6) +
    partial(fund, 6,  0.030)*   fastEnv7*vol *legatoDip(0.6) +
    partial(fund, 7,  0.025)*   medEnv*vol *legatoDip(0.6) +
    partial(fund, 8,  0.025)*   medEnv*vol *legatoDip(0.6) +
    partial(fund, 9,  0.02) *   medEnv*vol *legatoDip(0.6) +
    partial(fund, 10, 0.02) *   medEnv*vol *legatoDip(0.6) +
    (pulse*0.2)*en.adsr(0.01,0.01,0.8,0.03,trigger)
)*amp*am(vibratoHz, vibratoWidth):fi.resonbp(fund*fm(vibratoHz, 
        vibratoWidth*.0018),0.025,200)*vol*fastEnv2; 

oboe = oboeComponents:pm.modalModel(5, 
    (233.88, 467.6, 935.52, 1871.04, 3742.08),
    (0.15, 0.05,0.02,0.01,0.01),
    (
        ba.db2linear(-3), 
        ba.db2linear(-1),
        ba.db2linear(-2),
        ba.db2linear(-5),
        ba.db2linear(-12)
    )
    )*0.01 +(no.noise*0.003*fastEnv1);

process = oboe(fund, 0.1):fi.low_shelf(loShelfGain,loShelfFreq)<:_,_;