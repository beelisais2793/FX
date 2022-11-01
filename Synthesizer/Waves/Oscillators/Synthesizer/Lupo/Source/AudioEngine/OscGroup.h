/*
  ==============================================================================

    OscGroup.h
    Created: 14 Mar 2020 11:53:07am
    Author:  mpue

  ==============================================================================
*/

#pragma once
#include "MultimodeOscillator.h"
#include "Modulator.h"
#include "ModTarget.h"
#include <vector>

class OscGroup : public ModTarget {

public:
	OscGroup();
	~OscGroup();
	virtual void setModulator(Modulator* mod);
	void addTarget(ModTarget* target);
	std::vector<ModTarget*> getTargets();
private:
	std::vector<ModTarget*> targets;
	
	Modulator* modulator = nullptr;
};