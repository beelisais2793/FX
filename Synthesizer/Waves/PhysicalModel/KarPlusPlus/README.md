

## Kar++ Feedback Synthesiser

Kar++ is a simple polyphonic Synthesiser written in C++ with the JUCE Framework using the Karplus Strong Algorithm and resonant filtering in combination with a non-linear transfer function. The transfer function allow for Feedback Values > 1.0 without Breakout, pushing the timbre into more distorted and broken territories.

## Parameters

Karplus Vol   - Volume of Karplus Strong  
Dampening     - Low Pass Filter on Impulse going into the Karplus Strong Algorithm  
Noise Sustain - Sustain Amount of Noise Impulse  
Tail          - Feedback Amount  
Instability   - Randomization of Delaytime resulting in a diffuse Pitch  
  
Resonator Vol - Volume of Resonant Feedback  
Delaytime     - Length of Buffer  
Q             - Resonance Amount  
Age           - Randomization of Delaytime resulting in a diffuse Pitch  
Detune        - Detune Amount between left and right channel  
Phase Offset  - Offsetting Phase between left and right channel  
Release       - Release Amount of Envelope  
  
Volume        - Global Volume  

## Demo

https://soundcloud.com/minim23/krma-demo/s-SSv1u3iuS8X

## References

For more information on the Karplus Strong Algorithm:  
  
[1] Karplus K., Strong A. "Digital Synthesis of Plucked-String and Drum Timbres", Computer Music Journal, 7, No. 2, The MIT Press, 1983.  
[2] Jaffe, David A., and Julius O. Smith. “Extensions of the Karplus-Strong Plucked-String Algorithm.” Computer Music Journal 7, No. 2, 1983.  
[3] Karjalainen, Matti, Vesa Valimaki, and Tero Tolonen. “Plucked-String Models: From the Karplus-Strong Algorithm to Digital Waveguides and Beyond.” Computer Music Journal 22, No. 3, 1998.  
