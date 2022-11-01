#pragma once
#include <cmath>
#include <array>
#include <vector>
#include <cstdint>
#include <exception>
#include <array>
#include <random>
#include <cassert>
#include <algorithm>
#include <cstring>

#include "Undenormal.hpp"

///////////////////////////////////////////////////////////////////////////////////////////
// Noise kaka
///////////////////////////////////////////////////////////////////////////////////////////
// +/-0.05dB above 9.2Hz @ 44,100Hz
class PinkingFilter
{
	double b0, b1, b2, b3, b4, b5, b6;
public:
	PinkingFilter() : b0(0), b1(0), b2(0), b3(0), b4(0), b5(0), b6(0) {}
	double process(const double s)
	{
        Undenormal denormal;
		b0 = 0.99886 * b0 + s * 0.0555179;
		b1 = 0.99332 * b1 + s * 0.0750759;
		b2 = 0.96900 * b2 + s * 0.1538520;
		b3 = 0.86650 * b3 + s * 0.3104856;
		b4 = 0.55000 * b4 + s * 0.5329522;
		b5 = -0.7616 * b5 - s * 0.0168980;
		const double pink = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + (s * 0.5362)) * 0.11;
		b6 = s * 0.115926;
		return pink;
	}
};

class BrowningFilter
{
double l;
public:
	BrowningFilter() : l(0) {}
	double process(const double s)
	{
        Undenormal denormal;
		double brown = (l + (0.02f * s)) / 1.02f;
		l = brown;
		return brown * 3.5f; // compensate for gain
	}
};

struct WhiteNoiseSource
{
	WhiteNoiseSource() : dist(-1, 1) {}
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;
};

// Full spectrum noise
struct WhiteNoise : public WhiteNoiseSource
{
	float operator()() { return dist(engine); }
};

// Pink noise has a decrease of 3dB/Octave
struct PinkNoise : public WhiteNoiseSource
{
	float operator()() { return f.process(dist(engine)); }
	PinkingFilter f;
};

 // Brown noise has a decrease of 6dB/Octave
struct BrownNoise : public WhiteNoiseSource
{
	float operator()() { return f.process(dist(engine)); }
	BrowningFilter f;
};

// Note! This noise is only valid for 44100 because of the hard-coded filter coefficients
struct NoiseGenerator
{
	enum NoiseType
	{
		WHITE,
		PINK,
		BROWN,
	} noise_type = PINK;
	
	std::vector<float> produce(NoiseType t, int sampleRate, int channels, float seconds)
	{
		int samplesToGenerate = sampleRate * seconds * channels;
		std::vector<float> samples;
		samples.resize(samplesToGenerate);
		
		switch (t)
		{
		case NoiseType::WHITE:
		{
			WhiteNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		case NoiseType::PINK:
		{
			PinkNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		case NoiseType::BROWN:
		{
			BrownNoise n;
			for(int s = 0; s < samplesToGenerate; s++) samples[s] = n();
		} break;
		default: throw std::runtime_error("Invalid noise type");
		}
		return samples;
	}
	
	float Tick() {
		switch (noise_type)
		{
		case NoiseType::WHITE:
		{
			WhiteNoise n;
			return n();
		} break;
		case NoiseType::PINK:
		{
			PinkNoise n;
			return n();
		} break;
		case NoiseType::BROWN:
		{
			BrownNoise n;
			return n();
		} break;
		default: throw std::runtime_error("Invalid noise type");
		}
	}
	void Process(size_t n, float * input, float * output) {
		for(size_t i = 0; i < n; i++) output[i] = input[i]*Tick();
	}
	void Process(float * samples,size_t n) {
		for(size_t i = 0; i < n; i++) samples[i] = samples[i]*Tick();
	}
};
