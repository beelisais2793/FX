/////////////////////////////////////////////////////////////////////////////////////////////////////
// KfrCore.hpp
// this is mainly intended for wrapping with Swig but also useable as a simplied wrapper in c++.
/////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once 
#include <kfr/kfr.h>
#include <kfr/dsp.hpp>
#include <kfr/dft.hpp>
#include <kfr/io.hpp>
#include <kfr/math.hpp>
#include <iostream>
#include <random> 

typedef float SampleType;

#include "kfr_biquad.hpp"
#include "kfr_bessel.hpp"
#include "kfr_butterworth.hpp"
#include "kfr_chebyshev1.hpp"
#include "kfr_chebyshev2.hpp"
#include "kfr_convolve.hpp"
#include "kfr_dct.hpp"
#include "kfr_dft.hpp"
#include "kfr_fileio.hpp"
#include "kfr_fir.hpp"
#include "kfr_resample.hpp"
#include "kfr_window.hpp"



namespace DSP {
    
    template<typename T>
    T energy_to_loudness(T energy) {
        return kfr::energy_to_loudness(energy);
    }
    template<typename T>
    T loudness_to_energy(T loudness) {
        return kfr::loudness_to_energy(loudness);
    }    
    template<typename T> T normalize_frequency(T f, T sample_rate) {
        return f/sample_rate;
    }
    template<typename T>
    kfr::univector<T> autocorrelate(const kfr::univector<T> & v) {
        return kfr::univector<T>(kfr::autocorrelate(v));
    }
    template<typename T>
    kfr::univector<T> correlate(const kfr::univector<T> & v1,const kfr::univector<T> & v2) {
        return kfr::univector<T>(kfr::correlate(v1,v2));
    }
    template<typename T>
    kfr::univector<T> amp_to_dB(const kfr::univector<T> & in) {
        return kfr::univector<T>(kfr::amp_to_dB(in));
    }    
    template<typename T>
    kfr::univector<T> dcremove(const kfr::univector<T> & input, T cutoff) {
        return kfr::univector<T>(kfr::CMT_ARCH_NAME::dcremove(input,cutoff));
    }
    template<typename T>
    kfr::univector<T> sinewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        kfr::univector<T> r(n);                
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sine(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    template<typename T>
    kfr::univector<T> squarewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        kfr::univector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::square(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    template<typename T>
    kfr::univector<T> trianglewave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        kfr::univector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::triangle(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    template<typename T>
    kfr::univector<T> sawtoothwave(size_t n, T freq, T sample_rate, T phase=(T)0) {
        kfr::univector<T> r(n);
        for(size_t i = 0; i < n; i++)
        {
            r[i] = kfr::sawtooth(2*M_PI*phase);
            phase += freq/sample_rate;
            if(phase > (T)1.0) phase-=(T)1.0;
        }
        return r;
    }
    template<typename T>
    kfr::univector<T> generate_sin(size_t n, T start, T step) {
        kfr::univector<T> r(n);
        r = kfr::gen_sin(start,step);
        return r;
    }

    template<typename T>
    kfr::univector<T> generate_linear(size_t n, T start, T step) {
        kfr::univector<T> r(n);
        r = kfr::gen_linear(start,step);
        return r;
    }

    template<typename T>
    kfr::univector<T> generate_exp(size_t n, T start, T step) {
        kfr::univector<T> r(n);
        r = kfr::gen_exp(start,step);
        return r;
    }

    template<typename T>
    kfr::univector<T> generate_exp2(size_t n, T start, T step) {
        kfr::univector<T> r(n);
        r = kfr::gen_exp2(start,step);
        return r;        
    }

    template<typename T>
    kfr::univector<T> generate_cossin(size_t n, T start, T step) {
        kfr::univector<T> r(n);
        r = kfr::gen_cossin(start,step);
        return r;
    }

    template<typename T>
    void do_deinterleave(kfr::univector2d<T> & out, const T * ptr, size_t size, size_t channels) {                
        out.resize(channels);
        for(size_t i = 0; i < channels; i++) out[i].resize(size);
        std::vector<T*> p(out.size());
        for(size_t i = 0; i < out.size(); i++) p[i] = out[i].data();
        kfr::deinterleave(p.data(),ptr,channels,size);
    }
    
    template<typename T>
    kfr::univector<T> do_interleave(const kfr::univector2d<T> & input) {        
        return kfr::univector<T>(kfr::interleave(input));
    }

    template<typename T>
    kfr::univector<T> generate_sequence(int n, int start, int step) {
        kfr::univector<T> r(n);
        for(size_t i = 0; i < n; i++)
            r[i] = start + step*n;
        return r;
    }

    std::string dB_to_string(const double value, double min=-140.0f) { return kfr::dB_to_string(value,min); }
    std::string dB_to_utf8string(const double value, double min=-140.0f) { return kfr::dB_to_utf8string(value,min); }
}


