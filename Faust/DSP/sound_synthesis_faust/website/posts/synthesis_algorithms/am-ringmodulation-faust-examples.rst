.. title: AM & Ringmodulation: Faust Examples
.. slug: am-ringmodulation-faust-examples
.. date: 2020-05-12 17:43:57 UTC
.. tags: 
.. category: faust:am-ringmod
.. link: 
.. description: 
.. type: text


Ringmodulator with Audio Input
==============================

The Ringmodulator is a simple, characteristic audio
effect which has been used in many contextes.
There is a large variety of guitar effect pedals
based on ringmodulation.
Another popular application is alienating voices,
as done in vintage SciFi movies.
The following example ``ringmod-input.dsp`` from the
`Faust repository <https://gitlab.tubit.tu-berlin.de/henrikvoncoler/sound_synthesis_faust>`_ modulates an audio input signal with a
sine wave of adjustable frequency.

.. code-block:: cpp

    // ringmod-input.dsp
    //
    // Ringmodulator for audio input
    //
    // - fader for controlling modulator frequency
    // - fader for controlling mix of ringmod
    //
    // Henrik von Coler
    // 2020-05-12

    import("stdfaust.lib");

    f_m     = hslider("Modulator Frequency",100,0.01,1000,0.1);

    mix     = hslider("Modulation Mix",0.5,0,1,0.01);

    am(x, fm) =  (1-mix) * x  +  mix * x *  os.osc(fm);

    process(x) =     am(x,f_m) <: _,_;		


AM - Ringmod Explorer
=====================

When used with both sinusoidal carrier and modulator,
Ringmodulator an AM become precice means for generating
timbres in electronic music contexts.
The example ``am-ringmod.dsp`` makes the tonal difference
between AM and Ringmodulation audible.

.. code-block:: cpp

		// am-ringmod.dsp
		//
		// Example for amplitude modulation 
		// and ringmodulation.
		//
		// - steady sound
		// - adjustable frequencies
		// - fader for morphing between am/ringmod
		//
		// Henrik von Coler
		// 2020-05-11

		import("stdfaust.lib");
		
		f_x = hslider("Signal Frequency",100,0.01,1000,0.1);
		f_m = hslider("Modulator Frequency",100,0.01,1000,0.1);

		m_off = hslider("Modulator Offset",0,0,0.5,0.01);


		am(fx, fm) = os.osc(fx) * ((1-m_off) * os.osc(fm) + m_off);
		

		process =  am(f_x,f_m) <: _,_;
