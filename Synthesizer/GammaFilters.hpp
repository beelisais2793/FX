#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"

namespace Filters
{

	/// Filter types
	enum FilterType{
		LOW_PASS,			/**< Low-pass */
		HIGH_PASS,			/**< High-pass */
		BAND_PASS,			/**< Band-pass */
		RESONANT,			/**< Resonant band-pass */
		BAND_REJECT,		/**< Band-reject */
		ALL_PASS,			/**< All-pass */
		PEAKING,			/**< Peaking */
		LOW_SHELF,			/**< Low-shelf */
		HIGH_SHELF,			/**< High-shelf */
		SMOOTHING,			/**< Smoothing */
		THRU_PASS			/**< Thru-pass (no filter) */
	};

	using AllPass1 = gam::AllPass1<float,float>;
	using Biquad   = gam::Biquad<float,float>;
	using BlockDC  = gam::BlockDC<float,float>;
	using BlockNyquist  = gam::BlockNyq<float,float>;
	using AllPass2 = gam::AllPass2<float,float>;
	using Notch = gam::Notch<float,float>;
	using Reson = gam::Reson<float,float>;
	using Hilbert = gam::Hilbert<float,float>;
	using Integrator = gam::Integrator<float,float>;
	using Differencer = gam::Differencer<float>;
	using MovingAvg = gam::MovingAvg<float>;
	using OnePole   = gam::OnePole<float,float>;

	using Biquad3 = gam::Biquad3;

	using Vowel = gam::Vowel;

}