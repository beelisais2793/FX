#pragma once

namespace Filters
{
    void prewarp(DspFloatType *a0, DspFloatType *a1, DspFloatType *a2, DspFloatType fc, DspFloatType fs)
    {
        DspFloatType wp, pi;

        pi = 4.0 * std::atan(1.0);
        wp = 2.0 * fs * std::tan(pi * fc / fs);

        *a2 = (*a2) / (wp * wp);
        *a1 = (*a1) / wp;
    }
    void prewarpR(DspFloatType *a0, DspFloatType *a1, DspFloatType *a2, DspFloatType fc, DspFloatType fs, DspFloatType R)
    {
        DspFloatType wp, pi;

        pi = 4.0 * std::atan(1.0);
        wp = 2.0 * fs * std::tan(pi * fc / fs);

        *a2 = R * R * (*a2) / (wp * wp);
        *a1 = R * (*a1) / wp;
    }
    void prewarpQ(DspFloatType *a0, DspFloatType *a1, DspFloatType *a2, DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType wp, pi;

        pi = 4.0 * std::atan(1.0);
        wp = 2.0 * fs * std::tan(pi * fc / fs);

        *a2 = (*a2) / (Q * Q * wp * wp);
        *a1 = (*a1) / (Q * wp);
    }
    void prewarpRQ(DspFloatType *a0, DspFloatType *a1, DspFloatType *a2, DspFloatType fc, DspFloatType fs, DspFloatType R, DspFloatType Q)
    {
        DspFloatType wp, pi;

        pi = 4.0 * std::atan(1.0);
        wp = 2.0 * fs * std::tan(pi * fc / fs);

        *a2 = R * R * (*a2) / (Q * Q * wp * wp);
        *a1 = R * (*a1) / (Q * wp);
    }

    void inversebilinear(
        DspFloatType z[3], DspFloatType p[3],
        DspFloatType k,    /* overall gain factor */
        DspFloatType fs,   /* sampling rate */
        DspFloatType *coef /* pointer to 4 iir coefficients */
    )
    {
        DspFloatType ad, bd;
        DspFloatType b0 = k;
        DspFloatType b1 = coef[0];
        DspFloatType b2 = coef[1];
        DspFloatType a0 = 1;
        DspFloatType a1 = coef[2];
        DspFloatType a2 = coef[3];

        ad = 1 / 4. * a2 / (fs * fs) + a1 / (2 * fs) + a0;
        bd = 1 / 4. * b2 / (fs * fs) + b1 / (2 * fs) + b0;

        z[0] = k * bd / ad;
        z[1] = b1 * bd;
        z[2] = b2 * bd;
        p[0] = 1;
        p[1] = a1 * ad;
        p[2] = a2 * ad;
    }

    void bilinear1(
        DspFloatType a0, DspFloatType a1, DspFloatType a2, /* numerator coefficients */
        DspFloatType b0, DspFloatType b1, DspFloatType b2, /* denominator coefficients */
        DspFloatType *k,                       /* overall gain factor */
        DspFloatType fs,                       /* sampling rate */
        DspFloatType *coef                     /* pointer to 4 iir coefficients */
    )
    {
        DspFloatType ad, bd;

        /* alpha (Numerator in s-domain) */
        ad = 2. * a1 * fs + a0;
        /* beta (Denominator in s-domain) */
        bd = 2. * b1 * fs + b0;

        /* update gain constant for this section */
        *k *= ad / bd;

        /* Nominator */
        *coef++ = (2. * a0 ) / ad;         /* alpha1 */
        *coef++ = (a1 * fs + a0) / ad; /* alpha2 */

        *coef++ = (2. * b0) / bd;           /* beta1 */
        *coef++ = (b1 * fs + b0) / bd; /* beta2 */

    }
    void bilinear(
        DspFloatType a0, DspFloatType a1, DspFloatType a2, /* numerator coefficients */
        DspFloatType b0, DspFloatType b1, DspFloatType b2, /* denominator coefficients */
        DspFloatType *k,                       /* overall gain factor */
        DspFloatType fs,                       /* sampling rate */
        DspFloatType *coef                     /* pointer to 4 iir coefficients */
    )
    {
        DspFloatType ad, bd;
        
        /* alpha (Numerator in s-domain) */
        ad = 4. * a2 * fs * fs + 2. * a1 * fs + a0;
        /* beta (Denominator in s-domain) */
        bd = 4. * b2 * fs * fs + 2. * b1 * fs + b0;

        /* update gain constant for this section */
        *k *= ad / bd;

        /* Nominator */
        *coef++ = (2. * a0 - 8. * a2 * fs * fs) / ad;         /* alpha1 */
        *coef++ = (4. * a2 * fs * fs - 2. * a1 * fs + a0) / ad; /* alpha2 */

        *coef++ = (2. * b0 - 8. * b2 * fs * fs) / bd;           /* beta1 */
        *coef++ = (4. * b2 * fs * fs - 2. * b1 * fs + b0) / bd; /* beta2 */

    }
    /*
    // lowpass to lowpass
    void lowpass_transform( DspFloatType wc, DspFloatType &a0, DspFloatType &a1, DspFloatType &a2, 
                            DspFloatType &b0, DspFloatType &b1, DspFloatType &b2)
    {                
        // (s/wc)^2 + (s/wv) + 1
        // s^2/wc^2 + s/wc + 1
        // s^2*wc + s*wc^2 + wc*(wc+1)         
        DspFloatType d = wc*wc;
        a0 *= 1.0;
        a1 *= 0.0;
        a2 *= 0.0;
        b0 *= 1.0;
        b1 *= (b1*wc)/d;
        b2 *= b2/wc; 
    }
    // lowpass to highpass
    void highpass_transform( DspFloatType wc, DspFloatType &a0, DspFloatType &a1, DspFloatType &a2, 
                            DspFloatType &b0, DspFloatType &b1, DspFloatType &b2)
    {                
        
        DspFloatType d = wc*wc;
        a0 *= 1.0/d;
        a1 *= 0;
        a2 *= 1.0/d;
        b0 *= 1.0;
        b1 *= wc/d;
        b2 *= 1.0/d;        
    }
    // bandpass and bandstop require solving the roots  it will be twice the order
    */
}