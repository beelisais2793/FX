/*
  ==============================================================================

    MessageBus.h
    Created: 16 May 2018 8:52:48pm
    Author:  Matthias Pueski

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Topic.h"
#include "BusListener.h"
#include <map>
#include <vector>

class MessageBus {
public:
    
    MessageBus() {
		listener = new std::map<juce::String, std::vector<BusListener*>>();
		topics = new std::map<juce::String, Topic*>();
    }
    
    ~MessageBus() {
        for(std::map<String,Topic*>::iterator it = topics->begin(); it != topics->end();++it) {
            delete (*it).second;
        }
        topics->clear();
		delete topics;
		listener->clear();
		delete listener;
    }
    Topic* getTopic(juce::String name);
    juce::StringArray getTopics();

    void updateTopic(String name, float value);
    void addListener(String name, BusListener* listener);
    
private:

    std::map<juce::String,std::vector<BusListener*>>* listener;
    std::map<juce::String,Topic*>* topics;
    
};
