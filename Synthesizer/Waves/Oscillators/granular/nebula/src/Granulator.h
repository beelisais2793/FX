// Granulator.h
//

#ifndef GRANULATOR_H
#define GRANULATOR_H

#include "Envelope.h"
#include "Phasor.h"

#include <ctime>
#include <cstdlib>

// NOTE: - granulazione su campioni: speed, selezione parte tab.
//		 - attack/decay su finestra
//		 - glissato in maniera diversa rispetto a iDensity
//		 - randomicita non congruente con iDensity
//		 - lunghezza libera per gli inviluppi
//

namespace soundmath {
	//! A granular-synthesizer for protypes or soundfiles
	template <typename T, template <typename X> class OscillatorT>
	class Granulator {
	private:
		Granulator& operator= (Granulator&);
		Granulator (const Granulator&);
	public:
		Granulator (T sr, T* p1, T* p2, T* win, int tablen1, int tablen2,
					int winlen, int poly = 1024) {
			m_sr = sr;
			m_sr2 = m_sr / 2;
			m_poly = poly;
			m_lpoly = m_poly;
	
			m_p1 = p1;
			m_p2 = p2;
			m_tablen = tablen1;
			m_tratio = (T) tablen2 / m_tablen;
			m_fn1 = m_sr / m_tablen;
			m_fn2 = m_sr / tablen2;
	
			m_relative = false;
	
			//m_tab = new T[m_tablen];
			
			m_left = new T[m_poly];
			m_right = new T[m_poly];
			
			m_morphings = new T[m_poly];
			m_umorphings = new T[m_poly];
			
			m_gliss1 = new T[m_poly];
			m_gincr1 = new T[m_poly];
			m_gliss2 = new T[m_poly];
			m_gincr2 = new T[m_poly];
	
			m_delays = new int[m_poly];
			
			m_osc1 = new OscillatorT<T>*[m_poly];
			m_osc2 = new OscillatorT<T>*[m_poly];
			m_env = new Envelope<T>*[m_poly];
			for (int i = 0; i < m_poly; ++i) {
				m_osc1[i] = new OscillatorT<T> (m_sr, m_p1, m_tablen);
				m_osc2[i] = new OscillatorT<T> (m_sr, m_p2, tablen2);
				m_env[i] = new Envelope<T> (m_sr, win, winlen);
			}
			
			m_phasor1 = new Phasor<T> (m_sr);
			m_phasor2 = new Phasor<T> (m_sr);
	
			m_seed = time (NULL);
			density (10, 0);
			length (1. / 10, 0);
			amplitude (.1);
			frequency (440, 0);
			jitter (.5, 1);
			morphing (0);
			grainlets (0);
			speed (1);
	
			m_time = 0;
		}
		virtual ~Granulator () {
			for (int i = 0; i < m_poly; ++i) {
				delete m_osc1[i];
				delete m_osc2[i];
				delete m_env[i];
			}
	
			delete [] m_osc1;
			delete [] m_osc2;
			delete [] m_env;
	
			delete [] m_delays;
			delete [] m_left;
			delete [] m_right;
			delete [] m_gliss1;
			delete [] m_gincr1;
			delete [] m_gliss2;
			delete [] m_gincr2;
			
			delete m_phasor1;
			delete m_phasor2;
	
			//delete [] m_tab;
		}
		void schedule (int delay, T stepPhi1, T stepPhi2) {
			for (int i = 0; i < m_lpoly; ++i) {
				if (!m_env[i]->busy ()) {
					m_delays[i] = delay;
					m_osc1[i]->phase (stepPhi1);
					m_osc2[i]->phase (stepPhi2);
	
					T f1 = (rnd () * (m_freqMax - m_freqMin)) + m_freqMin;
					T f2 = f1;
					if (m_relative) {
						f1 *= m_fn1;
						f2 *= m_fn2;
					}
	
					m_osc1[i]->frequency (f1);
					m_osc2[i]->frequency (f2);
	
	
					// NB: only f1 used for grainlet rescaling
					T l = 0;
					if (m_gletScaling) {
						l = f1 ? (m_sr2 / f1 * m_gletScaling) : 0;
						m_env[i]->length (l);
					} else {
						l = (rnd () * (m_lengthMax - m_lengthMin)) + m_lengthMin;
						m_env[i]->length (l);
					}
	
					int bf = (int) (l * m_sr);
					T p1 = f1 * m_glissato;
					T min1 = f1 - p1;
					T max1 = f1 + p1;
					m_gliss1[i] = f1;
					m_gincr1[i] = (((rnd () * (max1 - min1)) + min1) - f1) / bf;
	
					T p2 = f2 * m_glissato;
					T min2 = f2 - p2;
					T max2 = f2 + p2;
					m_gliss2[i] = f2;
					m_gincr2[i] = (((rnd () * (max2 - min2)) + min2) - f2) / bf;
	
					T pan = (rnd () * (m_jitterMax - m_jitterMin)) + m_jitterMin;
					m_left[i] = pan; //sin (p);
					m_right[i] = 1. - pan; //cos (p);
					m_morphings[i] = 1. - m_morph;
					m_umorphings[i] = m_morph;
					m_olap = i;
					break;
				}
			}
	
			//for (int k = 0; k < m_tablen; ++k) {
			//    m_tab[k] = (1. - m_morph) * m_p1[k] + m_morph * m_p2[(int) (k * m_tratio)];
			//}
	
			m_time = 0;
		}
		int olap () const {
			return m_olap;
		}
		void poly (int p) {
			if (p > 0 && p <= m_poly) m_lpoly = p;
		}
	
