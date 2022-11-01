#pragma once
#include "Delay.hpp"
#include "../../utils/audio/Audio.hpp"
#include "../generators/Oscillator.hpp"

class Chorus : public Effect
{
	Oscillator lfo;
	Delay delay;
	double mix = 0.5;
	double intensity = 1;
	double feedback = 0;

public:

	Chorus();

	Chorus& Mix(double);
	Chorus& Frequency(double);
	Chorus& Feedback(double);
	Chorus& Intensity(double);

	Sample Apply(Sample) override;
};