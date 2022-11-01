HarmonicOsc : PureUGen  {
	*ar { arg displacement = 0.5, velocity = 0.0, acceleration = 0.0, force = 0.0, mass = 0.000001, b = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', displacement, velocity, acceleration, force, mass, b).madd(mul, add)
	}
	*kr { arg displacement = 0.5, velocity = 0.0, acceleration = 0.0, force = 0.0, mass = 0.000001, b = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', displacement, velocity, acceleration, force, mass, b).madd(mul, add)
	}
}


