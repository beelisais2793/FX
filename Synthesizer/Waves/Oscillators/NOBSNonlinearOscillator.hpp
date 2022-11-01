#pragma once
#include <cmath>
#include <complex>
#include <vector>

namespace NOBS{
constexpr auto pi  = 3.14159265359;
template<typename nobs_float> class bank {
private:
	const int SCALE_MAX = 8192;
	nobs_float samplerate, framerate, f0, bands_per_octave, octaves, ent;
	vector<nobs_float> eps;
	// class storing the data for a complex sinusoidal oscillator: 
	// phase*=frequency at each sample
	// output = real(phase)*amp
	class oscillator{
	public:
		nobs_float amp, target_amp, rfreq;
		complex<nobs_float> freq, phase, temp;
		oscillator(){
			amp = target_amp = 0.;
			phase = temp = polar(nobs_float(1), nobs_float(0));
			setFrequency(0);
		}
		//hang on to the real frequency (in radians/sample)
		//and set the complex frequency increment
		void setFrequency(nobs_float f){
			freq = polar(nobs_float(1), f);
			rfreq = f;
		}
	};
	vector<oscillator> oscs;
	//store the last complex sample at each scale, needed for interpolation b/t ticks
	vector< complex<nobs_float> > prev_vals;

	//from silicon graphics via https://en.wikipedia.org/wiki/Fast_inverse_square_root
	inline nobs_float invSqrt( nobs_float x )
	{
		const float x2 = x * 0.5;
		float y  = x;
		long i  = * ( long * ) &y;                   
		i = 0x5f375a86 - ( i >> 1 );
		//i  = 0x5f3759df - ( i >> 1 );
		y  = * ( float * ) &i;
		y  = y * ( float(1.5) - ( x2 * y * y ) ); 
	//	y  = y * ( float(1.5) - ( x2 * y * y ) ); 
		return y;
	}
	inline nobs_float fastSqrt(nobs_float x){
		return x*invSqrt(x);
	}
	//wrap small x to -pi, pi
	inline nobs_float wrap(nobs_float x){
		while(x > pi) x -= 2*pi;
	    while(x < -pi) x += 2*pi;
	    return x;
	}
	//from http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
	inline nobs_float fastSinNoWrap(nobs_float x){
		const nobs_float B = 4/pi;
		const nobs_float C = -4/(pi*pi);

		nobs_float y = B * x + C * x * abs(x);

		const nobs_float P = 0.225;

		return P * (y * abs(y) - y) + y;   // Q * y + P * y * abs(y)
	}
	inline nobs_float fastSin(nobs_float x){
		return fastSinNoWrap(wrap(x));
	}
	/*inline nobs_float fastArg(complex<nobs_float> c){
		return arg(c);//fastAtan2(imag(c), real(c));
	}
	inline nobs_float fastAbs(complex<nobs_float> c){
		return fastSqrt(real(c)*real(c)+imag(c)*imag(c));
	}*/
	inline complex<nobs_float> fastNormalize(complex<nobs_float> c){
		return c*invSqrt(real(c)*real(c)+imag(c)*imag(c));
	}
	/*inline complex<nobs_float> fastPolar(nobs_float theta){
		return complex<nobs_float>(fastSin(theta+(.5*pi)), fastSin(theta));
	}
	inline complex<nobs_float> fastPolarWithWrap(nobs_float& theta){
		theta = wrap(theta);
		const nobs_float theta2 = wrap(theta+(.5*pi));
		return complex<nobs_float>(fastSinNoWrap(theta2), fastSinNoWrap(theta));
	}*/
	//local spectral envelope at b
	inline nobs_float localEnv(nobs_float a, nobs_float b, nobs_float c){
		return .25*(a+c)+.5*b;
	    /*//if b is smallest, return b
	    if(b<=c && b<=a) return b;
	    //else return clamped sum of larger two
	    if(a<c) return min(nobs_float(1), b+c);
	    return min(nobs_float(1), a+b);
	    */
	}
	inline nobs_float mix(nobs_float a, nobs_float b, nobs_float m){
		return (b-a)*m + a;
	}
	inline complex<nobs_float> mix(complex<nobs_float> a, complex<nobs_float> b, nobs_float m){
		return (b-a)*m + a;
	}
	//class managing a vector of signals at different scales
	template<typename T> class multiscaleVector{
	private:
		vector<T> storage;
		inline int convert(int scale_idx, int samp_idx){
			if((samp_idx<<scale_idx) >= n || scale_idx>=nscales){
				cout << "warning: overflow in multiscaleVector of size " << n
					<< " at scale " << scale_idx << ", sample " << samp_idx << endl;
				return 2*n;
			}
			return ((((1<<scale_idx)-1)*n)>>(scale_idx-1))+samp_idx;
		}
	public:
		int n, nscales;
		multiscaleVector(int n){
			this->n = n;
			storage = vector<T>(2*n-1, 0);
	 		nscales = log2(n)+1;
	 		//cout << "multiscaleVector with n = " << n << ", nscales = "<<nscales<<endl;
		}
		//scales are indexed from finest (all n samples) to coarsest (1 sample)
		inline void accum(int scale_idx, int samp_idx, T value){
			storage[convert(scale_idx, samp_idx)] += value;
		}
		inline T get(int scale_idx, int samp_idx){
			int idx = convert(scale_idx, samp_idx);
			//cout << "mapped ("<<scale_idx<<", "<<samp_idx<<") to "<<idx<<endl;
			return storage[idx];
		}
	};
	//class to manage multiscale evaluation in tick()
	class scaleManager{
	public:
		int max_scale, scale, scale_idx;
		nobs_float nyquist;
		inline void reset(){
			scale = 1;
			nyquist = pi; //in radians per sample
			scale_idx = 0;
		}
		scaleManager(int max_scale){
			this->max_scale = max_scale;
			reset();
		}
		inline bool updateAndTest(int samp, nobs_float freq){
			if(scale < max_scale && freq < nyquist*.5){
        		scale = scale<<1;
        		nyquist *= .5;
        		scale_idx++;
        	}
        	return (samp & (scale-1));/* scale not computed this sample */
		}
	};
public:
	bank(nobs_float samplerate, nobs_float f0=30, nobs_float bands_per_octave=36, nobs_float octaves=9){
		int nbands = int(octaves*bands_per_octave);
		oscs = vector<oscillator>(nbands);

		this->bands_per_octave = bands_per_octave;
		this->octaves = octaves;
		this->f0 = f0;
		setSampleRate(samplerate);
		setFrameRate(30);

		//set up constants for amplitude smoothing filter at different scales
		//also populate prev_vals
		//todo: a better low pass filter
		for(int scale=1; scale<=SCALE_MAX; scale*=2){
			prev_vals.push_back(complex<nobs_float>());
		}
	}
	//get the next /samps/ samples
	vector<nobs_float> tick(int samps){
		const nobs_float inv_nbands = 1/sqrt(oscs.size());
    	const int nbands = oscs.size();
    	//amplitude floor
        const nobs_float delta = 1e-12;

		multiscaleVector< complex<nobs_float> > acc(samps);
		scaleManager sm(samps);

		for(int samp = 0; samp<samps; samp++){
			//only compute every /scale/ samples
			//since oscillators are ordered high to low freq, we can increase scale only when we see
			// a frequency below half nyquist for the current scale to know the appropriate scale for each osc
			// then if the scale has increased past the lowest which should be computed this sample, break

        	//first pass: accumulate signal;
        	//update amplitudes to approach control-rate target amplitude
        	//and rotate phase according to nominal frequency
        	sm.reset();
        	for(int idx = nbands-1; idx>=0; idx--){
            	oscillator &osc = oscs[idx];
            	if(sm.updateAndTest(samp, osc.rfreq))
            		break;

            	//avoid denormals
	            if(osc.amp<delta && osc.target_amp<delta){
	            	osc.amp = osc.target_amp=0;
	                continue;
	            }

            	osc.phase = osc.temp;
                acc.accum(sm.scale_idx, samp>>(sm.scale_idx), osc.amp*osc.phase);

            	osc.amp = mix(osc.amp, osc.target_amp, eps[sm.scale_idx]);

            	//need to multiply frequency up to scale
            	complex<nobs_float> freq = osc.freq;
            	for(int s=0; s<sm.scale_idx; s++)
            		freq*=freq;
            	osc.phase *= freq;
            	osc.temp = osc.phase;
        	}
        	if(ent<=0) continue;
    	   	//second pass: entrain oscillators by averaging their complex phase
        	//and projecting back to unit circle
        	sm.reset();
        	for(int idx = nbands-1; idx>=0; idx--){
	            oscillator &osc = oscs[idx];
	            if(sm.updateAndTest(samp, osc.rfreq))
            		break;
	            //faster to store these outside of loop and only access oscs once per iteration?
	            oscillator &osc_below = oscs[max(0,idx-1)];
	            oscillator &osc_above = oscs[min(nbands-1,idx+1)];

	            complex<nobs_float> phase = osc.phase;
	            nobs_float w_c = osc.amp;
	            nobs_float w_b = osc_below.amp;
	            nobs_float w_a = osc_above.amp;
	            nobs_float sigma = w_c+w_a+w_b;
	            if(sigma>delta){
	                phase = ( w_b*osc_below.phase
	                        + w_a*osc_above.phase
	                        + w_c*osc.phase
	                        ) / sigma;
	                phase = mix(osc.phase, phase, ent);
	                phase = fastNormalize(phase);
				}
				osc.temp = phase;
			}
		}

		//cout << "max scale idx reached = " << sm.max_reached << endl;

		//now interpolate within each scale and sum
		vector<nobs_float> ret = vector<nobs_float>(samps);
		for(int scale_idx = 0; scale_idx<acc.nscales; scale_idx++){
			complex<nobs_float> cur, prev = prev_vals[scale_idx];
			nobs_float prev_abs = abs(prev);
			nobs_float prev_arg = arg(prev);
			nobs_float scale = (1<<scale_idx);
			for(int samp_idx = 0; samp_idx<(samps>>scale_idx); samp_idx++){
				//note: can probably optimize by rolling own atan/arg approximation
				//also it's just ugly to involve pi here and note above
				cur = acc.get(scale_idx, samp_idx);
				nobs_float cur_abs = abs(cur);
				nobs_float cur_arg = arg(cur);
				for(int interp_idx = 0; interp_idx<scale; interp_idx++){
					nobs_float m = (interp_idx)/scale;
					if(prev_arg > cur_arg) prev_arg -= (2*pi);
					nobs_float interp_abs = mix(prev_abs, cur_abs, m);
					nobs_float interp_arg = mix(prev_arg, cur_arg, m);
					ret[(samp_idx<<scale_idx)+interp_idx] +=
						.25*inv_nbands*interp_abs*sin(interp_arg);
				}
				prev_abs = cur_abs;
				prev_arg = cur_arg;
			}
			prev_vals[scale_idx] = cur;
		}

		//for(int samp = 0; samp<samps; samp++)
		//	ret[samp] = .25*inv_nbands*imag(acc.get(0, samp));

		return ret;
	}

	nobs_float getSampleRate(){
		return samplerate;
	}
	nobs_float getBandsPerOctave(){
		return bands_per_octave;
	}
	int getNumBands(){
		return oscs.size();
	}
	nobs_float getFreqByIdx(int idx){
		return oscs[idx].rfreq;
	}
	void setAmplitude(int idx, nobs_float value){
		//if (idx < 0 || idx > nbands-1) return;
		oscs[idx].target_amp = abs(value);
	}
	void setEntrainment(nobs_float e){
		ent = max(nobs_float(0),e);
	}
	void setSampleRate(nobs_float sr){
		samplerate = sr;
		for(int i=0; i<oscs.size(); i++){
	        nobs_float freq = f0*pow(2., i/bands_per_octave)/samplerate*2*pi; //in radians/sample
	        oscs[i].setFrequency(freq);
    	}
    }
	void setFrameRate(nobs_float fr){
		framerate = fr;
		this->eps.clear();
		for(int scale=1; scale<=SCALE_MAX; scale*=2){
			this->eps.push_back(1-pow(.05, scale*framerate/samplerate));
		}
	}
};
}