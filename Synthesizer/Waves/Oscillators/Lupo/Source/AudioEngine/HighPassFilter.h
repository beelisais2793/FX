//
//  LowPassFilter.hpp
//  Trio
//
//  Created by Matthias Pueski on 16.11.16.
//
//

#ifndef HighPassFilter_hpp
#define HighPassFilter_hpp

#include <stdio.h>
#include "Filter.h"
#include "Modulator.h"
#include "ModTarget.h"

#include "../JuceLibraryCode/JuceHeader.h"

class HighPassFilter : public Filter, public ModTarget {
    
public:
    virtual void coefficients(float sampleRate, float frequency, float resonance) override;
    virtual void process(float *in, float *out,int numSamples);
    virtual void setModulator(Modulator* mod) override;
    
    
    HighPassFilter();
    virtual ~HighPassFilter();
    
private:
    juce::ScopedPointer<juce::IIRFilter> filter1;
    juce::ScopedPointer<juce::IIRFilter> filter2;
    float frequency;
    float resonance;
    JUCE_LEAK_DETECTOR(HighPassFilter);
    
};


#endif /* HighPassFilter_hpp */

