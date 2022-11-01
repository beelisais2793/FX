////////////////////////////////////////////////////////////////////////////////////////////////////
// SoundAlchemy
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


#include <vector>
#include <map>
#include <list>
#include <functional>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>
#include <iostream>

#include "Undenormal.hpp"
#include "ClipFunctions.hpp"
/*
#include "Amplifiers.hpp"
#include "Filters.hpp"
#include "Modulation.hpp"
#include "FunctionGenerator.hpp"
#include "FunctionLFO.hpp"
*/
using std::vector;

typedef float sample_type;

// use samples
// samples + sample_dsp + resamplers 
//#include <samples/sample.hpp>
//#include <samples/sample_dsp.hpp>

// use std
//#include<samples/sample.hpp>
//#include<samples/sample_dsp.hpp>

//use Kfr
#include<samples/kfr_sample.hpp>
#include<samples/kfr_sample_dsp.hpp>

// Signal(I,A)
// Modulated(I,A,M)
// Parametric(I,A,X,Y)
// Quadratic F(I,A,A,B,C)
// Quadrant F(I,A,I,II,III,IV)
// Polar    F(Ix,Iy,A,Xx,Xy,Yx,Yy)
// Index Modulated F(I,A,Y,Z) = Z = table interpolation
// Spherical/Cubical F(I,A,X,Y,Z)

namespace SoundAlchemy {
    template<typename T>
    struct TObject
    {
        int num_channels=1;
        T   sample_rate = 44100;
        T   input_gain = 1;
        T   output_gain = 1;
        T   minA = - 1;
        T   maxA =   1;
        
        TObject(T sr = 44100, T IG =1, T OG = 1, T mina = -1, T maxa=1) : sample_rate(sr), input_gain(IG), output_gain(OG),minA(mina),maxA(maxa) {}

            
        virtual T Tick() { assert(1==0); } 
        virtual T Tick(T I, T A = 1, T X = 0, T Y = 0) { assert(1==0); }   
        

        T F0(T I, T A = 1, T X = 0, T Y = 0) {         
            A = clamp(A,-1,1);
            X = clamp(X,-1,1);
            Y = clamp(Y,-1,1);
            return Tick(I,A,X,Y);
        }
                
        T ObjectTick(T I, TObject<T> * Af = NULL, TObject<T>* Ff = NULL, TObject<T> * Pf = NULL) { 
            T A = T(1);
            T F = T(1);
            T P = T(1);
            if(Af != NULL) A = Af->Tick();
            if(Ff != NULL) F = Ff->Tick();
            if(Pf != NULL) P = Pf->Tick();
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return F0(I,A,F,P);
        }

        T TanhTick(T I, T A = 1, T X = 0, T Y = 0) {
            return std::tanh(F0(I,A,X,Y));
        }
        T TanhNormalTick(T I, T A = 1, T X = 0, T Y = 0) {
            return std::tanh(F0(I,A,X,Y))/std::tanh(1);
        }
        T SigmoidTick(T I, T A = 1, T X = 0, T Y = 0) {
            T sign = I < 0? T(-1) : T(1);
            return sign * T(1) / (1 + std::exp(-F0(I,A,X,Y)));
        }
                                        
        void Process(sample_vector<T>& buffer, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) 
        {
            T a = T(1);
            T f = T(1);
            T p = T(1);
            for(size_t i = 0; i < buffer.size(); i++) {
                if(A.size() > 0) a = A[i];
                if(F.size() > 0) f = F[i];
                if(P.size() > 0) p = P[i];
                buffer[i] = F0(buffer[i],a,f,p);
            }
        }
        void  Process(sample_vector<T>& buffer, TObject<T> *A = NULL, TObject<T> * F = NULL, TObject<T> * P = NULL)
        {            
            for(size_t i = 0; i < buffer.size(); i++) {
                float a = 1;
                if(A) a = A->Tick();
                float x = 0;
                if(F) x = F->Tick();
                float y = 0;
                if(P) y = P->Tick();
                buffer[i] = F0(buffer[i],a,x,y);
            }
        }        
        void  Generate(size_t n, sample_vector<T>& buffer)
        {
            for(size_t i = 0; i < buffer.size(); i++) {
                buffer[i] = Tick();
            }
        }            

