#include "SC_PlugIn.h"
#include <math.h>

//  Created by So Oishi on 30/10/14.


// BASIC ADMINISTRATION

static InterfaceTable *ft;

struct HarmonicOsc : public Unit
{
    float displacement;
    float velocity;
    float acceleration;
    float force;
};

// declare unit generator functions
extern "C"
{
    void HarmonicOsc_Ctor(HarmonicOsc *unit);
    void HarmonicOsc_next(HarmonicOsc *unit, int inNumSamples);
};

//////////////////////////////////////////////////////////////////

// CONSTRUCTOR

void HarmonicOsc_Ctor(HarmonicOsc *unit)
{
    SETCALC(HarmonicOsc_next);

    unit->displacement = ZIN0(0);
    unit->velocity = ZIN0(1);
    unit->acceleration = ZIN0(2);
    unit->force = ZIN0(3);

    HarmonicOsc_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

// UGEN CALCULATION

void HarmonicOsc_next(HarmonicOsc *unit, int inNumSamples)
{
    float *out = ZOUT(0);
    
    float displacement = unit->displacement;
    float velocity = unit->velocity;
    float acceleration = unit->acceleration;
    float force = unit->force;
    float mass = ZIN0(4);
    float b = ZIN0(5);
    
    LOOP(inNumSamples,
          acceleration = force / mass;
          velocity = velocity + (acceleration / SAMPLERATE);
          displacement = displacement + (velocity / SAMPLERATE);
          force = - b * displacement;
          ZXP(out) = displacement;
         )
    unit->displacement = displacement;
    unit->velocity = velocity;
    unit->acceleration = acceleration;
    unit->force = force;
}

////////////////////////////////////////////////////////////////////

// LOAD FUNCTION

// the load function is called by the host when the plugin is loaded
// Unless the flag for STATIC_PLUGINS is on, the argument (PB_SINE) does not seem to matter
// it can also be omitted (except when STATIC-PLUGINS are desired)

PluginLoad(HarmonicOsc)
{
    ft = inTable;
    
    DefineSimpleUnit(HarmonicOsc);
}

////////////////////////////////////////////////////////////////////

