/*
  ==============================================================================

    Saul 7.26

  ==============================================================================
*/

//==============================================================================
/**
*/
#include <cmath>

namespace Filters::Curtis
{
    class HybridCurtisVCF
    {
    public:

        //constructor
        HybridCurtisVCF() noexcept;

        //destructor
        ~HybridCurtisVCF() noexcept;

        //==============================================================================
        //does pretty much what it says - deactivates thread safety
        void makeInactive() noexcept;

        //sets all coefficients and intermediate variables
        void makeCurtis (double sampleRate, double frequency, double resonance) noexcept;
        //==============================================================================
        //resets pipeline via coefficient reset
        void reset() noexcept;

        //processes all samples fed via buffer
        void processSamples (float* samples, int numSamples) noexcept;
        
        float Tick(float input) {
        float r = input;
        processSamples(&r,1);
        return r;
        }

        void Process(size_t n, float * input, float * output) {
            memcpy(output,input,n*sizeof(float));
            processSamples(output, n);
        }

        void Process(float * samples, size_t n ) {
            processSamples(samples, n);
        }

    private:
        
        //core variables
        float cutoff;   //in Hz
        float fs;       //format eg - 44100
        float res;      // 0 - 1
        
        //intermediates
        float f;
        float k;
        float p;
        float scale;
        float r;
        
        //coeffs
        float c1;
        float c2;
        float c3;
        float c4;
        
        float oldC1;
        float oldC2;
        float oldC3;
        
        float c0;
        float oldC0;

    protected:
        //==============================================================================
        
        //thread safety    
        bool active = false;

    };


    //==============================================================================
    HybridCurtisVCF::HybridCurtisVCF() noexcept
    {
    }

    HybridCurtisVCF::~HybridCurtisVCF() noexcept
    {
    }

    //==============================================================================
    void HybridCurtisVCF::makeInactive() noexcept
    {    
        active = false;
    }
        
    //==============================================================================
    void HybridCurtisVCF::makeCurtis (double sampleRate,
                                            double frequency,
                                            double resonance) noexcept
    {
        
        //NOTE! need the following safety checks
        /*
        sample rate is above 0
        frequency is below nyquist
        resonance is in range
        */
        
        //core variables
        cutoff = (float)frequency;
        fs = (float)sampleRate;
        res = (float)resonance;
        
        //reset all coeffs
        c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;

        //intermediate variables
        f = 2.f * (float)cutoff / (float)fs;    //should range 0 - 1
        k = 3.6f * f - 1.6f * f * f - 1.f;      //empirical tuning
        p = (k + 1.f) * 0.5f;
        scale = exp((1.f - p) * 1.386249f);
        r = (float)res * scale;

        //thread-safety    
        active = true;
    }

    //==============================================================================
    void HybridCurtisVCF::reset() noexcept
    {
        //thread-safety and reset all coeffs    
        c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;
    }

    void HybridCurtisVCF::processSamples (float* const samples, const int numSamples) noexcept
    {
        if (active)
        {
            float sample;
            
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
}