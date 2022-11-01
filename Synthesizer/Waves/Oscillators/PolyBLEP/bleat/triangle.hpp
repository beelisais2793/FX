#pragma once

#include "oscillator.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

inline float phase_to_triangle(float phase)
{
    const auto omega = 2.0f * phase;
    const auto triangle = phase > 0.5f ? 2.0f - omega : omega;

    return (2.0f * triangle) - 1.0f;
}

class Triangle : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [](float phase)
		{
			return phase_to_triangle(phase);
		};

		return calculate(calculator, freq, sync);
	}
};

}}}