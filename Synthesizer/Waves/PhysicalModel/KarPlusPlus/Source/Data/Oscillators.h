#include <math.h>
#include <cmath>

#ifndef Oscillators_h
#define Oscillators_h

// PHASOR PARENT Class
class Phasor
{
public:
    Phasor() {}
    
    virtual ~Phasor()
    {
    }
    
    // Update phase and output next sample
    float process()
    {
        phase += phaseDelta;
        
        if (phase > 1.0f)
            phase -= 1.0f;
        
        return output (phase);
    }
    
    // this function can be replaced
    virtual float output(float p)
    {
        return p;
    }
    
    void setSampleRate(float sr)
    {
        sampleRate = sr;
    }
    
    void setFrequency(float freq)
    {
        frequency = freq;
        phaseDelta = frequency / sampleRate;
    }
    
private:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
};

    
class Oscillator : public Phasor
{
public:

    void setWaveType (const int choice)
    {
        waveType = choice;
    }
    
    float output (float p) override
    {
        // SINE
        if (waveType == 0) {
            return std::sin (p * 2.0f * 3.141592653);
        }
        
        // TRIANGLE
        if (waveType == 1) {
            return fabsf (p - 0.5f) - 0.5f;
        }
        
        // SQUARE
        if (waveType == 2) {
            float outVal = 0.5;
            if (p > 0.5) // Fixed Pulsewidth
                outVal = -5;
            return outVal;
        }
        
        // SAW
        if (waveType == 3) {
            return p;
        }
        
        // WHITE NOISE
        if (waveType == 4) {
            float noise = (random.nextFloat() - 0.5) * 2.0f;
            return noise;
        }
        
        // BREAK PROGRAM OTHERWISE
        else
        {
            return p;
            jassertfalse;
        }
    }
    
    
private:
    juce::Random random;
    
    int waveType;
};


#endif /* Oscillators_h */
