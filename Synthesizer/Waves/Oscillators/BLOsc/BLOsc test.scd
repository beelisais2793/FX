// SynthDef
(
SynthDef(\blOscExample, { arg freq = 440.0, lo = 1, num = 15, slope = 1.0, eRatio = 1.0, amp = 1.0;
	Out.ar(0, BLOsc.ar(freq, lo, num, slope, eRatio, mul: amp))
}).load
)

// Spectrum Analyzer
(
~openAnalyzer = {
var window, func;
window = Window("Analyzer", Rect(100, 100, 511, 300)); // width should be 511
func = FreqScopeView(window, window.view.bounds);
func.active_(true); // turn it on the first time;
window.onClose_({ func.kill }); // you must have this
window.front;
}.value
)

// Demonstration with Gui
(
var window, sound, sl1, sl2, sl3, sl4, sl5, sl6;
window = Window("Band Limited Oscillator", Rect(100,420,620,300));
window.view.decorator = FlowLayout(window.view.bounds);
StaticText(window,620 @ 40).string_("Band Limited Oscillator").align_(\center)
	.font_(Font("Helvetica", 16));
window.view.decorator.nextLine;
sl1 = EZSlider(window, 600 @ 20, "Fundamental Freq", ControlSpec(55.0,880,\exponential,default: 440.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\freq, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl2 = EZSlider(window, 600 @ 20, "Lowest Harmonics", ControlSpec(1,100,step: 1,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\lo, view.value)}
	 	},
	labelWidth: 130);
window.view.decorator.nextLine;
sl3 = EZSlider(window, 600 @ 20, "Num of Harmonics", ControlSpec(1,100,step: 1,default: 7),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\num, view.value)}
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
		Synth(\blOscExample,
			[\freq,sl1.value,
			\lo, sl2.value,
			\num, sl3.value,
			\slope, sl4.value,
			\eRatio, sl5.value,
			\amp, sl6.value])},
						{sound.free; sound = nil }
						)
		});
window.front;
)


// Frequency Modulation
{BLOsc.ar(440.0 + SinOsc.ar(MouseX.kr(1,220), mul:MouseY.kr(1,110)), 1, 9, 0.8, 1, mul:0.5)}.play

// Slope Modulation
{BLOsc.ar(440.0, 1, 9, SinOsc.ar(MouseX.kr(1,220), mul:MouseY.kr(0,0.9), add:1), 1, mul:0.5)}.play

// Even to Odd Ratio Modulation
{BLOsc.ar(440.0, 1, 9, 0.8, SinOsc.ar(MouseX.kr(1,220), mul:MouseY.kr(0,0.5), add:0.5), mul:0.5)}.play

// Modulation with Gui
(
SynthDef(\blOscTripleModulation, { arg fmCar = 110, fmMod = 0.1, fmDev = 55.0, lo = 1, num = 7, slCtr = 1.0, slMod = 0.1, slDev = 0.1, erCtr = 0.5, erMod = 0.1, erDev = 0.1, amp = 0.5;
	var freqSin, slopeSin, eRatioSin;
	freqSin = SinOsc.ar(fmMod, mul: fmDev, add: fmCar);
	slopeSin = SinOsc.ar(slMod, mul: slDev, add: slCtr);
	eRatioSin = SinOsc.ar(erMod, mul: erDev, add: erCtr);
	Out.ar(0, BLOsc.ar(freqSin, lo, num, slopeSin, eRatioSin, mul: amp))
}).load
)

