import("stdfaust.lib");
import("maths.lib");
declare options "[midi:on]";


// Declaring main process knobs
c_f    = nentry("freq",200,40,2000,0.01) : si.polySmooth(gate,0.999,2);
m_gain = nentry("gain",1,0,1,0.01) : si.polySmooth(gate,0.999,2);
gate = button("gate") : si.smoo;
glob_att = hslider("Global_Attack", 0.01, 0, 1, 0.01):si.smoo;
glob_rel = hslider("Global_Release", 0.1, 0, 5, 0.01):si.smoo;

//Mod Freq'S as fixed multiples of Carrier Frequency
mod_freq1 = 1/4*c_f;

mod_freq2 = 3/4*c_f;

mod_freq3 = 1/2*c_f;

mod_freq4 = 2*c_f;

mod_freq5 = 3*c_f;

mod_freq6 = 4*c_f;

// Index for amount of FM Modulation per Osci
index_max = 2000;

index1 = hgroup("[1]Index Mods",index1) 
with{   
    index1 = vslider("[0]index1", 1, 1, index_max, 0.5);
};
index2 = hgroup("[1]Index Mods",index2) 
with{   
    index2 = vslider("[1]index2", 1, 1, index_max, 0.2);
};
index3 = hgroup("[1]Index Mods",index3) 
with{   
    index3 = vslider("[2]index3", 1, 1, index_max, 0.2);
}; 
index4 = hgroup("[1]Index Mods",index4) 
with{   
    index4 = vslider("[3]index4", 1, 1, index_max, 0.2);
};
index5 = hgroup("[1]Index Mods",index5) 
with{   
    index5 = vslider("[4]index5", 1, 1, index_max, 0.2);
};
index6 = hgroup("[1]Index Mods",index6) 
with{   
    index6 = vslider("[5]index6", 1, 1, index_max, 0.2);
};

//Define the modulating frequencies with individual envelopes & volumes/////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////
///////////////////////////////////
//------>

/////----MODFREQ1----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////
modFreq1 = ((os.osc(mod_freq1)*index1));

