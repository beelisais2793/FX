#pragma once

#include <cmath>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <complex>
#include <cassert>

#include "speex_resampler.h"
#include <pffft.h>

namespace RackDSP
{
	namespace simd
	{
		float ifelse(bool e, float t1, float t2) {
			return e? t1:t2;
		}
	}

	/** Useful for storing arrays of samples in ring buffers and casting them to `float*` to be used by interleaved processors, like SampleRateConverter */
	template <size_t CHANNELS, typename T = float>
	struct Frame {
		T samples[CHANNELS];
	};

	inline double sinc(double x)
	{
		if(x == 0.0) return 1.0f;
		x *= M_PI;
		return std::sin(x) / x;
	}
}