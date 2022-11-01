#pragma once

#define _GNU_SOURCE 1
#define _USE_GNU 1

/* unlocking some standard math calls. */
#define __USE_ISOC99 1
#define __USE_ISOC9X 1
#define _ISOC99_SOURCE 1
#define _ISOC9X_SOURCE 1

#include <stdlib.h>
#include <string.h>

#include <math.h>
#include <float.h>

#include <assert.h>
#include <stdio.h>

#include <complex>


typedef __int8_t			int8;
typedef __uint8_t			uint8;
typedef __int16_t			int16;
typedef __uint16_t		uint16;
typedef __int32_t			int32;
typedef __uint32_t		uint32;
typedef __int64_t			int64;
typedef __uint64_t		uint64;

#define MIN_GAIN 1e-6 /* -120 dB */
/* smallest non-denormal 32 bit IEEE float is 1.18e-38 */
#define NOISE_FLOOR 1e-20 /* -400 dB */

/* //////////////////////////////////////////////////////////////////////// */

typedef float sample_t;
typedef unsigned int uint;
typedef unsigned long ulong;

/* prototype that takes a sample and yields a sample */
typedef sample_t (*clip_func_t) (sample_t);

#ifndef max
template <class X, class Y> X min (X x, Y y) { return x < (X)y ? x : (X)y; }
template <class X, class Y> X max (X x, Y y) { return x > (X)y ? x : (X)y; }
#endif /* ! max */

namespace capsdsp
{
	template <class T>
	T clamp (T value, T lower, T upper)
	{
		if (value < lower) return lower;
		if (value > upper) return upper;
		return value;
	}

	static inline float frandom() { return (float) random() / (float) RAND_MAX; }

	/* NB: also true if 0  */
	inline bool 
	is_denormal (float & f)
	{
		int32 i = *((int32 *) &f);
		return ((i & 0x7f800000) == 0);
	}

	/* not used, check validity before using */
	inline bool 
	is_denormal (double & f)
	{
		int64 i = *((int64 *) &f);
		return ((i & 0x7fe0000000000000ll) == 0);
	}

	/* lovely algorithm from 
	http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
	*/
	inline uint 
	next_power_of_2 (uint n)
	{
		assert (n <= 0x40000000);

		--n;
		n |= n >> 1;
		n |= n >> 2;
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;

		return ++n;
	}

	inline double db2lin (double db) { return pow(10, .05*db); }
	inline double lin2db (double lin) { return 20*log10(lin); }

	#if defined(__i386__) || defined(__amd64__)
		#define TRAP asm ("int $3;")
	#else
		#define TRAP
	#endif


	inline float pow2 (float x) { return x * x; }
	inline float pow3 (float x) { return x * pow2(x); }
	inline float pow4 (float x) { return pow2 (pow2(x)); }
	inline float pow5 (float x) { return x * pow4(x); }
	inline float pow6 (float x) { return pow3 (pow2(x)); }
	inline float pow7 (float x) { return x * (pow6 (x)); }
	inline float pow8 (float x) { return pow2 (pow4 (x)); }

	inline float 
	sgn (float x)
	{
		union { float f; uint32 i; } u;
		u.f = x;
		u.i &= 0x80000000;
		u.i |= 0x3F800000;
		return u.f;
	}

	inline bool
	isprime (int v)
	{
		if (v <= 3)
			return true;
		
		if (!(v & 1))
			return false;

		for (int i = 3; i < (int) std::sqrt (v) + 1; i += 2)
			if ((v % i) == 0)
				return false;

		return true;
	}


	class Sine
	{
		public:
			int z;
			double y[2];
			double b;

		public:
			Sine()
				{ 
					b = 0;
					y[0] = y[1] = 0;
					z = 0;
				}

			Sine (double f, double fs, double phase)
				{
					set_f (f, fs, phase);
				}

			Sine (double w, double phase = 0.)
				{
					set_f (w, phase);
				}

			inline void set_f (double f, double fs, double phase)
				{
					set_f (f*2*M_PI/fs, phase);
				}

			inline void set_f (double w, double phase)
				{
					b = 2 * std::cos(w);
					y[0] = std::sin (phase - w);
					y[1] = std::sin (phase - 2*w);
					z = 0;
				}

			/* advance and return 1 sample */
			inline double get()
				{
					double s = b*y[z]; 
					z ^= 1;
					s -= y[z];
					return y[z] = s;
				}

			double get_phase()
				{
					double x0 = y[z], x1 = b*y[z] - y[z^1];
					double phi = std::asin(x0);
					
					/* slope is falling: into the 2nd half. */
					return x1 < x0 ? M_PI - phi : phi;
				}
	};

