/*
  ==============================================================================

    ModMatrixConfig.h
    Created: 12 Dec 2016 6:45:42pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef MODMATRIXCONFIG_H_INCLUDED
#define MODMATRIXCONFIG_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModSlotConfig.h"

#include <vector>

using namespace std;

class ModMatrixConfig {

public:
	ModMatrixConfig();
	~ModMatrixConfig();

	void addConfig(ModSlotConfig* config);
	ValueTree getConfiguation();
	ModSlotConfig* getSlotConfig(int i);
	void clearSlots();
	int getNumConfigs();

private:
	vector<ModSlotConfig*> slotConfigs;
    ValueTree config;

};




#endif  // MODMATRIXCONFIG_H_INCLUDED
