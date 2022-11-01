// AbstractOscillator.h
// 

#ifndef ABSTRACTOSCILLATOR_H
#define ABSTRACTOSCILLATOR_H 

#include <stdexcept>
#include <cmath>

namespace soundmath {
	//! Base class for all oscillators
	template <typename T>
	class AbstractOscillator {
	private:
		AbstractOscillator& operator= (AbstractOscillator&);
		AbstractOscillator (const AbstractOscillator&);
	public: 
		AbstractOscillator (T* tab, int tablen) :  m_freq (440), m_amp (1) {
			if (tablen > 4) m_tablen = tablen;
			else throw std::runtime_error ("invalid table size");
	
			m_tab = tab;
		}
		virtual ~AbstractOscillator () {
	
		}
		void frequency (T f) {
			m_freq = f;
		}
		void amplitude (T a) {
			m_amp = a;
		}
		
		// kfreq, kamp: this is the minimum ;)
		virtual void process (T* out, int len) = 0;
		
		static void gen (T* tab, long len, T* amplitudes, int harmonics, T phase) {
			phase *= 2. * M_PI;
			for (int i = 0; i < harmonics; i++) {
				for (int n = 0; n < len; n++) {
					T a = amplitudes ? amplitudes[i] : 1.f;
					T w = (T) (i + 1) * ((T) n * 2. * M_PI / len);
					tab[n] += (T) (a * cos ((T) w + (T) phase));
				}
			}
			
			normalizeTable (tab, len);
		}
		
		static void normalizeTable (T* tab, int len) {
			T max = tab[0];
			for (int n = 0; n < len; n++) {
				max = tab[n] >= max ? tab[n] : max;
			}
			
			if (max) {
				for (int n = 0; n < len; n++) {
					tab[n] /= max;
				}
			}
		}
		
		static void saw (T* tab, int len, int harmonics) {
			T* amps = new T[harmonics];
			for (int i = 0; i < harmonics; i++) {
				amps[i] = 1. / (i + 1);
			}
			gen (tab, len, amps, harmonics, -.25);
			delete [] amps;
		}
		static void square (T* tab, int len, int harmonics) {
			T* amps = new T[harmonics];
			memset (amps, 0, sizeof (T) * harmonics);
			for (int i = 0; i < harmonics; i+=2) {
				amps[i] = 1. / (i + 1);
			}
			gen (tab, len, amps, harmonics, -.25);
			delete [] amps;
		}
		static void triangle (T* tab, int len, int harmonics) {
			T* amps = new T[harmonics];
			memset (amps, 0, sizeof (T) * harmonics);
			for (int i = 0; i < harmonics; i+=2) {
				amps[i] = 1. / ((i + 1) * (i + 1));
			}
			gen (tab, len, amps, harmonics, -.25);
			delete [] amps;
		}
		
	protected:
		T* m_tab;
		T m_freq;
		T m_amp;
		int m_tablen;
		T m_sicvt;	
		
	};
}

#endif	// ABSTRACTOSCILLATOR_H 

// EOF
