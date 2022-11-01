// SynthDef
(
SynthDef(\blOsc2Example, { arg freq = 440.0, lo = 1.0, hi = 15.0, slope = 1.0, eRatio = 1.0, amp = 1.0;
	Out.ar(0, Pan2.ar(BLOsc2.ar(freq, lo, hi, slope, eRatio, mul: amp), 0))
}).load
)


// Spectrum Analyzer
(
~openAnalyzer = {
var window, func;
window = Window("Analyzer", Rect(100, 100, 511, 300)); // width should be 511
func = FreqScopeView(window, window.view.bounds);
func.active_(true); // turn it on the first time;
func.freqMode_(1); // 0:linear scale, 1:log scale
window.onClose_({ func.kill }); // you must have this
window.front;
}.value
)

// Demonstration with Gui
(
var window, sound, sl1, sl2, sl3, sl4, sl5, sl6;
window = Window("Band Limited Oscillator 2", Rect(100,420,620,300));
window.view.decorator = FlowLayout(window.view.bounds);
StaticText(window,620 @ 40).string_("Band Limited Oscillator 2").align_(\center)
	.font_(Font("Helvetica", 16));
window.view.decorator.nextLine;
sl1 = EZSlider(window, 600 @ 20, "Fundamental Freq", ControlSpec(55.0,880,\exponential,default: 440.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\freq, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl2 = EZSlider(window, 600 @ 20, "Lowest Harmonics", ControlSpec(1.0,100,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\lo, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl3 = EZSlider(window, 600 @ 20, "Highest Harmonics", ControlSpec(1.0,100,default: 15),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\hi, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl4 = EZSlider(window, 600 @ 20, "Slope", ControlSpec(0.1,2.0,default: 1.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slope, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl5 = EZSlider(window, 600 @ 20, "Even to Odd Ratio", ControlSpec(0,1.0,default: 1.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\eRatio, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl6 = EZSlider(window, 600 @ 20, "Amp", ControlSpec(0,1.0,\amp,default: 1.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\amp, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.left_(300).top_(230);
Button(window, 50@50)
	.states_([["Start"], ["Stop", Color.black, Color.red(alpha: 0.2)]])
	.action_({|view|  if (view.value ==1 && sound.isNil,
	{sound =
		Synth(\blOsc2Example,
			[\freq,sl1.value,
			\lo, sl2.value,
			\hi, sl3.value,
			\slope, sl4.value,
			\eRatio, sl5.value,
			\amp, sl6.value])},
						{sound.free; sound = nil }
						)
		});
window.front;
)


// Modulation with Gui
(
SynthDef(\blOsc2ModulateEverything,
	{ arg fmCarFreq = 110, fmModFreq = 110, fmDeviation = 0.0,
		loModFreq = 0.1, loMin = 1.0, loMax = 5.0,
		hiModFreq = 0.1, hiMin = 10.0, hiMax = 20.0,
		slModFreq = 0.1, slMin = 0.1, slMax = 1.0,
		erModFreq = 0.0, erMin = 0.0, erMax = 1.0,
		ampModFreq = 0.1, ampMin = 0.5, ampMax = 0.7;
		var freqSin, loSin, hiSin, slopeSin, eRatioSin, ampSin, sound;
		freqSin = SinOsc.ar(fmModFreq, mul: fmDeviation, add: fmCarFreq);
		loSin = SinOsc.ar(loModFreq).range(loMin, loMax);
		hiSin = SinOsc.ar(hiModFreq).range(hiMin, hiMax);
		slopeSin = SinOsc.ar(slModFreq).range(slMin, slMax);
		eRatioSin = SinOsc.ar(erModFreq).range(erMin, erMax);
		ampSin = SinOsc.ar(ampModFreq).range(ampMin, ampMax);
		sound = BLOsc2.ar(freqSin, loSin, hiSin, slopeSin, eRatioSin, ampSin);
		Out.ar(0, Pan2.ar(sound, 0))
}).load
)

(
var window, sound, sl01, sl02, sl03, sl04, sl05, sl06, sl07, sl08, sl09, sl10, sl11, sl12, sl13, sl14, sl15, sl16, sl17, sl18;
window = Window("Band Limited Oscillator2", Rect(700,100,620,550));
window.view.decorator = FlowLayout(window.view.bounds);
StaticText(window,620 @ 40).string_("Band Limited Oscillator").align_(\center)
	.font_(Font("Helvetica", 16));
window.view.decorator.nextLine;
sl01 = EZSlider(window, 600 @ 20, "Freq Mod: carrierFreq", ControlSpec(0.1,440,\exponential,default: 110),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmCarFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl02 = EZSlider(window, 600 @ 20, "Freq Mod: modFreq", ControlSpec(0.1,440,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl03 = EZSlider(window, 600 @ 20, "Freq Mod: deviation", ControlSpec(0.0,220,default: 0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmDeviation, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl04 = EZSlider(window, 600 @ 20, "LoHarmonics Mod: Freq", ControlSpec(0.01,20,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\loModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl05 = EZSlider(window, 600 @ 20, "LoHarmonics Mod: Min", ControlSpec(1.0,10,step: 0.5,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\loMin, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl06 = EZSlider(window, 600 @ 20, "LoHarmonics Mod: Max", ControlSpec(1.0,10,step: 0.5,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\loMax, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl07 = EZSlider(window, 600 @ 20, "HiHarmonics Mod: Freq", ControlSpec(0.01,20,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\hiModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl08 = EZSlider(window, 600 @ 20, "HiHarmonics Mod: Min", ControlSpec(1.0,30,step: 0.5,default: 10),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\hiMin, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl09 = EZSlider(window, 600 @ 20, "HiHarmonics Mod: Max", ControlSpec(1.0,30,step: 0.5,default: 10),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\hiMax, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl10 = EZSlider(window, 600 @ 20, "Slope Mod: Freq", ControlSpec(0.01,20,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl11 = EZSlider(window, 600 @ 20, "Slope Mod: Min", ControlSpec(0.05,1.5,step: 0.05,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slMin, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl12 = EZSlider(window, 600 @ 20, "Slope Mod: Max", ControlSpec(0.05,1.5,step: 0.05,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slMax, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl13 = EZSlider(window, 600 @ 20, "EvenOddRatio Mod: Freq", ControlSpec(0.01,20,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl14 = EZSlider(window, 600 @ 20, "EvenOddRatio Mod: Min", ControlSpec(0.0,1,step: 0.05,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erMin, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl15 = EZSlider(window, 600 @ 20, "EvenOddRatio Mod: Max", ControlSpec(0.0,1,step: 0.05,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erMax, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl16 = EZSlider(window, 600 @ 20, "Amp Mod: Freq", ControlSpec(0.1,440,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\ampModFreq, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl17 = EZSlider(window, 600 @ 20, "Amp Mod: Min", ControlSpec(0.0,1,step: 0.05,default: 0.8),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\ampMin, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl18 = EZSlider(window, 600 @ 20, "Amp Mod: Max", ControlSpec(0.0,1,step: 0.05,default: 0.8),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\ampMax, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.left_(400).top_(500);
Button(window, 40@40)
	.states_([["Start"], ["Stop", Color.black, Color.red(alpha: 0.2)]])
	.action_({|view|  if (view.value ==1 && sound.isNil,
	{sound =
		Synth(\blOsc2ModulateEverything,
			[\fmCarFreq, sl01.value, \fmModFreq, sl02.value, \fmDeviation, sl03.value,
		\loModFreq, sl04.value, \loMin, sl05.value, \loMax, sl06.value,
		\hiModFreq, sl07.value, \hiMin, sl08.value, \hiMax, sl09.value,
		\slModFreq, sl10.value, \slMin, sl11.value, \slMax, sl12.value,
		\erModFreq, sl13.value, \erMin, sl14.value, \erMax, sl15.value,
		\ampModFreq, sl16.value, \ampMin, sl17.value, \ampMax, sl18.value])},
						{sound.free; sound = nil }
						)
		});
window.front;
)

