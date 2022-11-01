// boids oscillator
// 200910 oswald berthold
// GPL

X75Boids : MultiOutUGen {
	*ar { arg in=0, numboids = 2, diss = 0.99, f1 = 0.002, f2 = 0.002, f3 = 0.005, mul = 1.0, add = 0.0;
		^this.multiNew('audio', in, numboids, diss, f1, f2, f3).madd(mul, add)
	}
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
