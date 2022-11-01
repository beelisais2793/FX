#pragma once
#include "../Effect.hpp"
#include "../../utils/audio/Audio.hpp"

class Delay : public Effect 
{
public:
	static const unsigned int MAX_SIZE = 2 * SAMPLE_RATE;

private:
	unsigned int current = 0;
	unsigned int offset = SAMPLE_RATE;
	Sample buffer[MAX_SIZE];
	double feedback = 0.1;
	double mix = 0.5;

public:
	Delay& Time(double);
	Delay& Feedback(double);
	Delay& Mix(double);

	Sample Apply(Sample) override;
};