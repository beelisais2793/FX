#pragma once

#include <bleat/math.hpp>

#include "sine.hpp"
#include "triangle.hpp"
#include "pulse.hpp"
#include "saw.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

class MultiWaveOsc : public Oscillator
{
public:

	static constexpr auto WAVE_SINE = 0.0f;
	static constexpr auto WAVE_TRIANGLE = 1.0f / 3.0f;
	static constexpr auto WAVE_PULSE = 2.0f / 3.0f;
	static constexpr auto WAVE_SAW = 1.0f;

	float operator()(float freq, float width, float wave, float sync = -1.0f)
	{
		const auto calculator = [freq, width, wave](float phase)
		{
			if (wave < WAVE_TRIANGLE)
			{
				const auto triangle = phase_to_triangle(phase);
				const auto sine = phase_to_sine(phase);

				const auto xfade = math::inverse_lerp(WAVE_SINE, WAVE_TRIANGLE, wave);

				return math::lerp(sine, triangle, xfade);
			}

			if (wave < WAVE_PULSE)
			{
				const auto triangle = phase_to_triangle(phase);
				const auto pulse = phase_to_pulse(phase, freq, width);

				const auto xfade = math::inverse_lerp(WAVE_TRIANGLE, WAVE_PULSE, wave);

				return math::lerp(triangle, pulse, xfade);
			}

			const auto pulse = phase_to_pulse(phase, freq, width);
			const auto saw = phase_to_saw(phase, freq);

			const auto xfade = math::inverse_lerp(WAVE_PULSE, WAVE_SAW, wave);

			return math::lerp(pulse, saw, xfade);
		};

		return calculate(calculator, freq, sync);
	}
};

}}}