#pragma once

#include <functional>
#include "phasor.hpp"
    
namespace bleat {
namespace oscillators {
namespace scalar {

class Oscillator
{
	Phasor phasor_;
	float value_ = 0.0f;

protected:

	float calculate(std::function<float(float)> calculator, float freq, float sync = -1.0f)
	{
		return (value_ = calculator(phasor_(freq, sync)));
	}

public:

	void reset() { phasor_.reset(); }
	float value() const { return value_; }
	float sync() const { return phasor_.sync(); }
};

}}}