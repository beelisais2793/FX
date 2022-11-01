#pragma once 

#include <cstdlib>
#include <math.h>
#include "SoundAlchemy.hpp"

namespace SoundAlchemy::Envelopes
{
    enum state_enum {
        idle, attack, decay, sustain, release
    };

    template<typename T>
    struct parameter_struct {
        T attackTime = 1.0;
        T attackSlope = 0.0;
        T decayTime = 1.0;
        T decaySlope = 0.0;
        T sustainLevel = 0.5;
        T releaseTime = 1.0;
        T releaseSlope = 0.0;
    };


    template<typename T>
    class TParametricEnvelope {
    public:

    private:
        T currentValue;
        T releaseInitialValue;
        unsigned long currentStep;
        state_enum currentState;
        state_enum previousState;

        // Amplitude scale
        T maxLevel;
        // Time scale. Essentially, how often is step() called?
        unsigned int stepFrequency;
        parameter_struct<T> parameters;

        void gotoState(state_enum newState);

        T calculateAttackValue(unsigned long currentStep, T time, T slope);

        T calculateDecayValue(unsigned long currentStep, T time, T slope, T targetLevel);

        T calculateReleaseValue(unsigned long currentStep, T time, T slope, T originLevel);

    public:
        TParametricEnvelope(T maxLevel = 10.0, unsigned int stepFrequency = 10000);

        //
        // Setters for all envelope parameters, with reasonable defaults
        //
        void setAttackTime(T time);

        void setAttackSlope(T slope);

        void setDecayTime(T time);

        void setDecaySlope(T slope);

        void setSustainLevel(T level);

        void setReleaseTime(T time);

        void setReleaseSlope(T slope);

        //
        // Get current time _within the current phase_
        // NB This gets reset at the beginning of the A, D and R phases and isn't updated during S
        T getCurrentTime();

        //
        // Even handlers for gate transitions
        // onGateOn will transition the envelop into STATE_ATTACK
        //
        void onGateOn();

        //
        // onGateOff will transition the envelope into STATE_RELEASE
        //
        void onGateOff();

        //
        // step is called at regular intervals to get the current signal value
        //
        T step();

        //
        // Reset the envelope
        //
        void reset();

        T Tick() {
            return step();
        }
        T Tick(T in, T A = 1, T X = 1, T Y = 1) {
            return A*X*Y*in*step();
        }

        //
        // Get the current state.
        //
        state_enum getState();

        //
        // Return current parameters
        parameter_struct<T> getParameters();
    };

    template<typename T>
    TParametricEnvelope<T>::TParametricEnvelope(T maxLevel, unsigned int stepFrequency) {
        this->maxLevel = maxLevel;
        this->stepFrequency = stepFrequency;
        reset();
    }

    template<typename T>
    void TParametricEnvelope<T>::reset() {
        currentState = idle;
        previousState = idle;
        currentValue = 0.0;
        currentStep = 0;
    }

    template<typename T>
    void TParametricEnvelope<T>::setAttackTime(T time) {
        parameters.attackTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setAttackSlope(T slope) {
        parameters.attackSlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setDecayTime(T time) {
        parameters.decayTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setDecaySlope(T slope) {
        parameters.decaySlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setReleaseTime(T time) {
        parameters.releaseTime = time;
    }

    template<typename T>
    void TParametricEnvelope<T>::setReleaseSlope(T slope) {
        parameters.releaseSlope = slope;
    }

    template<typename T>
    void TParametricEnvelope<T>::setSustainLevel(T level) {
        if (level>maxLevel) {
            parameters.sustainLevel = 1.0;
        } else {
            parameters.sustainLevel = level/maxLevel;
        }
    }

    template<typename T>
    parameter_struct<T> TParametricEnvelope<T>::getParameters() {
        return parameters;
    }

    template<typename T>
    state_enum TParametricEnvelope<T>::getState() {
        return currentState;
    }

    //
    // step is called periodically to update the inner state and the currentValue of this envelope. It returns the current value
    template<typename T>
    T TParametricEnvelope<T>::step() {
        //TODO Implement value update
        switch (currentState) {
            case idle:
                break;
            case attack:
                if (previousState == idle) {
                    currentStep = 0;
                    previousState = attack;
                }
                currentValue = calculateAttackValue(currentStep, parameters.attackTime, parameters.attackSlope);
                if (currentValue >= 1.0) {
                    gotoState(decay);
                } else {
                    currentStep++;
                }
                break;
            case decay:
                if (previousState != decay) {
                    currentStep = 0;
                    previousState = decay;
                }
                currentValue = calculateDecayValue(currentStep, parameters.decayTime, parameters.decaySlope,
                                                parameters.sustainLevel);
                if (currentValue <= parameters.sustainLevel) {
                    gotoState(sustain);
                } else {
                    currentStep++;
                }
                break;
            case sustain:
                if (previousState != sustain) {
                    previousState = sustain;
                    currentValue = parameters.sustainLevel;
                }
                break;
            case release:
                if (previousState != release) {
                    currentStep = 0;
                    previousState = release;
                    releaseInitialValue = currentValue;
                }
                currentValue = calculateReleaseValue(currentStep, parameters.releaseTime, parameters.releaseSlope,
                                                    releaseInitialValue);
                if (currentValue < 0.0) {
                    currentValue = 0.0;
                    gotoState(idle);
                } else {
                    currentStep++;
                }
        }
        return currentValue * maxLevel;
    }

    template<typename T>
    T TParametricEnvelope<T>::getCurrentTime() {
        return currentStep / (T)stepFrequency;
    }

    template<typename T>
    void TParametricEnvelope<T>::onGateOn() {
        gotoState(attack);
    }

    template<typename T>
    void TParametricEnvelope<T>::onGateOff() {
        gotoState(release);
    }

    template<typename T>
    void TParametricEnvelope<T>::gotoState(state_enum newState) {
        previousState = currentState;
        currentState = newState;
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateAttackValue(unsigned long currentStep, T time, T slope) {
        return std::pow(getCurrentTime() / time, std::pow(2.0, -slope));
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateDecayValue(unsigned long currentStep, T time, T slope, T targetLevel) {
        return std::pow(getCurrentTime() / time, std::pow(2.0, -slope)) * (targetLevel - 1.0) + 1.0;
    }

    template<typename T>
    T TParametricEnvelope<T>::calculateReleaseValue(unsigned long currentStep, T time, T slope, T originLevel) {
        return originLevel * ( 1- std::pow(getCurrentTime() / time, std::pow(2.0, -slope))) ;
    }
}