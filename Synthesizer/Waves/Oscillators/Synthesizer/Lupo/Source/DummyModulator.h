#pragma once

#include "AudioEngine/Modulator.h"

class DummyModulator : public Modulator {

public:
	DummyModulator() {

	}    
	
	float process() override {
		return 0;
	};

	float getOutput() override {
		return 0;
	}

};