        void ChannelProcess( uint32_t ch, sample_vector<T>& buffer, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) { 
            T a = T(1);
            T f = T(1);
            T p = T(1);
            for(size_t i = ch; i < buffer.size(); i+=2) {
                if(A.size() > 0) a = A[i];
                if(F.size() > 0) f = F[i];
                if(P.size() > 0) p = P[i];
                buffer[i] = F0(buffer[i],a,f,p);
            }
        }
                
        void Inplace(T * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick();
        }
        void Generator(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
        }
        void Processor(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        
        
        
    };

    template<typename T>
    inline T Tick(TObject<T>& o) { return o.Tick(); }

    template<typename T>
    inline T Tick(TObject<T>& o,T I, T A=1, T F=0, T P=0) { return o.Tick(I,A,F,P); }

    template<typename T>
    inline sample_vector<T> Generate(size_t n, TObject<T> & o, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) { 
        sample_vector<T> R(n); 
        zeros(R); 
        o.Generate(R,A,F,P); 
        return R; 
    }

    template<typename T>
    inline void Process(TObject<T> & o, sample_vector<T> & I,  sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {})
    {
        o.Process(I,A,F,P);
    }

    template<typename T>
    struct TObjectStateFunction
    {
        TObject<T>* obj;

        TObjectStateFunction(TObject<T> * o) : obj(o) {}

        T Tick() { return obj->Tick(); }
        T Tick(T I, T A = 1, T F = 0, T P = 0) { return obj->Tick(I,A,F,P); }

        T Tick(T I, TObject<T> * A = NULL, TObject<T> * F = NULL, TObject<T> * P = NULL) { return obj->Tick(I,A,F,P); }

        void  Process(sample_vector<T>& buffer, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) { obj->Process(buffer,A,F,P); }
        void  Process(size_t n, T * input, T * output) { obj->Process(n,input,output); }

        void  Process(T * samples, size_t n) { obj->Process(samples,n); }
        void  Generate(size_t n, sample_vector<T>& buffer, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) { obj->Generate(n,buffer,A,F,P); }

        void  Process(sample_vector<T>& buffer, TObject<T> * A = NULL, TObject<T> *F = NULL, TObject<T> * P = NULL) { obj->Process(buffer,A,F,P); }
        void  Generate(size_t n, sample_vector<T>& buffer, TObject<T> * A = NULL, TObject<T> *F = NULL, TObject<T> * P = NULL) { obj->Generate(n,buffer,A,F,P); }
        
    };
    
    /////////////////////////////////////////////////////////////////
    // Functionals
    /////////////////////////////////////////////////////////////////
    // fuck gcc
    inline double normalized_tanh(double x) 
    {
        return std::tanh(x) / std::tanh(1);
    }

    // fuck gcc
    inline double clamp(double X, double Min, double Max) {
        if(X < Min) X = Min;
        if(X > Max) X = Max;
        return X;
    }

    // fuck gcc
    inline double hardclip(double x, double a, double b) {
        if(x < a) x = a;
        if(x > b) x = b;
        return x;
    }
    // fuck gcc
    inline double amplify(double x, double amp)
    {
        return x*std::pow(10,amp/20);
    }

    // fuck gcc
    inline double dB(double a) {
        return 20*std::log10(a);
    }

    
    template<typename T>
    sample_vector<T> Multiply(sample_vector<T> a, sample_vector<T> b) {
        sample_vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = a[i] * b[i];
        return r;
    }

    template<typename T>
    sample_vector<T> Modulus(sample_vector<T> a, sample_vector<T> b) {
        sample_vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = std::fmod(a[i],b[i]);
        return r;
    }

    template<typename T>
    sample_vector<T> Add(sample_vector<T> a, sample_vector<T> b) {
        sample_vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = a[i] + b[i];
        return r;
    }

    template<typename T>
    sample_vector<T> Subtract(sample_vector<T> a, sample_vector<T> b) {
        sample_vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = a[i] - b[i];
        return r;
    }

