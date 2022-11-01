/*
  ==============================================================================
    FirstOrderAllPass.h
    Created: 17 Mar 2019 10:47:21pm
    Author:  Joonas
  ==============================================================================
*/

#pragma once

#include "SoundObject.hpp"
#include <vector>

class FirstOrderAllPass : public MonoFXProcessor
{
public:
	FirstOrderAllPass();
	~FirstOrderAllPass();

	enum
	{
		COEFFICIENT_PORT,
	};

	void setPort(int port, DspFloatType v)
	{
		if(port == COEFFICIENT_PORT) mC = v; 
	}
	void printPorts() {
		printf("PORTS\n");
		printf("COEFFICIENT_PORT=0\n");
	}
	void    prepare(const int& numChannels);
	void    ProcessBlock(size_t n, DspFloatType * input, DspFloatType * output);
	DspFloatType   Tick(const DspFloatType& sample, const int& channel);
	void    updateCoefficients(const DspFloatType& c);

	void randomize()
	{
		mC = noise.rand();		
	}

private:
	std::vector<DspFloatType> mXh;
	DspFloatType mC    = -.89f;
	DspFloatType mOldC = -.89f;
};

//==============================================================================
inline
FirstOrderAllPass::FirstOrderAllPass() : MonoFXProcessor()
{
}

//==============================================================================
inline
FirstOrderAllPass::~FirstOrderAllPass()
{
}

//==============================================================================
inline
void FirstOrderAllPass::prepare(const int& numChannels)
{
	for (auto channel = 0; channel < numChannels; ++channel)
	{
		mXh.push_back(0.f);
	}
}

//==============================================================================
inline
void FirstOrderAllPass::ProcessBlock(size_t numSamples, DspFloatType * inputs, DspFloatType * outputs)
{
	for (auto channel = 0; channel < 2; ++channel)
	{
		const DspFloatType* input = inputs;
		DspFloatType* output = outputs;

		for (auto sample = 0; sample < numSamples; ++sample)
		{
			// Transposed Direct Form II
			DspFloatType inputSample = input[sample];
			output[sample] = mC * inputSample + mXh[channel];
			mXh[channel] = inputSample - mC * output[sample];
		}
	}
}

//==============================================================================
inline
DspFloatType FirstOrderAllPass::Tick(const DspFloatType& input, const int& channel)
{
	// Transposed Direct Form II
	DspFloatType output = mC * input + mXh[channel];
	mXh[channel] = input - mC * output;
	return output;
}

//==============================================================================
inline
void FirstOrderAllPass::updateCoefficients(const DspFloatType& c)
{
	mC = c;
}