#pragma once

#include "CapsDSP.hpp"
namespace capsdsp
{
	/* 
		Brute-force FIR filter with downsampling method (decimating). 
	*/
	class FIR
	{
		public:
			/* kernel length, history length - 1 */
			uint n, m;
			
			/* coefficients, history */
			sample_t * c, * x;

			/* history index */
			int h; 
			
			FIR() { c = x = 0; }
			~FIR() { free(c); free(x); }
			
			void init (uint N)
				{
					n = N;

					/* keeping history size a power of 2 makes it possible to wrap the
					* history pointer by & instead of %, saving a few cpu cycles. */
					m = next_power_of_2 (n);

					c = (sample_t *) malloc (n * sizeof (sample_t));
					x = (sample_t *) malloc (m * sizeof (sample_t));

					m -= 1;

					reset();
				}
		
			void reset()
				{
					h = 0;
					memset (x, 0, n * sizeof (sample_t));
				}
			
			inline sample_t process (sample_t s)
				{
					x[h] = s;
					
					s *= c[0];

					for (uint Z = 1, z = h - 1; Z < n; --z, ++Z)
						s += c[Z] * x[z & m];

					h = (h + 1) & m;

					return s;
				}

			/* Z is the time, in samples, since the last non-zero sample.
			* OVER is the oversampling factor. just here for documentation, use
			* a FIRUpsampler instead.
			*/
			template <uint Z, uint OVER>
			inline sample_t upsample (sample_t s)
				{
					x[h] = s;
					
					s = 0;

					/* for the interpolation, iterate over the history in z ^ -OVER
					* steps -- all the samples between are 0.
					*/
					for (uint j = Z, z = h - Z; j < n; --z, j += OVER)
						s += c[j] * x[z & m];

					h = (h + 1) & m;

					return s;
				}

			/* used in downsampling */
			inline void store (sample_t s)
				{
					x[h] = s;
					h = (h + 1) & m;
				}
	};

	/* FIR upsampler, optimised not to store the 0 samples */
	template <int N, int Oversample>
	class FIRUpsampler
	{
		public:
			uint m; /* history length - 1 */
			int h; /* history index */

			sample_t * c, * x; /* coefficients, history */

			FIRUpsampler()
				{
					c = x = 0;
					init();
				}

			~FIRUpsampler()
				{
					if (c) free (c);
					if (x) free (x);
				}
			
			void init()
				{
					/* FIR kernel length must be a multiple of the oversampling ratio */
					assert (N % Oversample == 0);

					/* like FIR, keep the history buffer a power of 2; additionally,
					* don't store the 0 samples inbetween. */
					m = next_power_of_2 ((N + Oversample - 1) / Oversample);

					c = (sample_t *) malloc (N * sizeof (sample_t));
					x = (sample_t *) malloc (m * sizeof (sample_t));

					m -= 1;

					reset();
				}
		
			void reset()
				{
					h = 0;
					memset (x, 0, (m + 1) * sizeof (sample_t));
				}
			
			/* upsample the given sample */
			inline sample_t upsample (sample_t s)
				{
					x[h] = s;
					
					s = 0;

					for (uint Z = 0, z = h; Z < N; --z, Z += Oversample)
						s += c[Z] * x[z & m];

					h = (h + 1) & m;

					return s;
				}

			/* upsample a zero sample (interleaving), Z being the time, in samples,
			* since the last non-0 sample. */
			inline sample_t pad (uint Z)
				{
					sample_t s = 0;

					for (uint z = h-1; Z < N; --z, Z += Oversample)
						s += c[Z] * x[z & m];

					return s;
				}

	};
}