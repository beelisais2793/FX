# Nonlinear Oscillator Bank Synthesizer

compiles with C++11; for older standards, you probably need to replace NOBS::pi with M_PI or something

	#include "NOBS.h"

	samplerate = 44100; //audio sample rate in Hz
	bands_per_octave = 72; //number of oscillators per octave
	lowest_band = 30; //frequency of lowest band in Hz
	octaves = 9; //number of octaves above lowest band

	//the frequency of oscillator i will be 30*2^(i/72)
	//and there will be 9*72 ocillators

	//template parameter sets the floating point precision
	NOBS::bank *oscbank = new NOBS::bank<float>(samplerate, lowest_band, bands_per_octave, octaves);

	//the framerate parameter can be set; it controls the smoothing factor for amplitudes. you want to set it relative to whatever rate you update amplitudes at
	//default is 30 Hz
	framerate = 60;
	oscbank->setFrameRate(framerate);

	//you can set the samplerate too (though it is fairly expensive)
	oscbank->setSampleRate(2*samplerate);

	//the strength of coupling from 0 to 1
	// negative value are clipped to 0, values >1 are allowed but likely to be noisy and weird
	//default is 0
	//high values will cause large groups of oscillators to become phase locked
	oscbank->setEntrainment(.1);

	void call_at_control_rate(){
		int osc_idx = 77;
		float amp = .5;
		oscbank->setAmplitude(osc_idx, amp); //set oscillator #77's amplitude to .5
	}

	int blocksize = 32;
	vector<float> audio = oscbank->tick(blocksize); //get the next 32 samples of audio

	//the nominal oscillator frequencies are fixed but can be accessed:
	assert(bands_per_octave == oscbank->getBandsPerOctave());
	assert(int(bands_per_octave*octaves) == oscbank->getNumBands());
	assert(lowest_band == bank->getFreqByIdx(0));
