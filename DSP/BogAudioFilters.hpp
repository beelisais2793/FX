#pragma once

#include "BogAudioDSP.hpp"

namespace DSP::BogAudio
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Filters
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Filter : public FilterProcessor {

		Filter() : FilterProcessor() {}
		virtual ~Filter() {}

		virtual double next(double sample) = 0;

		double Tick(double I, double A=1, double X=1, double Y=1) {
			return A*next(I);
		}
	};

	struct ResetableFilter : Filter {
		virtual void reset() = 0;
	};

	template<typename T>
	struct BiquadFilter : ResetableFilter {
		T _a0 = 0.0;
		T _a1 = 0.0;
		T _a2 = 0.0;
		T _b1 = 0.0;
		T _b2 = 0.0 ;

		T _x[3] {};
		T _y[3] {};

		void setParams(T a0, T a1, T a2, T b0, T b1, T b2) {
			T ib0 = 1.0 / b0;
			_a0 = a0 * ib0;
			_a1 = a1 * ib0;
			_a2 = a2 * ib0;
			_b1 = b1 * ib0;
			_b2 = b2 * ib0;
		}

		void reset() override {
			_x[0] = _x[1] = _x[2] = 0.0;
			_y[0] = _y[1] = _y[2] = 0.0;
		}

		double next(double sample) override {
			_x[2] = _x[1];
			_x[1] = _x[0];
			_x[0] = sample;

			_y[2] = _y[1];
			_y[1] = _y[0];
			_y[0] = _a0 * _x[0];
			_y[0] += _a1 * _x[1];
			_y[0] += _a2 * _x[2];
			_y[0] -= _b1 * _y[1];
			_y[0] -= _b2 * _y[2];

			return _y[0];
		}
	};


    
	struct LowPassFilter : ResetableFilter {
		double _sampleRate = 0.0f;
		double _cutoff = 0.0f;
		double _q = 0.0f;

		BiquadFilter<double> _biquad; // double is necessary here to make low cutoffs work at high sample rates.

		LowPassFilter(double sampleRate = 1000.0f, double cutoff = 100.0f, double q = 0.001f) {
			setParams(sampleRate, cutoff, q);
		}

		void setParams(double sampleRate, double cutoff, double q = 0.001f);
		void reset() override { _biquad.reset(); }
		double next(double sample) override {
			return _biquad.next(sample);
		}
	};

	// See: http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
	void LowPassFilter::setParams(double sampleRate, double cutoff, double q) {
		if (_sampleRate == sampleRate && _cutoff == cutoff && _q == q) {
			return;
		}
		_sampleRate = sampleRate;
		_cutoff = cutoff;
		_q = q;
		// printf("\nLPF set param: sr=%f c=%f q=%f\n", _sampleRate, _cutoff, _q);

		double w0 = 2.0 * M_PI * (double)(_cutoff / _sampleRate);
		double cosw0 = cos(w0);
		double alpha = sin(w0) / (2.0 * _q);

		_biquad.setParams(
			(1.0 - cosw0) / 2.0,
			1.0 - cosw0,
			(1.0 - cosw0) / 2.0,
			1.0 + alpha,
			-2.0 * cosw0,
			1.0 - alpha
		);
	}


    
	template<typename T, int N>
	struct BiquadBank : ResetableFilter {
	#ifdef RACK_SIMD
		Biquad4 _biquads[N / 4];
	#else
		BiquadFilter<T> _biquads[N];
		int _n = N;
	#endif

		void setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2);
		void reset() override;
		void setN(int n, bool minDelay = false);
		double next(double sample) override;
	};

	struct MultimodeTypes {
		typedef double T;
		typedef std::complex<T> TC;

		enum Type {
			UNKNOWN_TYPE,
			BUTTERWORTH_TYPE,
			CHEBYSHEV_TYPE
		};

		enum Mode {
			UNKNOWN_MODE,
			LOWPASS_MODE,
			HIGHPASS_MODE,
			BANDPASS_MODE,
			BANDREJECT_MODE
		};

		enum BandwidthMode {
			UNKNOWN_BANDWIDTH_MODE,
			LINEAR_BANDWIDTH_MODE,
			PITCH_BANDWIDTH_MODE
		};

		enum DelayMode {
			UNKNOWN_DELAY_MODE,
			FIXED_DELAY_MODE,
			MINIMUM_DELAY_MODE
		};

		static constexpr int minPoles = 1;
		static constexpr int maxPoles = 16;
		static constexpr int modPoles = 1;
		static constexpr double minFrequency = 3.0f; // FIXME: this can go down to at least 1.0f if T is double.
		static constexpr double maxFrequency = 21000.0f;
		static constexpr double minQbw = 0.0f;
		static constexpr double maxQbw = 1.0f;
		static constexpr double minBWLinear = 10.0f;
		static constexpr double maxBWLinear = 5000.0f;
		static constexpr double minBWPitch = 1.0f / (1.0f * 12.0f * 100.0f / 25.0f);
		static constexpr double maxBWPitch = 2.0f;
	};

    
	template<int N>
	struct MultimodeDesigner : MultimodeTypes {
		struct Pole {
			TC p;
			T x = 0.0;
			T y = 0.0;
			TC pc;
			TC p2;
			TC i2p;
			TC i2pc;
			T r = 0.0;

			Pole() {}
			Pole(T re, T im, T x, T y) : p(TC(re, im)), x(x), y(y) {
				pc = std::conj(p);
				p2 = p * p;
				i2p = (T)1.0 / ((T)2.0 * p);
				i2pc = (T)1.0 / ((T)2.0 * pc);
				r = std::abs(p);
			}
		};

		double _sampleRate = 44100.0f;
		double _half2PiST = 0.0f;
		Type _type = UNKNOWN_TYPE;
		Mode _mode = UNKNOWN_MODE;
		int _nPoles = 0;
		double _frequency = -1.0f;
		double _qbw = -1.0f;
		BandwidthMode _bandwidthMode = UNKNOWN_BANDWIDTH_MODE;
		DelayMode _delayMode = UNKNOWN_DELAY_MODE;
		Pole _poles[maxPoles / 2];
		int _nBiquads = 0;

		double effectiveMinimumFrequency();
		void setParams(
			BiquadBank<T, N>& biquads,
			double& outGain,
			double sampleRate,
			Type type,
			int poles,
			Mode mode,
			double frequency,
			double qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		);
	};

    
	struct MultimodeFilter : MultimodeTypes, ResetableFilter {
		virtual void setParams(
			double sampleRate,
			Type type,
			int poles,
			Mode mode,
			double frequency,
			double qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		) = 0;
	};

	template<int N>
	struct MultimodeBase : MultimodeFilter {
		MultimodeDesigner<N> _designer;
		BiquadBank<T, N> _biquads;
		double _outGain = 1.0f;

		inline double effectiveMinimumFrequency() { return _designer.effectiveMinimumFrequency(); }
		void setParams(
			double sampleRate,
			Type type,
			int poles,
			Mode mode,
			double frequency,
			double qbw,
			BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
			DelayMode dm = FIXED_DELAY_MODE
		) override;
		double next(double sample) override;
		void reset() override;
	};

	typedef MultimodeBase<16> MultimodeFilter16;
	typedef MultimodeBase<8> MultimodeFilter8;
	typedef MultimodeBase<4> MultimodeFilter4;

    

	struct FourPoleButtworthLowpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			double sampleRate,
			double frequency,
			double q
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::LOWPASS_MODE,
				frequency,
				q
			);
		}
		inline double next(double sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	struct FourPoleButtworthHighpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			double sampleRate,
			double frequency,
			double q
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::HIGHPASS_MODE,
				frequency,
				q
			);
		}
		inline double next(double sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};



	struct TwoPoleButtworthBandpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			double sampleRate,
			double frequency,
			double bw,
			MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				2,
				MultimodeFilter::BANDPASS_MODE,
				frequency,
				bw,
				bwm
			);
		}
		inline double next(double sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};


	struct FourPoleButtworthBandpassFilter {
		MultimodeFilter4 _filter;

		inline void setParams(
			double sampleRate,
			double frequency,
			double bw,
			MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
		) {
			_filter.setParams(
				sampleRate,
				MultimodeFilter::BUTTERWORTH_TYPE,
				4,
				MultimodeFilter::BANDPASS_MODE,
				frequency,
				bw,
				bwm
			);
		}
		inline double next(double sample) { return _filter.next(sample); }
		inline void reset() { _filter.reset(); }
	};

	template<typename T, int N> void BiquadBank<T, N>::setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2) {
		assert(i >= 0 && i < N);
		_biquads[i].setParams(a0, a1, a2, b0, b1, b2);
	}

	template<typename T, int N> void BiquadBank<T, N>::reset() {
		for (int i = 0; i < N; ++i) {
			_biquads[i].reset();
		}
	}

	template<typename T, int N> void BiquadBank<T, N>::setN(int n, bool _minDelay) {
		assert(n <= N);
		_n = n;
		for (; n < N; ++n) {
			_biquads[n].reset();
		}
	}

	template<typename T, int N> double BiquadBank<T, N>::next(double sample) {
		for (int i = 0; i < _n; ++i) {
			sample = _biquads[i].next(sample);
		}
		return sample;
	}



	template struct BiquadBank<MultimodeTypes::T, 4>;
	template struct BiquadBank<MultimodeTypes::T, 8>;
	template struct BiquadBank<MultimodeTypes::T, 16>;


	///////////////////////////////////////////////////////////////////////////////
	// Multimode
	//////////////////////////////////////////////////////////////////////////////
	constexpr int MultimodeTypes::minPoles;
	constexpr int MultimodeTypes::maxPoles;
	constexpr int MultimodeTypes::modPoles;
	constexpr double MultimodeTypes::minFrequency;
	constexpr double MultimodeTypes::maxFrequency;
	constexpr double MultimodeTypes::minQbw;
	constexpr double MultimodeTypes::maxQbw;
	constexpr double MultimodeTypes::minBWLinear;
	constexpr double MultimodeTypes::maxBWLinear;
	constexpr double MultimodeTypes::minBWPitch;
	constexpr double MultimodeTypes::maxBWPitch;


	template<int N> double MultimodeDesigner<N>::effectiveMinimumFrequency() {
		return minFrequency * std::max(1.0f, roundf(_sampleRate / 44100.0f));
	}

	template<int N> void MultimodeDesigner<N>::setParams(
		BiquadBank<T, N>& biquads,
		double& outGain,
		double sampleRate,
		Type type,
		int poles,
		Mode mode,
		double frequency,
		double qbw,
		BandwidthMode bwm,
		DelayMode dm
	) {
		assert(N >= minPoles && N <= maxPoles);
		assert(poles >= minPoles && (poles <= N || (poles <= 2*N && (mode == LOWPASS_MODE || mode == HIGHPASS_MODE))));
		assert(poles % modPoles == 0);
		assert(frequency >= minFrequency - 0.00001f && frequency <= maxFrequency);
		frequency = std::max(frequency, effectiveMinimumFrequency());
		frequency = std::min(frequency, 0.49f * sampleRate);
		assert(qbw >= minQbw && qbw <= maxQbw);

		bool repole = _type != type || _mode != mode || _nPoles != poles || (type == CHEBYSHEV_TYPE && (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) && _qbw != qbw);
		bool redesign = repole || _frequency != frequency || _qbw != qbw || _sampleRate != sampleRate || _bandwidthMode != bwm || _delayMode != dm;
		_sampleRate = sampleRate;
		_half2PiST = M_PI * (1.0f / sampleRate);
		_type = type;
		_nPoles = poles;
		_mode = mode;
		_frequency = frequency;
		_qbw = qbw;
		_bandwidthMode = bwm;
		_delayMode = dm;

		if (repole) {
			switch (_type) {
				case BUTTERWORTH_TYPE: {
					int np = _nPoles / 2 + (_nPoles % 2 == 1);
					for (int k = 1, j = np - 1; k <= np; ++k, --j) {
						T a = (T)(2 * k + _nPoles - 1) * M_PI / (T)(2 * _nPoles);
						T re = std::cos(a);
						T im = std::sin(a);
						_poles[j] = Pole(-re, im, re + re, re * re + im * im);
					}

					outGain = 1.0f;
					break;
				}

				case CHEBYSHEV_TYPE: {
					T ripple = 3.0;
					if (mode == LOWPASS_MODE || mode == HIGHPASS_MODE) {
						ripple += std::max(0.0f, 6.0f * qbw);
					}
					T e = ripple / (T)10.0;
					e = std::pow((T)10.0, e);
					e -= (T)1.0;
					e = std::sqrt(e);
					T ef = std::asinh((T)1.0 / e) / (double)_nPoles;
					T efr = -std::sinh(ef);
					T efi = std::cosh(ef);

					int np = _nPoles / 2 + (_nPoles % 2 == 1);
					for (int k = 1, j = np - 1; k <= np; ++k, --j) {
						T a = (T)(2 * k - 1) * M_PI / (T)(2 * _nPoles);
						T re = efr * std::sin(a);
						T im = efi * std::cos(a);
						_poles[j] = Pole(-re, im, re + re, re * re + im * im);
					}

					outGain = 1.0 / (e * std::pow(2.0, (T)(_nPoles - 1)));
					// outGain = 1.0f / std::pow(2.0f, (T)(_nPoles - 1));
					break;
				}

				default: {
					assert(false);
				}
			}
		}

		if (redesign) {
			switch (_mode) {
				case LOWPASS_MODE:
				case HIGHPASS_MODE: {
					_nBiquads = _nPoles / 2 + _nPoles % 2;
					biquads.setN(_nBiquads, _delayMode == MINIMUM_DELAY_MODE);

					// T iq = (1.0 / std::sqrt(2.0)) - 0.65 * _qbw;
					T iq = (T)0.8 - (T)0.6 * _qbw;
					T wa = std::tan(_frequency * _half2PiST);
					T wa2 = wa * wa;

					if (_mode == LOWPASS_MODE) {
						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T wap = wa * std::real(_poles[0].p);
							biquads.setParams(0, wa, wa, 0.0, wap + (T)1.0, wap - (T)1.0, (T)0.0);
						}
						T a0 = wa2;
						T a1 = wa2 + wa2;
						T a2 = wa2;
						for (int i = 0; i < nb; ++i) {
							Pole& pole = _poles[ni + i];
							T ywa2 = pole.y * wa2;
							T ywa21 = ywa2 + (T)1.0;
							T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
							T xwa = x * wa;
							T b0 = ywa21 - xwa;
							T b1 = (T)-2.0 + (ywa2 + ywa2);
							T b2 = ywa21 + xwa;
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
					}
					else {
						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T rp = std::real(_poles[0].p);
							biquads.setParams(0, 1.0, -1.0, 0.0, wa + rp, wa - rp, 0.0);
						}
						T a0 = 1.0;
						T a1 = -2.0f;
						T a2 = 1.0;
						for (int i = 0; i < nb; ++i) {
							Pole& pole = _poles[ni + i];
							T wa2y = wa2 + pole.y;
							T x = (((T)(i == nb / 2) * (iq - (T)1.0)) + (T)1.0) * pole.x;
							T xwa = x * wa;
							T b0 = wa2y - xwa;
							T b1 = (wa2 + wa2) - (pole.y + pole.y);
							T b2 = wa2y + xwa;
							biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
						}
					}
					break;
				}

				case BANDPASS_MODE:
				case BANDREJECT_MODE: {
					_nBiquads = ((_nPoles / 2) * 2) + (_nPoles % 2);
					biquads.setN(_nBiquads, _delayMode == MINIMUM_DELAY_MODE);

					T wdl = 0.0;
					T wdh = 0.0;
					switch (_bandwidthMode) {
						case LINEAR_BANDWIDTH_MODE: {
							double bandwidth = std::max(minBWLinear, maxBWLinear * _qbw);
							wdl = std::max(minFrequency, _frequency - 0.5f * bandwidth);
							wdh = std::min(maxFrequency, std::max((double)wdl + 10.0f, _frequency + 0.5f * bandwidth));
							break;
						}
						case PITCH_BANDWIDTH_MODE: {
							double bandwidth = std::max(minBWPitch, maxBWPitch * _qbw);
							wdl = std::max(minFrequency, powf(2.0f, -bandwidth) * _frequency);
							wdh = std::min(maxFrequency, std::max((double)wdl + 10.0f, powf(2.0f, bandwidth) * _frequency));
							break;
						}
						default: {
							assert(false);
						}
					}
					T wal = std::tan(wdl * _half2PiST);
					T wah = std::tan(wdh * _half2PiST);
					T w = wah - wal;
					T w2 = w * w;
					T w02 = wah * wal;

					if (_mode == BANDPASS_MODE) {
						T a0 = w;
						T a1 = 0.0;
						T a2 = -w;

						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T wp = w * std::real(_poles[0].p);
							biquads.setParams(
								0,
								a0,
								a1,
								a2,
								(T)1.0 + wp + w02,
								(T)-2.0 + (w02 + w02),
								(T)1.0 - wp + w02
							);
						}
						for (int i = 0; i < nb; i += 2) {
							Pole& pole = _poles[ni + i / 2];
							TC x = pole.p2;
							x *= w2;
							x -= (T)4.0 * w02;
							x = std::sqrt(x);
							TC xc = std::conj(x);
							TC wp = w * pole.p;
							TC wpc = w * pole.pc;
							TC y1 = (x - wp) * (T)0.5;
							TC y1c = (xc - wpc) * (T)0.5;
							TC y2 = (-x - wp) * (T)0.5;
							TC y2c = (-xc - wpc) * (T)0.5;
							TC cf1a = -(y1 + y1c);
							TC cf2a = y1 * y1c;
							TC cf1b = -(y2 + y2c);
							TC cf2b = y2 * y2c;
							T f1a = std::real(cf1a);
							T f1b = std::real(cf1b);
							T f2a = std::real(cf2a);
							T f2b = std::real(cf2b);

							{
								T b0 = (T)1.0 + f1a + f2a;
								T b1 = (T)-2.0 + (f2a + f2a);
								T b2 = (T)1.0 - f1a + f2a;
								biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
							}
							{
								T b0 = (T)1.0 + f1b + f2b;
								T b1 = (T)-2.0 + (f2b + f2b);
								T b2 = (T)1.0 - f1b + f2b;
								biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
							}
						}
					}
					else {
						T a0 = (T)1.0 + w02;
						T a1 = (T)-2.0 + (w02 + w02);
						T a2 = a0;

						int ni = 0;
						int nb = _nBiquads;
						if (_nPoles % 2 == 1) {
							++ni;
							--nb;
							T rp = std::real(_poles[0].p);
							T rpw02 = rp * w02;
							biquads.setParams(
								0,
								a0,
								a1,
								a2,
								rp + w + rpw02,
								(T)-2.0 * rp + (rpw02 + rpw02),
								rp - w + rpw02
							);
						}
						for (int i = 0; i < nb; i += 2) {
							Pole& pole = _poles[ni + i / 2];
							TC x = pole.p2;
							x *= (T)-4.0 * w02;
							x += w2;
							x = std::sqrt(x);
							TC xc = std::conj(x);
							TC y1 = (x - w) * pole.i2p;
							TC y1c = (xc - w) * pole.i2pc;
							TC y2 = (-x - w) * pole.i2p;
							TC y2c = (-xc - w) * pole.i2pc;
							TC cf1a = -pole.r * (y1 + y1c);
							TC cf2a = pole.r * y1 * y1c;
							TC cf1b = -pole.r * (y2 + y2c);
							TC cf2b = pole.r * y2 * y2c;
							T f1a = std::real(cf1a);
							T f1b = std::real(cf1b);
							T f2a = std::real(cf2a);
							T f2b = std::real(cf2b);

							{
								T b0 = pole.r + f1a + f2a;
								T b1 = (T)-2.0 * pole.r + (f2a + f2a);
								T b2 = pole.r - f1a + f2a;
								biquads.setParams(ni + i, a0, a1, a2, b0, b1, b2);
							}
							{
								T b0 = pole.r + f1b + f2b;
								T b1 = (T)-2.0 * pole.r + (f2b + f2b);
								T b2 = pole.r - f1b + f2b;
								biquads.setParams(ni + i + 1, a0, a1, a2, b0, b1, b2);
							}
						}
					}
					break;
				}

				default: {
					assert(false);
				}
			}
		}
	}

	template struct MultimodeDesigner<4>;
	template struct MultimodeDesigner<8>;
	template struct MultimodeDesigner<16>;


	template<int N> void MultimodeBase<N>::setParams(
		double sampleRate,
		Type type,
		int poles,
		Mode mode,
		double frequency,
		double qbw,
		BandwidthMode bwm,
		DelayMode dm
	) {
		_designer.setParams(
			_biquads,
			_outGain,
			sampleRate,
			type,
			poles,
			mode,
			frequency,
			qbw,
			bwm,
			dm
		);
	}

	template<int N> double MultimodeBase<N>::next(double sample) {
		return _outGain * _biquads.next(sample);
	}

	template<int N> void MultimodeBase<N>::reset() {
		_biquads.reset();
	}

	template struct MultimodeBase<4>;
	template struct MultimodeBase<8>;
	template struct MultimodeBase<16>;




	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Experiments
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct ComplexBiquadFilter : BiquadFilter<double> {
		double _gain = 1.0f;
		double _zeroRadius = 1.0f;
		double _zeroTheta = M_PI;
		double _poleRadius = 0.9f;
		double _poleTheta = 0.0f;

		ComplexBiquadFilter() {
			updateParams();
		}

		void setComplexParams(
			double gain,
			double zeroRadius,
			double zeroTheta,
			double poleRadius,
			double poleTheta
		);
		void updateParams();
	};

	struct MultipoleFilter : Filter {
		enum Type {
			LP_TYPE,
			HP_TYPE
		};

		static constexpr int maxPoles = 20;
		static constexpr double maxRipple = 0.29f;
		Type _type = LP_TYPE;
		int _poles = 1;
		double _sampleRate = 0.0f;
		double _cutoff = 0.0f;
		double _ripple = 0.0f;
		BiquadFilter<double> _biquads[maxPoles / 2];

		MultipoleFilter() {}

		void setParams(
			Type type,
			int poles,
			double sampleRate,
			double cutoff,
			double ripple // FIXME: using this with more than two poles creates large gain, need compensation.
		);
		double next(double sample) override;
	};

	void ComplexBiquadFilter::setComplexParams(
		double gain,
		double zeroRadius,
		double zeroTheta,
		double poleRadius,
		double poleTheta
	) {
		if (
			_gain != gain ||
			_zeroRadius != zeroRadius ||
			_zeroTheta != zeroTheta ||
			_poleRadius != poleRadius ||
			_poleTheta != poleTheta
		) {
			assert(gain >= 0.0f && gain <= 1.0f);
			assert(zeroRadius >= 0.0f && zeroRadius <= 1.0f);
			assert(zeroTheta >= 0.0f && zeroTheta <= 2.0f*M_PI);
			assert(poleRadius >= 0.0f && poleRadius <= 1.0f);
			assert(poleTheta >= 0.0f && poleTheta <= 2.0f*M_PI);
			_gain = gain;
			_zeroRadius = zeroRadius;
			_zeroTheta = zeroTheta;
			_poleRadius = poleRadius;
			_poleTheta = poleTheta;
			updateParams();
		}
	}

	void ComplexBiquadFilter::updateParams() {
		setParams(
			_gain,
			-2.0f * _zeroRadius * cosf(_zeroTheta) * _gain,
			_zeroRadius * _zeroRadius * _gain,
			1.0f,
			-2.0f * _poleRadius * cosf(_poleTheta),
			_poleRadius * _poleRadius
		);
	}


	// Adapted from Smith 1997, "The Scientist and Engineer's Guide to DSP"
	void MultipoleFilter::setParams(
		Type type,
		int poles,
		double sampleRate,
		double cutoff,
		double ripple
	) {
		if (
			_type == type &&
			_poles == poles &&
			_sampleRate == sampleRate &&
			_cutoff == cutoff &&
			_ripple == ripple
		) {
			return;
		}
		assert(poles >= 1 && poles <= maxPoles);
		assert(poles % 2 == 0); // relax this later?
		assert(sampleRate >= 0.0f);
		assert(cutoff >= 0.0f && cutoff <= sampleRate / 2.0f);
		assert(ripple >= 0.0f && ripple <= maxRipple);
		_type = type;
		_poles = poles;
		_sampleRate = sampleRate;
		_cutoff = cutoff;
		_ripple = ripple;

		for (int p = 0, pn = _poles / 2; p < pn; ++p) {
			double a0 = 0.0;
			double a1 = 0.0;
			double a2 = 0.0;
			double b1 = 0.0;
			double b2 = 0.0;
			{
				double angle = M_PI/(_poles*2.0) + p*M_PI/_poles;
				double rp = -cos(angle);
				double ip = sin(angle);

				if (_ripple > 0.01f) {
					double es = sqrt(pow(1.0 / (1.0 - _ripple), 2.0) - 1.0);
					double esi = 1.0 / es;
					double esis = esi * esi;
					double polesi = 1.0 / (double)_poles;
					double vx = polesi * log(esi + sqrt(esis + 1.0));
					double kx = polesi * log(esi + sqrt(esis - 1.0));
					kx = (exp(kx) + exp(-kx)) / 2.0;
					rp *= ((exp(vx) - exp(-vx)) / 2.0) / kx;
					ip *= ((exp(vx) + exp(-vx)) / 2.0) / kx;
					// printf("\n\n\ncheb p=%d pn=%d rip=%f rp=%f ip=%f es=%f vx=%f kx=%f\n", p, pn, _ripple, rp, ip, es, vx, kx);
				}

				const double t = 2.0 * tan(0.5);
				const double ts = t * t;
				// printf("\n\nco=%f sr=%f fc=%f\n", _cutoff, _sampleRate, _cutoff / _sampleRate);
				double m = rp*rp + ip*ip;
				double mts = m * ts;
				double d = 4.0 - 4.0*rp*t + mts;
				double x0 = ts / d;
				double x1 = 2.0 * x0;
				double x2 = x0;
				double y1 = (8.0 - 2.0*mts) / d;
				double y2 = (-4.0 - 4.0*rp*t - mts) / d;
				// printf("vs p=%d t=%f w=%f m=%f d=%f x0=%f x1=%f x2=%f y1=%f y2=%f\n", p, t, w, m, d, x0, x1, x2, y1, y2);

				// FIXME: optimization: everything above here only depends on type, poles and ripple.

				double w = 2.0 * M_PI * (_cutoff / _sampleRate);
				double w2 = w / 2.0;
				double k = 0.0;
				switch (_type) {
					case LP_TYPE: {
						k = sin(0.5 - w2) / sin(0.5 + w2);
						break;
					}
					case HP_TYPE: {
						k = -cos(w2 + 0.5) / cos(w2 - 0.5);
						break;
					}
				}
				double ks = k * k;
				d = 1.0 + y1*k - y2*ks;
				a0 = (x0 - x1*k + x2*ks) / d;
				a1 = (-2.0*x0*k + x1 + x1*ks - 2.0*x2*k) / d;
				a2 = (x0*ks - x1*k + x2) / d;
				b1 = (2.0*k + y1 + y1*ks - 2.0*y2*k) / d;
				b2 = (-ks - y1*k + y2) / d;
				if (_type == HP_TYPE) {
					a1 = -a1;
					b1 = -b1;
				}

				// printf("pole %d: rp=%f ip=%f a0=%f a1=%f a2=%f b1=%f b2=%f\n\n\n", p, rp, ip, a0, a1, a2, b1, b2);
				_biquads[p].setParams(a0, a1, a2, 1.0, -b1, -b2);
			}
		}
	}

	double MultipoleFilter::next(double sample) {
		for (int p = 0, pn = _poles / 2; p < pn; ++p) {
			sample = _biquads[p].next(sample);
		}
		return sample;
	}


}