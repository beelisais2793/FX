import("stdfaust.lib");
declare name "TrueScreamer";
declare version "0.1";
declare author "ChekPuk";
declare description "Tebubescreamerish pedal, closest to my dreams of right pedal";
declare license "GPL 3.0+";

bias = hslider("t:[2]/v:advanced/[0]Bias voltage",0,-4.5,4.5,0.001);
boost = hslider("t:[2]/v:advanced/[1]pre Boost",10,1,10,0.001);
trig = hslider("t:[2]/v:advanced/[2]Diode voltage",1,0,2,0.001);
loopback_c = hslider("t:[2]/v:advanced/[3]Loopback correction",0,-0.5,0.5,0.001);
l_const = 0.232;
gain = vslider("t:[0]/h:main/[1]Gain",1,1,100,0.001);
tone = vslider("t:[0]/h:main/[2]Tone",1,0,1,0.001);
volume= vslider("t:[0]/h:main/[3]Volume",1,0,1,0.001);
bpc = checkbox("On/Off");

hpass(a,b) = fi.highpass(a,b);
lopass(a,b) = fi.lowpass(a,b);

amp= (negative,positive:opamp) ~*(loopback)
with{
    loopback = l_const + l_const*loopback_c;
    negative = hpass(1,720):lopass(1,6000);
    positive = +(1*bias): *(boost);
    diodes = atan: max(trig*-1):min(trig);
    acfilter = fi.peak_eq(10*(db),freq,bw);
    opamp(x,y)= y-x:gainsag(x):diodes: fi.dcblocker;
    gainsag(x) = *(gain*(1+abs(x)*gain));

};

filter = lopass(1,723*(1+tone)): _<: lowtone*(2-tone),hitone*tone:>_
with{
    lowtone = lopass(1,breakfreq);
    hitone = lopass(2,5000*(1+tone)):hpass(1,breakfreq);
    breakfreq = 100*(1+tone);
};
post = /(boost):*(volume);
process = _<: select2(bpc,_): _<: _,(amp:filter:post): select2(bpc,_);
Footer
