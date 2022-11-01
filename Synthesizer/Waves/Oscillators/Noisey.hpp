#pragma once 

#include <random>
#include <chrono>
#include <queue>
#include "Std/StdObject.h"
#include "Std/StdRandom.h"

class NoiseFilter 
{
private:
    // DC blocker variables
    float R;
    float y = 0, xm1 = 0, ym1 = 0;
    
    // smoothing filter variables
    // N - smooth length
    // i - counter for initialization
    int N, i = 0;
    // opS - output of smoothing filter
    // sub - subsitution : x[n] - x[n - N]
    // acc - accumulator
    float sub, acc;
    // mem - memory for x[i - N] values
    std::queue<float> mem;
    
    // use to clear memory queue on changes to N
    void clear( std::queue<float> &q )
    {
        std::queue<float> empty;
        std::swap( q, empty );
    }
    
public:
    // constructor
    NoiseFilter(float R_in = 0.99, int N_in = 4) : R(R_in), N(N_in) {}
    
    // DC blocking filter
    // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
    float dc_blocking_filter (float ip) {
        y = ip - xm1 + R * ym1;
        xm1 = ip;
        ym1 = y;
        return y;
    }
    
    // moving average/smoothing filter
    // https://zipcpu.com/dsp/2017/10/16/boxcar.html
    float smoothing_filter (float ip) {
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
    void setDCfiltConst (float sliderVal) { R = (sliderVal < 1.0) ? sliderVal : R; } // if ip < 1, pass to R, else leave it
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
struct NoiseMinStd
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
        FISHER,
        LOGNORMAL,
        NORMAL,
        PIECEWISE_CONSTANT,
        PIECEWISE_LINEAR
    } NoiseType = UNIFORM;

    float uniform_generate() {
        return rng.uniform_real_distribution(-1.0,1.0);
    }    
    float cauchy_generate() {
        return rng.cauchy_distribution(-1.0,1.0);
    }
    float chisquared_generate(double x) {
        return rng.chi_squared_distribution(x);
    }
    float exponential_generate(double x) {
        return rng.exponential_distribution(x);
    }
    float fisher_generate() {
        return rng.fisher_distribution(-1.0,1.0);
    }
    // this is probably too cpu intensive to real time
    float gamma_generate() {
        return rng.gamma_distribution(-1.0,1.0);
    }
    float lognormal_generate() {
        return rng.lognormal_distribution(-1.0,1.0);
    }
    float normal_generate() {
        return rng.normal_distribution(-1.0,1.0);
    }
    float piecewise_constant_generate() {
        return rng.piecewise_constant_distribution();
    }
    float piecewise_linear_generate() {
        return rng.piecewise_linear_distribution();
    }

    float Tick(float x = 0) {
        switch(NoiseType) {
            case UNIFORM: return uniform_generate();
            case CAUCHY : return cauchy_generate();
            case CHISQUARED: return chisquared_generate(x);
            case EXPONENTIAL: return exponential_generate(x);
            case FISHER: return fisher_generate();
            case GAMMA: return gamma_generate();
            case LOGNORMAL: return lognormal_generate();
            case NORMAL: return normal_generate();
            case PIECEWISE_CONSTANT: return piecewise_constant_generate();
            case PIECEWISE_LINEAR: return piecewise_linear_generate();
        }
        return uniform_generate();
    }
    
    void Process(float * samples,size_t n) {
        for(size_t i = 0; i < n; i++) samples[i] = Tick();
    }
    void Process(size_t n, float * samples, float * output) {
        for(size_t i = 0; i < n; i++) output[i] = samples[i]*Tick();
    }
};
