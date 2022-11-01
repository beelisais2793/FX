#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Pitch
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	const double referenceFrequency = 261.626; // C4; frequency at which Rack 1v/octave CVs are zero.
	const double referenceSemitone = 60.0; // C4; value of C4 in semitones is arbitrary here, so have it match midi note numbers when rounded to integer.
	const double twelfthRootTwo = 1.0594630943592953;
	const double logTwelfthRootTwo = logf(1.0594630943592953);


	inline double frequencyToSemitone(double frequency) {
		return logf(frequency / referenceFrequency) / logTwelfthRootTwo + referenceSemitone;
	}

	inline double semitoneToFrequency(double semitone) {
		return powf(twelfthRootTwo, semitone - referenceSemitone) * referenceFrequency;
	}

	inline double frequencyToCV(double frequency) {
		return log2f(frequency / referenceFrequency);
	}

	inline double cvToFrequency(double cv) {
		return powf(2.0, cv) * referenceFrequency;
	}

	inline double cvToSemitone(double cv) {
		return frequencyToSemitone(cvToFrequency(cv));
	}

	inline double semitoneToCV(double semitone) {
		return frequencyToCV(semitoneToFrequency(semitone));
	}
}