// OscillatorF.h
//

#ifndef OSCILLATORF_H
#define OSCILLATORF_H

#include "AbstractOscillator.h"

#include <cmath>
#include <stdexcept>

#include <iostream>

namespace soundmath  {
	template <typename T>
	//! Digital oscillator with linear interpolation and fractional phase
	class OscillatorF : public AbstractOscillator<T> {
		typedef AbstractOscillator<T> Base;
	public:
		OscillatorF (T sr, T* tab, long len) : AbstractOscillator<T> (tab, len),
				m_phs (0) {
	
			if (sr > 2) m_sr = sr;
			else m_sr = 44100;
			Base::m_sicvt = (T) Base::m_tablen / m_sr;
	
			phase (0);
			Base::frequency (440);
			Base::amplitude (1);
		}
		void phase (T phi) {
			if (phi >= 0 && phi <= 1) m_phs = phi * Base::m_tablen;
		}
	
		// kfreq, kamp
		virtual void process (T* out, int len) {
			T incr = Base::m_sicvt * Base::m_freq;
	
			T* buff = out;
			while (len--) {
				m_phs += incr;
	
				while (m_phs >= Base::m_tablen) {
					m_phs -= Base::m_tablen;
				}
				while (m_phs < 0) {
					m_phs += Base::m_tablen;
				}
	
				long index = (long) m_phs;
				long next;
				if (index == Base::m_tablen)
					next = 0;
				else
					next = index + 1;
				T frac = m_phs - index;
				T lo = Base::m_tab[index];
				T hi = Base::m_tab[next];
	
				*buff++ = (lo + frac * (hi - lo)) * Base::m_amp;
			}
		}
		// afreq, kamp
		virtual void process (T* out, int len, T freq) {
			T* buff = out;
			T fincr = (freq - Base::m_freq) / len;
			while (len--) {
				T incr = Base::m_sicvt * Base::m_freq;
				m_phs += incr;
	
				while (m_phs >= Base::m_tablen) {
					m_phs -= Base::m_tablen;
				}
				while (m_phs < 0) {
					m_phs += Base::m_tablen;
				}
	
				long index = (long) m_phs;
				long next;
				if (index == Base::m_tablen)
					next = 0;
				else
					next = index + 1;
				T frac = m_phs - index;
				T lo = Base::m_tab[index];
				T hi = Base::m_tab[next];
	
				*buff++ = (lo + frac * (hi - lo)) * Base::m_amp;
				Base::m_freq += fincr;
			}
		}
		// afreq, aamp
		virtual void process (T* out, int len, T freq, T amp) {
			T* buff = out;
			T fincr = (freq - Base::m_freq) / len;
			T aincr = (amp - Base::m_amp) / len;
	
			while (len--) {
				T incr = Base::m_sicvt * Base::m_freq;
				m_phs += incr;
	
				while (m_phs >= Base::m_tablen) {
					m_phs -= Base::m_tablen;
				}
				while (m_phs < 0) {
					m_phs += Base::m_tablen;
				}
	
				long index = (long) m_phs;
				long next;
				if (index == Base::m_tablen)
					next = 0;
				else
					next = index + 1;
				T frac = m_phs - index;
				T lo = Base::m_tab[index];
				T hi = Base::m_tab[next];
	
				*buff++ = (lo + frac * (hi - lo)) * Base::m_amp;
				Base::m_freq += fincr;
				Base::m_amp += aincr;
			}
		}
	private:
		T m_sr;
		T m_phs;
	};
}

#endif	// OSCILLATORF_H

// EOF
