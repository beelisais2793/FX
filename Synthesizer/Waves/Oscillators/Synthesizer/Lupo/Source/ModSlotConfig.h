/*
  ==============================================================================

    ModSlotConfig.h
    Created: 12 Dec 2016 6:45:21pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef MODSLOTCONFIG_H_INCLUDED
#define MODSLOTCONFIG_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class ModSlotConfig {
    

    
public:
	
	ModSlotConfig();
	ModSlotConfig(int sourceId, int targetId, float amount);
	~ModSlotConfig();

	void setSourceId(int id);
	int getSourceId();

	void setTargetId(int id);
	int getTargetId();

	void setAmount(float amount);
	float getAmount();
	
	bool isSlotEnabled();
	void setEnabled(bool enabled);

	// ValueTree* getSlotConfig();

    friend class ModMatrixConfig;
    
private:
	int sourceId;
	int targetId;   
	float amount = 0;
	bool enabled;

};



#endif  // MODSLOTCONFIG_H_INCLUDED
