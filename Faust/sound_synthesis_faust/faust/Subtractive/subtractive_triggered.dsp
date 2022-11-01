// subtractive_triggered.dsp
//
// A four voice subtractive synth. 
//
// - trigger
// - control over f0, cutoff, resonance, gain
//
// Henrik von Coler
// 2020-05-17

import("stdfaust.lib");
 
trigger0 =  button("trigger0 [midi:key 0]");
trigger1=  button("trigger1 [midi:key 1]");
trigger2=  button("trigger2 [midi:key 2]");
trigger3=  button("trigger3 [midi:key 3]");

//////////////////////////////////////////////////////////////////////////
// Define three 'module' functions 
//////////////////////////////////////////////////////////////////////////

vco(f0)          = os.sawtooth(f0);
vcf(c,r)          = ve.moog_vcf(r,c);
vca(x,gain)    = gain * x;


//////////////////////////////////////////////////////////////////////////
// A function with envelopes
//////////////////////////////////////////////////////////////////////////

voice(index,trig) =  vco(f0) : vcf(fc,res) : vca(env1) * 0.5
with
{
	// use an individual hslider for every 
         f0                = hslider("Pitch %index", 100, 5, 1000, 0.001):si.smoo;

	//trig = button("trigger%index");

         rel1 = hslider("rel_vca%index", 0.5, 0.01, 3, 0.01):si.smoo;  
         rel2 = hslider("rel_vcf%index", 0.25, 0.01, 3, 0.01):si.smoo;  

          env1 = en.arfe(0.02, rel1, 0,trig); // en.adsre(0.001,0.3,1,1,trig);
          env2 = en.arfe(0.01, rel2, 0,trig); //en.adsre(0.001,0.3,1,1,trig);

          cutoff = hslider("cutoff%index", 100, 5, 6000, 0.001):si.smoo;
          res     = hslider("res%index", 0.1, 0, 1, 0.01):si.smoo;          

          fc         = 10+env2* cutoff;

};

process = voice(0,trigger0),voice(1,trigger1),voice(2,trigger2),voice(3,trigger3) :> _,_ ;
