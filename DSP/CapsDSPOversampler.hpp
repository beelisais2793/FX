#pragma once

#include "CapsDSP.hpp"
namespace capsdsp
{
/* do-nothing 1:1 oversampler substitute, occasionally needed as a 
	* template parameter */
	class NoOversampler 
	{
		public:
			enum { Ratio = 1 };
			sample_t downsample (sample_t x) { return x; }
			sample_t upsample (sample_t x) { return x; }
			void downstore (sample_t) { }
			sample_t uppad (uint) { return 0; }
	};

	template <int Oversample, int FIRSize>
	class Oversampler
	{
		public:
			enum { Ratio = Oversample };
			/* antialias filters */
			struct {
				FIRUpsampler<FIRSize, Oversample> up;
				FIRn<FIRSize> down;
			} fir;

			Oversampler()
				{ init(); }

			void init (float fc = .5) 
				{
					double f = fc * M_PI / Oversample;
					
					/* construct the upsampler filter kernel */
					sinc (f, fir.up.c, FIRSize);
					kaiser<apply_window> (fir.up.c, FIRSize, 6.4);

					/* copy upsampler filter kernel for downsampler, make sum */
					double s = 0;
					for (uint i = 0; i < FIRSize; ++i)
						fir.down.c[i] = fir.up.c[i],
						s += fir.up.c[i];
					
					/* scale downsampler kernel for unity gain */
					s = 1/s;
					for (uint i = 0; i < FIRSize; ++i)
						fir.down.c[i] *= s;

					/* scale upsampler kernel for unity gain */
					s *= Oversample;
					for (uint i = 0; i < FIRSize; ++i)
						fir.up.c[i] *= s;
				}

			void reset() 
				{
					fir.up.reset();
					fir.down.reset();
				}

			inline sample_t upsample(sample_t x)
				{ return fir.up.upsample(x); }
			inline sample_t uppad(uint z)
				{ return fir.up.pad(z); }

			inline sample_t downsample(sample_t x)
				{ return fir.down.process(x); }
			inline void downstore(sample_t x)
				{ fir.down.store(x); }
	};
}