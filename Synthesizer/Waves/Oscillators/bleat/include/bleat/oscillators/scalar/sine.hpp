#pragma once

#include "oscillator.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

/*
 * Based on oscillator code from madronalabs/madronalib
*/

// input: phasor on (0, 1)
// output: sine aproximation using Taylor series on range(-1, 1). There is distortion in odd
// harmonics only, with the 3rd harmonic at about -40dB.
inline float phase_to_sine(float phase)
{
    constexpr auto sqrt2 = static_cast<float>(const_math::sqrt(2.0f));
    constexpr auto range = sqrt2 - sqrt2 * sqrt2 * sqrt2 / 6.0f;
    constexpr auto scale = 1.0f / range;
    constexpr auto domain = sqrt2 * 4.0f;
    constexpr auto flip_offset = sqrt2 * 2.0f;
    constexpr auto one_sixth = 1.0f / 6.0f;

    // scale and offset input phasor on (0, 1) to sine approx domain (-sqrt(2), 3*sqrt(2))
    const auto omega = phase * (domain)+(-sqrt2);

    // reverse upper half of phasor to get triangle
    const auto triangle = omega > sqrt2 ? flip_offset - omega : omega;

    return scale * triangle * (1.0f - triangle * triangle * one_sixth);
}

class Sine : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [](float phase)
		{
			return phase_to_sine(phase);
		};

		return calculate(calculator, freq, sync);
	}
};

}}}