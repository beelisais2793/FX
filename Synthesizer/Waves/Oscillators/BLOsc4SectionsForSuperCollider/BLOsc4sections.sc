BLOsc4sections : PureUGen  {
	*ar {
		arg
		freq = 440.0,

		loHarmonics1 = 1.0,
		hiHarmonics1 = 5.0,
		slope1 = 1.0,
		evenOddRatio1 = 1.0,
		spread1 = 1,
		sectionAmp1 = 1.0,

		loHarmonics2 = 0.0,
		hiHarmonics2 = 0.0,
		slope2 = 1.0,
		evenOddRatio2 = 1.0,
		spread2 = 1,
		sectionAmp2 = 1.0,

		loHarmonics3 = 0.0,
		hiHarmonics3 = 0.0,
		slope3 = 1.0,
		evenOddRatio3 = 1.0,
		spread3 = 1,
		sectionAmp3 = 1.0,

		loHarmonics4 = 0.0,
		hiHarmonics4 = 0.0,
		slope4 = 1.0,
		evenOddRatio4 = 1.0,
		spread4 = 1,
		sectionAmp4 = 1.0,

		spreadCompensation = 1,

		mul = 1.0,
		add = 0.0;

		^this.multiNew('audio',
		freq,

		loHarmonics1,
		hiHarmonics1,
		slope1,
		evenOddRatio1,
		spread1,
		sectionAmp1,

		loHarmonics2,
		hiHarmonics2,
		slope2,
		evenOddRatio2,
		spread2,
		sectionAmp2,

		loHarmonics3,
		hiHarmonics3,
		slope3,
		evenOddRatio3,
		spread3,
		sectionAmp3,

		loHarmonics4,
		hiHarmonics4,
		slope4,
		evenOddRatio4,
		spread4,
		sectionAmp4,

		spreadCompensation,
		).madd(mul, add)
	}

	*kr { arg
		freq = 1.0,

		loHarmonics1 = 1.0,
		hiHarmonics1 = 5.0,
		slope1 = 1.0,
		evenOddRatio1 = 1.0,
		spread1 = 1,
		sectionAmp1 = 1.0,

		loHarmonics2 = 0.0,
		hiHarmonics2 = 0.0,
		slope2 = 1.0,
		evenOddRatio2 = 1.0,
		spread2 = 1,
		sectionAmp2 = 1.0,

		loHarmonics3 = 0.0,
		hiHarmonics3 = 0.0,
		slope3 = 1.0,
		evenOddRatio3 = 1.0,
		spread3 = 1,
		sectionAmp3 = 1.0,

		loHarmonics4 = 0.0,
		hiHarmonics4 = 0.0,
		slope4 = 1.0,
		evenOddRatio4 = 1.0,
		spread4 = 1,
		sectionAmp4 = 1.0,

		spreadCompensation = 1,

		mul = 1.0,
		add = 0.0;

		^this.multiNew('control',
		freq,

		loHarmonics1,
		hiHarmonics1,
		slope1,
		evenOddRatio1,
		spread1,
		sectionAmp1,

		loHarmonics2,
		hiHarmonics2,
		slope2,
		evenOddRatio2,
		spread2,
		sectionAmp2,

		loHarmonics3,
		hiHarmonics3,
		slope3,
		evenOddRatio3,
		spread3,
		sectionAmp3,

		loHarmonics4,
		hiHarmonics4,
		slope4,
		evenOddRatio4,
		spread4,
		sectionAmp4,

		spreadCompensation,
		).madd(mul, add)
	}
}