(
var window, sound, sl1, sl2, sl3, sl4, sl5, sl6, sl7, sl8, sl9, sl10, sl11, sl12;
window = Window("Band Limited Oscillator", Rect(100,420,620,400));
window.view.decorator = FlowLayout(window.view.bounds);
StaticText(window,620 @ 40).string_("Band Limited Oscillator").align_(\center)
	.font_(Font("Helvetica", 16));
window.view.decorator.nextLine;
sl1 = EZSlider(window, 600 @ 20, "Freq Mod: carrierFreq", ControlSpec(55.0,440,\exponential,default: 110),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmCar, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl2 = EZSlider(window, 600 @ 20, "Freq Mod: modFreq", ControlSpec(0.1,440,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmMod, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl3 = EZSlider(window, 600 @ 20, "Freq Mod: deviation", ControlSpec(0.0,220,default: 55),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\fmDev, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl4 = EZSlider(window, 600 @ 20, "Lowest Harmonics", ControlSpec(1,100,step: 1,default: 1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\lo, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl5 = EZSlider(window, 600 @ 20, "Num of Harmonics", ControlSpec(1,100,step: 1,default: 7),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\num, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl6 = EZSlider(window, 600 @ 20, "Slope Mod: center", ControlSpec(0.1,2.0,default: 1.0),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slCtr, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl7 = EZSlider(window, 600 @ 20, "Slope Mod: modFreq", ControlSpec(0.1,440,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slMod, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl8 = EZSlider(window, 600 @ 20, "Slope Mod: deviation", ControlSpec(0,1.0,default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\slDev, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl9 = EZSlider(window, 600 @ 20, "Even Ratio Mod: center", ControlSpec(0,1.0,default: 0.5),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erCtr, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl10 = EZSlider(window, 600 @ 20, "Even Ratio Mod: modFreq", ControlSpec(0.1,440,\exponential, default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erMod, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl11 = EZSlider(window, 600 @ 20, "Even Ratio Mod: deviation", ControlSpec(0,0.5,default: 0.1),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\erDev, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.nextLine;
sl12 = EZSlider(window, 600 @ 20, "Amp", ControlSpec(0,1.0,\amp,default: 0.5),
	 	{| view | if (sound.notNil)
	 			   { sound.set(\amp, view.value)}
	 	},
	labelWidth: 180);
window.view.decorator.left_(400).top_(350);
Button(window, 40@40)
	.states_([["Start"], ["Stop", Color.black, Color.red(alpha: 0.2)]])
	.action_({|view|  if (view.value ==1 && sound.isNil,
	{sound =
		Synth(\blOscTripleModulation,
			[\fmCar,sl1.value,
				\fmMod, sl2.value,
				\fmDev, sl3.value,
				\lo, sl4.value,
				\num, sl5.value,
				\slCtr, sl6.value,
				\slMod, sl7.value,
				\slDev, sl8.value,
				\erCtr, sl9.value,
				\erMod, sl10.value,
				\erDev, sl11.value,
				\amp, sl12.value])},
						{sound.free; sound = nil }
						)
		});
window.front;
)

// Colorful sound demonstration
(
SynthDef( "perkyblosc", {
	arg freq = 400, lo = 1, num = 7, slope = 0.8, eratio = 1.0, dur=1.0, amp = 1;
	var osc;
	osc = BLOsc.ar( [freq,freq + rrand(-5, 5)], lo, num, slope, eratio, mul: amp )
		* EnvGen.ar(
		Env.perc, doneAction: 2, timeScale: dur
	);
	OffsetOut.ar(0,osc);
}).load;
)

(
Pbind(\instrument,\perkyblosc,
	\midinote,Pn(Pseq([91,81,59,50,88,64])),
	\delta, 0.2,
	\lo, Pwrand([1,2,3,4,5], [10,4,3,2,1].normalizeSum, inf),
	\num, Pbrown(1, 15, 1),
	\slope, Pbrown(0.1, 1.2, 0.1),
	\eratio, Pbrown(0.1, 1.0, 0.1),
).play
)


// Fluctuating timbre demonstration: shift from simple sound to colourful sound
(
SynthDef(\oneblosc, { arg freq = 440, lo = 1, num = 9, offsetT = 0, attackT = 5.0, holdT = 10.0, decayT = 5.0, amp = 0.1;
	var breakpoints, dur, slope, erate, ampflct, ampenv, sound;
	breakpoints = Array.fill(150, {exprand(0.01, 10)}).sort({ arg a, b; a > b }) + 1.0;
	dur = Dseq(breakpoints, 1);
	slope = Duty.kr(dur, 0, Dwhite(0.1, Dseries(0.1,0.01,150)));
	erate = Duty.kr(dur, 0, Dwhite(Dseries(1.0,-0.006,150), 1.0));
	ampflct = Duty.kr(dur, 0, Dwhite(0.3, 1.0));
	ampenv = EnvGen.kr(Env.new([0,0,amp,amp,0], [offsetT, attackT, holdT, decayT], \sin), doneAction: 2);
	sound = BLOsc.ar(freq, lo, num, VarLag.kr(slope, 1.0, warp: \sin), VarLag.kr(erate, 1.0, warp: \sin), mul: VarLag.kr(ampflct, 1.0, warp: \sin)) * ampenv;
	sound = Pan2.ar(sound, rrand(-1.0, 1.0));
	Out.ar(0, sound)
}).load
)

(
Synth(\oneblosc, [\freq, [38,42,45,50,54,57,62,66,69,74,78,81].choose.midicps,
	\num, 1,
	\attackT, rrand(5.0, 10.0),
	\holdT, rrand(10.0, 20.0),
	\decayT, rrand(10.0, 20.0),
	\amp, 0.1]);
{var offsettime, loharms, numharms;
	offsettime = rrand(0,90);
	loharms = [1,2,3].wchoose([0.8,0.1,0.1]);
	numharms = rrand(pow(2,(offsettime/30)).round, pow(2,(offsettime/20)).round+1);
	Synth(\oneblosc, [\freq, [38,42,45,50,54,57,62,66,69,74,78,81].choose.midicps,
	\lo, loharms,
	\num, numharms,
	\offsetT, offsettime,
	\attackT, rrand(5.0, 10.0),
	\holdT, rrand(10.0, 20.0),
	\decayT, rrand(10.0, 20.0),
	\amp, 0.1])
	}!15
)
