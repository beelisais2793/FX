.. title: Faust: A Simple Envelope
.. slug: faust-a-simple-envelope
.. date: 2020-05-07 09:27:37 UTC
.. tags:
.. category: faust:basics
.. link:
.. description:
.. type: text
.. priority: 7


Temporal envelopes are essential for many sound synthesis
applications. Often, triggered envelopes are used,
which can be started by a single trigger value.
Faust offers a selection of useful envelopes in
the ``envelopes`` library.
The following example uses an attack-release envelope
with exponential trajectories which can be handy for
plucked sounds.
The output of the sinusoid with fixed frequency
is simply multiplied with the ``en.arfe()`` function.

Check the envelopes in the library list for more
information and other envelopes:

https://faust.grame.fr/doc/libraries/#en.asr

.. code:: cpp

   // envelope.dsp
   //
   // A fixed frequency sine with
   // a trigger and controllable release time.
   //
   // - mono (left channel only)
   //
   // Henrik von Coler
   // 2020-05-07

   import("stdfaust.lib");

   // a simple trigger button
   trigger  = button("Trigger");

   // a slider for the release time
   release  = hslider("Decay",0.5,0.01,5,0.01);

   // generate a single sine and apply the arfe envelope
   // the attack time is set to 0.01
   process = os.osc(666) * 0.77 * en.arfe(0.01, release, 0,trigger) : _ ;
