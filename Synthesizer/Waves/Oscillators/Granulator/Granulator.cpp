/*
  ==============================================================================

    Granulator.cpp
    Created: 29 Mar 2021 11:40:26pm
    Author:  ciaran

  ==============================================================================
*/

#include "Granulator.h"
#include "../Data/ParamData.h"
#include "GranulatorSettings.h"
#include "RollingCache.h"

Granulator::Granulator() {}
Granulator::Granulator(double sampleRate, int samplesPerBlock,
                       GranulatorSettings *settings, RollingCache *cache)
    : settings{settings}, cache{cache} {
  outBuf.resize(samplesPerBlock);
  grain.resize(GRAIN_SIZE_PARAM_DATA.max * sampleRate);
}

std::vector<float> Granulator::read(int totalSamples, int grainSize,
                                    float overlap, float randomness) {
  int samplesToFill = totalSamples;
  int samplesFilled = 0;

  if (!cache->is_full()) {
    for (; samplesFilled < samplesToFill; ++samplesFilled)
      outBuf[samplesFilled] = 0.f;
    return outBuf;
  }

  int maxOffset = (cache->get_capacity() - grainSize) * randomness;
  for (; samplesFilled < totalSamples;
       ++samplesFilled, --oh.samplesToNextGrain) {
    float samp = 0;
    if (oh.samplesToNextGrain <= 0) {
      auto offset =
          maxOffset <= 0
              ? 0
              : random.nextInt(
                    maxOffset); // conditional prevents assertion failure
      cache->read_chunk(grainSize, offset, grain);
      apply_ramp(grain, grainSize);

      oh.samplesToNextGrain = grainSize * overlap;

      for (int i = 0; i < grainSize; ++i) {
        if (i < oh.data.size())
          oh.data[i] += grain[i];
        else
          oh.data.push_back(grain[i]);
      }
    }

    if (!oh.data.empty()) {
      samp += oh.data.front();
      oh.data.pop_front();
    }
    outBuf[samplesFilled] = samp;
  }

  return outBuf;
}

std::vector<float> Granulator::read(int totalSamples) {
  return read(totalSamples, settings->grainSize, settings->overlap,
              settings->randomness);
}

void Granulator::clear_overhang() { oh = {}; }

void Granulator::apply_ramp(std::vector<float> &dest, int length) {
  int ramp_length = float(length) * settings->grainShape; // truncated
  float inc = 1.0f / float(ramp_length);
  for (int i = 0; i < ramp_length; ++i) {
    float mult = inc * i;

    // account for envelope's effect on average amplitude
    mult *= (settings->grainShape + 1.f);

    dest[i] *= mult;
    dest[length - 1 - i] *= mult;
  }
}