#pragma once

namespace Filters::IIR::ZolzerFilters
{

    struct ZolzerBiquadFilter : public BiquadTransposedTypeII
    {

        FilterType filter_type;
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerBiquadFilter(FilterType type = LOWPASS) : BiquadTransposedTypeII()
        {
            Fc = 1000.0;
            Fs = sampleRate;
            Q = 0.5;
            G = 1.0;
            filter_type = type;
            setCoefficients(Fc, Q, G);
        }
        ZolzerBiquadFilter(FilterType type, DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) : BiquadTransposedTypeII()
        {
            filter_type = type;
            Fs = sr;
            Q = q;
            G = g;
            setCoefficients(Fc, Q, G);
        }        
        void setCoefficients(DspFloatType fc, DspFloatType q, DspFloatType g=1.0)
        {            
            return;
            FilterCoefficients c;
            Fc = fc/4.0;
            Q = q;
            G = g;
            
            switch (filter_type)
            {
            case LOWPASS:
                c = ZolzerLowpass(Fc, Fs, Q);
                break;
            case LOWPASS1P:
                c = ZolzerLowpass1p(Fc, Fs);
                break;
            case HIGHPASS:
                c = ZolzerHighpass(Fc, Fs, Q);
                break;
            case HIGHPASS1P:
                c = ZolzerHighpass1p(Fc, Fs);
                break;
            case BANDPASS:
                c = ZolzerBandpass(Fc, Fs, Q);
                break;
            case NOTCH:
                c = ZolzerNotch(Fc, Fs, Q);
                break;
            case PEAKBOOST:
                c = ZolzerBoost(Fc, Fs, Q, G);
                break;
            case PEAKCUT:
                c = ZolzerCut(Fc, Fs, Q, G);
                break;
            case LOWSHELFBOOST:
                c = ZolzerLFBoost(Fc, Fs, G);
                break;
            case LOWSHELFCUT:
                c = ZolzerLFCut(Fc, Fs, G);
                break;
            case HIGHSHELFBOOST:
                c = ZolzerHFBoost(Fc, Fs, G);
                break;
            case HIGHSHELFCUT:
                c = ZolzerHFCut(Fc, Fs, G);
                break;
            case ALLPASS:
                c = ZolzerAllpass(Fc, Fs, Q);
                break;
            case ALLPASS1P:
                c = ZolzerAllpass1p(Fc, Fs);
                break;
            }
            biquad.setCoefficients(c);
        }
        void setCutoff(DspFloatType fc)
        {
            if(fc < 0 || fc >= Fs/2.0) return;
            setCoefficients(fc, Q, G);
        }
        void setQ(DspFloatType q)
        {
            if(q < 0 || q > 1000.0) return;
            setCoefficients(Fc, q, G);
        }
        void setRadius(DspFloatType r)
        {
            if(r < 0 || r >= 1.0) return;            
        }
        void setGain(DspFloatType g)
        {
            if(g < 0 || g > 10.0) return;
            setCoefficients(Fc, Q, g);
        }
    };

    struct ZolzerLowPassFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerLowPassFilter() : ZolzerBiquadFilter(LOWPASS)
        {
        
        }

        ZolzerLowPassFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(LOWPASS,fc,sr,q,g)
        {

        }
    };

    struct ZolzerHighPassFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerHighPassFilter() : ZolzerBiquadFilter(HIGHPASS)
        {
        
        }

        ZolzerHighPassFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(HIGHPASS,fc,sr,q,g)
        {

        }
    };

    struct ZolzerAllPassFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerAllPassFilter() : ZolzerBiquadFilter(ALLPASS)
        {
        
        }

        ZolzerAllPassFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(ALLPASS,fc,sr,q,g)
        {

        }
    };

    struct ZolzerLowPass1pFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerLowPass1pFilter() : ZolzerBiquadFilter(LOWPASS1P)
        {
        
        }

        ZolzerLowPass1pFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(LOWPASS1P,fc,sr,q,g)
        {

        }
    };

    struct ZolzerHighPass1pFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerHighPass1pFilter() : ZolzerBiquadFilter(HIGHPASS1P)
        {
        
        }

        ZolzerHighPass1pFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(HIGHPASS1P,fc,sr,q,g)
        {

        }
    };

    struct ZolzerAllPass1pFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerAllPass1pFilter() : ZolzerBiquadFilter(ALLPASS1P)
        {
        
        }

        ZolzerAllPass1pFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(ALLPASS1P,fc,sr,q,g)
        {

        }
    };

    struct ZolzerBandPassFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerBandPassFilter() : ZolzerBiquadFilter(BANDPASS)
        {
        
        }

        ZolzerBandPassFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(BANDPASS,fc,sr,q,g)
        {

        }
    };
    struct ZolzerNotchFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerNotchFilter() : ZolzerBiquadFilter(NOTCH)
        {
        
        }

        ZolzerNotchFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(NOTCH,fc,sr,q,g)
        {

        }
    };

    struct ZolzerPeakBoostFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerPeakBoostFilter() : ZolzerBiquadFilter(PEAKBOOST)
        {
        
        }

        ZolzerPeakBoostFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(PEAKBOOST,fc,sr,q,g)
        {

        }
    };
    struct ZolzerPeakCutFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerPeakCutFilter() : ZolzerBiquadFilter(PEAKCUT)
        {
        
        }

        ZolzerPeakCutFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(PEAKCUT,fc,sr,q,g)
        {

        }
    };


    struct ZolzerLowShelfBoostFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerLowShelfBoostFilter() : ZolzerBiquadFilter(LOWSHELFBOOST)
        {
        
        }

        ZolzerLowShelfBoostFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(LOWSHELFBOOST,fc,sr,q,g)
        {

        }
    };
    struct ZolzerLowShelfCutFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerLowShelfCutFilter() : ZolzerBiquadFilter(LOWSHELFCUT)
        {
        
        }

        ZolzerLowShelfCutFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(LOWSHELFCUT,fc,sr,q,g)
        {

        }
    };

    struct ZolzerHighShelfBoostFilter : public ZolzerBiquadFilter
    {

        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerHighShelfBoostFilter() : ZolzerBiquadFilter(HIGHSHELFBOOST)
        {
        
        }

        ZolzerHighShelfBoostFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(HIGHSHELFBOOST,fc,sr,q,g)
        {

        }
    };
    struct ZolzerHighShelfCutFilter : public ZolzerBiquadFilter
    {        
        DspFloatType Fc, Fs, Q, G, R;

        ZolzerHighShelfCutFilter() : ZolzerBiquadFilter(HIGHSHELFCUT)
        {
        
        }

        ZolzerHighShelfCutFilter(DspFloatType fc, DspFloatType sr, DspFloatType q = 0.5, DspFloatType g = 1) 
        : ZolzerBiquadFilter(HIGHSHELFCUT,fc,sr,q,g)
        {

        }
    };

}