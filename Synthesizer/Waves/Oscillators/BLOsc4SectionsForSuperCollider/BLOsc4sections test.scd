// The mechanism of this UGen is explained in “Additive synthesis with band-limited oscillator sections”, Peter Pabon & So Oishi, ICMC 2016.


/* Arguments and default values
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
add = 0.0

Activate a section by giving non-zero value to both Lowest or Highest Harmonic Index.
Deactivate a section by giving “0” to either Lowest or Highest Harmonic Index.
*/



//////////// Example Codes ////////////

// SynthDef
(
SynthDef(\BLOsc4sectionsExample, {
	arg
	freq = 220.0,

	loHarmonics1 = 1.0,
	hiHarmonics1 = 6.0,
	slope1 = 1.0,
	evenOddRatio1 = 1.0,
	spread1 = 1,
	sectionAmp1 = 1,

	loHarmonics2 = 0.0,
	hiHarmonics2 = 0.0,
	slope2 = 1.0,
	evenOddRatio2 = 1.0,
	spread2 = 1,
	sectionAmp2 = 1,

	loHarmonics3 = 0.0,
	hiHarmonics3 = 0.0,
	slope3 = 1.0,
	evenOddRatio3 = 1.0,
	spread3 = 1,
	sectionAmp3 = 1,

	loHarmonics4 = 0.0,
	hiHarmonics4 = 0.0,
	slope4 = 1.0,
	evenOddRatio4 = 1.0,
	spread4 = 1,
	sectionAmp4 = 1,

	spreadCompensation = 1,

	amp = 1.0
	;

	Out.ar(0,
		BLOsc4sections.ar(
			freq: freq,

			loHarmonics1: loHarmonics1,
			hiHarmonics1: hiHarmonics1,
			slope1: slope1,
			evenOddRatio1: evenOddRatio1,
			spread1: spread1,
			sectionAmp1: sectionAmp1,

			loHarmonics2: loHarmonics2,
			hiHarmonics2: hiHarmonics2,
			slope2: slope2,
			evenOddRatio2: evenOddRatio2,
			spread2: spread2,
			sectionAmp2: sectionAmp2,

			loHarmonics3: loHarmonics3,
			hiHarmonics3: hiHarmonics3,
			slope3: slope3,
			evenOddRatio3: evenOddRatio3,
			spread3: spread3,
			sectionAmp3: sectionAmp3,

			loHarmonics4: loHarmonics4,
			hiHarmonics4: hiHarmonics4,
			slope4: slope4,
			evenOddRatio4: evenOddRatio4,
			spread4: spread4,
			sectionAmp4: sectionAmp4,

			spreadCompensation: spreadCompensation,

			mul: amp))
}).add;

SynthDef(\BLOsc4sectionsWithEnv, {
	arg
	freq = 220.0,

	loHarmonics1 = 1.0,
	hiHarmonics1 = 6.0,
	slope1 = 1.0,
	evenOddRatio1 = 1.0,
	spread1 = 1,
	sectionAmp1 = 1,

	loHarmonics2 = 0.0,
	hiHarmonics2 = 0.0,
	slope2 = 1.0,
	evenOddRatio2 = 1.0,
	spread2 = 1,
	sectionAmp2 = 1,

	loHarmonics3 = 0.0,
	hiHarmonics3 = 0.0,
	slope3 = 1.0,
	evenOddRatio3 = 1.0,
	spread3 = 1,
	sectionAmp3 = 1,

	loHarmonics4 = 0.0,
	hiHarmonics4 = 0.0,
	slope4 = 1.0,
	evenOddRatio4 = 1.0,
	spread4 = 1,
	sectionAmp4 = 1,

	spreadCompensation = 1,

	// Amplitude Envelope
	atk = 0.01, dec = 0.001, susL = 1, sus = 0, rel = 1.0, envCurveAmp = -4,

	amp = 1.0, pan = 0;

	var sound, ampEnv;

	sound =
	BLOsc4sections.ar(
		freq: freq,

		loHarmonics1: loHarmonics1,
		hiHarmonics1: hiHarmonics1,
		slope1: slope1,
		evenOddRatio1: evenOddRatio1,
		spread1: spread1,
		sectionAmp1: sectionAmp1,

		loHarmonics2: loHarmonics2,
		hiHarmonics2: hiHarmonics2,
		slope2: slope2,
		evenOddRatio2: evenOddRatio2,
		spread2: spread2,
		sectionAmp2: sectionAmp2,

		loHarmonics3: loHarmonics3,
		hiHarmonics3: hiHarmonics3,
		slope3: slope3,
		evenOddRatio3: evenOddRatio3,
		spread3: spread3,
		sectionAmp3: sectionAmp3,

		loHarmonics4: loHarmonics4,
		hiHarmonics4: hiHarmonics4,
		slope4: slope4,
		evenOddRatio4: evenOddRatio4,
		spread4: spread4,
		sectionAmp4: sectionAmp4,

		spreadCompensation: spreadCompensation,

		mul: amp);

	// Amplitude Envelope
	ampEnv = Env([0, amp, amp * susL, amp * susL, 0], [atk, dec, sus, rel], curve:envCurveAmp);

	sound = sound * EnvGen.ar(ampEnv, doneAction: 2);
	sound = Pan2.ar(sound, pan);

	Out.ar(0,sound)
}).add;

SynthDef(\BLOsc4sectionsSineModulationWithEnv, {
	arg
	freqMin = 220.0,
	freqMax = 220.0,
	freqModfreq = 1,
	freqLFNoiseFreq = 1.0,
	freqLFNoiseRange = 50.0,

	// Section 1 //
	loHarmonics1Min = 1.0,
	loHarmonics1Max = 1.0,
	loHarmonics1Modfreq = 1.0,

	hiHarmonics1Min = 6.0,
	hiHarmonics1Max = 6.0,
	hiHarmonics1Modfreq = 6.0,

	slope1Min = 1.0,
	slope1Max = 1.0,
	slope1Modfreq = 1.0,

	evenOddRatio1Min = 1.0,
	evenOddRatio1Max = 1.0,
	evenOddRatio1Modfreq = 1.0,

	spread1 = 1,

	sectionAmp1Min = 1,
	sectionAmp1Max = 1,
	sectionAmp1Modfreq = 1,

	// Section 2 //
	loHarmonics2Min = 0.0,
	loHarmonics2Max = 0.0,
	loHarmonics2Modfreq = 0.0,

	hiHarmonics2Min = 0.0,
	hiHarmonics2Max = 0.0,
	hiHarmonics2Modfreq = 0.0,

	slope2Min = 1.0,
	slope2Max = 1.0,
	slope2Modfreq = 1.0,

	evenOddRatio2Min = 1.0,
	evenOddRatio2Max = 1.0,
	evenOddRatio2Modfreq = 1.0,

	spread2 = 1,

	sectionAmp2Min = 1,
	sectionAmp2Max = 1,
	sectionAmp2Modfreq = 1,

	// Section 3 //
	loHarmonics3 = 0.0,
	hiHarmonics3 = 0.0,
	slope3 = 1.0,
	evenOddRatio3 = 1.0,
	spread3 = 1,
	sectionAmp3 = 1,

	// Section 4 //
	loHarmonics4 = 0.0,
	hiHarmonics4 = 0.0,
	slope4 = 1.0,
	evenOddRatio4 = 1.0,
	spread4 = 1,
	sectionAmp4 = 1,

	spreadCompensation = 1,

	// Amplitude Envelope
	atk = 0.01, dec = 0.001, susL = 1, sus = 0, rel = 1.0, envCurveAmp = -4,

	amp = 1.0, pan = 0;

	var sound, ampEnv;

	sound =
	BLOsc4sections.ar(
		freq:
		SinOsc.ar(freqModfreq).range(freqMin,freqMax) + LFNoise2.ar(freqLFNoiseFreq).range(freqLFNoiseRange*(-0.5), freqLFNoiseRange*0.5),

		loHarmonics1: SinOsc.ar(loHarmonics1Modfreq).range(loHarmonics1Min,loHarmonics1Max),
		hiHarmonics1: SinOsc.ar(hiHarmonics1Modfreq).range(hiHarmonics1Min,hiHarmonics1Max),
		slope1: SinOsc.ar(slope1Modfreq).range(slope1Min,slope1Max),
		evenOddRatio1: SinOsc.ar(evenOddRatio1Modfreq).range(evenOddRatio1Min,evenOddRatio1Max),
		spread1: spread1,
		sectionAmp1: SinOsc.ar(sectionAmp1Modfreq).range(sectionAmp1Min,sectionAmp1Max),

		loHarmonics2: SinOsc.ar(loHarmonics2Modfreq).range(loHarmonics2Min,loHarmonics2Max),
		hiHarmonics2: SinOsc.ar(hiHarmonics2Modfreq).range(hiHarmonics2Min,hiHarmonics2Max),
		slope2: SinOsc.ar(slope2Modfreq).range(slope2Min,slope2Max),
		evenOddRatio2: SinOsc.ar(evenOddRatio2Modfreq).range(evenOddRatio2Min,evenOddRatio2Max),
		spread2: spread2,
		sectionAmp2: SinOsc.ar(sectionAmp2Modfreq).range(sectionAmp2Min,sectionAmp2Max),

		loHarmonics3: loHarmonics3,
		hiHarmonics3: hiHarmonics3,
		slope3: slope3,
		evenOddRatio3: evenOddRatio3,
		spread3: spread3,
		sectionAmp3: sectionAmp3,

		loHarmonics4: loHarmonics4,
		hiHarmonics4: hiHarmonics4,
		slope4: slope4,
		evenOddRatio4: evenOddRatio4,
		spread4: spread4,
		sectionAmp4: sectionAmp4,

		spreadCompensation: spreadCompensation,

		mul: amp);

	// Amplitude Envelope
	ampEnv = Env([0, amp, amp * susL, amp * susL, 0], [atk, dec, sus, rel], curve:envCurveAmp);

	sound = sound * EnvGen.ar(ampEnv, doneAction: 2);
	sound = Pan2.ar(sound, pan);

	Out.ar(0,sound)
}).add;

SynthDef(\BLOsc4sectionsLFNoiseModulationWithEnv, {
	arg
	freqMin = 220.0,
	freqMax = 220.0,
	freqModfreq = 1,

	// Section 1 //
	loHarmonics1Min = 1.0,
	loHarmonics1Max = 1.0,
	loHarmonics1Modfreq = 1.0,

	hiHarmonics1Min = 6.0,
	hiHarmonics1Max = 6.0,
	hiHarmonics1Modfreq = 6.0,

	slope1Min = 1.0,
	slope1Max = 1.0,
	slope1Modfreq = 1.0,

	evenOddRatio1Min = 1.0,
	evenOddRatio1Max = 1.0,
	evenOddRatio1Modfreq = 1.0,

	spread1 = 1,

	sectionAmp1Min = 1,
	sectionAmp1Max = 1,
	sectionAmp1Modfreq = 1,

	// Section 2 //
	loHarmonics2Min = 0.0,
	loHarmonics2Max = 0.0,
	loHarmonics2Modfreq = 0.0,

	hiHarmonics2Min = 0.0,
	hiHarmonics2Max = 0.0,
	hiHarmonics2Modfreq = 0.0,

	slope2Min = 1.0,
	slope2Max = 1.0,
	slope2Modfreq = 1.0,

	evenOddRatio2Min = 1.0,
	evenOddRatio2Max = 1.0,
	evenOddRatio2Modfreq = 1.0,

	spread2 = 1,

	sectionAmp2Min = 1,
	sectionAmp2Max = 1,
	sectionAmp2Modfreq = 1,

	// Section 3 //
	loHarmonics3Min = 0.0,
	loHarmonics3Max = 0.0,
	loHarmonics3Modfreq = 0.0,

	hiHarmonics3Min = 0.0,
	hiHarmonics3Max = 0.0,
	hiHarmonics3Modfreq = 0.0,

	slope3Min = 1.0,
	slope3Max = 1.0,
	slope3Modfreq = 1.0,

	evenOddRatio3Min = 1.0,
	evenOddRatio3Max = 1.0,
	evenOddRatio3Modfreq = 1.0,

	spread3 = 1,

	sectionAmp3Min = 1,
	sectionAmp3Max = 1,
	sectionAmp3Modfreq = 1,

	// Section 4 //
	loHarmonics4Min = 0.0,
	loHarmonics4Max = 0.0,
	loHarmonics4Modfreq = 0.0,

	hiHarmonics4Min = 0.0,
	hiHarmonics4Max = 0.0,
	hiHarmonics4Modfreq = 0.0,

	slope4Min = 1.0,
	slope4Max = 1.0,
	slope4Modfreq = 1.0,

	evenOddRatio4Min = 1.0,
	evenOddRatio4Max = 1.0,
	evenOddRatio4Modfreq = 1.0,

	spread4 = 1,

	sectionAmp4Min = 1,
	sectionAmp4Max = 1,
	sectionAmp4Modfreq = 1,


	spreadCompensation = 1,

	// Amplitude Envelope
	atk = 0.01, sus = 0, rel = 1.0,

	amp = 1.0, pan = 0;

	var sound, ampEnv;

	sound =
	BLOsc4sections.ar(
		freq:
		LFNoise2.ar(freqModfreq).range(freqMin, freqMax),

		loHarmonics1: LFNoise2.ar(loHarmonics1Modfreq).range(loHarmonics1Min,loHarmonics1Max),
		hiHarmonics1: LFNoise2.ar(hiHarmonics1Modfreq).range(hiHarmonics1Min,hiHarmonics1Max),
		slope1: LFNoise2.ar(slope1Modfreq).range(slope1Min,slope1Max),
		evenOddRatio1: LFNoise2.ar(evenOddRatio1Modfreq).range(evenOddRatio1Min,evenOddRatio1Max),
		spread1: spread1,
		sectionAmp1: LFNoise2.ar(sectionAmp1Modfreq).range(sectionAmp1Min,sectionAmp1Max),

		loHarmonics2: LFNoise2.ar(loHarmonics2Modfreq).range(loHarmonics2Min,loHarmonics2Max),
		hiHarmonics2: LFNoise2.ar(hiHarmonics2Modfreq).range(hiHarmonics2Min,hiHarmonics2Max),
		slope2: LFNoise2.ar(slope2Modfreq).range(slope2Min,slope2Max),
		evenOddRatio2: LFNoise2.ar(evenOddRatio2Modfreq).range(evenOddRatio2Min,evenOddRatio2Max),
		spread2: spread2,
		sectionAmp2: LFNoise2.ar(sectionAmp2Modfreq).range(sectionAmp2Min,sectionAmp2Max),

		loHarmonics3: LFNoise2.ar(loHarmonics3Modfreq).range(loHarmonics3Min,loHarmonics3Max),
		hiHarmonics3: LFNoise2.ar(hiHarmonics3Modfreq).range(hiHarmonics3Min,hiHarmonics3Max),
		slope3: LFNoise2.ar(slope3Modfreq).range(slope3Min,slope3Max),
		evenOddRatio3: LFNoise2.ar(evenOddRatio3Modfreq).range(evenOddRatio3Min,evenOddRatio3Max),
		spread3: spread3,
		sectionAmp3: LFNoise2.ar(sectionAmp3Modfreq).range(sectionAmp3Min,sectionAmp3Max),

		loHarmonics4: LFNoise2.ar(loHarmonics4Modfreq).range(loHarmonics4Min,loHarmonics4Max),
		hiHarmonics4: LFNoise2.ar(hiHarmonics4Modfreq).range(hiHarmonics4Min,hiHarmonics4Max),
		slope4: LFNoise2.ar(slope4Modfreq).range(slope4Min,slope4Max),
		evenOddRatio4: LFNoise2.ar(evenOddRatio4Modfreq).range(evenOddRatio4Min,evenOddRatio4Max),
		spread4: spread4,
		sectionAmp4: LFNoise2.ar(sectionAmp4Modfreq).range(sectionAmp4Min,sectionAmp4Max),

		spreadCompensation: spreadCompensation,

		mul: amp);

	// Amplitude Envelope
	ampEnv = Env([0, amp, amp, 0], [atk, sus, rel], curve:\sin);

	sound = sound * EnvGen.ar(ampEnv, doneAction: 2);
	sound = Pan2.ar(sound, pan);

	Out.ar(0,sound)
}).add;
)

