#pragma once

namespace DSPFX
{

    struct FxSpectrumAnalyzer
    {
        SpectrumAnalyzer *spc;

        FxSpectrumAnalyzer(size_t len, DspFloatType sr=sampleRate) {
            spc = SpectrumAnalyzerInitD(len,sr);
            assert(spc != NULL);
        }
        ~FxSpectrumAnalyzer() {
            if(spc) SpectrumAnalyzerFreeD(spc);
        }
        DspFloatType getCentroid() {
            return SpectrumAnalyzerCentroidD(spc);
        }
        DspFloatType getSpread() {
            return SpectrumAnalyzerSpreadD(spc);
        }
        DspFloatType getSkewness() {
            return SpectrumAnalyzerSkewnesD(spc);
        }
        DspFloatType getKurtosis() {
            return SpectrumAnalyzerKurtosisD(spc);
        }
        // todo : should be able to get magnitude and phase
        // should be able to set the bins and inverse it to samples too    
        void Analyze(DspFloatType * signal) {
            SpectrumAnalyzerAnalyzeD(spc,signal);
        }
    };
}
