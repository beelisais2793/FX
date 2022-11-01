# modfm

This is a simple synthesizer which is an attempt to implement the "Modified Frequency Modulation" algorithm described by
Victor Lazzarini and Joseph Timoney in their paper
@ https://mural.maynoothuniversity.ie/4697/1/JAES_V58_6_PG459hirez.pdf

The ModFM method produces a sound which is less "nasal" than traditional FM (and its phase modulation variant) as the
amplitude of the sidebands are said to vary in a more linear fashion.

This synthesizer is a bit of a hybrid of a partials-additive synthesizer (ala the Kawai K5000) and an FM synth in that
it allows for multiple "drawbars" defining partials, which themselves can be modulated using frequency modulation to
produce even more complex timbres. Each of these drawbars are named a "generator" and correspond roughly to an "operator"
in a traditional phase modulation "FM" synthesizer, with the caveat that they are not chainable. 

To produce complex timbres as in a 6-op etc. FM synth, one would use multiple unchained generators rather than chained
operators like on a Yamaha synth. Up to 16 generators can be set up, each with their own envelopes and FM parameters.

It currently works as a standlone program (not a VSTi or etc.) using PortAudio to produce sound and PortMidi to
receive MIDI note events. It has a simple GUI written which enables basic parameter editing and an additive "drawbar"
style interface.

Work in progress / TODO:
  * Multi-stage, looping, envelopes instead of simple ADSR.
  * Superior modulation options beyond just envelopes. (LFO and maybe other modulator types)
  * Envelope presets / groups for easing modulation configuration.
  * User interface to support graphical editing of the above features.
  * General UI improvements.
    
It should in theory be portable to multiple platforms but I have so far run it only on Linux.

It is currently 8 voice polyphonic, but monotimbral. Amplitude mixing for multiple voices is not ideal, as it produces
clipping on chords. Legato, portamento etc have not been implemented. The envelope generator needs tweeking. There is no
support for saving or loading patches yet. There is no support for MIDI continuous controller input yet.

There are undoubtably bugs, and I can't guarantee my implementation of the math described in the paper is correct. It
has also not been optimized for performance at this time.