// Spectrum Analyzer
FreqScope.new;


// Demonstration with Gui
(
var window, sound,

knob_freq,

knob_loHarmonics1,
knob_hiHarmonics1,
knob_slope1,
knob_evenOddRatio1,
knob_spread1,
knob_sectionAmp1,

knob_loHarmonics2,
knob_hiHarmonics2,
knob_slope2,
knob_evenOddRatio2,
knob_spread2,
knob_sectionAmp2,

knob_loHarmonics3,
knob_hiHarmonics3,
knob_slope3,
knob_evenOddRatio3,
knob_spread3,
knob_sectionAmp3,

knob_loHarmonics4,
knob_hiHarmonics4,
knob_slope4,
knob_evenOddRatio4,
knob_spread4,
knob_sectionAmp4,

knob_spreadCompensation,
knob_amp;


window = Window("Band Limited Oscillator", Rect(left: 100, top: 100, width: 800, height: 700));
window.view.decorator = FlowLayout(window.view.bounds);
StaticText(window,500 @ 40).string_("Band Limited Oscillator").align_(\center)
.font_(Font("Helvetica", 16));
window.view.decorator.nextLine;

knob_freq = EZKnob(window, 120 @ 90, "freq", ControlSpec(20,440, \lin, step: 10, default: 220),
	{|view| if (sound.notNil) { sound.set(\freq, view.value) }},
	labelWidth: 50);


window.view.decorator.nextLine;


knob_loHarmonics1 = EZKnob(window, 120 @ 90, "loHarmonics1", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\loHarmonics1, view.value) }},
	labelWidth: 50);

