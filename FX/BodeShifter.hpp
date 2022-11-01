/*
  ==============================================================================
    BodeShifter.h
    Created: 24 May 2020 12:49:44pm
    Author:  mw
  ==============================================================================
*/
#pragma once
#include <vector>
#include <cmath>

namespace FX::BodeShifter
{
    class QuadratureOscillator : public StereoOscillatorProcessor
    {
    public:
        QuadratureOscillator(DspFloatType frequency, unsigned int sample_rate = 44100)
        : StereoOscillatorProcessor()
        {
            setFrequency(frequency, sample_rate);
        }

        void setSampleRate(unsigned int sample_rate)
        {
            setFrequency(frequency_, sample_rate_);
        }

        void setFrequency(DspFloatType frequency, unsigned int sample_rate) 
        {
            frequency_ = frequency;
            sample_rate_ = sample_rate;

            cyclesPerSample_ = frequency / (DspFloatType)sample_rate;
            angleDelta_ = cyclesPerSample_ * 2.f * M_PI;
        }

        void setFrequency(DspFloatType frequency)
        {
            cyclesPerSample_ = frequency / (DspFloatType)sample_rate_;
            angleDelta_ = cyclesPerSample_ * 2.f * M_PI;
        }
        enum {
            PORT_FREQ,
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_FREQ) setFrequency(v);
            else printf("No port %d\n",port);
        }
        std::pair<DspFloatType, DspFloatType> processSample() 
        {  
            currentAngle_ += angleDelta_;
            fmod(currentAngle_, cyclesPerSample_);
            return std::make_pair(sinf(currentAngle_), cosf(currentAngle_));
        }

        DspFloatType Tick(DspFloatType iL, DspFloatType iR, DspFloatType & oL, DspFloatType &oR, DspFloatType A=1,DspFloatType X=1, DspFloatType Y=1) {
            std::pair<DspFloatType,DspFloatType> p = processSample();
            DspFloatType ca = currentAngle_;
            DspFloatType f  = cyclesPerSample_;
            setFrequency(f * fabs(Y));
            currentAngle_ *= fabs(X);            
            oL = A*p.first;
            oR = A*p.second;
            setFrequency(f);
            currentAngle_ = ca;
            return 0.5*(oL+oR);
        }

    private:
        DspFloatType frequency_;
        unsigned int sample_rate_;
        DspFloatType cyclesPerSample_;
        DspFloatType angleDelta_;
        DspFloatType currentAngle_ = 0.f;
    };

    class IIRLowPass : public FilterProcessor
    {
    public:
        IIRLowPass(DspFloatType cutoff_frequency, unsigned int sample_rate = 44100) :
        FilterProcessor(),
        previous_(0.f),
        sample_rate_(sample_rate),
        cutoff_frequency_(cutoff_frequency)
        {
            setCutoffFrequency(cutoff_frequency);
        }

        void setSampleRate(unsigned int sample_rate)
        {
            sample_rate_ = sample_rate;
            setCutoffFrequency(cutoff_frequency_);
        }

        void setCutoffFrequency(DspFloatType cutoff_frequency)
        {
            cutoff_frequency_ = cutoff_frequency;
            DspFloatType delta_t = 1.f / sample_rate_;
            DspFloatType numerator = 2.f * M_PI * delta_t * cutoff_frequency;
            alpha_ = numerator / (numerator + 1.f);
        }
        enum {
            PORT_CUTOFF,
        };
        void setPort(int port, DspFloatType v) {
            if(port == PORT_CUTOFF) setCutoffFrequency(v);
        }
        DspFloatType processSample(DspFloatType sample)
        {
            DspFloatType out = previous_ + alpha_ * (sample - previous_);
            previous_ = sample;
            return out;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            DspFloatType c = cutoff_frequency_;
            setCutoffFrequency(c * fabs(X));
            DspFloatType r = processSample(Y*I);
            setCutoffFreqnecy(c);
            return A*r;
        }

    private:
        DspFloatType alpha_;
        DspFloatType previous_;
        unsigned int sample_rate_;
        DspFloatType cutoff_frequency_;
    };

    class BodeShifter : public MonoFXProcessor
    {
    public:
        BodeShifter(unsigned int sample_rate = 44100) :
            MonoFXProcessor(),
            filter_1_(sample_rate, sample_rate),
            filter_2_(sample_rate, sample_rate),
            oscillator_1_(sample_rate/2, sample_rate),
            oscillator_2_(sample_rate/2, sample_rate)
        {
            setFrequency1(sample_rate);
            setFrequency2(0);
        }

        DspFloatType processSample(DspFloatType sample) 
        {
            auto o1 = oscillator_1_.processSample();
            auto o2 = oscillator_2_.processSample();

            DspFloatType m1a = o1.first * sample;
            DspFloatType m1b = o1.second * sample;

            DspFloatType m2a = o2.first * sample;
            DspFloatType m2b = o2.second * sample;

            DspFloatType f2a = filter_1_.processSample(sample);
            DspFloatType f2b = filter_2_.processSample(sample);

            return f2a * o2.first + f2b * o2.second;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return processSample(I);
        }
        void setSampleRate(unsigned int sample_rate)
        {
            oscillator_1_.setSampleRate(sample_rate);
            oscillator_2_.setSampleRate(sample_rate);
            filter_1_.setSampleRate(sample_rate);
            filter_2_.setSampleRate(sample_rate);
        }

        void setFrequency1(DspFloatType frequency) 
        {
            oscillator_1_.setFrequency(frequency);
            filter_1_.setCutoffFrequency(frequency);
            filter_2_.setCutoffFrequency(frequency);
        }

        void setFrequency2(DspFloatType frequency)
        {
            oscillator_2_.setFrequency(frequency);
        }

        enum {
            PORT_FREQ1,
            PORT_FREQ2,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_FREQ1: setFrequency1(v); break;
                case PORT_FREQ2: setFrequency2(v); break;
            }
        }
    private:
        QuadratureOscillator oscillator_1_;
        QuadratureOscillator oscillator_2_;
        IIRLowPass filter_1_;
        IIRLowPass filter_2_;
    };
}