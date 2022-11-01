.. title: Wavetable Oscillator with Phase Reset
.. slug: wavetable-oscillator-with-phase-reset
.. date: 2020-08-31 21:01:01 UTC
.. tags:
.. category: faust:basics
.. link:
.. description:
.. type: text
.. priority: 9


The Faust *oscillators.lib* comes with many different
implementations of oscillators for various waveforms.
At some point one might still need a behavior not included
and lower level approaches are necessary.

This example shows how to use a
phasor to read a wavetable with a
sine waveform.
This implementation has an additional
trigger input for resetting the phase
of the oscillator on each positive
zero crossing.
This can come handy in various applications,
especially for phase-sensitive transients,
as for example in kick drums.

The example is derived from Barkati et. al (2013) and part of the
`repository <https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sound_synthesis_faust>`_:

.. code-block:: cpp

   import("stdfaust.lib");

   // some basic stuff
   sr = SR;
   twopi = 2.0*ma.PI;

   // define the waveform in table
   ts =  1<<16; // size = 65536 samples (max of unsigned short)
   time = (+(1) ~ _ ) , 1 : - ;
   sinewave =  ((float(time) / float(ts)) * twopi) : sin;

   phase = os.hs_phasor(ts,freq,trig);

   // read from table
   sin_osc( freq) = rdtable(ts ,sinewave , int(phase)) ;

   // generate a one sample impulse from the gate
   trig =  pm.impulseExcitation(reset);

   reset = button ("reset");
   freq = hslider("freq", 100, 0, 16000, 0.00001);

   // offset = hslider("offset", 0, 0, 1, 0.00001);

   process = sin_osc(freq);


.. publication_list:: bibtex/oscillators.bib
		      :style: unsrt
