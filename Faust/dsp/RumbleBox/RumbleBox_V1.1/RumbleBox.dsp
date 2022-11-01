import("stdfaust.lib");

wobWob = hslider("wobWob",0,0,10,0.01):si.smoo;
wobFreq = hslider("wobFreq",1, 0.5,10,0.01);
aMinVol = hslider("aMinVol",0,0,1,0.01):si.smoo; 
aMinEno = hslider("aMinEno",0,0,1,0.01):si.smoo;
aMinLPF = hslider("aMinLPF",250,5,800,0.01);
aMinDist = hslider("aMinDist",0.01,0,1,0.001);
techno = hslider("techno",0,0,1,0.01):si.smoo;
setPip = hslider("setPip",81, 80,110,1);
pipVol = hslider("pipVol",0.03, 0,10,0.01);
pipLP = hslider("pipLP",800,5,800,0.01);
pipDist = hslider("pipDist",0.01,0,1,0.001);
kickVol = hslider("kickVol",1.2,0,10,0.01):si.smoo;
rscal = hslider("rscal",0.2, 0,1,0.01):si.smoo;
mvol = hslider("mvol",0,0,1,0.01):si.smoo;
mcomp = hslider("mcomp",-10,-50,0,0.01):si.smoo;
mcompra = hslider("mcompra",100,1,100,0.01):si.smoo;
kikSpd = hslider("kikSpd",0.1,0.1,2,0.01);

// //WOBWOB
freqGen(freq, mul, add) = os.osc(freq)*mul + add;
wobWobOut = (
    os.osc(freqGen(2.07*wobFreq,wobFreq,35+wobFreq*20))*0.5 +
    os.osc(freqGen(3.32*wobFreq,wobFreq,35+wobFreq*21))*0.5 +
    os.osc(freqGen(5*wobFreq,wobFreq,25+wobFreq*10))*0.5
    )*wobWob:ef.cubicnl(0.01,0):fi.lowpass3e(200) : co.compressor_mono(100,-2,0.01,0.05);

//AMINENO
dust(freq, thresh) = no.lfnoise0(freq) >(thresh);
drone(freq, dens) = os.osc(freq)*(en.ar(1/(dens*dens*5),1/(dens*dens*5), dust(dens*200, 0.99)):si.smoo); 
aMDens = 5;
aMinor = (
    drone(110, aMinEno*1.1) + 
    drone(130.81, aMinEno*1.01) +
    drone(164.81, aMinEno*1.02) +
    drone(196, aMinEno*1.21) +
    drone(293.66, aMinEno*1.05) 
):ef.cubicnl(aMinDist,0):fi.lowpass3e(aMinLPF) :co.compressor_mono(100,-5,0.01,0.05)*aMinVol;


//TECHNO
kickFreq = 0.7+(techno*kikSpd);
kickTrig = os.lf_imptrain(kickFreq);
kickFM = kickTrig:si.lag_ud(0.0,0.02)*400;
kickOsc = os.osc(27.5+kickFM)+os.osc(55+kickFM);
kickEnv = en.ar(
    0.025,
    (1/kickFreq)*0.9,
    kickTrig
    );
kick = kickOsc*kickEnv:co.compressor_mono(100,-10,0.005,0.01);


pipNote = setPip:ba.sAndH(os.lf_imptrain(kickFreq*4));
timePip(note, freq, thresh) = 
    os.osc(ba.midikey2hz(note))*
    en.asr(
            (1/freq)*0.01,
            1,
            (1/freq)*0.99,
            (no.noise+(1))*os.lf_imptrain(freq):ba.sAndH(os.lf_imptrain(freq)) > (thresh)
        )
    ;
timePips=(
    timePip(pipNote, kickFreq*4, 0.8)
):ef.cubicnl(pipDist,0):fi.lowpass3e(pipLP)*pipVol; 

technoL = kick*kickVol:co.compressor_mono(100,-5,0.005,0.01)*techno:fi.lowpass3e(500);
technoR = (timePips+(technoL)):co.compressor_mono(100,-5,0.005,0.01)*techno;


//OUTPUT
left = technoL + (aMinor + (wobWobOut)) : co.compressor_mono(mcompra, mcomp, 0.01,0.02)*mvol; //+ wobWobOut;
right = (technoR+ (aMinor*0.5)+ (wobWobOut))*rscal : co.compressor_mono(mcompra, mcomp, 0.01,0.02)*mvol;
// process = left*mvol,right*mvol;
process = left,right;