a1 = hgroup("[3]Mod Attacks[style:knob]",a1)
with{
    a1 = hslider("[0]Attack1[style:knob]", 0.001, 0.001, 1.5, 0.01);
};
d1 = hgroup("[3]Mod Decays[style:knob]", d1)
with{
    d1 = hslider("[0]Decay1[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s1 = hgroup("[3]Mod Sustains[style:knob]", s1)
with{
     s1 = hslider("[0]Sustain1[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r1 = hgroup("[3]Mod Releases[style:knob]", r1)
with{
     r1 = hslider("[0]Release1[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};

modFreq1_g = modFreq1* hgroup("[1]Vol Mod1", modvol1)
with{
    modvol1 = hslider("[1]I1[style:knob]",0, 0, 1, 0.01):si.smoo;
};

/////----MODFREQ2----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////
modFreq2 = (os.osc(mod_freq2)*index2);
a2 = hgroup("[3]Mod Attacks[style:knob]",a2)
with{
    a2 = hslider("[0]Attack2[style:knob]", 0.001, 0.001, 1.5, 0.01):si.smoo;
};
d2 = hgroup("[3]Mod Decays[style:knob]", d2)
with{
    d2 = hslider("[0]Decay2[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s2 = hgroup("[3]Mod Sustains[style:knob]", s2)
with{
     s2= hslider("[0]Sustain2[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r2 = hgroup("[3]Mod Releases[style:knob]", r2)
with{
     r2 = hslider("[0]Release2[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};

modFreq2_g = modFreq2* hgroup("[1]Vol Mod1", modvol2)
with{
    modvol2 = hslider("[1]I2[style:knob]",0, 0, 1, 0.01):si.smoo;
};
/////----MODFREQ3----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////
modFreq3 = (os.osc(mod_freq3)*index3);

a3 = hgroup("[3]Mod Attacks[style:knob]",a3)
with{
    a3 = hslider("[0]Attack3[style:knob]", 0.001, 0.001, 1.5, 0.01):si.smoo;
};
d3 = hgroup("[3]Mod Decays[style:knob]", d3)
with{
    d3 = hslider("[0]Decay3[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s3 = hgroup("[3]Mod Sustains[style:knob]", s3)
with{
     s3= hslider("[0]Sustain3[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r3 = hgroup("[3]Mod Releases[style:knob]", r3)
with{
     r3 = hslider("[0]Release3[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};

modFreq3_g = modFreq3* hgroup("[1]Vol Mod1", modvol3)
with{
    modvol3 = hslider("[1]I3[style:knob]",0, 0, 1, 0.01):si.smoo;
};
/////----MODFREQ4----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////
modFreq4 = (os.osc(mod_freq4)*index4);
a4 = hgroup("[3]Mod Attacks[style:knob]",a4)
with{
    a4 = hslider("[0]Attack4[style:knob]", 0.001, 0.001, 1.5, 0.01):si.smoo;
};
d4 = hgroup("[3]Mod Decays[style:knob]", d4)
with{
    d4 = hslider("[0]Decay4[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s4 = hgroup("[3]Mod Sustains[style:knob]", s4)
with{
     s4= hslider("[0]Sustain4[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r4 = hgroup("[3]Mod Releases[style:knob]", r4)
with{
     r4 = hslider("[0]Release4[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};

modFreq4_g = modFreq4* hgroup("[1]Vol Mod1", modvol4)
with{
    modvol4 = hslider("[1]I4[style:knob]",0, 0, 1, 0.01):si.smoo;
};

/////----MODFREQ5----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////
modFreq5 = (os.osc(mod_freq5)*index5);
a5 = hgroup("[3]Mod Attacks[style:knob]",a5)
with{
    a5 = hslider("[0]Attack5[style:knob]", 0.001, 0.001, 1.5, 0.01):si.smoo;
};
d5 = hgroup("[3]Mod Decays[style:knob]", d5)
with{
    d5 = hslider("[0]Decay5[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s5 = hgroup("[3]Mod Sustains[style:knob]", s5)
with{
     s5= hslider("[0]Sustain5[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r5 = hgroup("[3]Mod Releases[style:knob]", r5)
with{
     r5 = hslider("[0]Release5[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};

modFreq5_g = modFreq5* hgroup("[1]Vol Mod1", modvol5)
with{
    modvol5 = hslider("[1]I5[style:knob]",0, 0, 1, 0.01):si.smoo;
};
/////----MODFREQ6----------///////////////////////////////////////
/////////////////////////////////////////////////////////////////

modFreq6 = (os.osc(mod_freq6)*index6);
a6 = hgroup("[3]Mod Attacks[style:knob]",a6)
with{
    a6 = hslider("[0]Attack6[style:knob]", 0.001, 0.001, 1.5, 0.01):si.smoo;
};
d6 = hgroup("[3]Mod Decays[style:knob]", d6)
with{
    d6 = hslider("[0]Decay6[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
s6 = hgroup("[3]Mod Sustains[style:knob]", s6)
with{
     s6= hslider("[0]Sustain6[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};
r6 = hgroup("[3]Mod Releases[style:knob]", r6)
with{
     r6 = hslider("[0]Release6[style:knob]",0.001, 0.001, 1.5, 0.01):si.smoo;
};


modFreq6_g = modFreq6* hgroup("[1]Vol Mod1", modvol6)
with{
    modvol6 = hslider("[1]I6[style:knob]",0, 0, 1, 0.01):si.smoo;
};
////////////////////////////////////////////////////////////////////////////


////Processsing algorithm//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////7
///////////////////////////////////////////////////////////////7
///////////////////////////////////////////////////////////
///Added a djembe for plucky attack//////////////

FM = (os.osc((c_f + modFreq1)))*en.adsre(a1,d1,s1,r1,gate), (os.osc((c_f + modFreq2)))*en.adsre(a2,d2,s2,r2,gate), (os.osc((c_f + modFreq3)))*en.adsre(a3,d3,s3,r3,gate),(os.osc((c_f + modFreq4)))*en.adsre(a4,d4,s4,r4,gate),(os.osc((c_f + modFreq5)))*en.adsre(a5,d5,s5,r5,gate),(os.osc((c_f + modFreq6)))*en.adsre(a6,d6,s6,r6,gate):>_;

//FX!!///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//Resonant Lowpass Fitler with LFO

FM_ResLof = FM: hgroup("[4]ResLoFilter_del", fi.resonlp(resFreq, q, 1))
with{   
    ctFreq = hslider("[0]CutOff Freq[style:knob]", 2000, 50, 6000, 0.1);
    q = hslider("[1]Q[style:knob]", 5, 1, 30, 0.1);
    lfoFreq = hslider("[2]LFO_Freq[style:knob]", 10, 0.1, 30, 0.01);
    lfoDepth = hslider("[3]LFO_Depth[style:knob]", 500, 1, 10000, 1);
    resFreq = ctFreq + os.osc(lfoFreq)*lfoDepth : max(30);
    
};

process =  FM_ResLof*en.ar(glob_att,glob_rel,gate) *1/6*m_gain <:_,_;


