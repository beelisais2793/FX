// Phasor.h
// 

#ifndef PHASOR_H
#define PHASOR_H 

namespace soundmath {
	//! A simple phasor; sample-by-sample version
	template <typename T>
	class Phasor {
	private:
		Phasor& operator= (Phasor&);
		Phasor (const Phasor&);
	public: 
		Phasor (T sr) : m_phs (0) {
			m_sicvt = (T) 1. / sr;
			m_freq = 10.;
		}
		virtual ~Phasor () {
		}
		
		void frequency (T freq) {
			m_freq = freq;
		}
		void phase (T phi) {
			if (phi >= 0 && phi <= 1) m_phs = phi;
		}
		T process (int buffer = 1) {		
			T incr = m_sicvt * m_freq * buffer;
			m_phs += incr;
			if (m_phs >= 1) m_phs -= 1.;
	
			return m_phs;
		}
	private:
		T m_phs;
		T m_sicvt;
		T m_freq;
	};
}

#endif	// PHASOR_H 

// EOF
