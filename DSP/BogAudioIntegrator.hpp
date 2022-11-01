#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{


	struct Integrator : public FunctionProcessor{
		double _alpha = 0.0f;
		double _last = 0.0f;

		Integrator(double alpha = 1.0f) : FunctionProcessor() {
			setParams(alpha);
		}

		void setParams(double alpha);
		double next(double sample);

		double Tick(double I, double A=1, double X=1, double  Y=1) {
			return next(I);
		}
	};

 	void Integrator::setParams(double alpha) {
		assert(alpha >= 0.0f);
		assert(alpha <= 1.0f);
		_alpha = alpha;
	}

	double Integrator::next(double sample) {
		// "leaky integrator"
		return _last = (1.0f - _alpha)*_last + _alpha*sample;
	}

}