knob_hiHarmonics1 = EZKnob(window, 120 @ 90, "hiHarmonics1", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 6.0),
	{|view| if (sound.notNil) { sound.set(\hiHarmonics1, view.value) }},
	labelWidth: 50);

knob_slope1 = EZKnob(window, 120 @ 90, "slope1", ControlSpec(0.1, 2.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\slope1, view.value) }},
	labelWidth: 50);

knob_evenOddRatio1 = EZKnob(window, 120 @ 90, "evenOddRatio1", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\evenOddRatio1, view.value) }},
	labelWidth: 50);

knob_spread1 = EZKnob(window, 120 @ 90, "spread1", ControlSpec(1, 5, \lin, step: 1, default: 1),
	{|view| if (sound.notNil) { sound.set(\spread1, view.value) }},
	labelWidth: 50);

knob_sectionAmp1 = EZKnob(window, 120 @ 90, "sectionAmp1(in dB)", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\sectionAmp1, view.value) }},
	labelWidth: 50);


window.view.decorator.nextLine;


knob_loHarmonics2 = EZKnob(window, 120 @ 90, "loHarmonics2", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\loHarmonics2, view.value) }},
	labelWidth: 50);

knob_hiHarmonics2 = EZKnob(window, 120 @ 90, "hiHarmonics2", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\hiHarmonics2, view.value) }},
	labelWidth: 50);

