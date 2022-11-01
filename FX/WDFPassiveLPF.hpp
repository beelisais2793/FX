#pragma once

// passive 2nd order RC smoother
#include <chowdsp_wdf/chowdsp_wdf.h>

using namespace chowdsp::wdft;

class PassiveLPF
{
public:
    PassiveLPF() = default;

    void prepare (DspFloatType sampleRate)
    {
        C1.prepare ((DspFloatType) sampleRate);
        C2.prepare ((DspFloatType) sampleRate);
    }

    void reset()
    {
        C1.reset();
        C2.reset();
    }

    void setCutoffFrequency (DspFloatType cutoff)
    {
        const auto Cap = 5.0e-8f;
        const auto Res = 1.0f / (2 * M_PI * cutoff * Cap);

        R1.setResistanceValue (Res);
        R2.setResistanceValue (Res);
        C1.setCapacitanceValue (Cap);
        C2.setCapacitanceValue (Cap);
    }

    inline DspFloatType processSample (DspFloatType x)
    {
        Vs.setVoltage (x);

        Vs.incident (S2.reflected());
        auto y = voltage<DspFloatType> (C2);
        S2.incident (Vs.reflected());

        return y;
    }

private:
    ResistorT<DspFloatType> R1 { 1000.0f };
    ResistorT<DspFloatType> R2 { 1000.0f };

    CapacitorT<DspFloatType> C1 { 5.0e-8f, 48000.0f };
    CapacitorT<DspFloatType> C2 { 5.0e-8f, 48000.0f };

    WDFSeriesT<DspFloatType, decltype (R2), decltype (C2)> S1 { R2, C2 };
    WDFParallelT<DspFloatType, decltype (C1), decltype (S1)> P1 { C1, S1 };
    WDFSeriesT<DspFloatType, decltype (R1), decltype (P1)> S2 { R1, P1 };

    IdealVoltageSourceT<DspFloatType, decltype (S2)> Vs { S2 };
};

