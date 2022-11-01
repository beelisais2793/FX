#pragma once

#include "BogAudioDSP.hpp"


namespace DSP::BogAudio
{
	////////////////////////////////////////////////////////////////////////////////////////
	// Noise
	////////////////////////////////////////////////////////////////////////////////////////
	class Seeds {
	private:
		std::mt19937 _generator;
		Seeds();
		unsigned int _next();

	public:
		Seeds(const Seeds&) = delete;
		void operator=(const Seeds&) = delete;
		static Seeds& getInstance();

		static unsigned int next();
	};

	struct NoiseGenerator : Generator {
		std::minstd_rand _generator; // one of the faster options.

		NoiseGenerator() : _generator(Seeds::next()) {}
	};

	struct WhiteNoiseGenerator : NoiseGenerator {
		std::uniform_real_distribution<double> _uniform;

		WhiteNoiseGenerator() : _uniform(-1.0, 1.0) {}

		double _next() override {
			return _uniform(_generator);
		}
	};


    

	template<typename G>
	struct BasePinkNoiseGenerator : NoiseGenerator {
		static const int _n = 7;
		G _g;
		G _gs[_n];
		uint32_t _count = _g.next();

		double _next() override {
			// See: http://www.firstpr.com.au/dsp/pink-noise/
			double sum = _g.next();
			for (int i = 0, bit = 1; i < _n; ++i, bit <<= 1) {
				if (_count & bit) {
					sum += _gs[i].next();
				}
				else {
					sum += _gs[i].current();
				}
			}
			++_count;
			return sum / (double)(_n + 1);
		}
	};

	struct PinkNoiseGenerator : BasePinkNoiseGenerator<WhiteNoiseGenerator> {};

	struct RedNoiseGenerator : BasePinkNoiseGenerator<PinkNoiseGenerator> {};

	struct BlueNoiseGenerator : NoiseGenerator {
		PinkNoiseGenerator _pink;
		double _last = 0.0f;

		double _next() override {
			double t = _last;
			_last = _pink.next();
			return _last - t;
		}
	};



	struct GaussianNoiseGenerator : NoiseGenerator {
		std::normal_distribution<double> _normal;

		GaussianNoiseGenerator(double mean = 0.0f, double stdDev = 1.0f) : _normal(mean, stdDev) {}

		double _next() override {
			return _normal(_generator);
		}
	};


	struct RandomWalk : Generator {
		double _min;
		double _max;
		double _last = 0.0f;
		double _lastOut = 0.0f;
		double _damp;
		double _bias = 0.0f;
		double _biasDamp = 1.0f;
		WhiteNoiseGenerator _noise;
		LowPassFilter _filter;

		RandomWalk(
			double min = -5.0f,
			double max = 5.0f,
			double sampleRate = 1000.0f,
			double change = 0.5f
		)
		: _min(min)
		, _max(max)
		{
			assert(_min < _max);
			setParams(sampleRate, change);
		}

		void setParams(double sampleRate = 1000.0f, double change = 0.5f);
		void jump();
		void tell(double v);
		double _next() override;
	};

    Seeds::Seeds() {
#ifdef ARCH_WIN
  _generator.seed(time(0));
#else
  std::random_device rd;
  _generator.seed(rd());
#endif
}

unsigned int Seeds::_next() {
  return _generator();
}

Seeds& Seeds::getInstance() {
  static Seeds instance;
  return instance;
}

unsigned int Seeds::next() {
  return getInstance()._next();
};


void RandomWalk::setParams(double sampleRate, double change) {
	assert(sampleRate > 0.0f);
	assert(change >= 0.0f);
	assert(change <= 1.0f);

	_filter.setParams(sampleRate, std::max(2.0f, change * 0.49f * std::min(44100.0f, sampleRate)));

	const double maxDamp = 0.98;
	const double minDamp = 0.9999;
	_damp = maxDamp + (1.0f - change)*(minDamp - maxDamp);

	_biasDamp = 1.0f - change*(2.0f / sampleRate);
}

void RandomWalk::jump() {
	double x = fabsf(_noise.next()) * (_max - _min);
	x += _min;
	tell(x);
}

void RandomWalk::tell(double v) {
	assert(v >= _min && v <= _max);
	_last = _bias = v;
	_filter.reset();
}

double RandomWalk::_next() {
	double delta = _noise.next();
	if ((_lastOut >= _max && delta > 0.0f) || (_lastOut <= _min && delta < 0.0f)) {
		delta = -delta;
	}
	_last = _damp*_last + delta;
	_bias *= _biasDamp;
	return _lastOut = std::min(std::max(_bias + _filter.next(_last), _min), _max);
}
}