knob_slope2 = EZKnob(window, 120 @ 90, "slope2", ControlSpec(0.1, 2.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\slope2, view.value) }},
	labelWidth: 50);

knob_evenOddRatio2 = EZKnob(window, 120 @ 90, "evenOddRatio2", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\evenOddRatio2, view.value) }},
	labelWidth: 50);

knob_spread2 = EZKnob(window, 120 @ 90, "spread2", ControlSpec(1, 5, \lin, step: 1, default: 1),
	{|view| if (sound.notNil) { sound.set(\spread2, view.value) }},
	labelWidth: 50);

knob_sectionAmp2 = EZKnob(window, 120 @ 90, "sectionAmp2(in dB)", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\sectionAmp2, view.value) }},
	labelWidth: 50);


window.view.decorator.nextLine;


knob_loHarmonics3 = EZKnob(window, 120 @ 90, "loHarmonics3", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\loHarmonics3, view.value) }},
	labelWidth: 50);

knob_hiHarmonics3 = EZKnob(window, 120 @ 90, "hiHarmonics3", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\hiHarmonics3, view.value) }},
	labelWidth: 50);

knob_slope3 = EZKnob(window, 120 @ 90, "slope3", ControlSpec(0.1, 2.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\slope3, view.value) }},
	labelWidth: 50);

