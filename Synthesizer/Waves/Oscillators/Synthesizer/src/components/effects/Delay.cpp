#include "Delay.hpp"
#include <algorithm>

Sample Delay::Apply(Sample a) 
{
	buffer[current] = a;
	unsigned int index = (current + MAX_SIZE - offset) % MAX_SIZE;
	Sample now = buffer[index];
	buffer[current] += now * feedback;
	current = (current + 1) % MAX_SIZE;
	return now * mix + a * (1 - mix);
};

Delay& Delay::Time(double a)
{
	offset = std::max(std::min(a * (double)SAMPLE_RATE, (double) MAX_SIZE), 1.0);
	return *this;
}

Delay& Delay::Feedback(double a)
{
	feedback = a;
	return *this;
}

Delay& Delay::Mix(double a)
{
	mix = a;
	return *this;
}