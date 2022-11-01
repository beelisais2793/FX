/*
  ==============================================================================

    MultiGranulator.cpp
    Created: 4 Apr 2021 7:01:04pm
    Author:  ciaran

  ==============================================================================
*/

#include "MultiGranulator.h"
#include "GranulatorSettings.h"

MultiGranulator::MultiGranulator(){}; // don't use the default constructor!

MultiGranulator::MultiGranulator(double sampleRate, int samplesPerBlock,
                                 GranulatorSettings *settings,
                                 RollingCache *cache)
    : settings{settings}, cache{cache}, samplesPerBlock{samplesPerBlock} {
  for (int g = 0; g < granulators.size(); ++g)
    granulators[g] = {sampleRate, samplesPerBlock, settings, cache};
  tempBuf.resize(samplesPerBlock);
};

const float MultiGranulator::MIN_RAND_MULTI_VOICE = 0.01;

std::vector<float> MultiGranulator::read() {
  int gs = settings->grainSize;
  float ov = settings->overlap;
  float rand = settings->randomness < MIN_RAND_MULTI_VOICE && voiceCount > 1
                   ? MIN_RAND_MULTI_VOICE
                   : settings->randomness;
  for (int g = 0; g < granulators.size(); ++g) {
    std::vector<float> voice =
        granulators[g].read(samplesPerBlock, gs, ov, rand);
    if (g < voiceCount) {
      for (int s = 0; s < samplesPerBlock; ++s) {
        if (g == 0)
          tempBuf[s] = 0.f;
        tempBuf[s] += voice[s] / sqrt(double(voiceCount));
      }
    }
  }

  return tempBuf;
}

void MultiGranulator::fill(juce::AudioBuffer<float> &source) {
  for (int samp = 0; samp < source.getNumSamples(); ++samp) {
    tempBuf[samp] = 0;
  }

  for (int ch = 0; ch < source.getNumChannels(); ++ch) {
    auto *lineIn = source.getReadPointer(ch);
    for (int samp = 0; samp < source.getNumSamples(); ++samp) {
      tempBuf[samp] += lineIn[samp];
    }
  }

  for (int samp = 0; samp < source.getNumSamples(); ++samp) {
    cache->write(tempBuf[samp]);
  }
}

void MultiGranulator::set_voice_count(uint count) {
  voiceCount = count < MAX_GRANULATOR_COUNT ? count : MAX_GRANULATOR_COUNT;
  for (int g = 0; g < granulators.size(); ++g) {
    granulators[g].clear_overhang();
  }
}