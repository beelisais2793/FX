/*
  ==============================================================================

    Modulation.h
    Created: 10 Dec 2016 9:55:40am
    Author:  Matthias Pueski

  ==============================================================================
*/

#ifndef MODULATION_H_INCLUDED
#define MODULATION_H_INCLUDED

#include "Modulator.h"
#include "ModTarget.h"
#include <vector>


using namespace std;

class Modulation {
    
    
public:
     Modulation(Modulator* modulator, ModTarget *target);
     Modulation();
     ~Modulation();
   
    Modulator* getModulator();
    void setTarget(ModTarget* target);
    ModTarget* getTarget();
	void clearTarget();
    void setModulator(Modulator* modulator);
    void process();
    void setEnabled(bool enabled);
    bool isEnabled();
	
	
    
private:
    
    
    Modulator* modulator = nullptr;
	ModTarget* target = nullptr;
    bool enabled = true;
	
};




#endif  // MODULATION_H_INCLUDED
