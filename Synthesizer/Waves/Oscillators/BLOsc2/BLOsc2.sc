BLOsc2 : PureUGen  {
	*ar { arg freq = 440.0, loHarmonics = 1.0, hiHarmonics = 15.0, slope = 1.0, evenOddRatio = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('audio', freq, loHarmonics, hiHarmonics, slope, evenOddRatio).madd(mul, add)
	}
	*kr { arg freq = 5.0, loHarmonics = 1.0, hiHarmonics = 15.0, slope = 1.0, evenOddRatio = 1.0, mul = 1.0, add = 0.0;
		^this.multiNew('control', freq, loHarmonics, hiHarmonics, slope, evenOddRatio).madd(mul, add)
	}
}
