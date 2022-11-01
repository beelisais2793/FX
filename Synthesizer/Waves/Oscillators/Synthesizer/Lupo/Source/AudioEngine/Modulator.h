//
//  Modulator.h
//  Trio
//
//  Created by Matthias Pueski on 17.11.16.
//
//

#ifndef Modulator_h
#define Modulator_h
#include "../JuceLibraryCode/JuceHeader.h"

class Modulator {
public:
    
    inline virtual ~Modulator() {};
    virtual float process() = 0;
    virtual float getOutput() = 0;
	
	juce::String getName() {
		return name;
	}

	void setName(juce::String name) {
		this->name = name;
	}

	virtual void setModAmount(float amount) {
		this->modAmount = amount;
	}
	virtual float getModAmount() {
		return this->modAmount;
	}
protected:
	float modAmount = 0;
	juce::String name = "unknown";
};

#endif /* Modulator_h */