knob_evenOddRatio3 = EZKnob(window, 120 @ 90, "evenOddRatio3", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\evenOddRatio3, view.value) }},
	labelWidth: 50);

knob_spread3 = EZKnob(window, 120 @ 90, "spread3", ControlSpec(1, 5, \lin, step: 1, default: 1),
	{|view| if (sound.notNil) { sound.set(\spread3, view.value) }},
	labelWidth: 50);

knob_sectionAmp3 = EZKnob(window, 120 @ 90, "sectionAmp3(in dB)", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\sectionAmp3, view.value) }},
	labelWidth: 50);


window.view.decorator.nextLine;


knob_loHarmonics4 = EZKnob(window, 120 @ 90, "loHarmonics4", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\loHarmonics4, view.value) }},
	labelWidth: 50);

knob_hiHarmonics4 = EZKnob(window, 120 @ 90, "hiHarmonics4", ControlSpec(0.0, 50.0, \lin, step: 0.25, default: 0.0),
	{|view| if (sound.notNil) { sound.set(\hiHarmonics4, view.value) }},
	labelWidth: 50);

knob_slope4 = EZKnob(window, 120 @ 90, "slope4", ControlSpec(0.1, 2.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\slope4, view.value) }},
	labelWidth: 50);

knob_evenOddRatio4 = EZKnob(window, 120 @ 90, "evenOddRatio4", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\evenOddRatio4, view.value) }},
	labelWidth: 50);

knob_spread4 = EZKnob(window, 120 @ 90, "spread4", ControlSpec(1, 5, \lin, step: 1, default: 1),
	{|view| if (sound.notNil) { sound.set(\spread4, view.value) }},
	labelWidth: 50);

knob_sectionAmp4 = EZKnob(window, 120 @ 90, "sectionAmp4(in dB)", ControlSpec(0.0, 1.0, \lin, step: 0.1, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\sectionAmp4, view.value) }},
	labelWidth: 50);


window.view.decorator.nextLine;

knob_spreadCompensation = EZKnob(window, 120 @ 90, "spreadCompensation", ControlSpec(0, 1, \lin, step: 1, default: 1),
	{|view| if (sound.notNil) { sound.set(\spreadCompensation, view.value) }},
	labelWidth: 50);

knob_amp = EZKnob(window, 120 @ 90, "amp", ControlSpec(0.1, 1.0, \lin, default: 1.0),
	{|view| if (sound.notNil) { sound.set(\amp, view.value) }},
	labelWidth: 50);

