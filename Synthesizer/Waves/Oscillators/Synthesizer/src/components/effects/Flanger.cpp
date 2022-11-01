
#include "Flanger.hpp"

Flanger::Flanger()
{
	lfo.Frequency(0.5);
}

Sample Flanger::Apply(Sample a)
{
	return a >> delay
		.Mix(mix)
		.Time(lfo * (intensity * 0.001) + (intensity * 0.001))
		.Feedback(feedback);
};


Flanger& Flanger::Frequency(double a)
{
	lfo.Frequency(a);
	return *this;
}

Flanger& Flanger::Mix(double a)
{
	mix = a;
	return *this;
}

Flanger& Flanger::Feedback(double a)
{
	feedback = a;
	return *this;
}

Flanger& Flanger::Intensity(double a)
{
	intensity = a;
	return *this;
}

