#pragma once 

#include "resampler.cc"   // define struct PluginLV2
#include "resampler-table.cc"   // define struct PluginLV2
#include "resampler.h"

class SimpleResampler {
 private:
    Resampler r_up, r_down;
    int m_fact;
 public:
    SimpleResampler(): r_up(), r_down(), m_fact() {}
    void setup(int sampleRate, unsigned int fact);
    void up(int count, float *input, float *output);
    void down(int count, float *input, float *output);
};

void SimpleResampler::setup(int sampleRate, unsigned int fact)
{
	m_fact = fact;
	const int qual = 16; // resulting in a total delay of 2*qual (0.7ms @44100)
	// upsampler
	r_up.setup(sampleRate, sampleRate*fact, 1, qual);
	// k == inpsize() == 2 * qual
	// pre-fill with k-1 zeros
	r_up.inp_count = r_up.inpsize() - 1;
	r_up.out_count = 1;
	r_up.inp_data = r_up.out_data = 0;
	r_up.process();
	// downsampler
	r_down.setup(sampleRate*fact, sampleRate, 1, qual);
	// k == inpsize() == 2 * qual * fact
	// pre-fill with k-1 zeros
	r_down.inp_count = r_down.inpsize() - 1;
	r_down.out_count = 1;
	r_down.inp_data = r_down.out_data = 0;
	r_down.process();
}

void SimpleResampler::up(int count, float *input, float *output)
{
	r_up.inp_count = count;
	r_up.inp_data = input;
	r_up.out_count = count * m_fact;
	r_up.out_data = output;
	r_up.process();
	assert(r_up.inp_count == 0);
	assert(r_up.out_count == 0);
}

void SimpleResampler::down(int count, float *input, float *output)
{
	r_down.inp_count = count * m_fact;
	r_down.inp_data = input;
	r_down.out_count = count; // +1 == trick to drain input
	r_down.out_data = output;
	r_down.process();
	//assert(r_down.inp_count == 0);
	//assert(r_down.out_count == 1);
}