window.view.decorator.left_(10).top_(630);
Button(window, 50@50)
.states_([["Start"], ["Stop", Color.black, Color.red(alpha: 0.2)]])
.action_({|view|  if (view.value ==1 && sound.isNil,
	{sound =
		Synth(\BLOsc4sectionsExample,
			[

				\freq, knob_freq.value,

				\loHarmonics1, knob_loHarmonics1.value,
				\hiHarmonics1, knob_hiHarmonics1.value,
				\slope1, knob_slope1.value,
				\evenOddRatio1, knob_evenOddRatio1.value,
				\spread1, knob_spread1.value,
				\sectionAmp1, knob_sectionAmp1.value,

				\loHarmonics2, knob_loHarmonics2.value,
				\hiHarmonics2, knob_hiHarmonics2.value,
				\slope2, knob_slope2.value,
				\evenOddRatio2, knob_evenOddRatio2.value,
				\spread2, knob_spread2.value,
				\sectionAmp2, knob_sectionAmp2.value,

				\loHarmonics3, knob_loHarmonics3.value,
				\hiHarmonics3, knob_hiHarmonics3.value,
				\slope3, knob_slope3.value,
				\evenOddRatio3, knob_evenOddRatio3.value,
				\spread3, knob_spread3.value,
				\sectionAmp3, knob_sectionAmp3.value,

				\loHarmonics4, knob_loHarmonics4.value,
				\hiHarmonics4, knob_hiHarmonics4.value,
				\slope4, knob_slope4.value,
				\evenOddRatio4, knob_evenOddRatio4.value,
				\spread4, knob_spread4.value,
				\sectionAmp4, knob_sectionAmp4.value,

				\spreadCompensation, knob_spreadCompensation.value,

				\mul, knob_amp.value

	])},
	{sound.free; sound = nil }
	)
});
window.front;
)



// Demonstrating modulation

(
{
	BLOsc4sections.ar(
		freq: SinOsc.ar(0.01).range(100,200),

		loHarmonics1: 1.0,
		hiHarmonics1: LFNoise2.ar(0.4).range(3.0, 5.0),
		slope1: LFNoise2.ar(0.3).range(0.1, 1.9),
		evenOddRatio1: LFNoise2.ar(0.2).range(0.0, 1),
		spread1: 2,
		sectionAmp1: LFNoise2.ar(0.1).range(-128.0, 0).dbamp,

		loHarmonics2: LFNoise2.ar(0.4).range(5.0, 7.0),
		hiHarmonics2: LFNoise2.ar(0.4).range(7.0, 9.0),
		slope2: LFNoise2.ar(0.3).range(0.1, 1.9),
		evenOddRatio2: LFNoise2.ar(0.2).range(0.0, 1),
		spread2: 1,
		sectionAmp2: LFNoise2.ar(0.1).range(-128.0, 0).dbamp,

		loHarmonics3: LFNoise2.ar(0.4).range(9.0, 11.0),
		hiHarmonics3: LFNoise2.ar(0.4).range(11.0, 13.0),
		slope3: LFNoise2.ar(0.3).range(0.1, 1.9),
		evenOddRatio3: LFNoise2.ar(0.2).range(0.0, 1),
		spread3: 2,
		sectionAmp3: LFNoise2.ar(0.1).range(-128.0, 0).dbamp,

		loHarmonics4: LFNoise2.ar(0.4).range(13.0, 15.0),
		hiHarmonics4: LFNoise2.ar(0.4).range(15.0, 18.0),
		slope4: LFNoise2.ar(0.3).range(0.1, 1.9),
		evenOddRatio4: LFNoise2.ar(0.2).range(0.0, 1),
		spread4: 1,
		sectionAmp4: LFNoise2.ar(0.1).range(-128.0, 0).dbamp,

		spreadCompensation: 1,

		mul: 1.0,
		add: 0.0
	)
}.play
)