	/* same as above but including a damping coefficient d */
	class DampedSine
	: public Sine
	{
		public:
			double d;

		public:
			DampedSine()
				{ d = 1; }

			inline double get()
				{
					double s = b * y[z]; 
					z ^= 1;
					s -= d * y[z];
					return y[z] = d * s;
				}
	};

	inline void
	sinc (double omega, sample_t * s, int n)
	{
		/* initial phase */
		double phi = (n / 2) * -omega;

		Sine sine (omega, phi);
		
		for (int i = 0; i < n; ++i, phi += omega)
		{
			double sin_phi = sine.get();

			if (std::fabs (phi) < 0.000000001)
				s[i] = 1.;
			else
				s[i] = sin_phi / phi;
		}
	}

	/* prototypes for window value application ... */
	typedef void (*window_sample_func_t) (sample_t &, sample_t);

	/* ... which go as template parameters for the window calculation below */
	inline void store_sample (sample_t &d, sample_t s) { d = s; }
	inline void apply_window (sample_t &d, sample_t s) { d *= s; }

	template <window_sample_func_t F>
	void
	hann (sample_t * s, int n, double step = 1)
	{
		step = M_PI*step/n;
		
		double f = 0;
		for (int i=0; i<n; ++i)
		{
			f = i*step;
			float x = std::sin(f);
			F(s[i], x*x);
		}
	}

	/* faster but less accurate version */
	template <window_sample_func_t F>
	void
	hann2 (sample_t *s, int n)
	{
		double phi = M_PI/(n-1);
		Sine sin(phi, 0);
		for (int i=0; i<n; ++i)
		{
			float x = sin.get();
			F(s[i], x*x);
		}
	}


	template <window_sample_func_t F>
	void
	hamming (sample_t * s, int n)
	{
		float in = 1. / n;
		
		for (int i = 0; i < n; ++i)
		{
			double f = i*in;
			F (s[i], .54 - .46* std::cos (2*M_PI*f));
		}
	}

	template <window_sample_func_t F>
	void
	blackman (sample_t *s, int n)
	{
		float in = 1. / n;

		for (int i = 0; i < n; ++i)
		{
			float f = (float) i;

			double b = .42f - 
							.5f* std::cos (2.f*f*M_PI*in) + 
							.08* std::cos (4.f*f*M_PI*in);

			F (s[i], b);
		}
	}

	template <window_sample_func_t F>
	void
	blackman_harris (sample_t *s, int n)
	{
		double w1 = 2.f*M_PI / (n - 1);
		double w2 = 2.f*w1;
		double w3 = 3.f*w1;

		for (int i = 0; i < n; ++i)
		{
			double f = (double) i;

			double bh = .35875f - 
					.48829f* std::cos (w1*f) + 
					.14128f* std::cos (w2*f) - 
					.01168f* std::cos (w3*f);

			F (s[i], bh);
		}
	}

	/* by way of dobson and csound */
	inline double 
	besseli (double x)
	{
		double a = std::fabs(x);
		if (a < 3.75)     
		{
			double y = x/3.75;
			y *= y;
			return 1. + y*(3.5156229 + y*(3.0899424 + y*(1.2067492 +
						y*(.2659732 + y*(.0360768 + y*.0045813)))));
		}
		double y = 3.75/a;
		return (std::exp(a)/std::sqrt(a)) * (.39894228 + y*(.01328592 + 
				y*(.00225319 + y*(-.00157565 + y*(.00916281 + y*(-.02057706 + 
				y*(.02635537 + y*(-.01647633 + y*.00392377))))))));
	}

	/* step = .5 : window [-n to 0] */
	template <window_sample_func_t F>
	void
	kaiser (sample_t *s, int n, double beta, double step = 1)
	{
		double bb = besseli(beta);
		int si = 0;

		for(double i = -n/2.+.5; si < n; ++si, i += step)
		{
			double a = 1 - std::pow((2*i / (n - 1)), 2);
			double k = besseli((beta*(a < 0 ? 0 : std::sqrt(a))) / bb);
			F(s[si], k);
		}
	}

	template <window_sample_func_t F>
	void
	xfade (sample_t *s, int n, int dir) /* dir [-1,1] */
	{
		Sine cos(.5*M_PI/n, dir>0 ? .5*M_PI : 0);
		for (int i=0; i<n; ++i)
		{
			float c = cos.get();
			F(s[i], c*c);
		}
	}	
}