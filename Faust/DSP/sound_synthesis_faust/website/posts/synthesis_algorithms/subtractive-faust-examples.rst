.. title: Subtractive: Faust Examples
.. slug: subtractive-faust-examples
.. date: 2020-05-19 14:45:37 UTC
.. tags: 
.. category: faust:subtractive
.. link: 
.. description: 
.. type: text



VCO-VCA-VCF
===========

The first example for subtractive synthesis implements
a virtual chain of VCO, VCF and VCA, as shown in the
Faust diagram in [Fig.1]_.

-----

.. figure:: /images/Sound_Synthesis/subtractive/process_subtractive_1.svg
    :width: 300

.. [Fig.1] Faust diagram for the VCO-VCA-VCF example.

-----

The three modules are definied as individual
functions, with paramters controlled by horizontal sliders.
In the processing function, they are chained using
the ``:`` operator.

A resonant low pass from the  ``filters.lib`` -
the Faust Filters library - is used.

-----

.. code:: cpp
	  
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

	    cutoff      = hslider("Cutoff", 100, 5, 6000, 0.001):si.smoo;
	    f0          = hslider("Pitch", 100, 5, 16000, 0.001):si.smoo;
	    q           = hslider("Q", 1, 0.1, 5, 0.01):si.smoo;               
	    gain        = hslider("Gain", 1, 0, 1, 0.01):si.smoo;

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



	    
Triggered 
=========

The example ``subtractive_triggered.dsp`` from the repository
extends the previous sawtooth example with temporal envelopes
for VCF and VCA and implements four voices with individual
control. The block diagram is shown in [Fig.2]_.

-----

.. figure:: /images/Sound_Synthesis/subtractive/process_subtractive_2.svg
    :width: 400

.. [Fig.2] Faust diagram for the triggered subtractive example.

-----

- The example makes use of the Moog filter from the ``vaeffects.lib`` library of  virtual analog filter effects.

- Individual control over the voices is realized through the ``%`` command within the ``voice()`` function.




.. code:: cpp

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
	  
	  trigger0 =  button("trigger0 [midi:key 33]");
	  trigger1=  button("trigger1 [midi:key 34]");
	  trigger2=  button("trigger2 [midi:key 35]");
	  trigger3=  button("trigger3 [midi:key 36]");

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