// Female vowel "a", major scale
(
Pbind(
	\instrument, \BLOsc4sectionsSineModulationWithEnv,
	\freqMin,
	Pseq([60,62,64,65,67,69,71,72],1).midicps,
	\freqMax, Pkey(\freqMin) * 1.05,
	\freqModfreq, 6,
	\freqLFNoiseFreq, 7.0,
	\freqLFNoiseRange, 10.0,


	// Section 1 //
	\loHarmonics1Min, 1.0,
	\loHarmonics1Max, 1.0,
	\loHarmonics1Modfreq, 1.0,

	\hiHarmonics1Min, (1200/Pkey(\freqMin)).round,
	\hiHarmonics1Max, Pkey(\hiHarmonics1Min),
	\hiHarmonics1Modfreq, 1.0,

	\slope1Min, 0.5,
	\slope1Max, 0.5,
	\slope1Modfreq, 1.0,

	\evenOddRatio1Min, 1.0,
	\evenOddRatio1Max, 1.0,
	\evenOddRatio1Modfreq, 1.0,

	\spread1, 1,

	\sectionAmp1Min, 1,
	\sectionAmp1Max, 1,
	\sectionAmp1Modfreq, 1,

	// Section 2 //
	\loHarmonics2Min, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Max, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Modfreq, 1.0,

	\hiHarmonics2Min, 34.0,
	\hiHarmonics2Max, 34.0,
	\hiHarmonics2Modfreq, 1.0,

	\slope2Min, 0.5,
	\slope2Max, 0.5,
	\slope2Modfreq, 1.0,

	\evenOddRatio2Min, 1.0,
	\evenOddRatio2Max, 1.0,
	\evenOddRatio2Modfreq, 1.0,

	\spread2, 1,

	\sectionAmp2Min, 0.2,
	\sectionAmp2Max, 0.2,
	\sectionAmp2Modfreq, 1,

	\delta, 2.0,

	// Amplitude Envelope
	\atk, Pwhite(0.4,0.6),
	\dec, Pwhite(0.1,0.2),
	\susL, Pwhite(0.8, 1.0),
	\sus, Pwhite(0.4, 0.5),
	\rel, Pwhite(0.6, 0.8),
	\envCurveAmp, -4,

	\amp, 0.8,
).play
)


// Female vowel "a", major scale, with slope1 modulation
(
Pbind(
	\instrument, \BLOsc4sectionsSineModulationWithEnv,
	\freqMin,
	Pseq([60,62,64,65,67,69,71,72],1).midicps,
	\freqMax, Pkey(\freqMin) * 1.05,
	\freqModfreq, 6,
	\freqLFNoiseFreq, 7.0,
	\freqLFNoiseRange, 10.0,


	// Section 1 //
	\loHarmonics1Min, 1.0,
	\loHarmonics1Max, 1.0,
	\loHarmonics1Modfreq, 1.0,

	\hiHarmonics1Min, (1200/Pkey(\freqMin)).round,
	\hiHarmonics1Max, Pkey(\hiHarmonics1Min),
	\hiHarmonics1Modfreq, 1.0,

	\slope1Min, 0.1,
	\slope1Max, 1.0,
	\slope1Modfreq, 3,

	\evenOddRatio1Min, 1.0,
	\evenOddRatio1Max, 1.0,
	\evenOddRatio1Modfreq, 1.0,

	\spread1, 1,

	\sectionAmp1Min, 1,
	\sectionAmp1Max, 1,
	\sectionAmp1Modfreq, 1,

	// Section 2 //
	\loHarmonics2Min, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Max, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Modfreq, 1.0,

	\hiHarmonics2Min, 34.0,
	\hiHarmonics2Max, 34.0,
	\hiHarmonics2Modfreq, 1.0,

	\slope2Min, 0.5,
	\slope2Max, 0.5,
	\slope2Modfreq, 1.0,

	\evenOddRatio2Min, 1.0,
	\evenOddRatio2Max, 1.0,
	\evenOddRatio2Modfreq, 1.0,

	\spread2, 1,

	\sectionAmp2Min, 0.2,
	\sectionAmp2Max, 0.2,
	\sectionAmp2Modfreq, 1,

	\delta, 2.0,

	// Amplitude Envelope
	\atk, Pwhite(0.4,0.6),
	\dec, Pwhite(0.1,0.2),
	\susL, Pwhite(0.8, 1.0),
	\sus, Pwhite(0.4, 0.5),
	\rel, Pwhite(0.6, 0.8),
	\envCurveAmp, -4,

	\amp, 0.8,
).play
)


