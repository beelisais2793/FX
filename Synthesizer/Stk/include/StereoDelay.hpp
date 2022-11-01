#pragma once

#include "BasicDelayLine.h"

class StereoEffect {
    
public:
    inline virtual ~StereoEffect() {};
    virtual void processStereo(float *const left, float *const right, const int numSamples) = 0;
    inline void setEnabled(bool _enabled) {
        this->enabled = _enabled;
    }

protected:
    bool enabled = true;
    
};

class StereoDelay : public StereoEffect {
    
public:
    StereoDelay();
    ~StereoDelay();
    
    enum Channel {
        LEFT,
        RIGHT
    };
    
    void processStereo (float* const left, float* const right, const int numSamples);

    void setDelay(Channel channel, float time);
    void setFeedback(Channel channel, float fb);
    void setMix(Channel channel, float mix);
    void setByPass(bool bp);

    void resetDelay();
    
private:
    BasicDelayLine delayLeft;
    BasicDelayLine delayRight;
    
};

StereoDelay::StereoDelay() {
    delayLeft.setMix(50);
    delayLeft.setDelay(500);
    delayLeft.setFeedback(50);
    //delayLeft.setUseExternalFeedback(false);
        
    delayRight.setMix(50);
    delayRight.setDelay(375);
    delayRight.setFeedback(50);
    //delayRight.setUseExternalFeedback(false);
}

StereoDelay::~StereoDelay() {

}

void StereoDelay::processStereo(float *const left, float *const right, const int numSamples) {
	for (int sample = 0; sample < numSamples; ++sample) {
		left[sample]  = delayLeft.next(left[sample]);
		right[sample] = delayRight.next(right[sample]);
	}
}

void StereoDelay::setDelay(StereoDelay::Channel channel, float time) {

	// Logger::getCurrentLogger()->writeToLog("Setting delayTime for channel " + String(channel) + " to " + String(time) + "ms");

    if (channel == LEFT) {
            delayLeft.setDelay(time);
    }
    else if (channel == RIGHT) {
            delayRight.setDelay(time);
    }
}

void StereoDelay::setMix(StereoDelay::Channel channel, float mix) {
    if (channel == LEFT) {
            delayLeft.setMix(mix);
    }
    else if (channel == RIGHT) {
            delayRight.setMix(mix);
    }
}

void StereoDelay::setFeedback(StereoDelay::Channel channel, float fb) {
    if (channel == LEFT) {
            delayLeft.setFeedback(fb);
    }
    else if (channel == RIGHT) {
            delayRight.setFeedback(fb);
    }
}

void StereoDelay::resetDelay() {
     delayLeft.resetDelay();
     delayRight.resetDelay();
    
}


