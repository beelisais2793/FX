

#include "FirLpf.h"

FirLpf::FirLpf(int fs, int numChannels, int samplesPerFame, int order)
: filterWindow(dsp::WindowingFunction<float>::blackman)
{
    filterOrder = order;
    
    cutoff = new AudioParameterFloat("lpf_fc", "FIR Lowpass cutoff", { 20.f, float(fs/2), 1.0f }, float(fs/2));
    
    dsp::ProcessSpec spec = {double(fs), (uint32)samplesPerFrame, (uint32)numChannels};
    filter.state = dsp::FilterDesign<float>::designFIRLowpassWindowMethod (cutoff->get(), fs, filterOrder, filterWindow); // filter design
    filter.prepare(spec);

    *cutoff = 60;
}

FirLpf::~FirLpf()
{
    delete cutoff;
}

void FirLpf::process(AudioBuffer<float>& buff)
{	
    dsp::AudioBlock<float> block(buff); // init audio block with output buffer
    filter.process(dsp::ProcessContextReplacing<float>(block)); // process block
}

void FirLpf::setCutoff(const float& fc)
{
    *cutoff = fc;
}

