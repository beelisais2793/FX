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

typedef float sample_type;
// use samples
// samples + sample_dsp + resamplers 
//#include <samples/sample.hpp>
//#include <samples/sample_dsp.hpp>


// use std
#include<samples/sample.hpp>
#include<samples/sample_dsp.hpp>

//use Kfr
//#include<samples/kfr_sample.hpp>
//#include<samples/kfr_sample_dsp.hpp>

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
        virtual T Tick(T I) { assert(1==0); }

        T AT(T A, T F = 1, T P = 1) {         
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return A * (F * P) * Tick();
        }
        T IT(T I, T A = 1, T F = 0, T P = 0) {         
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return A * (F * P) * Tick(I);
        }
                
        T ObjectTick(T I, TObject<T> * Af = 1, TObject<T>* Ff = 1, TObject<T> * Pf = 1) { 
            T A = T(1);
            T F = T(1);
            T P = T(1);
            if(Af != NULL) A = Af->Tick();
            if(Ff != NULL) F = Ff->Tick();
            if(Pf != NULL) P = Pf->Tick();
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return IT(I,A,F,P);
        }

        T TanhTick(T I, T A = 1, T F = 1, T P = 1) {
            return std::tanh(IT(I,A,F,P));
        }
        T TanhNormalTick(T I, T A = 1, T F = 1, T P = 1) {
            return std::tanh(IT(I,A,F,P))/std::tanh(1);
        }
        T SigmoidTick(T I, T A = 1, T F = 1, T P = 1) {
            T sign = I < 0? T(-1) : T(1);
            return sign * T(1) / (1 + std::exp(-IT(I,A,F,P)));
        }
        T AddTick(T I, T A = 1, T F = 1, T P = 1) { 
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return A * (F + P) * Tick(I);
        }
        T SubTick(T I, T A = 1, T F = 1, T P = 1) { 
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return A * (F - P) * Tick(I);
        }        
        T ModTick(T I, T A = 1, T F = 1, T P = 1) { 
            A = clamp(A,-1,1);
            F = clamp(F,-1,1);
            P = clamp(P,-1,1);
            return A * std::fmod(F,P) * Tick(I);
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
                buffer[i] = IT(buffer[i],a,f,p);
            }
        }
        void Generate(size_t n, sample_vector<T>& buffer, sample_vector<T> A = {}, sample_vector<T> F = {}, sample_vector<T> P = {}) 
        {
            T a = T(1);
            T f = T(1);
            T p = T(1);
            for(size_t i = 0; i < buffer.size(); i++) {
                if(A.size() > 0) a = A[i];
                if(F.size() > 0) f = F[i];
                if(P.size() > 0) p = P[i];
                buffer[i] = IT(buffer[i],a,f,p);
            }
        }            

        
        void  Process(sample_vector<T>& buffer, TObject<T> *A = NULL, TObject<T> * F = NULL, TObject<T> * P = NULL)
        {            
            for(size_t i = 0; i < buffer.size(); i++) {
                buffer[i] = IT(buffer[i],A,F,P);
            }
        }
        void  Generate(size_t n, sample_vector<T>& buffer, TObject<T> *A = NULL, TObject<T> * F = NULL, TObject<T> * P = NULL)
        {
            for(size_t i = 0; i < buffer.size(); i++) {
                buffer[i] = IT(buffer[i],A,F,P);
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
                buffer[i] = IT(buffer[i],a,f,p);
            }
        }
        void ChannelProcess( uint32_t ch, sample_vector<T>& buffer, TObject<T>* A = NULL, TObject<T>* F = NULL, TObject<T>* P = NULL) 
        {
            for(size_t i = ch; i < buffer.size(); i+=2) {
                buffer[i] = IT(buffer[i],A,F,P);
            }
        }
        
        void Streamer(T * samples, size_t n) {
            for(size_t i = 0; i < n; i++) samples[i] = Tick();
        }
        void Processor(size_t n, T * input, T * output) {
            for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
        }
        void Generator(size_t n, T * input, T * output) {
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
    template<typename T>
    inline T normalized_tanh(T x) 
    {
        return std::tanh(x) / std::tanh(1);
    }

    template<typename T>
    inline T clamp(T X, T Min, T Max) {
        if(X < Min) X = Min;
        if(X > Max) X = Max;
        return X;
    }

    template<typename T>
    inline T hardclip(T x, T a, T b) {
        if(x < a) x = a;
        if(x > b) x = b;
        return x;
    }
    template<typename T>
    inline T amplify(T x, T amp)
    {
        return x*std::pow(10,amp/20);
    }

    template<typename T>
    inline T dB(T a) {
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

    template<typename T>
    inline T AMpositive(T x, T y) { return std::abs(x) * y; }

    template<typename T>
    inline T AMnegative(T x, T y) { return -x*y; }

    template<typename T>
    inline T CircularMod(T x, T y) { return fmod(x,y); }

    template<typename T>
    inline T RingMod(T x, T y) { return x*y; }

    template<typename T>
    inline T QuadraticMultiply(T x, T y) {
        if(x >= 0 && x < M_PI/2)  return x*y;
        if(x >= M_PI/2 && x < M_PI) return -x*y; 
        if(x >= M_PI && x < 3*M_PI/4) return -x * -y;
        if(x >= 3*M_PI/4 && x <= M_PI) return x * -y;
    }

    template<typename T>
    inline T Series(T x, T y) { return x*y; }

    template<typename T>
    inline T Parallel(T x, T y) { return x+y; }
    
    template<typename T>
    void Shuffler(sample_vector<T> & input) {
        std::random_shuffle(input.begin(),input.end());
    }

    template<typename T>
    T Integrate(T & X, T I) {
        X += I;
        return X;
    }

    template<typename T>
    T Integrator(T & X, sample_vector<T>& I) {
        for(T n : I) {
            X += n;
        }
        return X;
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

    template<typename T>
    T sinc(T x) {
        return std::sin(M_PI*x)/(M_PI *x);
    }

    template<typename T>
    T Differencer(T X, T Y) {
        return X - Y;
    }

    template<typename T>
    sample_vector<T> Differencer(sample_vector<T>& X, sample_vector<T>& Y) {
        sample_vector<T> r(X.size());
        for(size_t i = 0; i < r.size(); i++) r[i] = X[i] - Y[i];
        return X;
    }

    template<typename T>
    T DifferentialAccumulator(T &R, T x, T y)
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

    template<typename T>
    inline T sigmoidDistortionFunction(T x, T gain, T max, T dc) {
        return max * gain * x / std::sqrt(1 + (gain * std::pow(gain * x, 2))) + dc;
    }

    template<typename T>
    inline T asymmetricSigmoidDistortionFunction(T x) {
        // Cutoff for chopping top
        static T cutoff = 0.05;
        static T slope = 0.1;
        static T gain = 20;
        static T max = 0.3;
        static T dc = 0;
        
        static T b =
            sigmoidDistortionFunction(x, gain, max, dc) - slope * cutoff;

        if (x > cutoff) {
            return slope * x + b;
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }

    template<typename T>
    inline T asymmetricSigmoidDistortionFunction2(T x) {
        // Cutoff for chopping top
        static T cutoff = 0.05;
        static T gain = 20;
        static T max = 0.3;
        static T dc = 0;
        if (x > cutoff) {
            return sigmoidDistortionFunction(sigmoidDistortionFunction(x, gain, max, dc), gain * 2, max, dc);
        } else {
            return sigmoidDistortionFunction(x, gain, max, dc);
        }
    }

    template<typename T>
    inline T Distortion(T x) {
        if (x < -0.08905) {
            // Assume x >= -1
            // Therefore, this first interval is actually -1 <= x < -0.08905
            return -(3 / 4) * (1 - std::pow((1 - (-x - 0.032847)), 12) +
                            (1 / 3) * (-x - 0.032847)) +
                0.01;
        } else if (x < 0.320018) {
            return -6.153 * std::pow(x, 2) + 3.9375 * x;
        } else {
            // Assume x <= 1
            // Therefore, this last interval is actually 0.320018 <= x <= 1
            return 0.630035;
        }
    }

    /// activations
    template<typename T>
    inline T unisigmoid(T in) {
        return in / (1 + std::exp(-in));
    }
    template<typename T>    
    inline T unirelu(T in) {
        if(in < 0) return 0;
        return in;
    }
    template<typename T>    
    inline T sigmoid_shaper(T in) {
        T r = in / (1 + std::exp(-in));
        if(in < 0) return -r;
        return r;
    }
    template<typename T>    
    inline T tanh_shaper(T in) {
        return std::tanh(in);
    }
    template<typename T>    
    inline T relu_shaper(T in) {
        T r = in;
        if(in < 0) r = -in;
        return r;
    }
    template<typename T>
    inline T cubic_shaper(T in, T a = 1.0f) {
        return in - a * (1.f / 3.f) * std::pow(in, 3.f);
    }
    template<typename T>
    inline T arctan_shaper(T in, T gain=1.0f) {
        return (2.f / M_PI) * std::atan(in * gain);
    }

    template<typename T>
    T signum(T x) {
        if(x < 0) return -1.0f;
        return 1.0f;
    }

    template<typename T>
    T exp_shaper(T in, T gain=1.0f) {
        return signum(in) * (1.f - expf(-fabs(in * gain)));
    }

    /////////////////////////////////////////////////////////////////
    // PolyBLEP
    // t = phase / persio ?
    // dt = deltaPhase / period ?
    /////////////////////////////////////////////////////////////////
    template<typename T>
    inline T polyblep(T t, T dt) 
    {
        T y = 0;
        if (0 <= t && t < 2 * dt) {
            T x = t / dt;
            T u = 2 - x;
            u *= u;
            u *= u;
            y += u;
            if (t < dt) {
                T v = 1 - x;
                v *= v;
                v *= v;
                y -= 4 * v;
            }
        }
        return y / 12;
    }

    template<typename T>
    inline T polysaw(T t, T dt) {
        T v = 2 * t - 1;
        v += blep(t, dt);
        v -= blep(1 - t, dt);
        return v;
    }
    template<typename T>
    inline T polysquare(T t, T dt) {
        T v = signum(2 * t - 1);
        v += blep(t, dt);
        v -= blep(1 - t, dt);
        v += blep(0.5 - t, dt);
        v -= blep(t - 0.5, dt);
        return v;
    }

    template<typename T>
    inline T polyblamp(T t, T dt) {
        T y = 0;
        if (0 <= t && t < 2 * dt) {
            T x = t / dt;
            T u = 2 - x, u2 = u * u;
            u *= u2 * u2;
            y -= u;
            if (t < dt) {
                T v = 1 - x, v2 = v * v;
                v *= v2 * v2;
                y += 4 * v;
            }
        }
        return y * dt / 15;
    }

    template<typename T>
    inline T polytriangle(T t, T dt) 
    {
        T v = 2 * std::fabs(2 * t - 1) - 1;
        v += blamp(t, dt);
        v += blamp(1 - t, dt);
        t += 0.5;
        t -= std::floor(t);
        v -= blamp(t, dt);
        v -= blamp(1 - t, dt);
        return v;
    }
};



    