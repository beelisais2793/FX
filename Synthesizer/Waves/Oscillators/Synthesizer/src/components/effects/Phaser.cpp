#include "Phaser.hpp"
#include <algorithm>

Sample Phaser::Apply(Sample a)
{
	return a - intensity * (a >> delay
		.Mix(1)
		.Time(d)
		.Feedback(feedback));
};

Phaser& Phaser::Gap(double a)
{
	d = a;
	return *this;
}

Phaser& Phaser::Mix(double a)
{
	mix = a;
	return *this;
}

Phaser& Phaser::Feedback(double a)
{
	feedback = a;
	return *this;
}

Phaser& Phaser::Intensity(double a)
{
	intensity = a;
	return *this;
}

