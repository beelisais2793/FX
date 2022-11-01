/*
  ==============================================================================

    Saul 7.26

  ==============================================================================
*/
// it is just the musicdsp moog filter

//==============================================================================
/**
*/
#include <cmath>
#include "SoundObject.hpp"

namespace Analog::Filters::Curtis
{
    class HybridCurtisVCF : public FilterProcessor
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
        void makeCurtis (DspFloatType sampleRate, DspFloatType frequency, DspFloatType resonance) noexcept;
        //==============================================================================
        //resets pipeline via coefficient reset
        void reset() noexcept;

        void setCutoff(DspFloatType f) {
            makeCurtis(fs,f,res);
        }
        void setResonance(DspFloatType r) {
            makeCurtis(fs,cutoff,r);
        }
        //processes all samples fed via buffer
        void processSamples (DspFloatType* samples, int numSamples) noexcept;
        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            DspFloatType r = I;
            processSamples(&r,1);
            return A*r;
        }

        void ProcessBlock(size_t n, float * input, float * output) {
            memcpy(output,input,n*sizeof(DspFloatType));
            processSamples(output, n);
        }
        
        enum {
            PORT_CUTOFF,
            PORT_Q,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_CUTOFF: makeCurtis(fs,v,res); break;
                case PORT_Q: makeCurtis(fs,cutoff,v); break;
                default: printf("No port %d\n",port);
            }
        }
    private:
        
        //core variables
        DspFloatType cutoff;   //in Hz
        DspFloatType fs;       //format eg - 44100
        DspFloatType res;      // 0 - 1
        
        //intermediates
        DspFloatType f;
        DspFloatType k;
        DspFloatType p;
        DspFloatType scale;
        DspFloatType r;
        
        //coeffs
        DspFloatType c1;
        DspFloatType c2;
        DspFloatType c3;
        DspFloatType c4;
        
        DspFloatType oldC1;
        DspFloatType oldC2;
        DspFloatType oldC3;
        
        DspFloatType c0;
        DspFloatType oldC0;

    protected:
        //==============================================================================
        
        //thread safety    
        bool active = false;

    };


    //==============================================================================
    HybridCurtisVCF::HybridCurtisVCF() noexcept : FilterProcessor()
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
    void HybridCurtisVCF::makeCurtis (DspFloatType sampleRate,
                                            DspFloatType frequency,
                                            DspFloatType resonance) noexcept
    {
        
        //NOTE! need the following safety checks
        /*
        sample rate is above 0
        frequency is below nyquist
        resonance is in range
        */
        
        //core variables
        cutoff = (DspFloatType)frequency;
        fs = (DspFloatType)sampleRate;
        res = (DspFloatType)resonance;
        
        //reset all coeffs
        //c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;

        //intermediate variables
        f = 2.f * (DspFloatType)cutoff / (DspFloatType)fs;    //should range 0 - 1
        k = 3.6f * f - 1.6f * f * f - 1.f;      //empirical tuning
        p = (k + 1.f) * 0.5f;
        scale = exp((1.f - p) * 1.386249f);
        r = (DspFloatType)res * scale;

        //thread-safety    
        active = true;
    }

    //==============================================================================
    void HybridCurtisVCF::reset() noexcept
    {
        //thread-safety and reset all coeffs    
        c1 = c2 = c3 = c4 = oldC0 = oldC1 = oldC2 = oldC3 = 0.f;
    }

    void HybridCurtisVCF::processSamples (DspFloatType* const samples, const int numSamples) noexcept
    {
        if (active)
        {
            DspFloatType sample;
            
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
                
                c4 = c4 - std::pow(c4, 3.f) / 6.f;
                
                samples[i] = c4;
            }
        }
    }
}