#pragma once 

#include <random>
#include <chrono>
#include <queue>
#include "Std/StdObject.h"
#include "Std/StdRandom.h"

namespace SoundAlchemy
{
    template<typename T>
    class TNoiseFilter 
    {
    private:
        // DC blocker variables
        T R;
        T y = 0, xm1 = 0, ym1 = 0;
        
        // smoothing filter variables
        // N - smooth length
        // i - counter for initialization
        int N, i = 0;
        // opS - output of smoothing filter
        // sub - subsitution : x[n] - x[n - N]
        // acc - accumulator
        T sub, acc;
        // mem - memory for x[i - N] values
        std::queue<T> mem;
        
        // use to clear memory queue on changes to N
        void clear( std::queue<T> &q )
        {
            std::queue<T> empty;
            std::swap( q, empty );
        }
        
    public:
        // constructor
        TNoiseFilter(T R_in = 0.99, int N_in = 4) : R(R_in), N(N_in) {}
        
        // DC blocking filter
        // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
        T dc_blocking_filter (T ip) {
            y = ip - xm1 + R * ym1;
            xm1 = ip;
            ym1 = y;
            return y;
        }
        
        // moving average/smoothing filter
        // https://zipcpu.com/dsp/2017/10/16/boxcar.html
        T smoothing_filter (T ip) {
            // init
            if (i < N) {
                mem.push(ip); // load memory buffer
                //opS = ip;
                i++;
                return ip;
            }
            else {
                // update sample memory buffer
                mem.push(ip);
                // filter
                //sub = ip - mem;
                sub = ip - mem.front();
                acc += sub;
                // trim sample memory buffer
                mem.pop();
            }
            // return avg/N
            return acc/N;
        }
        
        // sets for UI control
        void setDCfiltConst (T sliderVal) { R = (sliderVal < 1.0) ? sliderVal : R; } // if ip < 1, pass to R, else leave it
        void setSmoothLength (int sliderVal) {
            N = sliderVal;
            // reset memory buffer counter
            i = 0;
            // clear the memory buffer
            clear(mem);
        }    
    };

    // you wont hear a big different but it is subtle
    // some distributions are softer, others harsher, gamma will most likely not work in realtime
    template<typename T>
    struct TNoiseMinStd
    {
    private:
        Std::RandomMinStd rng;
    public:
        enum RandomType {
            UNIFORM,
            CAUCHY,
            CHISQUARED,
            EXPONENTIAL,
            GAMMA,
            LOGNORMAL,
            NORMAL,
            PIECEWISE_CONSTANT,
            PIECEWISE_LINEAR
        } NoiseType = UNIFORM;

        T uniform_generate() {
            return rng.uniform_real_distribution(-1.0,1.0);
        }    
        T cauchy_generate() {
            return rng.cauchy_distribution(-1.0,1.0);
        }
        T chisquared_generate(double x) {
            return rng.chi_squared_distribution(x);
        }
        T exponential_generate(double x) {
            return rng.exponential_distribution(x);
        }
        T fisher_generate() {
            return rng.fisher_distribution(-1.0,1.0);
        }
        // this is probably too cpu intensive to real time
        T gamma_generate() {
            return rng.gamma_distribution(-1.0,1.0);
        }
        T lognormal_generate() {
            return rng.lognormal_distribution(-1.0,1.0);
        }
        T normal_generate() {
            return rng.normal_distribution(-1.0,1.0);
        }
        T piecewise_constant_generate() {
            return rng.piecewise_constant_distribution();
        }
        T piecewise_linear_generate() {
            return rng.piecewise_linear_distribution();
        }

        T Tick() {
            switch(NoiseType) {
                case UNIFORM: return uniform_generate();
                case CAUCHY : return cauchy_generate();
                case CHISQUARED: return chisqured_generate();
                case EXPONENTIAL: return exponential_generate();
                case FISHER: return fisher_generate();
                case GAMMA: return gamma_generate();
                case LOGNORMAL: return lognormal_generate();
                case NORMAL: return normal_generate();
                case PIECEWISE_CONSTANT: return piecewise_constant_generate();
                case PIECEWISE_LINEAR: return piecewise_linear_generate();
            }
            return uniform_generate();
        }
        
        T Tick(T input, T A = 1, T F = 1, T P = 1) {
            return A*F*P*input*Tick();

            
        }
        
    };
}