		void density (T d, T drnd) {
			T p = d * drnd;
	
			m_densMin = (int) (((d - p) == 0 ? 0 : (T) 1. / (d - p)) * m_sr);
			m_densMax = (int) (((T) 1. / (d + p)) * m_sr);
	
			m_density = (int) (rnd () * (m_densMax - m_densMin)) + m_densMin;
		}
		void length (T l, T lrnd) {
			T p = l * lrnd;
			m_lengthMin = l - p;
			m_lengthMax = l + p;
		}
		void frequency (T f, T frnd) {
			T p = f * frnd;
	
			m_freqMin = (f - p);
			m_freqMax = (f + p);
		}
		void speed (T s) {
			m_phasor1->frequency (s * m_fn1);
			m_phasor2->frequency (s * m_fn2);
		}
		void relativePitch (bool r) {
			m_relative = r;
		}
		void jitter (T j, T jrnd) {
			T p = j * jrnd;
			m_jitterMin = j - p;
			m_jitterMax = j + p;
		}
		void amplitude (T a) {
			for (int i = 0; i < m_lpoly; ++i) {
				m_osc1[i]->amplitude (a);
				m_osc2[i]->amplitude (a);
			}
		}
		void morphing (T m) {
			m_morph = m;
		}
		void glissato (T g) {
			m_glissato = g;
		}
		void grainlets (T scale) {
			m_gletScaling = scale;
		}
		void process (T* outL, T* outR, T* buffer1, T* buffer2, T* buffer3, int frames) {
			for (int j = 0; j < frames; ++j) {
				++m_time;
	
				T stepPhi1 = m_phasor1->process ();
				T stepPhi2 = m_phasor2->process ();
	
				if (m_time >= m_density) {
					schedule (j, stepPhi1, stepPhi2);
					m_density = (int) (rnd () * (m_densMax - m_densMin)) + m_densMin;
				}
			}
			T interp = 0;
			for (int i = 0; i < m_lpoly; ++i) {
				if (m_env[i]->busy ()) {
					memset (buffer1, 0, sizeof (float) * frames);
					m_env[i]->process (buffer1, frames - m_delays[i]);
					m_osc1[i]->process (buffer2, frames - m_delays[i], m_gliss1[i]);
					m_osc2[i]->process (buffer3, frames - m_delays[i], m_gliss2[i]);
	
					for (int j = 0; j < frames - m_delays[i]; ++j) {
						interp = buffer2[j] * m_morphings[i] + buffer3[j] * m_umorphings[i];
	
						outL[j + m_delays[i]] += (buffer1[j] * interp * m_left[i]);
						outR[j + m_delays[i]] += (buffer1[j] * interp * m_right[i]);
					}
					m_gliss1[i] += m_gincr1[i] * frames;
					m_gliss2[i] += m_gincr2[i] * frames;
					m_delays[i] = 0;
				}
			}
		}
		T rnd () {
			m_seed *= 16807;
			return (T) (m_seed * 4.6566129e-010f + 1) * .5;
			//return (T) rand () / RAND_MAX;
		}
	protected:
		T m_sr;
		T m_sr2;
	
		int m_poly;
		int m_lpoly; // limited poly
		int m_time;
		int m_density, m_densMin, m_densMax;
		int m_olap;
		int m_seed;
	
		T m_jitter, m_jitterMin, m_jitterMax;
		T m_length, m_lengthMin, m_lengthMax;
		T m_freq, m_freqMin, m_freqMax;
	
		T* m_left;
		T* m_right;
		T* m_gliss1;
		T* m_gincr1;
		T* m_gliss2;
		T* m_gincr2;
	
		int* m_delays;
	
		int m_tablen;
		T m_tratio;
		T m_fn1;
		T m_fn2;
		//T* m_tab;
		T* m_p1;
		T* m_p2;
		T m_morph;
		T* m_morphings;
		T* m_umorphings;
		T m_glissato;
		bool m_relative;
	
		T m_gletScaling;
	
		OscillatorT<T>** m_osc1;
		OscillatorT<T>** m_osc2;
		Envelope<T>** m_env;
		Phasor<T>* m_phasor1;
		Phasor<T>* m_phasor2;
	};
}

#endif	// GRANULATOR_H

// EOF
