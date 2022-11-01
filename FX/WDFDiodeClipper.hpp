#pragma once

#include <chowdsp_wdf/chowdsp_wdf.h>
using namespace chowdsp::wdft;

namespace Analog
{
    class WDFDiodeClipper
    {
    public:
        WDFDiodeClipper() = default;

        void prepare (DspFloatType sampleRate)
        {
            C1.prepare ((DspFloatType) sampleRate);
        }

        void reset()
        {
            C1.reset();
        }

        void setCircuitParams (DspFloatType cutoff)
        {
            constexpr auto Cap = 47.0e-9f;
            const auto Res = 1.0f / (2 * M_PI * cutoff * Cap);

            C1.setCapacitanceValue (Cap);
            R1.setResistanceValue (Res);
        }

        void setCircuitElements (DspFloatType res, DspFloatType cap)
        {
            C1.setCapacitanceValue (cap);
            R1.setResistanceValue (res);
        }

        inline DspFloatType processSample (DspFloatType x)
        {
            Vs.setVoltage (x);

            dp.incident (P1.reflected());
            auto y = voltage<DspFloatType> (C1);
            P1.incident (dp.reflected());

            return y;
        }

    private:
        ResistorT<DspFloatType> R1 { 4700.0f };
        ResistiveVoltageSourceT<DspFloatType> Vs;
        WDFSeriesT<DspFloatType, decltype (Vs), decltype (R1)> S1 { Vs, R1 };

        CapacitorT<DspFloatType> C1 { 47.0e-9f };
        WDFParallelT<DspFloatType, decltype (S1), decltype (C1)> P1 { S1, C1 };

        // GZ34 diode pair
        DiodePairT<DspFloatType, decltype (P1)> dp { P1, 2.52e-9f };
    };
}