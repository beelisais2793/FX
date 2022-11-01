#pragma once
#include <cmath>

template<typename T>
class TAnalogFilter {
public:
    TAnalogFilter() {}
    
    TAnalogFilter(const unsigned int p) : poles(p) {}
    
    TAnalogFilter(const unsigned int p, const double cf) : poles(p), cornerFrequency(cf) {}

    virtual ~TAnalogFilter() {}

    void bindSampleRate (double* sampleRate) { this->sampleRate = sampleRate; }
    
    virtual void setPoles (unsigned int p) noexcept = 0;
    
    virtual void setCornerFrequency (double frequency) noexcept = 0;
    
    virtual void process (double& buffer, unsigned int channel) noexcept = 0;
    
    T getProcessed (T sample, unsigned int channel) noexcept;

protected:
    double* sampleRate = nullptr;
    double tau = 0.0;
    unsigned int poles = 1.0;
    double cornerFrequency = 1.0;
    double caps[32][32] = {0.0};
    double* cap_ptr;
    double sampleDelta = 0.0;
    double pi = 3.1415926535;
};

template <typename SampleType>
inline SampleType TAnalogFilter<SampleType>::getProcessed (SampleType sample, unsigned int channel) noexcept
{
    double sample2 = sample;
    process (sample2, channel);
    return sample2;
}


template<typename T>
class TLowPassFilter : public TAnalogFilter<T> {
    public:
        TLowPassFilter();
        
        TLowPassFilter(const unsigned int poles);
    
        TLowPassFilter(const unsigned int poles, const double cornerFrequency);
    
        void setPoles (unsigned int p) noexcept override;
    
        void setCornerFrequency(double frequency) noexcept override;
    
        void process (double& sample, unsigned int channel) noexcept override;
    };


template<typename T>
inline TLowPassFilter<T>::LowPassFilter ()
{
    poles = 1;
    setCornerFrequency(1.0e5);
}

template<typename T>
inline TLowPassFilter<T>::LowPassFilter(const unsigned int poles) : TAnalogFilter ()
{
    this->poles = poles;
    setCornerFrequency(1.0e5);
}

template<typename T>
inline TLowPassFilter<T>::LowPassFilter (const unsigned int poles, const double cornerFrequency) : TAnalogFilter ()
{
    this->poles = poles;
    this->cornerFrequency = cornerFrequency;
    setCornerFrequency(cornerFrequency);
}

template<typename T>
inline void TLowPassFilter<T>::setPoles(unsigned int p) noexcept
{
    if (p > 0 and p <= 32 and p % 2 == 0) poles = p;
    setCornerFrequency (cornerFrequency);
}

template<typename T>
inline void TLowPassFilter<T>::setCornerFrequency (double frequency) noexcept
{
    cornerFrequency = frequency;
    // tau = 1.0 / (1.0 + frequency / (0.5 * poles)) * (44100.0 / (sampleRate ? *sampleRate : 44100.0));
    
    if (poles > 0)
    {
        tau = 1.0 / (2.0 * pi * cornerFrequency)
                   * (sampleRate ? *sampleRate : 44100.0)
                   * std::sqrt (std::pow (2.0, 1.0 / poles) - 1.0);
    }
}

template<typename T>
inline void TLowPassFilter<T>::process (double& sample, unsigned int channel) noexcept
{
    cap_ptr = caps[channel];
    for (int p = 0; p < poles; p++, cap_ptr++)
    {
        *cap_ptr += (sample - *cap_ptr) / (1.0 + tau);
        sample = *cap_ptr;
    }
}

template<typename T>
class TResonantLowPassFilter : public LowPassFilter<T> {
public:
    TResonantLowPassFilter();

    TResonantLowPassFilter(const unsigned int poles) : LowPassFilter(poles), resonance(0.0) {}
    
    TResonantLowPassFilter(const unsigned int poles, const double cornerFrequency)
        : LowPassFilter(poles),
          resonance(0.0)
    {
        setCornerFrequency (cornerFrequency);
    }

    void process (double& sample, unsigned int channel) noexcept override;
    
    void setResonance (double res) noexcept;
    
protected:
    double resonance;
    double sampleFeedbackBuffer[32] = {0.0};
};


template<typename T>
inline TResonantLowPassFilter<>T::ResonantLowPassFilter () : LowPassFilter()
{
    resonance = 0.0;
    tau = 1.0;
}

template<typename T>
inline void TResonantLowPassFilter<>T::process (double& sample, unsigned int channel) noexcept {

    sample -= resonance * sampleFeedbackBuffer[channel];
    TLowPassFilter<T>::process(sample, channel);
    sampleFeedbackBuffer[channel] = sample;
    sample *= 1.0 + resonance;

}

template<typename T>
inline void TResonantLowPassFilter<>T::setResonance (double res) noexcept {
    resonance = res * 0.99;
}
