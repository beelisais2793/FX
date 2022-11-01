#pragma once

namespace Filters::IIR
{
    BiquadTypeI AnalogBiquadTypeI(const BiquadSection &section, DspFloatType fc, DspFloatType fs, bool odd = false)
    {
        BiquadSection ns = section;

        prewarp(&ns.z[0], &ns.z[1], &ns.z[2], fc, fs);
        prewarp(&ns.p[0], &ns.p[1], &ns.p[2], fc, fs);

        std::vector<DspFloatType> coeffs(4);
        DspFloatType k = 1;

        if(odd == false)
            bilinear(ns.z[0], ns.z[1], ns.z[2], ns.p[0], ns.p[1], ns.p[2], &k, fs, coeffs.data());
        else
            bilinear1(ns.z[0], ns.z[1], ns.z[2], ns.p[0], ns.p[1], ns.p[2], &k, fs, coeffs.data());

        BiquadTypeI bq;
        bq.biquad.z[0] = k;
        bq.biquad.z[1] = k*coeffs[0];
        bq.biquad.z[2] = k*coeffs[1];
        bq.biquad.p[0] = coeffs[2];
        bq.biquad.p[1] = coeffs[3];
        bq.biquad.p[2] = 0;
        return bq;
    }
    // convert analog setion to biquad type I
    BiquadSection AnalogBiquadSection(const BiquadSection &section, DspFloatType fc, DspFloatType fs, bool odd = false)
    {
        BiquadSection ns = section;
        prewarp(&ns.z[0], &ns.z[1], &ns.z[2], fc, fs);
        prewarp(&ns.p[0], &ns.p[1], &ns.p[2], fc, fs);
        std::vector<DspFloatType> coeffs(4);
        DspFloatType k = 1;

        if(odd == false)
            bilinear(ns.z[0], ns.z[1], ns.z[2], ns.p[0], ns.p[1], ns.p[2], &k, fs, coeffs.data());
        else
            bilinear1(ns.z[0], ns.z[1], ns.z[2], ns.p[0], ns.p[1], ns.p[2], &k, fs, coeffs.data());

        ns.z[0] = k;
        ns.z[1] = k*coeffs[0];
        ns.z[2] = k*coeffs[1];
        ns.p[0] = coeffs[2];
        ns.p[1] = coeffs[3];
        ns.p[2] = 0;
        return ns;
    }
    // H(s) => Bilinear/Z => H(z)
    // convert analog sos to biquad cascade type I
    BiquadSOS AnalogBiquadCascade(const BiquadSOS &sos, DspFloatType fc, DspFloatType fs, bool odd = false)
    {
        BiquadSOS nsos = sos;
        for (size_t i = 0; i < sos.size(); i++)
        {            
            BiquadSection b = AnalogBiquadSection(sos[i], fc, fs,odd);
            nsos[i] = b;
            if(odd && i == 0) odd = false;
        }
        return nsos;
    }
}