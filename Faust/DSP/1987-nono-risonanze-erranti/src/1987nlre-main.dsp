import("stdfaust.lib");
import("../faust-libraries/seam.lib");

// ----------------------------------------------------------------- INSTRUMENTS
contr = vgroup("[01] CONTRALTO", chstrip);// <: hgroup("[90] CONTRALTO", (*(fader) : vmeter), sends));
flaut = vgroup("[02] FLAUTI", chstrip);// <: hgroup("[90] FLAUTI", (*(fader) : vmeter), sends));
btuba = vgroup("[03] TUBA", chstrip);// <: hgroup("[90] TUBA", (*(fader) : vmeter), sends));
csard = vgroup("[04] CAMPANE SARDE", chstrip);// <: hgroup("[90] CAMPANE SARDE", (*(fader) : vmeter), sends));
bongo = vgroup("[05] BONGOS", chstrip);// <: hgroup("[90] BONGOS", (*(fader) : vmeter), sends));
crota = vgroup("[06] CROTALI", chstrip);// <: hgroup("[90] CROTALI", (*(fader) : vmeter), sends));

instruments = si.bus(18) <: hgroup("[01] INSTRUMENTS", contr, flaut, btuba, csard, bongo, crota);// :> si.bus(8) ;

// ----------------------------------------------------------------------- SENDS
adel = checkbox("[01] DELAY") : si.smoo;
are4 = checkbox("[02] REVERB 4 SEC") : si.smoo;
ar80 = checkbox("[03] REVERB 10~80 SEC") : si.smoo;
ahar = checkbox("[04] HARMONIZER") : si.smoo;
aha1 = checkbox("[05] HALAPHON 1") : si.smoo;
aha2 = checkbox("[06] HALAPHON 2") : si.smoo;
aha3 = checkbox("[07] HALAPHON 3") : si.smoo;

dire = checkbox("[08] DIRECT") : si.smoo;

sends = vgroup("[99] SENDS" , *(adel), *(are4), *(ar80), *(ahar), *(aha1), *(aha2), *(aha3), *(dire)) ;

// ----------------------------------------------------------------- ELECTRONICS
// ---------------------------------------------------------------------- DELAYS
fbgroup(x) = hgroup("Feedback Delay", x);

fbgain1 = fbgroup(vslider("Fb 1", 0.,0.,1.,0.1) : si.smoo);
fbgain2 = fbgroup(vslider("Fb 3", 0.,0.,1.,0.1) : si.smoo);
fbgain3 = fbgroup(vslider("Fb 5", 0.,0.,1.,0.1) : si.smoo);
fbgain4 = fbgroup(vslider("Fb 7", 0.,0.,1.,0.1) : si.smoo);

D1 = ba.sec2samp(5.0);
D2 = ba.sec2samp(5.5);
D3 = ba.sec2samp(6.2);
D4 = ba.sec2samp(6.6);
D5 = ba.sec2samp(7.3);
D6 = ba.sec2samp(7.7);
D7 = ba.sec2samp(8.2);
D8 = ba.sec2samp(9.1);

dlbk = delbank : _, ro.cross(2), _, _, ro.cross(3) : si.bus(5), ro.cross(2), _; // route the delayed signal to 1 3 2 4 5 7 8 6

// ----------------------------------------------------------------- HALAPHON x3
// ---------------------------------- gli halaphon dovvrebbero stare in nono.lib
h1ramp = os.lf_sawpos(1.0/(hslider("[01] h1 time", 3.0, -23.0, 23.0, 0.01)));
h2ramp = os.lf_sawpos(1.0/(hslider("[01] h2 time", 3.0, -23.0, 23.0, 0.01)));
h3ramp = os.lf_sawpos(1.0/(hslider("[01] h3 time", 3.0, -23.0, 23.0, 0.01)));

h1dist = hslider("[02] h1 distance", 1, 0, 1, 0.01);
h2dist = hslider("[02] h2 distance", 1, 0, 1, 0.01);
h3dist = hslider("[02] h3 distance", 1, 0, 1, 0.01);

//gain = vslider("[1]", 0, -70, +0, 0.1) : ba.db2linear : si.smoo;

h1(v) = vgroup("Ch %v", hmeter);
h2(v) = vgroup("Ch %v", hmeter);
h3(v) = vgroup("Ch %v", hmeter);

h1meters = vgroup("h1 meters", par(i, 4, h1(i)));
h2meters = vgroup("h2 meters", par(i, 4, h2(i)));
h3meters = vgroup("h3 meters", par(i, 4, h3(i)));

hal1 = vgroup("h1", sp.spat(4, h1ramp, h1dist) : h1meters);
hal2 = vgroup("h2", sp.spat(4, h2ramp, h2dist) : h2meters);
hal3 = vgroup("h3", sp.spat(4, h3ramp, h3dist) : h3meters);

hals = hgroup("HALAPHONS", hal1, hal2, hal3 :> si.bus(4));

rev4 =  _ <: rev_quattro(16,5,3);

rev80 = _ <: rev_ottanta(16,5,3);

ch1a8 = delbank, rev80 :> si.bus(8);

main = vgroup("[03] MAIN", sends, sends, sends, sends, sends, sends :> dlbk, rev4, rev80, harm, hals, direct);

outs = si.bus(10);

harm = harmonizer;

direct = _;

process = tgroup("PANELS", instruments <: main);// : meterbridge : main);

vmeter(x)		= attach(x, envelop(x) : vbargraph("[02][unit:dB] Meter", -70, +5));
hmeter(x)		= attach(x, envelop(x) : hbargraph("[05][unit:dB] Meter", -70, +5));
envelop = abs : max ~ -(1.0/ma.SR) : max(ba.db2linear(-70)) : ba.linear2db;

fader	= vslider("[01] Volume", -96, -96, +12, 0.1) : ba.db2linear : si.smoo ;
