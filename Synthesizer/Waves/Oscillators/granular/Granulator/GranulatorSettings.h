/*
  ==============================================================================

    GranulatorSettings.h
    Created: 10 Apr 2021 11:12:51am
    Author:  ciaran

  ==============================================================================
*/

#pragma once

class GranulatorSettings {
public:
  GranulatorSettings(int grainSize = 44100, float grainShape = 0.5,
                     float randomness = 0.8, float overlap = 1.0)
      : grainSize{grainSize}, grainShape{grainShape},
        randomness{randomness}, overlap{overlap} {}
  int grainSize;      // grain size (in samples)
  float grainShape;   // [0.0, 1.0] interpolates from square to triangle
  float randomness;   // [0.0, 1.0] how far back in the cache can we pull samples from?
  float overlap;      // [0.5, 2.0] ranges from interlinked to equally spaced
};