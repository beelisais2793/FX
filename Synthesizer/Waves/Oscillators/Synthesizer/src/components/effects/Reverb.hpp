#pragma once
#include "../Effect.hpp"
#include "../../utils/audio/Audio.hpp"
#include "../generators/Oscillator.hpp"

class Reverb : public Effect
{
public:
	static const unsigned int MAX_SIZE = 20 * SAMPLE_RATE;
	double dif = 0.005;

private:
	unsigned int current = 0;
	unsigned int offsets[32];
	
	Sample buffer[MAX_SIZE];
	double mix = 0.3;

public:

	Sample Apply(Sample) override;

	Reverb& Mix(double);
	Reverb& Offset(double);
};