    template<typename T>
    sample_vector<T> Lambda(sample_vector<T> a, std::function<T (T)> func) {
        sample_vector<T> r(a.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = func(a[i]);
        return r;
    }

    template<typename T>
    T Sigma(sample_vector<T> a) {
        T r = 0.0f;
        for(T &x : a) r += x;
        return r;
    }

    template<typename T>
    T Prod(sample_vector<T> a) {
        T r = 1.0f;
        for(T &x : a) r *= x;
        return r;
    }

    // fuck gcc
    inline double AMpositive(double x, double y) { return std::fabs(x) * y; }

    // fuck gcc
    inline double AMnegative(double x, double y) { return -x*y; }

    // fuck gcc
    inline double CircularMod(double x, double y) { return std::fmod(x,y); }

    // fuck gcc
    inline double RingMod(double x, double y) { return x*y; }

    // fuck gcc
    inline double QuadraticMultiply(double x, double y) {
        if(x >= 0 && x < M_PI/2)  return x*y;
        if(x >= M_PI/2 && x < M_PI) return -x*y; 
        if(x >= M_PI && x < 3*M_PI/4) return -x * -y;
        if(x >= 3*M_PI/4 && x <= M_PI) return x * -y;
    }

    // fuck gcc
    inline double Series(double x, double y) { return x*y; }

    // fuck gcc
    inline double Parallel(double x, double y) { return x+y; }
    
    template<typename T>
    void Shuffler(sample_vector<T> & input) {
        std::random_shuffle(input.begin(),input.end());
    }

    template<typename T>
    T Integrator(T X, sample_vector<T>& I) {
        for(T n : I) {
            X += n;
        }
        return X;
    }
    template<typename T>
    T Averager(T X, sample_vector<T>& I) {
        for(T n : I) {
            X += n;
        }
        return X/I.size();
    }
    template<typename T>
    T Integration(T & X, T I, std::function<T (T)> func) {
        X += func(I);
        return X;
    }
    
    template<typename T>
    T LeakIntegrator(T & X, sample_vector<T>& I) {
        for(T n : I) {
            X += 0.995*n;
        }
        return X;
    }

    template<typename T>
    T LeakIntegrator(T & X, T I, std::function<T (T)> func) {
        X += 0.995*func(I);
        return X;
    }

    
    double sinc(double x) {
        return std::sin(M_PI*x)/(M_PI *x);
    }
    
    double Differencer(double X, double Y) {
        return X - Y;
    }

    template<typename T>
    sample_vector<T> Differencer(sample_vector<T>& X, sample_vector<T>& Y) {
        sample_vector<T> r(X.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = X[i] - Y[i];
        return X;
    }

    double DifferentialAccumulator(double R, double x, double y)
    {
        R += (x-y);
        return R;
    }

    template<typename T>
    T DifferentialAccumulator(T & R, sample_vector<T> & x, sample_vector<T> & y) {    
        for(size_t i = 0; i < x.size(); i++) R = DifferentialAccumulator(R,x[i],y[i]);
        return R;
    }



    template<typename T>
    inline sample_vector<T> WaveShape(sample_vector<T> & input, std::function<T (T)> func) {
        sample_vector<T> r(input.size());
        for(size_t i = 0; i < input.size(); i++)
            r[i] = func(input[i]);
        return r;
    }
    
    template<typename T>    
    inline T WaveShape(T input, std::function<T (T)> func) {    
        return func(input);    
    }

    template<typename T>
    T BiPolarWaveShaper(T x, std::function<T (T)> funcplus, std::function<T (T)> funcminus)
    {
        if(x < 0) return funcminus(x);
        return funcplus(x);
    }

    template<typename T>    
    T QuadraticWaveShaper(T x, std::function<T (T)> func1, std::function<T (T)> func2, std::function<T (T)> func3, std::function<T (T)> func4)
    {
        if( x >= 0 && x < M_PI/2) return func1(x);
        if( x >= M_PI/2 && x < M_PI) return func2(x);
        if( x >= M_PI && x < 3*M_PI/4) return func3(x);
        return func4(x);
    }
    template<typename T>
    void PolarWaveShaper(T x, T & o1, T & o2, std::function<T (T)> func)
    {
        o1 = func(x);
        o2 = -func(x); // or func(-x)
    }

    /////////////////////////////////////////////////////////////////
    // WaveShaping and Clipping
    /////////////////////////////////////////////////////////////////

    
    inline double sigmoidDistortionFunction(double x, double gain, double max, double dc) {
        return max * gain * x / std::sqrt(1 + (gain * std::pow(gain * x, 2))) + dc;
    }

    
    inline double asymmetricSigmoidDistortionFunction(double x) {
        // Cutoff for chopping top
        static double cutoff = 0.05;
        static double slope = 0.1;
        static double gain = 20;
        static double max = 0.3;
        static double dc = 0;
        
        static double b =
            sigmoidDistortionFunction(x, gain, max, dc) - slope * cutoff;

        if (x > cutoff) {
            return slope * x + b;
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }

    
    inline double asymmetricSigmoidDistortionFunction2(double x) {
        // Cutoff for chopping top
        static double cutoff = 0.05;
        static double gain = 20;
        static double max = 0.3;
        static double dc = 0;
        if (x > cutoff) {
            return sigmoidDistortionFunction(sigmoidDistortionFunction(x, gain, max, dc), gain * 2, max, dc);
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }

    
    inline double distortion(double x) {
        if (x < -0.08905) {
            // Assume x >= -1
            // doubleherefore, this first interval is actually -1 <= x < -0.08905
            return -(3 / 4) * (1 - std::pow((1 - (-x - 0.032847)), 12) +
                            (1 / 3) * (-x - 0.032847)) +
                0.01;
        } else if (x < 0.320018) {
            return -6.153 * std::pow(x, 2) + 3.9375 * x;
        } else {
            // Assume x <= 1
            // doubleherefore, this last interval is actually 0.320018 <= x <= 1
            return 0.630035;
        }
    }

    /// activations
    
    inline double unisigmoid(double in) {
        return in / (1 + std::exp(-in));
    }
        
    inline double unirelu(double in) {
        if(in < 0) return 0;
        return in;
    }
        
    inline double sigmoid_shaper(double in) {
        double r = in / (1 + std::exp(-in));
        if(in < 0) return -r;
        return r;
    }
        
    inline double tanh_shaper(double in) {
        return std::tanh(in);
    }
        
    inline double relu_shaper(double in) {
        double r = in;
        if(in < 0) r = -in;
        return r;
    }
    
    inline double cubic_shaper(double in, double a = 1.0f) {
        return in - a * (1.f / 3.f) * std::pow(in, 3.f);
    }
    
    inline double arctan_shaper(double in, double gain=1.0f) {
        return (2.f / M_PI) * std::atan(in * gain);
    }

    
    double signum(double x) {
        if(x < 0) return -1.0f;
        return 1.0f;
    }

    
    double exp_shaper(double in, double gain=1.0f) {
        return signum(in) * (1.f - std::exp(-std::fabs(in * gain)));
    }

    /////////////////////////////////////////////////////////////////
    // PolyBLEP
    // t = phase / persio ?
    // dt = deltaPhase / period ?
    /////////////////////////////////////////////////////////////////
    
    inline double polyblep(double t, double dt) 
    {
        double y = 0;
        if (0 <= t && t < 2 * dt) {
            double x = t / dt;
            double u = 2 - x;
            u *= u;
            u *= u;
            y += u;
            if (t < dt) {
                double v = 1 - x;
                v *= v;
                v *= v;
                y -= 4 * v;
            }
        }
        return y / 12;
    }

    
    inline double polysaw(double t, double dt) {
        double v = 2 * t - 1;
        v += polyblep(t, dt);
        v -= polyblep(1 - t, dt);
        return v;
    }
    
    inline double polysquare(double t, double dt) {
        double v = signum(2 * t - 1);
        v += polyblep(t, dt);
        v -= polyblep(1 - t, dt);
        v += polyblep(0.5 - t, dt);
        v -= polyblep(t - 0.5, dt);
        return v;
    }

    
    inline double polyblamp(double t, double dt) {
        double y = 0;
        if (0 <= t && t < 2 * dt) {
            double x = t / dt;
            double u = 2 - x, u2 = u * u;
            u *= u2 * u2;
            y -= u;
            if (t < dt) {
                double v = 1 - x, v2 = v * v;
                v *= v2 * v2;
                y += 4 * v;
            }
        }
        return y * dt / 15;
    }

    
    inline double polytriangle(double t, double dt) 
    {
        double v = 2 * std::fabs(2 * t - 1) - 1;
        v += polyblamp(t, dt);
        v += polyblamp(1 - t, dt);
        t += 0.5;
        t -= std::floor(t);
        v -= polyblamp(t, dt);
        v -= polyblamp(1 - t, dt);
        return v;
    }
};



/* fix
// Parametric
// Quadratic 
// Polar
// Unipolar +/- [0,1][-1,0]
// Bipolar [-1,1]
// I = Input
// A = Amplitude
// F = Frequency/Cutoff
// P = Phase/Q
// X * Y
// Tick()
// TICK(I,A,F,P)
// (A*I) * (F*P)            
// parametric circle
// r = sqrt(x^2 + y^2)
// sin = X/R
// cos = Y/R
// tan = Y/X
// theta = asin(x)
// theta = acos(x)
// theta = atan(x)
// cot = 1/tan
// sec = 1/cos
// csc = 1/sin
// sin^2 = 1-cos^2
// cos^2 = 1-sin^2
// tan^2 +1 = sec^2
// tan^2 = sec^2-1
// cot^2+1 = csc^2
// cot^2 = csc^2-1
// cos(asin) = sqrt(1-x^2)
// cos(atan) = 1/(sqrt(1+x^2))
// tan(acos) = sqrt(1-x^2)/x
// sin(acos) = sqrt(1-x^2)
// sin(atan) = x / sqrt(1+x^2)
// tan(asin) = x / sqrt(1-x^2)
// sin = cos(pi/2 - theta)
// cos = sin(pi/2 - theta)
// sin^2 + cos^2 = 1
// 1 + tan^2 = sec^2
// 1 + cot^2 = csc^2
// sin(-x) = -sin(x)
// tan(-x) = tan(x)
// cos(-x) = cos(x)
// tan = sin/cos
// cot = cos/sin
// sin(2a) = 2sin(x)cos(a)
// cos(2a) = cos^2(a) - sin^2(a)
// cos(2a) = 2cos^2(a) - 1
// cos(2a) = 1 - sin^2(a)
// tan(2a) = (2*tan(a))/(1-tan^2(a))
// sin(a +/- b) = sin(a)cos(b) +/- cos(a)sin(b)
// cos(a +/- b) = cos(a)cos(b) +/- sin(a)sin(b)
// tan(a +/- b) = (tan(a) +- tan(b))/(1 +/- tan(a)tan(b))
// polar (X,Y)
// quadratic (X,Y)
// bipolar +-X,+-Y            
// (X+Y)
// (X-Y)
// (X%Y)
// A - F*P*I
// A + F*P*I
// A % F*P*I
// (A+F) * P * I
// (A-F) * P * I 
// (A%F) * P * I             
// A * (F+P) * I 
// A * (F-P) * I
// A * (F%P) * I
// (A+I) * F * P 
// (A%I) * F * P
// A * I % (F*P)
// A * I + (F*P)
// A * I - (F*P)

template<typename T>
struct TOscillator
{
    T freq;
    T phase;    
    T period;
    T fs;
    std::function<(T (T,T,T))> generator;

    TOscillator(std::function<(T (T,T,T))> func, T freq, T sr) : generator(func), freq(f), fs(sr) { period = freq/fs; }
    // A * sin(2*PI*Index*(freq+fm)/fs+(phase+pm)
    T Tick(T A = 1, T fm = 0, T pm = 0) {              
        return A*generator(freq,phase,A,fm*freq,pm*period);     
    }  
    // Add = osc1 + osc2 ...
    // Modulate = osc1 * osc2 ...
    // Circular = osc1 % osc2 ...
};


template<typename T>
struct TWaveShaper
{
    std::function<T (T)> clipper;

    TWaveShaper(std::function<T (T)> f) : clipper(f) {}

    T Tick(T I, T A = 1) { return A*clipper(I); }
    // cascade = f(g(x))
    // add = f1 + f2 ...
    // Modulated = f1*f2..
    // Circular = f1%f2
};

// phase accumulator = Wrap(phase += inc)
template<typename T>
struct TPhaseAccumulator
{
    T phase;
    T inc;
    T freq;
    T sr;
    
    TPhaseAccumulator(T f, T fs) {
        phase = 0;
        inc = f/fs;
        freq = f;
        sr = fs;
    }
    T operatnr()() {
        phase += inc;
        if(phase >= 1.0) phase -= 1.0;
        return phase;
    }
    T inc() {
        return (*this)();
    }
    T set_frequency(T f) {
        freq = f;
        inc = f/sr;
    }
    int whole() { return (int)phase; }
    int fraction() { return phase - (int)phase; }
};

template<typename T>
struct TIntegrator
{
    T prev;

    TIntegrator() : prev(T(0)) {}

    T Tick(T I) {
        T r = I + prev;
        prev = I;
        return r;
    }
};

template<typename T>
struct TDifferentiator
{
    T prev;

    TIntegrator() : prev(T(0)) {}

    T Tick(T I) {
        T r = I - prev;
        prev = I;
        return r;
    }
};

template<typnemae T>
struct TRingBuffer
{
    sample_vector<T> buffer;

    TRingBuffer(size_t size) { buffer.resize(size); }

    T operator[](size_t n) { return buffer[n]; }
    size_t size() const { return buffer.size(); }
    T first() { return buffer[0]; }
    T last() { return buffer[buffer.size()-1]; }
    T insert(T x) {
        for(size_t i = 1; i < buffer.size()-1; i++) buffer[i] = buffer[i-1];
        T r = buffer[size()-1];
        buffer[0] = x;
        return r;
    }
    T get(size_t i) { return buffer[i]}; 
    T interpolate(T i) {
        float f = i - (int)i;
        int idx = (int)i;        
        if(idx >= size()-1) return buffer[idx-1];
        return f*buffer[idx] + (T(1)-f)*buffer[idx+1];
    }    
};

template<typnemae T>
struct TDelayLine
{
    sample_vector<T> buffer;

    TDelayLine(size_t size) { buffer.resize(size); }

    T operator[](size_t n) { return buffer[n]; }
    
    size_t size() const { return buffer.size(); }
    
    T first() { return buffer[0]; }
    
    T last() { return buffer[buffer.size()-1]; }
    
    T insert(T x) {
        for(size_t i = 1; i < buffer.size()-1; i++) buffer[i] = buffer[i-1];
        T r = buffer[size()-1];
        buffer[0] = x;
        return r;
    }
    
    T get(int i) { return buffer[i]; }
    
    T interpolate(T i) {
        int idx = (int)i;        
        if(idx >= size()) return buffer[size()-1];
        T f = i - idx;
        return f*buffer[idx] + (T(1)-f)*buffer[idx+1];
    }
};

template<typename T>
T parametric_modulation(T I1, T I2, T X, T Y) {
    return (I1*X)*(I2*Y);
}

template<typename T>
T parametric_modulus(T I1, T I2, T X, T Y) {
    return std::fmod(I1*X),(I2*Y));
}
template<typename T>
T parametric_dual_modulus(T I1, T I2, T X, T Y) {
    return std::fmod(std::fmod(I1,X)),std::fmod(I2,Y));
}

template<typname T>
T parametric_circle_x(float r, float theta) {
    return r * std::cos(theta);
}

template<typname T>
T parametric_circle_y(float r, float theta) {
    return r * std::sin(theta);
}

template<typename T>
T quadratic_solver_plus(T a, T b, T c)
{
    return (-b + std::sqrt(b*b - 4*a*c)) / (2*a);
}
template<typename T>
T quadratic_solver_minus(T a, T b, T c)
{
    return (-b - std::sqrt(b*b - 4*a*c)) / (2*a);
}

// unipolar positive [0..1]
// unipolar negative [-1..0]
// bipolar [-1..1]
// quadraitc [0,PI/2][PI/2,PI][PI,3PI/4][3PI/4,2PI]  ax + by +c = 0
// parametric circles x,y
// pwm unipolar positive
// x = saw + inverted saw 
// inverted saw = -saw out of phase 
// x = -saw + -inverted saw
// bipolar wave
// u = saw + isaw 
// n = -saw + -isaw
// x = u + n
*/



