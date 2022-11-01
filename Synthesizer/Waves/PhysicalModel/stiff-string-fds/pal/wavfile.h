#pragma once

#include <vector>
#include <string>

void writeWavFile(std::string path, std::vector<float> samples, float sampleRate);

bool readWavFile(std::string path, std::vector<float> &samples, int channel, float *sampleRateOut = nullptr);