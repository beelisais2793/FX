#pragma once
#include "Delay.hpp"
#include "../../utils/audio/Audio.hpp"
#include "../generators/Oscillator.hpp"

class Flanger : public Effect
{
	Oscillator lfo;
	Delay delay;
	double mix = 0.5;
	double intensity = 0.5;
	double feedback = 0;

public:
	Flanger();

	Flanger& Mix(double);
	Flanger& Frequency(double);
	Flanger& Feedback(double);
	Flanger& Intensity(double);

	Sample Apply(Sample) override;
};