# Oscillators

### A collection of oscillators for Arduino

The following oscillator classes are provided :

* `BlinkOscillator` : a binary oscillator with an on/off period ratio parameter, similar to what comes out of a PWM output.
* `NoiseOscillator` : a random value generator, ramping (linearly interpolating) between random values.
* `SequenceOscillator` : another binary oscillator based on a sequence of durations that define its default frequency.
* `ShapedOscillator` : a triangular waveshape oscillator, deformable on both dimensions (time and range).

Blink and Sequence oscillators output a boolean value, Noise and Shaped oscillators output a normalized float value.
You can map these values to whatever range suits your use case, e.g blinking or fading leds, controlling various kinds of motors and actuators, or even driving some audio synthesis processes.

All oscillators have a `setFrequency` or equivalent method, implement an `update` method, and also have their own specific parameters.
To avoid aliasing issues, `update` need to be called at a frequency higher than their own operating frequency.
