//
//  MultimodeFilter.hpp
//  Trio
//
//  Created by Matthias Pueski on 01.12.16.
//
//

#ifndef MultimodeFilter_hpp
#define MultimodeFilter_hpp

#include <stdio.h>
#include "Filter.h"
#include "LowPassFilter.h"
#include "HighPassFilter.h"
#include "StereoEffect.h"
#include "ModTarget.h"

#include "../JuceLibraryCode/JuceHeader.h"

class MultimodeFilter : public Filter, public StereoEffect, public ModTarget {

public:
    enum Mode {
        HIGHPASS,
        LOWPASS
    };
    
    MultimodeFilter();
    virtual ~MultimodeFilter();
    
    virtual void coefficients(float sampleRate,float frequency, float resonance) override;
    virtual void processStereo(float *const left, float *const right, const int numSamples) override;
	virtual void processMono(int channel, float *const samples, const int numSamples);
    virtual void setModulator(Modulator* mod) override;
    void setMode(Mode mode);
	void setKeyTrack(int track);

private:
    
    ScopedPointer<LowPassFilter> lowPassLeftStage1;
    ScopedPointer<LowPassFilter> lowPassRightStage1;

	ScopedPointer<LowPassFilter> lowPassLeftStage2;
	ScopedPointer<LowPassFilter> lowPassRightStage2;

    ScopedPointer<HighPassFilter> highPassLeft;
    ScopedPointer<HighPassFilter> highPassRight;
    
    Mode mode;

	int keyTrack;

    JUCE_LEAK_DETECTOR(MultimodeFilter);
    
};

#endif /* MultimodeFilter_hpp */