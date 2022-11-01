// OscillatorI.h
//

#ifndef OSCILLATORI_H
#define OSCILLATORI_H

#include "AbstractOscillator.h"
#include <iostream>
#include <stdexcept>
#include <cmath>

namespace soundmath {
	template <typename T>
	//! Digital oscillator with linear interpolation and integer phase
	class OscillatorI : public AbstractOscillator<T> {
	private:
		OscillatorI& operator= (OscillatorI&);
		OscillatorI (const OscillatorI&);
	public:
		typedef AbstractOscillator<T> Base;
		
		OscillatorI (T sr, T* tab, int tablen, int lobits = 12) : AbstractOscillator<T> (tab, tablen) {
			if ((((~Base::m_tablen + 1) & Base::m_tablen) != Base::m_tablen) || Base::m_tablen < 4) {
				throw std::runtime_error ("table size is not a power of 2");
			}
			m_tabbits = (int) log2 (Base::m_tablen);
	
			m_lobits = lobits;
			if (m_tabbits + lobits > 32) {
				throw std::runtime_error ("invalid number of bits");
			}
			m_lomask = (0x1 << m_lobits) - 1;
			m_lodiv = 1. / (0x1 << m_lobits);
			m_phmask = (0x1 << (m_lobits + m_tabbits)) - 1;
			Base::m_sicvt = (T) (m_phmask + 1) / sr;
	
			phase (0);
			Base::frequency (440);
			Base::amplitude (1);
		}
		virtual ~OscillatorI () {
		}
		void phase (int ph) {
			m_phs = ph;
			m_phs &= m_phmask;
		}
	
		// kfreq, kamp
		virtual void process (T* out, int len) {
			int inc = lrint (Base::m_freq * Base::m_sicvt);
	
			T* o = out;
			while (len--) {
				T fract = ((T)((m_phs) & m_lomask) * m_lodiv);
				int intPhi = (m_phs >> m_lobits);
				T* ftab = (T*)(Base::m_tab + intPhi);
				T v1 = ftab[0];
				*o++= (v1 + (ftab[1] - v1) * fract) * Base::m_amp;
	
				m_phs = (m_phs + inc) & m_phmask;
			}
		}
	
		// afreq, kamp
		virtual void process (T* out, int len, T freq) {
			T* o = out;
			T fincr = (freq - Base::m_freq) / len;
			while (len--) {
				int inc = lrint (Base::m_freq * Base::m_sicvt);
				T fract = ((T)((m_phs) & m_lomask) * m_lodiv);
				int intPhi = (m_phs >> m_lobits);
				T* ftab = (T*)(Base::m_tab + intPhi);
				T v1 = ftab[0];
				*o++= (v1 + (ftab[1] - v1) * fract) * Base::m_amp;
	
				m_phs = (m_phs + inc) & m_phmask;
				Base::m_freq += fincr;
			}
		}
	
		// afreq, aamp
		virtual void process (T* out, int len, T freq, T amp) {
			T* o = out;
			T fincr = (freq - Base::m_freq) / len;
			T aincr = (amp - Base::m_amp) / len;
			while (len--) {
				int inc = lrint (Base::m_freq * Base::m_sicvt);
				T fract = ((T)((m_phs) & m_lomask) * m_lodiv);
				int intPhi = (m_phs >> m_lobits);
				T* ftab = (T*)(Base::m_tab + intPhi);
				T v1 = ftab[0];
				*o++= (v1 + (ftab[1] - v1) * fract) * Base::m_amp;
	
				m_phs = (m_phs + inc) & m_phmask;
				Base::m_freq += fincr;
				Base::m_amp += aincr;
			}
		}
	protected:
		int m_tabbits;
		int m_lobits;
		int m_lomask;
		T m_lodiv;
		int m_phmask;
		int m_phs;
	
	};
}

#endif	// OSCILLATORi_H

// EOF
