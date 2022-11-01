#pragma once
#include "FeedbackDelay.h"
#include "NonLinAllpass.h"
#include <cmath> // Used for tanh()

// ====== KARPLUS STRONG =======
class KarplusStrong : public Delay
{
public:
    // ====== DAMPENING =======
    void setSamplerate (float samplerate)
    {
        sr = samplerate;
    }
    
    // ====== DAMPENING =======
    void setDampening (float damp) // Takes values between 0-1
    {
        float filterFreq = (damp + 0.01) // Prevent filter hitting 0 Hz
        * (sr / 2)  // Multiplies dampening by Nyquist Frequency
                            * 0.99f; // Get practical value

        dampen.setCoefficients (juce::IIRCoefficients::makeLowPass(sr, filterFreq, 1.0f));
        dcBlock.setCoefficients (juce::IIRCoefficients::makeHighPass(sr, filterFreq));
    }
    
    // ====== PITCH WITH INSTABILITY =======
    void setPitch (float freq)
    {
        float delayFreq = sr / freq; // Get delaytime from frequency
        setDelayTimeInSamples (delayFreq); // Set delaytime in original function
        
        float noise = (random.nextFloat() - 0.5) * 2;
        allpass.setCoefficients (noise, noise);
    }
    
    // ====== PROCESS =======
    float process (float& inSamp) override
    {
        float outVal = readVal();
        
        float currentSample = allpass.process (outVal);
        
        currentSample = clip (currentSample);
        currentSample = loPass (currentSample); // process through IRR Filter
        
        writeVal (inSamp + feedback * currentSample); // Feedback scales output back into input
        float floor (currentSample); // Calculate interpolation
        return floor;
    }
    
    // ====== TRANSFER FUNCTIONS =======
    float fold (float& inSamp)
    {
        if (inSamp > 1) {
            inSamp = inSamp - 1;
            inSamp = 1 - inSamp;
        }
        if (inSamp < -1) {
            inSamp = inSamp + 1;
            inSamp = 1 + inSamp;
        }
        
        return inSamp;
    }
    
    float clip (float& inSamp)
    {
        if (inSamp > 1) {
            inSamp = 1;
        }
        if (inSamp < -1) {
            inSamp = -1;
        }
        
        return inSamp;
    }
    
    float loPass (float& inSamp)
    {
        float currentSample = dampen.processSingleSampleRaw (inSamp); // process through IRR Filter
        dampen.processSingleSampleRaw (currentSample);
        dcBlock.processSingleSampleRaw (currentSample);
        
        return currentSample;
    }

private:
    juce::IIRFilter dampen, dcBlock;
    juce::Random random;
    
    // ====== ALLPASS =======
    NonLinearAllpass allpass;
};
