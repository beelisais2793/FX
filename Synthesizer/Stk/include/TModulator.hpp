// THis is Synthlab Modular
#pragma once
#include "SoundAlchemy.hpp"
#include <vector>
#include <memory>

using std::vector;

namespace SoundAlchemy
{

template<typename T>
class Modulator {
public:
    
    inline virtual ~Modulator() {};
    virtual T process() = 0;
    virtual T getOutput() = 0;
    
};

template<typename T>
class ModTarget {

public:
    
    inline virtual ~ModTarget() {};
    
    virtual void setModulator(std::shared_ptr<Modulator> mod) = 0;
    virtual void setModAmount(T amount) = 0; 
};

template<typename T>
class Modulation {
    
public:
     Modulation(std::shared_ptr<Modulator> modulator, std::shared_ptr<ModTarget> target);
     Modulation();
     ~Modulation();
    
    vector<ModTarget*> getTargets();
    
    Modulator* getModulator();
    void addTarget(ModTarget* target);
    void setModulator(Modulator* modulator);
    void process();
    void setEnabled(bool enabled);
    bool isEnabled();
    
    
private:
    
    vector<ModTarget*> targets;
    std::shared_ptr<Modulator> modulator;
    bool enabled = false;
    
};

template<typename T>
Modulation<T>::Modulation() {
}

template<typename T>
Modulation<T>::~Modulation() {
    this->targets.clear();
}

template<typename T>
Modulation<T>::Modulation(std::shared_ptr<Modulator> modulator, std::shared_ptr<ModTarget> target) {
    this->modulator = modulator;
    target->setModulator(modulator);
    this->targets.push_back(target.get());
}

template<typename T>
void Modulation<T>::process() {
    if (this->modulator != NULL) {
        this->modulator.get()->process();
    }
}

template<typename T>
Modulator* Modulation<T>::getModulator() {
    return this->modulator.get();
}

template<typename T>
void Modulation<T>::setModulator(std::shared_ptr<Modulator> modulator) {
    this->modulator = modulator;
}

template<typename T>
void Modulation<T>::addTarget(std::shared_ptr<ModTarget> target) {
    target.get()->setModulator(this->modulator);
    this->targets.push_back(target.get());
}

template<typename T>
vector<ModTarget*> Modulation<T>::getTargets() {
    return targets;
}

template<typename T>
void Modulation<T>::setEnabled(bool enabled) {
    this->enabled = enabled;
}

template<typename T>
bool Modulation<T>::isEnabled() {
    return this->enabled;
}

}

