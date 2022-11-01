#pragma once
//#include <JuceHeader.h>

class ADSRData  : public juce::ADSR
{
public:
    void updateADSR (const float attack, const float decay, const float sustain, const float release)
    {
        adsrParams.attack = attack;
        adsrParams.decay = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
        
        setParameters (adsrParams); /// get updated ADSR values
    }

private:
    juce::ADSR::Parameters adsrParams;
};


