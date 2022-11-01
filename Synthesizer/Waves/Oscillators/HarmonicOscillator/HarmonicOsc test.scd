(
{HarmonicOsc.ar(displacement:0.5, velocity:0.0, acceleration:0.0, force:0.0, mass:0.00001, b:2.0, mul:1.0, add:-0.25)}.scope
)

(
(SynthDef(\harmonicOscTest,
	{arg displacement = 0.5, velocity = 0.0, acceleration = 0.0, force = 0.0, mass = 0.000001, b = 1.0, mul = 1.0, add = 0.0;
		HarmonicOsc.ar(displacement, velocity, acceleration, force, mass, b, mul, add).scope})).load
)

a = Synth(\harmonicOscTest)
