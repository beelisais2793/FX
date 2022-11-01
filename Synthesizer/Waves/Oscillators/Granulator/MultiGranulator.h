/*
  ==============================================================================

    MultiGranulator.h
    Created: 4 Apr 2021 7:01:04pm
    Author:  ciaran

  ==============================================================================
*/

#pragma once

#include "Granulator.h"
#include "GranulatorSettings.h"
#include "RollingCache.h"
#include <JuceHeader.h>

class MultiGranulator {
public:
  MultiGranulator();
  MultiGranulator(double sampleRate, int samplesPerBlock,
                  GranulatorSettings *settings, RollingCache *cache);

  // read a full block of samples into tempBuf and return it
  std::vector<float> read();

  // PRE: ringBuf has enough space to contain entirety of the requested channel
  // fill ringBuf with the entirety of the specified channel of specified source
  void fill(juce::AudioBuffer<float> &source);

  void set_voice_count(uint count);

  int voiceCount = 1;
  GranulatorSettings *settings;
  RollingCache *cache;
  juce::Random random;

private:
  static const int MAX_GRANULATOR_COUNT = 16;
  static const float
      MIN_RAND_MULTI_VOICE; // minimum rand when >1 voices (prevents hard-sync)
  std::array<Granulator, MAX_GRANULATOR_COUNT> granulators;
  std::vector<float> tempBuf; // resized by constructor to blocksPerSample
  int samplesPerBlock;
};