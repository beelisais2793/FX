// Envelope.h
//

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "OscillatorF.h"
#include "OscillatorI.h"
#include <cmath>

namespace soundmath {
	//! An envelope generator
	template <typename T>
	class Envelope : protected OscillatorI<T> {
	public:
		Envelope (T sr, T* table, int tablen) :
				OscillatorI<T> (sr, table, tablen) {
			OscillatorI<T>::m_freq = 1;
			release (); // untriggered by default
		}
		void process (T* out, int len) {
			T* o = out;
			int inc = lrint (OscillatorI<T>::m_freq * OscillatorI<T>::m_sicvt);
			while (len-- && (m_ophs < OscillatorI<T>::m_phs)) {
				T fract = ((T)((OscillatorI<T>::m_phs) & OscillatorI<T>::m_lomask)
						   * OscillatorI<T>::m_lodiv);
				int intPhi = (OscillatorI<T>::m_phs >> OscillatorI<T>::m_lobits);
				T* ftab = (T*)(OscillatorI<T>::m_tab + intPhi);
				T v1 = ftab[0];
				*o++= (v1 + (ftab[1] - v1) * fract);
	
				m_ophs = OscillatorI<T>::m_phs;
				OscillatorI<T>::m_phs = (OscillatorI<T>::m_phs + inc) & OscillatorI<T>::m_phmask;
			}
		}
	
		bool busy () const {
			return m_ophs < OscillatorI<T>::m_phs;
		}
	
		void length (T d) { // setting the length engages the envelope
			OscillatorI<T>::m_freq = 1. / d;
			OscillatorI<T>::m_phs = 0;
			m_ophs = -1;
		}
		//void engage () { OscillatorI<T>::m_phs = 0; m_ophs = -1; }
		void release () {
			OscillatorI<T>::m_phs = m_ophs = 0;
		}
	
		static void window (T* out, int N, T a0, T a1, T a2) {
			// .5, .5, 0     --> hanning
			// .54, .46, 0   --> hamming
			// .42, .5, 0.08 --> blackman
			const T TWOPI = 2. * M_PI;
			for (int i = 0; i < N; ++i) {
				out[i] = a0 - a1 * cos ((TWOPI * (T) i) / (N - 1)) + a2 *
						 cos ((2 * TWOPI * (T) i) / (N - 1)); // hamming, hann or blackman
			}
		}
	protected:
		int m_ophs;
	};
}


#endif	// ENVELOPE_H

// EOF
