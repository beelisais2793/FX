.. title: Additive & Spectral: Faust Examples
.. slug: additive-spectral-faust-examples
.. date: 2020-05-06 07:25:43 UTC
.. tags: 
.. category: faust:spectral
.. link: 
.. description: 
.. type: text



Adding Partials 'Manually'
==========================


A simple example, well suited for approaching the idea
of additive synthesis in Faust is given
by Romain Michon within a `CCRMA workshop <https://ccrma.stanford.edu/~rmichon/faustWorkshops/course2015/#addSynth>`_:


.. code-block:: cpp

   import("music.lib");
   import("filter.lib");

   freq = hslider("freq",300,20,2000,0.01) : smooth(0.999);
   gain = hslider("gain",0.3,0,1,0.01) : smooth(0.999);
   t = hslider("attRel (s)",0.1,0.001,2,0.001);
   gate = button("gate") : smooth(tau2pole(t));

   process = osc(freq),osc(freq*2),osc(freq*3) :> /(3) : *(gain)*gate;


Within the process function, 
three oscillators are called in parallel by comma-separating them.
The ``:>_`` operator collects their outputs, which are subsequently
devided by 3 and amplified.


Fourier Series in a Loop
========================

The example ``fourier_series.dsp`` in the seminar's  
`Faust repository <https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sound_synthesis_faust>`_
makes use of the parallel directive within a loop,
allowing the use of more partials.

.. code-block:: cpp

    // fourier_series.dsp
    //
    // Generate a square wave through Fourier series.
    // - without control
    //
    // Henrik von Coler
    // 2020-05-06

    import("stdfaust.lib");

    // define a fundamental frequency
    f0 		  = 100;

    // define the number of partials
    n_partial = 50;

    // partial function with one argument ()
    partial(partIDX) = (4/ma.PI) * os.oscrs(f)*volume
    // arguments
    with {
    f = f0 * (2*partIDX+1);			
    volume = 1/(2*partIDX+1);
    };

    // the processing function, 
    // running 50 partials parallel
    // mono output
    process = par(i, n_partial, partial(i)) :> +;



The Faust Website Examples
==========================

The Faust website lists two `examples <https://faust.grame.fr/doc/examples/#faust-examples>`_ for additive Synthesis.
Here, each partial is represented in the graphical
user interface with individual control for temporal
envelope parameters.
This allows playing a triggered sound with a
dynamic timbre.

Expressive Timbral Control
==========================

For using additive synthesis in an expressive way,
metaparameters are essential. It is desirable to control
the behaviour od all partials and thus the timbre
with few meaningful controls.

The following example, found in the semiar's
`Faust repository <https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sound_synthesis_faust>`_, controlls the decrease in energy towards higher partials
with a single parameter:

.. code-block:: cpp

     // exponential.dsp
     //
     // Additive synthesizer with controllable 
     // exponential spectral decay.
     //
     // - continuous
     // - stereo output
     //
     // Henrik von Coler
     // 2020-05-05

     import("stdfaust.lib");

     // define a fundamental frequency
     f0 	  = 100;

     // define the number of partials
     n_partial = 50;

     slope     = hslider("s", 1, 0.1, 7, 0.01);


     // partial function 
     partial(partCNT,s) = os.oscrs(f) * volume
     // arguments
     with {
     f = f0 * (partCNT+1);					
     volume =  0.3 *  exp(s * -partCNT);
     };

     // the processing function, 
     // running 50 partials parallel
     // summing them up and applying a global gain
     process = par(i, n_partial,  partial(i,slope)) :>_ * hslider("Master Gain",0,0,1, 0.1) <: _,_;
     

   
