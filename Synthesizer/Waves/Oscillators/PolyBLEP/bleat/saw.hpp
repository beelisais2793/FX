#pragma once

#include "oscillator.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

/*
 * Based on oscillator code from madronalabs/madronalib
*/

// input: phasor on (0, 1), normalized freq
// output: antialiased saw on (-1, 1)
inline float phase_to_saw(float phase, float freq)
{
    // scale phasor to saw range (-1, 1)
    const auto saw = phase * 2.0f - 1.0f;

    // subtract BLEP from saw to smooth down-going transition
    return saw - polyBLEP(phase, freq);
}

class Saw : public Oscillator
{
public:

	float operator()(float freq, float sync = -1.0f)
	{
		const auto calculator = [freq](float phase)
		{
			return phase_to_saw(phase, freq);
		};

		return calculate(calculator, freq, sync);
	}
};

}}}