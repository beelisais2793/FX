#pragma once

#include "oscillator.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

/*
 * Based on oscillator code from madronalabs/madronalib
*/

// input: phasor on (0, 1), normalized freq, pulse width
// output: antialiased pulse
inline float phase_to_pulse(float phase, float freq, float width)
{
    float dummy;

    // get pulse selector mask
    const auto maskV = phase > width;

    // select -1 or 1 (could be a multiply instead?)
    auto pulse = phase > width ? -1.0f : 1.0f;

    // add blep for up-going transition
    pulse += polyBLEP(phase, freq);

    // subtract blep for down-going transition
    const auto down = std::modf(phase - width + 1.0f, &dummy);

    pulse -= polyBLEP(down, freq);

    return pulse;
}

class Pulse : public Oscillator
{
public:

	float operator()(float freq, float width, float sync = -1.0f)
	{
		const auto calculator = [freq, width](float phase)
		{
			return phase_to_pulse(phase, freq, width);
		};

		return calculate(calculator, freq, sync);
	}
};

}}}