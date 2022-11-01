#pragma once


#include "AudioEffectsSuite/DelayEffects/DelayEffects.h"
#include "AudioEffectsSuite/FilterEffects/FilterEffects.h"


float TimeToSamples(float t, float sampleRate) { return t * sampleRate; }
