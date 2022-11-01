#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{
	struct Limiter : public FunctionProcessor {
		double _shape;
		double _knee;
		double _limit;
		double _scale;
		double _normalization;
		FastTanhf _tanhf;

		Limiter() : FunctionProcessor() {}

		void setParams(double shape = 1.0f, double knee = 5.0f, double limit = 10.0f, double scale = 2.0f);
		double next(double sample);

		enum {
			PORT_SHAPE,
			PORT_KNEE,
			PORT_LIMIT,
			PORT_SCALE,		
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_SHAPE: setParams(v,_knee,_limit,_scale); break;
				case PORT_KNEE: setParams(_shape,v,_limit,_scale); break;
				case PORT_LIMIT: setParams(_shape,_knee,v,_scale); break;
				case PORT_SCALE: setParams(_shape,_knee,_limit,v); break;
			}
		}
		double Tick(double I, double A=1, double X=1, double Y=1) {
			double s = shape;
			double k = knee;
			setParams(s*fabs(X),k*fabs(Y));
			double r = next(I);
			setParams(s,k);
			return A*r;
		}
	};

    
	struct Saturator : public FunctionProcessor {
		static const double limit;

		Saturator() : FunctionProcessor() {

		}
		double next(double sample);

		double Tick(double I, double A=1, double X=1, double Y=1) {
			return A*next(X*Y*I);
		}
	};

	struct Compressor {
		static const double maxEffectiveRatio;
		double _detectorDb,_thresholdDb,_ratio,_softknee;
		Compressor() {
			_detectorDb = _thresholdDb = _ratio = _softknee =0.0;
		}
		double compressionDb(double detectorDb, double thresholdDb, double ratio, bool softKnee);
		enum {
			PORT_DETECTORDB,
			PORT_THRESHDB,
			PORT_RATIO,
			PORT_SOFTKNEE,
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_DETECTORDB: _detectorDb = v; break;
				case PORT_THRESHDB: _thresholdDb = v; break;
				case PORT_RATIO: _ratio = v; break;
				case PORT_SOFTKNEE: _softknee = v; break;
			}
		}
		double Tick(double I, double A=1, double X=1, double Y=1) {
			double r = _ratio;
			double k = _softknee;
			double g = compressionDb(_detectorDb,_thresholdDb,r*fabs(X),k*fabs(Y));
			return A*(g*I);
		}
	};

	struct NoiseGate {
		static const double maxEffectiveRatio;
		double _detectorDb,_thresholdDb,_ratio,_softknee;
		NoiseGate() {
			_detectorDb = _thresholdDb = _ratio = _softknee =0.0;
		}
		double compressionDb(double detectorDb, double thresholdDb, double ratio, bool softKnee);

		enum {
			PORT_DETECTORDB,
			PORT_THRESHDB,
			PORT_RATIO,
			PORT_SOFTKNEE,
		};
		void setPort(int port, double v) {
			switch(port) {
				case PORT_DETECTORDB: _detectorDb = v; break;
				case PORT_THRESHDB: _thresholdDb = v; break;
				case PORT_RATIO: _ratio = v; break;
				case PORT_SOFTKNEE: _softknee = v; break;
			}
		}
		double Tick(double I, double A=1, double X=1, double Y=1) {
			double r = _ratio;
			double k = _softknee;
			double g = compressionDb(_detectorDb,_thresholdDb,r*fabs(X),k*fabs(Y));
			return A*(g*I);
		}
	};

    void Limiter::setParams(double shape, double knee, double limit, double scale) {
		assert(shape >= 0.0f);
		assert(knee >= 0.0f);
		assert(limit >= 0.0f);
		assert(scale >= 1.0f);
		_shape = shape;
		_knee = knee;
		_limit = std::max(knee, limit);
		_scale = scale;

		if (_shape >= 0.1f) {
			if (_shape < 1.0f) {
				_normalization = 1.0f / tanhf(_shape * M_PI);
			}
			else {
				_normalization = 1.0f;
			}
		}
	}

	double Limiter::next(double sample) {
		double out = fabsf(sample);
		if (out > _knee) {
			out -= _knee;
			out /= _scale;
			if (_shape >= 0.1f) {
				// out /= _limit - _knee;
				// out = _tanhf.value(out * _shape * M_PI) * _normalization;
				// out *= _limit - _knee;
				double x = out / (_limit - _knee);
				x = _tanhf.value(x * _shape * M_PI) * _normalization;
				x = std::min(x, 1.0f);
				x *= _limit - _knee;
				out = std::min(fabsf(sample) - _knee, x);
			}
			else {
				out = std::min(out, _limit - _knee);
			}
			out += _knee;
		}

		if (sample < 0.0f) {
			return -out;
		}
		return out;
	}


	const double Saturator::limit = 12.0f;

	// Zavalishin 2018, "The Art of VA Filter Design", http://www.native-instruments.com/fileadmin/ni_media/downloads/pdf/VAFilterDesign_2.0.0a.pdf
	static inline double saturation(double x) {
		const double y1 = 0.98765f; // (2*x - 1)/x**2 where x is 0.9.
		const double offset = 0.075f / Saturator::limit; // magic.
		double x1 = (x + 1.0f) * 0.5f;
		return Saturator::limit * (offset + x1 - sqrtf(x1 * x1 - y1 * x) * (1.0f / y1));
	}

	double Saturator::next(double sample) {
		double x = sample * (1.0f / limit);
		if (sample < 0.0f) {
			return -saturation(-x);
		}
		return saturation(x);
	}


	const double Compressor::maxEffectiveRatio = 1000.0f;

	double Compressor::compressionDb(double detectorDb, double thresholdDb, double ratio, bool softKnee) {
		const double softKneeDb = 3.0f;

		if (softKnee) {
			double sDb = thresholdDb - softKneeDb;
			if (detectorDb <= sDb) {
				return 0.0f;
			}

			double ix = softKneeDb * std::min(ratio, maxEffectiveRatio) + thresholdDb;
			double iy = softKneeDb + thresholdDb;
			double t = (detectorDb - sDb) / (ix - thresholdDb);
			double px = t * (ix - thresholdDb) + thresholdDb;
			double py = t * (iy - thresholdDb) + thresholdDb;
			double s = (py - sDb) / (px - sDb);
			double compressionDb = detectorDb - sDb;
			compressionDb -= s * (detectorDb - sDb);
			return compressionDb;
		}

		if (detectorDb <= thresholdDb) {
			return 0.0f;
		}
		double compressionDb = detectorDb - thresholdDb;
		compressionDb -= compressionDb / ratio;
		return compressionDb;
	}


	const double NoiseGate::maxEffectiveRatio = Compressor::maxEffectiveRatio;

	double NoiseGate::compressionDb(double detectorDb, double thresholdDb, double ratio, bool softKnee) {
		const double softKneeDb = 6.0f;

		if (softKnee) {
			// FIXME: this achieves nothing.
			double range = thresholdDb - Amplifier::minDecibels;
			double ix = thresholdDb + softKneeDb;
			double iy = 0;
			if (detectorDb >= ix) {
				return 0.0f;
			}
			double ox = thresholdDb - range / ratio;
			if (detectorDb <= ox) {
				return -Amplifier::minDecibels;
			}
			const double oy = Amplifier::minDecibels;
			double t = (detectorDb - ox) / (ix - ox);
			double px = t * (ix - thresholdDb) + thresholdDb;
			double py = t * (iy - thresholdDb) + thresholdDb;
			double s = (py - oy) / (px - ox);
			return -(oy + s * (detectorDb - ox));
		}

		if (detectorDb >= thresholdDb) {
			return 0.0f;
		}
		double differenceDb = thresholdDb - detectorDb;
		double compressionDb = differenceDb * ratio - differenceDb;
		return std::min(compressionDb, -Amplifier::minDecibels);
	}
}