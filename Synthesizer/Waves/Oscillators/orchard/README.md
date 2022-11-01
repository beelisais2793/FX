# Orchard
An oscillators plantation for the Daisy platform.

Inspired by my old Reaktor ensemble "Aerosynth":
https://www.native-instruments.com/en/reaktor-community/reaktor-user-library/entry/show/3431/

There are 10 independent oscillator, 5 working at high frequency range (midi 53-115) and 5 at low (midi 30-52).

- 1 pure sine 
- 1 parabolic with symmetry control ("char")
- 1 bipolar ramp with pulse width control ("char")
- 1 triangle
- 1 ramp/pulse, "char" controls pulse width and up/down ramps slew rate

Additionally there are a noise (midi 30-115) and a geiger generator.

For each oscillator and generator can be specified:

- amplitude
- panning
- pitch (in the given range)
- "character", when present (e.g. pulse width)
- ring modulation matrix (it may be possible to specify the modulating osc)

For all the oscillators and the noise generator only:

- attack
- decay
- sustain

Other features:

- global pitch quantization
- global randomization or specific for:
    
    - amplitude
    - pan
    - pitch 
    - "character"
    - envelope, ring mod and FXs
    
Stereo effects in series:

- 2-poles filter followed by a saturator
    - fiter type can be HP, BD or LP
    - cutoff
    - resonance
    - overdrive 

- three-bands resonator (2-poles LPF) followed by a compressor
    - quantized pitch (4 octaves)
    - detuning
    - resonance
    - damp (hict)
    - decay

- simple delay
    - indipendent time for L and R (clocked)
    - feedback

- reverb, 4 diffuser delays (allpass filters) in series with a clocked sine LFO for each delay time, with phase offset. There are an high shelf (midi 96) and a low shelf (midi 60) filter in series at the input
    - HF damp (high shelf gain)
    - LF damp (low shelf gain)
    - reverb time
    - room size (diffusion)
    - LFO frequency and depth (amplitude) control

- limiter

All effects with dry/wet and on/off state.

White noise is filtered by an high shelf and a low shelf filter in series, whose transition frequency is controlled by pitch input and the gain by "char".

Geiger (particle or pulse trail) has input for controlling the average rate with pitch and randomness/regularity with "char".