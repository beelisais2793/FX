/*
  ==============================================================================

    ModMatrix.h
    Created: 10 Dec 2016 10:18:41am
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef MODMATRIX_H_INCLUDED
#define MODMATRIX_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "AudioEngine/Modulation.h"
#include <vector>
#include <map>
#include "Model.h"
#include "ModMatrixConfig.h"
#include "DummyModulator.h"
#include "AudioEngine/Modulator.h"

class LupoSynth;
using namespace std;

class ModMatrix : public ChangeBroadcaster {
  
public:
    ModMatrix();
    ~ModMatrix();
    
    void addModulator(Modulator* m);
	void addModTarget(ModTarget* m);
    void registerSource(String source, int id);
    void registerTarget(String target, int id);

	inline void process() {
		for (int i = 0; i < 6; i++) {
			if (this->modulations[i]->isEnabled()) {
				this->modulations[i]->process();
			}
		}
	}

	inline Modulation** getModulations() {
		return modulations;
	}

	inline vector<Modulator*> getModulators()
	{
		return modulators;
	}

	inline vector<ModTarget*> getModTargets()
	{
		return targets;
	}

    
    map<int,String>* getSources();
    map<int,String>* getTargets();
    
    double getSampleRate();
    void setSampleRate(double rate);
    Model* getModel();
	void setConfig(ModMatrixConfig* config);
	ModMatrixConfig* getConfiguration();
    void setModel(Model* model);

	void createDefaultConfig();

	DummyModulator* getDummy() {
		return dummy;
	}

private:
    Modulation** modulations;
	vector<Modulator*> modulators;
	vector<ModTarget*> targets;
    
    map<int,String>* modSources;
    map<int,String>* modTargets;
    
    double sampleRate;
    Model* model;
	ModMatrixConfig* config = nullptr;
	DummyModulator* dummy;
	
};



#endif  // MODMATRIX_H_INCLUDED
