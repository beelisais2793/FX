#pragma once
#include <cmath>

namespace SoundAlchemy::Filters 
{
    template<typename T>
    class TCurtisVCF
    {
    public:

        //constructor
        TCurtisVCF() noexcept;

        //destructor
        ~TCurtisVCF() noexcept;

        //==============================================================================
        //does pretty much what it says - deactivates thread safety
        void makeInactive() noexcept;

        //sets all coefficients and intermediate variables
        void makeCurtis (T sampleRate, T frequency, T resonance) noexcept;
        //==============================================================================
        //resets pipeline via coefficient reset
        void reset() noexcept;

        //processes all samples fed via buffer
        void processSamples (T* samples, int numSamples) noexcept;
        
        T Tick(T in) {
            T r = in;
            processSamples(&r,1);
            return r;
        }
        void Process(size_t n, T* input, T * output) {
            memcpy(output,input,n*sizeof(T));
            processSamples(output,n);
        }
        void Inplace(size_t n, T * samples) {
            processSamples(samples,n);
        }
        
    private:
        
        //core variables
        T cutoff;   //in Hz
        T fs;       //format eg - 44100
        T res;      // 0 - 1
        
        //intermediates
        T f;
        T k;
        T p;
        T scale;
        T r;
        
        //coeffs
        T c1;
        T c2;
        T c3;
        T c4;
        
        T oldC1;
        T oldC2;
        T oldC3;
        
        T c0;
        T oldC0;

    protected:
        //==============================================================================
        
        //thread safety    
        bool active = false;

    };


    //==============================================================================
    template<typename T>    
    TCurtisVCF<T>::TCurtisVCF() noexcept
    {
    }

    template<typename T>
    TCurtisVCF<T>::~TCurtisVCF() noexcept
    {
    }

    //==============================================================================
    template<typename T>
    void TCurtisVCF<T>::makeInactive() noexcept
    {    
        active = false;
    }
        
    //==============================================================================
    template<typename T>    
    void TCurtisVCF<T>::makeCurtis (T sampleRate,
                                            T frequency,
                                            T resonance) noexcept
    {
        
        //NOTE! need the following safety checks
        /*
        sample rate is above 0
        frequency is below nyquist
        resonance is in range
        */
        
        //core variables
        cutoff = (T)frequency;
        fs = (T)sampleRate;
        res = (T)resonance;
        
        //reset all coeffs
        c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;

        //intermediate variables
        f = 2.f * (T)cutoff / (T)fs;    //should range 0 - 1
        k = 3.6f * f - 1.6f * f * f - 1.f;      //empirical tuning
        p = (k + 1.f) * 0.5f;
        scale = exp((1.f - p) * 1.386249f);
        r = (T)res * scale;

        //thread-safety    
        active = true;
    }

    //==============================================================================
    template<typename T>
    void TCurtisVCF<T>::reset() noexcept
    {
        //thread-safety and reset all coeffs    
        c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;
    }

    template<typename T>
    void TCurtisVCF<T>::processSamples (T* const samples, const int numSamples) noexcept
    {
        if (active)
        {
            T sample;
            
            for (int i = 0; i < numSamples; i++)
            {
                sample = samples[i];
                
                c0 = sample - r * c4;
                
                c1 = c0 * p + oldC0 * p - k * c1;
                c2 = c1 * p + oldC1 * p - k * c2;
                c3 = c2 * p + oldC2 * p - k * c3;
                c4 = c3 * p + oldC3 * p - k * c4;
                
                oldC0 = c0;
                oldC1 = c1;
                oldC2 = c2;
                oldC3 = c3;
                
                c4 = c4 - pow(c4, 3.f) / 6.f;
                
                samples[i] = c4;
            }
        }
    }



};