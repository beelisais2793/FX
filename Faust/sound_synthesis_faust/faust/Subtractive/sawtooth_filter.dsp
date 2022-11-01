// sawtooth-filter.dsp
//
// First steps with a VCO-VCA-VCF setup.
// The three modules are connected in series.
// 
// No anti-aliasing!
//
// - steady sound
// - control over f0, cutoff, resonance, gain
//
// Henrik von Coler
// 2020-05-17

import("stdfaust.lib");

//////////////////////////////////////////////////////////////////////////
// Control Parameters
//////////////////////////////////////////////////////////////////////////

cutoff         = hslider("Cutoff [midi:ctrl 48]", 100, 5, 6000, 0.001):si.smoo;
f0                = hslider("Pitch[midi:ctrl 49]", 100, 5, 500, 0.001):si.smoo;
q                 = hslider("Q[midi:ctrl 50]", 1, 0.1, 100, 0.01):si.smoo;               
gain            = hslider("Gain[midi:ctrl 51]", 1, 0, 1, 0.01):si.smoo;

//////////////////////////////////////////////////////////////////////////
// Define three 'module' functions 
//////////////////////////////////////////////////////////////////////////

vco        = os.sawtooth(f0);
vcf         = fi.resonlp(cutoff,q,1) ;
vca(x)    = gain * x;

//////////////////////////////////////////////////////////////////////////
// Define three 'modules' 
//////////////////////////////////////////////////////////////////////////

voice =  vco  : vcf : vca;

process = voice  <: _,_ ;