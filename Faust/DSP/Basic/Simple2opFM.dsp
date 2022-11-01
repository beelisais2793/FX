import("stdfaust.lib");
//____________________________________________________________________________________FM2OSC
trig = button("trig");
freq = hslider("freq",200,10,20000,0.01):si.smoo;
mMul = hslider("mMul",0.5,0.0,100,0.0001):si.smoo;
dMul = hslider("dMul",0.5,0.00,10,0.0001):si.smoo;

aM = hslider("aM",0.1,0.01,10,0.01);
dM = hslider("dM",0.1,0.01,10,0.01);
sM = hslider("sM",0.8,0.0,1,0.01);
rM = hslider("rM",0.1,0.01,10,0.01);

aC = hslider("aC",0.1,0.01,10,0.01);
dC = hslider("dC",0.1,0.01,10,0.01);
sC = hslider("sC",0.8,0.0,1,0.01);
rC = hslider("rC",0.1,0.01,10,0.01);

fm2op(freq,mMul,dMul,trig, am,dm,sm,rm, ac,dc,sc,rc) =  
os.osc(
        freq+(os.osc(freq*mMul)*freq*dMul*en.adsr(am,dm,sm,rm,trig))
)*en.adsr(ac,dc,sc,rc, trig);

process = fm2op(freq,mMul,dMul,trig, aM,dM,sM,rM, aC,dC,sC,rC);
// re.mono_freeverb(0.5,0.9,0.1,0.5)