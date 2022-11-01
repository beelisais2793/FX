/*
 ==============================================================================
 
 ModSlotConfig.cpp
 Created: 12 Dec 2016 6:45:21pm
 Author:  Matthias Pueski
 
 ==============================================================================
 */

#include "ModSlotConfig.h"

ModSlotConfig::ModSlotConfig()
{
}

ModSlotConfig::ModSlotConfig(int sourceId, int targetId, float amount1) {
    this->sourceId = sourceId;
    this->targetId = targetId;
   
    this->amount = amount1;    
}

ModSlotConfig::~ModSlotConfig()
{
}

void ModSlotConfig::setSourceId(int id)
{
    this->sourceId = id;
}

int ModSlotConfig::getSourceId()
{
    return this->sourceId;
}

void ModSlotConfig::setTargetId(int id)
{
    this->targetId = id;
}

int ModSlotConfig::getTargetId()
{
    return this->targetId;
}


void ModSlotConfig::setAmount(float amount)
{
    this->amount = amount;
}

float ModSlotConfig::getAmount()
{
	return amount;
}


bool ModSlotConfig::isSlotEnabled()
{
    return enabled;
}

void ModSlotConfig::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

/*
ValueTree* ModSlotConfig::getSlotConfig()
{
    ScopedPointer<ValueTree> slotConfig = new ValueTree(Identifier("slotConfig"));
    
    slotConfig->setProperty("sourceId", this->sourceId, nullptr);
    slotConfig->setProperty("targetId1", this->targetId1, nullptr);
    slotConfig->setProperty("targetId2", this->targetId2, nullptr);
    slotConfig->setProperty("amount1", this->amount1, nullptr);
    slotConfig->setProperty("amount2", this->amount2, nullptr);
    slotConfig->setProperty("enabled", this->enabled, nullptr);
 
    return slotConfig.release();
    
}
*/