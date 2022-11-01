/*
 ==============================================================================
 
 ModMatrixConfig.cpp
 Created: 12 Dec 2016 6:45:42pm
 Author:  Matthias Pueski
 
 ==============================================================================
 */

#include "ModMatrixConfig.h"

ModMatrixConfig::ModMatrixConfig()
{
    this->config =  ValueTree(Identifier("modMatrix"));
}

ModMatrixConfig::~ModMatrixConfig()
{
	if (slotConfigs.size() > 0) {
		for (std::vector<ModSlotConfig*>::iterator it = slotConfigs.begin(); it != slotConfigs.end(); ++it) {
			delete *it;
		}
	}

}

void ModMatrixConfig::addConfig(ModSlotConfig* config)
{
    this->slotConfigs.push_back(config);
}


ValueTree ModMatrixConfig::getConfiguation()
{
    config.removeAllChildren(nullptr);
    
    for (int i = 0; i < slotConfigs.size(); i++) {
        
        ValueTree slotConfig = ValueTree(Identifier("slotConfig"));
        
        slotConfig.setProperty("sourceId", slotConfigs.at(i)->sourceId, nullptr);
        slotConfig.setProperty("targetId", slotConfigs.at(i)->targetId, nullptr);
        slotConfig.setProperty("amount", slotConfigs.at(i)->amount, nullptr);
        slotConfig.setProperty("enabled", slotConfigs.at(i)->enabled, nullptr);
        
        config.addChild(slotConfig, -1, nullptr);
    }
    
    return this->config;
}


ModSlotConfig* ModMatrixConfig::getSlotConfig(int i)
{
    return slotConfigs.at(i);
}

void ModMatrixConfig::clearSlots()
{
    this->slotConfigs.clear();
}

int ModMatrixConfig::getNumConfigs()
{
    return (int)slotConfigs.size();
}