// Female vowel "a", major scale, with hiHarmonics1 & loHarmonics2 modulation
(
Pbind(
	\instrument, \BLOsc4sectionsSineModulationWithEnv,
	\freqMin,
	Pseq([60,62,64,65,67,69,71,72],1).midicps,
	\freqMax, Pkey(\freqMin) * 1.05,
	\freqModfreq, 6,
	\freqLFNoiseFreq, 7.0,
	\freqLFNoiseRange, 10.0,


	// Section 1 //
	\loHarmonics1Min, 1.0,
	\loHarmonics1Max, 1.0,
	\loHarmonics1Modfreq, 1.0,

	\hiHarmonics1Min, (1200/Pkey(\freqMin)).round,
	\hiHarmonics1Max, Pkey(\hiHarmonics1Min)+2,
	\hiHarmonics1Modfreq, 3.0,

	\slope1Min, 1.0,
	\slope1Max, 1.0,
	\slope1Modfreq, 3,

	\evenOddRatio1Min, 1.0,
	\evenOddRatio1Max, 1.0,
	\evenOddRatio1Modfreq, 1.0,

	\spread1, 1,

	\sectionAmp1Min, 1,
	\sectionAmp1Max, 1,
	\sectionAmp1Modfreq, 1,

	// Section 2 //
	\loHarmonics2Min, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Max, (3300/Pkey(\freqMin)).round+2,
	\loHarmonics2Modfreq, 3.0,

	\hiHarmonics2Min, 34.0,
	\hiHarmonics2Max, 34.0,
	\hiHarmonics2Modfreq, 1.0,

	\slope2Min, 0.5,
	\slope2Max, 0.5,
	\slope2Modfreq, 1.0,

	\evenOddRatio2Min, 1.0,
	\evenOddRatio2Max, 1.0,
	\evenOddRatio2Modfreq, 1.0,

	\spread2, 1,

	\sectionAmp2Min, 0.2,
	\sectionAmp2Max, 0.2,
	\sectionAmp2Modfreq, 1,

	\delta, 2.0,

	// Amplitude Envelope
	\atk, Pwhite(0.4,0.6),
	\dec, Pwhite(0.1,0.2),
	\susL, Pwhite(0.8, 1.0),
	\sus, Pwhite(0.4, 0.5),
	\rel, Pwhite(0.6, 0.8),
	\envCurveAmp, -4,

	\amp, 0.8,
).play
)


// Modulate Even Odd Ratio, transform between voice like quality --> clarinet like quality
(
Pbind(
	\instrument, \BLOsc4sectionsSineModulationWithEnv,
	\freqMin,
	Pseq([60,62,64,65,67,69,71,72],1).midicps,
	\freqMax, Pkey(\freqMin) * 1.05,
	\freqModfreq, 6,
	\freqLFNoiseFreq, 7.0,
	\freqLFNoiseRange, 10.0,


	// Section 1 //
	\loHarmonics1Min, 1.0,
	\loHarmonics1Max, 1.0,
	\loHarmonics1Modfreq, 1.0,

	\hiHarmonics1Min, (1200/Pkey(\freqMin)).round,
	\hiHarmonics1Max, Pkey(\hiHarmonics1Min),
	\hiHarmonics1Modfreq, 1.0,

	\slope1Min, 0.5,
	\slope1Max, 0.5,
	\slope1Modfreq, 1.0,

	\evenOddRatio1Min, 0.0,
	\evenOddRatio1Max, 1.0,
	\evenOddRatio1Modfreq, 0.5,

	\spread1, 1,

	\sectionAmp1Min, 1,
	\sectionAmp1Max, 1,
	\sectionAmp1Modfreq, 1,

	// Section 2 //
	\loHarmonics2Min, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Max, (3300/Pkey(\freqMin)).round,
	\loHarmonics2Modfreq, 1.0,

	\hiHarmonics2Min, 34.0,
	\hiHarmonics2Max, 34.0,
	\hiHarmonics2Modfreq, 1.0,

	\slope2Min, 0.5,
	\slope2Max, 0.5,
	\slope2Modfreq, 1.0,

	\evenOddRatio2Min, 0.0,
	\evenOddRatio2Max, 1.0,
	\evenOddRatio2Modfreq, 0.5,

	\spread2, 1,

	\sectionAmp2Min, 0.2,
	\sectionAmp2Max, 0.2,
	\sectionAmp2Modfreq, 1,

	\delta, 2.0,

	// Amplitude Envelope
	\atk, Pwhite(0.4,0.6),
	\dec, Pwhite(0.1,0.2),
	\susL, Pwhite(0.8, 1.0),
	\sus, Pwhite(0.6, 0.7),
	\rel, Pwhite(0.4, 0.6),
	\envCurveAmp, -4,

	\amp, 0.8,
).play
)


// Strange modulation
(
{
var sound;
	sound =
	BLOsc4sections.ar(
		freq: SinOsc.ar(0.05, 1.5pi, 150, 200),
		loHarmonics1: SinOsc.ar(EnvGen.ar(Env.new([3,3,1,10],[10,20,10])), 0).range(1,5),
		hiHarmonics1: SinOsc.ar(EnvGen.ar(Env.new([3,3,1,10],[10,20,10])), pi).range(6, 30),
		slope1: EnvGen.ar(Env.new([0.8, 0.8, 0.1, 0.8],[10,5,5])),
		evenOddRatio1: EnvGen.ar(Env.new([1, 1, 0, 1],[20,5,5])),
		mul:0.5);
	sound = Pan2.ar(sound, 0);
	sound
}.play;
)


