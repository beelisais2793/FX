#pragma once

#include <cstdint>
#include <bleat/third_party/const_math.hpp>
#include "polyBLEP.hpp"

namespace bleat {
namespace oscillators {
namespace scalar {

/*
 * Based on phasor code from madronalabs/madronalib
*/

class Phasor
{
	std::int32_t phase_ = 0;
	bool up_ = false;
	float sync_;

	void update_phase(std::int32_t inc, float sync)
	{
		if (sync > 0.0f)
		{
			phase_ = std::int32_t(sync * inc);
		}
		else
		{
			phase_ += inc;
		}
	}

	void update_sync_value(std::int32_t prev_phase, std::int32_t inc)
	{
		if (prev_phase <= 0 && phase_ >= 0)
		{
			sync_ = float(phase_) / inc;
			return;
		}

		sync_ = -1.0f;
	}

public:

	void reset()
	{
		phase_ = 0;
		up_ = false;
	}

	float sync() const
	{
		return sync_;
	}

	float operator()(float cyclesPerSample, float sync = -1.0f)
	{
		constexpr float steps_per_cycle(static_cast<float>(const_math::pow(2., 32)));
		constexpr float cycles_per_step(1.f / steps_per_cycle);

		// calculate int steps per sample
		const auto steps_per_sample = cyclesPerSample * steps_per_cycle;
		const auto inc = int(steps_per_sample);
		const auto prev_phase = phase_;

		update_phase(inc, sync);
		update_sync_value(prev_phase, inc);

		// convert counter to float output range
		return float(phase_) * cycles_per_step + 0.5f;
	}
};

}}}

