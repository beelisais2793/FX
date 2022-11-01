#pragma once

#include "CapsDSP.hpp"
namespace capsdsp
{
	/* templating for kernel length allows g++ to optimise aggressively
	* resulting in appreciable performance gains. */
	template <int N>
	class FIRn
	{
		public:
			/* history length - 1 */
			uint m;
			
			/* coefficients, history */
			sample_t c[N], x[N];

			/* history index */
			int h; 
			
			FIRn()
				{
					/* keeping history size a power of 2 makes it possible to wrap the
					* history pointer by & instead of %, saving a few cpu cycles. */
					m = next_power_of_2 (N) - 1;

					reset();
				}
		
			void reset()
				{
					h = 0;
					memset (x, 0, N * sizeof (sample_t));
				}
			
			inline sample_t process (sample_t s)
				{
					x[h] = s;
					
					s *= c[0];

					for (uint Z = 1, z = h - 1; Z < N; --z, ++Z)
						s += c[Z] * x[z & m];

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

