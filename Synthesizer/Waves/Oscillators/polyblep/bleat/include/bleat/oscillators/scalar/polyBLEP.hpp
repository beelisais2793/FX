#pragma once

namespace bleat {
namespace oscillators {
namespace scalar {

inline float polyBLEP(float phase, float freq)
{
	auto t = phase;
	const auto dt = freq;

	auto c = 0.0f;

	if (t < dt)
	{
		t = t / dt;
		c = t + t - t * t - 1.0f;
	}
	else if (t > 1.0f - dt)
	{
		t = (t - 1.0f) / dt;
		c = t * t + t + t + 1.0f;
	}
	
	return c;
}

}}}