#pragma once

#include <cassert>
#include <cstring>
#include <vector>
#include <cmath>
#include "Undenormal.hpp"
#include "FX/ClipFunctions.hpp"

namespace FX::Filters
{    
    struct FilterCoefficients
    {
        DspFloatType a[2];
        DspFloatType b[3];
    };

    struct FilterBase : public FilterProcessor
    {
        enum FilterType
        {
            LOWPASS,
            HIGHPASS,
            BANDPASS,
            NOTCH,
            PEAK,
            LOWSHELF,
            HIGHSHELF,
            ALLPASS,
        };
    

        FilterType filter_type = LOWPASS;
        DspFloatType Fc,Fs,Q;

    
        FilterBase() : FilterProcessor()
        {

        }
        FilterBase(FilterType type, DspFloatType freq, DspFloatType sample_rate, DspFloatType resonance) 
        : FilterProcessor(),Fc(freq),Fs(sample_rate),Q(resonance),filter_type(type)
        {

        }
        virtual ~FilterBase() = default;
        
        virtual DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) = 0;

        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out, DspFloatType * a, DspFloatType * x, DspFloatType * y) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i],a[i],x[i],y[i]);
        }

        virtual void setCutoff(DspFloatType fc) {
            printf("Virtual function setCutoff\n");
        }
        virtual void setQ(DspFloatType fc) {
            printf("Virtual function setQ\n");
        }
        virtual void setGain(DspFloatType fc) {
            printf("Virtual function setGain\n");
        }
    };


    
    
    
    inline FilterCoefficients LowpassOnePole(DspFloatType fc, DspFloatType fs) {
        FilterCoefficients c;
        memset(&c,0x00,sizeof(c));
        c.a[0] = std::exp(-2*M_PI*fc/fs);
        c.a[1] = 0.0f;
        c.b[0] = 1.0-c.a[0];
        c.b[1] = 0.0f;
        return c;
    }
    inline FilterCoefficients HighpassOnePole(DspFloatType fc, DspFloatType fs) {
        FilterCoefficients c;
        memset(&c,0x00,sizeof(c));
        c.a[0] = std::exp(-2*M_PI*fc/fs);
        c.a[1] = 0.0f;
        c.b[0] = (1.0-c.a[0])/2;
        c.b[1] = 0.0f;
        return c;
    }
    inline FilterCoefficients AllpassOnePoleOneZero(DspFloatType fc, DspFloatType fs) {
        FilterCoefficients c;
        memset(&c,0x00,sizeof(c));
        c.a[0] = std::exp(-2*M_PI*fc/fs);
        c.a[1] = 0.0f;
        c.b[0] = -c.a[0];
        c.b[1] = 1.0f;
        return c;
    }
    inline FilterCoefficients LowpassOnePoleOneZero(DspFloatType fc, DspFloatType fs) {
        FilterCoefficients c;
        memset(&c,0x00,sizeof(c));
        DspFloatType wc = 2 * M_PI * fc/fs;
        DspFloatType K  = std::tan(wc/2);
        DspFloatType alpha = 1 + K;
        c.a[0] = (1-K)/alpha;
        c.a[1] = 0.0f;
        c.b[0] = K/alpha;
        c.b[1] = K/alpha;
        c.b[2] = 0.0f;
        return c;
    }
    inline FilterCoefficients HighpassOnePoleOneZero(DspFloatType fc, DspFloatType fs) {
        FilterCoefficients c;
        memset(&c,0x00,sizeof(c));
        DspFloatType wc = 2 * M_PI * fc/fs;
        DspFloatType K  = std::tan(wc/2);
        DspFloatType alpha = 1 + K;
        c.a[0] = (1-K)/alpha;
        c.a[1] = 0.0f;
        c.b[0] = 1/alpha;
        c.b[1] = 1/alpha;
        c.b[2] = 0.0f;
        return c;
    }
    inline FilterCoefficients LowpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        //DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm = 1 / ( 1 + K/Q + K*K);
        FilterCoefficients c;    
        c.b[0] = K*K*norm;
        c.b[1] = 2 * c.b[0];
        c.b[2] = c.b[0];
        c.a[0] = 2 * (K*K-1)*norm;
        c.a[1] = (1 - K / Q + K*K) * norm;
        return c;
    }
    inline FilterCoefficients HighpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        //DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm = 1 / ( 1 + K/Q + K*K);
        FilterCoefficients c;        
        c.b[0] = 1 * norm;
        c.b[1] = -2 * c.b[0];
        c.b[2] = c.b[0];
        c.a[0] = 2 * (K * K - 1) * norm;
        c.a[1] = (1 - K / Q + K * K) * norm;
        return c;
    }
    inline FilterCoefficients AllpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        //DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType W = K*K;
        DspFloatType alpha = 1 + K;
        DspFloatType DE = 1 + K/Q + W;
        DspFloatType norm =  1 + alpha;
        DspFloatType ccos = (-2*std::cos(2*M_PI*fc/fs))/norm;
        FilterCoefficients c;            
        c.b[0] = (1-alpha)/norm;
        c.b[1] = ccos;
        c.b[2] = (1+alpha)/norm;
        c.a[0] = ccos;
        c.a[1] = c.b[0];
        return c;
    }
    inline FilterCoefficients BandpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        //DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm = 1 / ( 1 + K/Q + K*K);
        FilterCoefficients c;        
        norm = 1 / (1 + K / Q + K * K);
        c.b[0] = K / Q * norm;
        c.b[1] = 0;
        c.b[2] = -c.b[0];
        c.a[0] = 2 * (K * K - 1) * norm;
        c.a[1] = (1 - K / Q + K * K) * norm;
        return c;
    }
    inline FilterCoefficients NotchBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        //DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm = 1 / ( 1 + K/Q + K*K);
        FilterCoefficients c;        
        c.b[0] = (1 + K * K) * norm;
        c.b[1] = 2 * (K * K - 1) * norm;
        c.b[2] = c.b[0];
        c.a[0] = c.b[1];;
        c.a[1] = (1 - K / Q + K * K) * norm;
        return c;
    }
    inline FilterCoefficients PeakBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakGain = 0)
    {
        DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm;
        FilterCoefficients c;        
        if (peakGain >= 0) {    // boost        
            norm = 1 / (1 + 1/Q * K + K * K);
            c.b[0] = (1 + V/Q * K + K * K) * norm;
            c.b[1] = 2 * (K * K - 1) * norm;
            c.b[2] = (1 - V/Q * K + K * K) * norm;
            c.a[0] = c.b[1];
            c.a[2] = (1 - 1/Q * K + K * K) * norm;
        }
        else {    // cut  
            norm = 1 / (1 + V/Q * K + K * K);      
            c.b[0] = (1 + 1/Q * K + K * K) * norm;
            c.b[1] = 2 * (K * K - 1) * norm;
            c.b[2] = (1 - 1/Q * K + K * K) * norm;
            c.a[0] = c.b[1];
            c.a[1] = (1 - V/Q * K + K * K) * norm;
        }

        return c;
    }
    inline FilterCoefficients LowshelfBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakGain = 0)
    {
        DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm;
        #undef sqrt2
        DspFloatType sqrt2  = std::sqrt(2);
        DspFloatType sqrtv2 = std::sqrt(2*V);
        FilterCoefficients c;        
        if (peakGain >= 0) {    // boost
            norm = 1 / (1 + sqrt2 * K + K * K);
            c.b[0] = (1 + sqrtv2 * K + V * K * K) * norm;
            c.b[1] = 2 * (V * K * K - 1) * norm;
            c.b[2] = (1 - sqrtv2 * K + V * K * K) * norm;
            c.a[0] = 2 * (K * K - 1) * norm;
            c.a[1] = (1 - sqrt2 * K + K * K) * norm;
        }
        else {    // cut
            norm = 1 / (1 + sqrtv2 * K + V * K * K);
            c.b[0] = (1 + sqrt2 * K + K * K) * norm;
            c.b[1] = 2 * (K * K - 1) * norm;
            c.b[2] = (1 - sqrt2 * K + K * K) * norm;
            c.a[0] = 2 * (V * K * K - 1) * norm;
            c.a[1] = (1 - sqrtv2 * K + V * K * K) * norm;
        }
        return c;
    }
    inline FilterCoefficients HighshelfBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakGain = 0)
    {
        DspFloatType V = std::pow(10, std::abs(peakGain) / 20);
        DspFloatType K = std::tan(M_PI * fc / fs);
        DspFloatType norm;
        DspFloatType sqrt2 = std::sqrt(2);
        DspFloatType sqrtv2 = std::sqrt(2*V);
        FilterCoefficients c;        
        if (peakGain >= 0) {    // boost
            norm = 1 / (1 + sqrt2 * K + K * K);
            c.b[0] = ((V + sqrtv2) * K + K * K) * norm;
            c.b[1] = 2 * (K * K - V) * norm;
            c.b[2] = (V - sqrtv2 * K + K * K) * norm;
            c.a[0] = 2 * (K * K - 1) * norm;
            c.a[1] = (1 - sqrt2* K + K * K) * norm;
        }
        else {    // cut
            norm = 1 / ((V + sqrtv2) * K + K * K);
            c.b[0] = (1 + sqrt2 * K + K * K) * norm;
            c.b[1] = 2 * (K * K - 1) * norm;
            c.b[2] = (1 - sqrt2 * K + K * K) * norm;
            c.a[0] = 2 * (K * K - V) * norm;
            c.a[1] = (V - sqrtv2 * K + K * K) * norm;
        }    
        return c;
    }

    /*
        A  = std::sqrt( 10^(dBgain/20) )
        =       10^(dBgain/40)     (for peaking and shelving EQ filters only)

        w0 = 2*M_PI*f0/Fs

        cos(w0)
        sin(w0)

        alpha = sin(w0)/(2*Q)                                       (case: Q)
            = sin(w0)*sinh( ln(2)/2 * BW * w0/sin(w0) )           (case: BW)
            = sin(w0)/2 * std::sqrt( (A + 1/A)*(1/S - 1) + 2 )         (case: S)

            FYI: The relationship between bandwidth and Q is
                1/Q = 2*sinh(ln(2)/2*BW*w0/sin(w0))     (digital filter w BLT)
            or   1/Q = 2*sinh(ln(2)/2*BW)             (analog filter prototype)

            The relationship between shelf slope and Q is
                1/Q = std::sqrt((A + 1/A)*(1/S - 1) + 2)

        2*std::sqrt(A)*alpha  =  sin(w0) * std::sqrt( (A^2 + 1)*(1/S - 1) + 2*A )
            is a handy intermediate variable for shelving EQ filters.


    Finally, compute the coefficients for whichever filter type you want:
    (The analog prototypes, H(s), are shown for each filter
            type for normalized frequency.)


    LPF:        H(s) = 1 / (s^2 + s/Q + 1)

                b0 =  (1 - cos(w0))/2
                b1 =   1 - cos(w0)
                b2 =  (1 - cos(w0))/2
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha



    HPF:        H(s) = s^2 / (s^2 + s/Q + 1)

                b0 =  (1 + cos(w0))/2
                b1 = -(1 + cos(w0))
                b2 =  (1 + cos(w0))/2
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha



    BPF:        H(s) = s / (s^2 + s/Q + 1)  (constant skirt gain, peak gain = Q)

                b0 =   sin(w0)/2  =   Q*alpha
                b1 =   0
                b2 =  -sin(w0)/2  =  -Q*alpha
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha


    BPF:        H(s) = (s/Q) / (s^2 + s/Q + 1)      (constant 0 dB peak gain)

                b0 =   alpha
                b1 =   0
                b2 =  -alpha
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha



    notch:      H(s) = (s^2 + 1) / (s^2 + s/Q + 1)

                b0 =   1
                b1 =  -2*cos(w0)
                b2 =   1
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha



    APF:        H(s) = (s^2 - s/Q + 1) / (s^2 + s/Q + 1)

                b0 =   1 - alpha
                b1 =  -2*cos(w0)
                b2 =   1 + alpha
                a0 =   1 + alpha
                a1 =  -2*cos(w0)
                a2 =   1 - alpha



    peakingEQ:  H(s) = (s^2 + s*(A/Q) + 1) / (s^2 + s/(A*Q) + 1)

                b0 =   1 + alpha*A
                b1 =  -2*cos(w0)
                b2 =   1 - alpha*A
                a0 =   1 + alpha/A
                a1 =  -2*cos(w0)
                a2 =   1 - alpha/A



    lowShelf: H(s) = A * (s^2 + (std::sqrt(A)/Q)*s + A)/(A*s^2 + (std::sqrt(A)/Q)*s + 1)

                b0 =    A*( (A+1) - (A-1)*cos(w0) + 2*std::sqrt(A)*alpha )
                b1 =  2*A*( (A-1) - (A+1)*cos(w0)                   )
                b2 =    A*( (A+1) - (A-1)*cos(w0) - 2*std::sqrt(A)*alpha )
                a0 =        (A+1) + (A-1)*cos(w0) + 2*std::sqrt(A)*alpha
                a1 =   -2*( (A-1) + (A+1)*cos(w0)                   )
                a2 =        (A+1) + (A-1)*cos(w0) - 2*std::sqrt(A)*alpha



    highShelf: H(s) = A * (A*s^2 + (std::sqrt(A)/Q)*s + 1)/(s^2 + (std::sqrt(A)/Q)*s + A)

                b0 =    A*( (A+1) + (A-1)*cos(w0) + 2*std::sqrt(A)*alpha )
                b1 = -2*A*( (A-1) + (A+1)*cos(w0)                   )
                b2 =    A*( (A+1) + (A-1)*cos(w0) - 2*std::sqrt(A)*alpha )
                a0 =        (A+1) - (A-1)*cos(w0) + 2*std::sqrt(A)*alpha
                a1 =    2*( (A-1) - (A+1)*cos(w0)                   )
                a2 =        (A+1) - (A-1)*cos(w0) - 2*std::sqrt(A)*alpha

    */
    inline FilterCoefficients RBJLowpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = ((1.0 - cc)/2)/n;
        c.b[1] = (1 - cc)/n;
        c.b[2] = ((1.0 - cc)/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJLowpassBiquadR(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType R)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= R*std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = ((1.0 - cc)/2)/n;
        c.b[1] = (1 - cc)/n;
        c.b[2] = ((1.0 - cc)/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJLowpassBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));        
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = ((1.0 - cc)/2)/n;
        c.b[1] = (1 - cc)/n;
        c.b[2] = ((1.0 - cc)/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJHighpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = ((1.0 + cc)/2)/n;
        c.b[1] = -(1 - cc)/n;
        c.b[2] = ((1.0 + cc)/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }    
    inline FilterCoefficients RBJHighpassBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));        
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = ((1.0 + cc)/2)/n;
        c.b[1] = -(1 - cc)/n;
        c.b[2] = ((1.0 + cc)/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJBandpassConstantSkirtBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = (sc/2)/n;
        c.b[1] = 0;
        c.b[2] = -(sc/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJBandpassConstantSkirtBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));        
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = (sc/2)/n;
        c.b[1] = 0;
        c.b[2] = -(sc/2)/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJBandpassConstant0dbBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = alpha/n;
        c.b[1] = 0;
        c.b[2] = -alpha/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJBandpassConstant0dbBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));   
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = alpha/n;
        c.b[1] = 0;
        c.b[2] = -alpha/n;    
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJNotchBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = 1/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = 1/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJNotchBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));   
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = 1/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = 1/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJAllpassBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = (1-alpha)/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = (1+alpha)/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJAllpassBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW)
    {
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));   
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha;
        c.b[0] = (1-alpha)/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = (1+alpha)/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJPeakBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha/A;
        c.b[0] = (1-alpha*A)/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = (1+alpha*A)/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha*A)/n;
        return c;
    }
    inline FilterCoefficients RBJPeakBiquadBW(DspFloatType fc, DspFloatType fs, DspFloatType BW, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType Q = 1.0 / (2*sinh(log(2)/2*BW*w0/sin(w0)));   
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;   
        DspFloatType n = 1+alpha/A;
        c.b[0] = (1-alpha*A)/n;
        c.b[1] = (-2*cc)/n;
        c.b[2] = (1+alpha*A)/n;
        c.a[0] = (-2*cc)/n;
        c.a[1] = (1-alpha*A)/n;
        return c;
    }
    inline FilterCoefficients RBJLowshelfBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;       
        DspFloatType sa = 2*std::sqrt(A);
        DspFloatType n = A*((A+1) - (A-1)*cc + sa);
        c.b[0] = (A*((A+1)-(A-1)*cc + sa*alpha))/n;
        c.b[1] = (2*A*((A-1) - (A+1)*cc))/n;
        c.b[2] = (A*((A+1) - (A-1)*cc - sa*alpha))/n;
        c.a[0] = (-2*((A+1)+(A+1)*cc))/n;
        c.a[1] = ((A+1) + (A-1)*cc - sa*alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJLowshelfBiquadSlope(DspFloatType fc, DspFloatType fs, DspFloatType S, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType Q  = 1.0 / (std::sqrt((A + 1/A)*(1/S - 1) + 2));
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;       
        DspFloatType sa = 2*std::sqrt(A);
        DspFloatType n = A*((A+1) - (A-1)*cc + sa);
        c.b[0] = (A*((A+1)-(A-1)*cc + sa*alpha))/n;
        c.b[1] = (2*A*((A-1) - (A+1)*cc))/n;
        c.b[2] = (A*((A+1) - (A-1)*cc - sa*alpha))/n;
        c.a[0] = (-2*((A+1)+(A+1)*cc))/n;
        c.a[1] = ((A+1) + (A-1)*cc - sa*alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJHighshelfBiquad(DspFloatType fc, DspFloatType fs, DspFloatType Q, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;       
        DspFloatType sa = 2*std::sqrt(A);
        DspFloatType n = A*((A+1) - (A-1)*cc + sa);
        c.b[0] = (A*((A+1)+(A-1)*cc + sa*alpha))/n;
        c.b[1] = (-2*A*((A-1) + (A+1)*cc))/n;
        c.b[2] = (A*((A+1) + (A-1)*cc - sa*alpha))/n;
        c.a[0] = (2*((A+1)-(A+1)*cc))/n;
        c.a[1] = ((A+1) - (A-1)*cc - sa*alpha)/n;
        return c;
    }
    inline FilterCoefficients RBJHighshelfBiquadSlope(DspFloatType fc, DspFloatType fs, DspFloatType S, DspFloatType peakDb)
    {
        DspFloatType A  = std::pow(10,peakDb/40.0f);
        DspFloatType Q  = 1.0 / (std::sqrt((A + 1/A)*(1/S - 1) + 2));
        DspFloatType w0 = 2*M_PI*fc/fs;
        DspFloatType alpha= std::sin(w0)/(2*Q);
        DspFloatType cc = std::cos(w0);
        DspFloatType sc = std::sin(w0);
        FilterCoefficients c;       
        DspFloatType sa = 2*std::sqrt(A);
        DspFloatType n = A*((A+1) - (A-1)*cc + sa);
        c.b[0] = (A*((A+1)+(A-1)*cc + sa*alpha))/n;
        c.b[1] = (-2*A*((A-1) + (A+1)*cc))/n;
        c.b[2] = (A*((A+1) + (A-1)*cc - sa*alpha))/n;
        c.a[0] = (2*((A+1)-(A+1)*cc))/n;
        c.a[1] = ((A+1) - (A-1)*cc - sa*alpha)/n;
        return c;
    }
    
    inline FilterCoefficients MassbergLowpassBiquad(DspFloatType fCutoffFreq, DspFloatType fs, DspFloatType fQ)
    {
        DspFloatType theta_c;
        DspFloatType g1;
        DspFloatType gr;
        DspFloatType gp;
        DspFloatType gz;
        DspFloatType sOmp;     //small omega
        DspFloatType sOmz;
        DspFloatType sOmr;
        DspFloatType sOmm;
        DspFloatType bOmr;
        DspFloatType bOms;     //big omega
        DspFloatType bOmm;
        DspFloatType Qp;       //Q
        DspFloatType Qz;
        DspFloatType gam0;     //gamma
        DspFloatType alp0;     //alpha
        DspFloatType alp1;
        DspFloatType alp2;
        DspFloatType bet1;     //beta
        DspFloatType bet2;

        theta_c = 2.0*M_PI*fCutoffFreq/(DspFloatType)fs;

        g1 = 2 / std::sqrt(std::pow(2 - std::pow((std::sqrt(2)*M_PI ) / theta_c, 2) , 2)  +  std::pow((2*M_PI) / (fQ*theta_c) , 2));


        // big Omega value is dependent on fQ:

        if(fQ > std::sqrt(0.5))
        {
            gr = (2*std::pow(fQ, 2))  /  std::sqrt(4*std::pow(fQ, 2) - 1);
            sOmr = theta_c * std::sqrt( 1 - ( 1 / std::pow(2*fQ, 2) ) );
            bOmr = std::tan (sOmr/2);
            bOms = bOmr * std::pow( ( std::pow(gr, 2) - std::pow(g1, 2 )) / ( std::pow(gr, 2) - 1) , (1/4));
        }
        else
        {
            sOmm = theta_c * std::sqrt(( 2 - ( 1 / (2*std::pow(fQ, 2) ) ) + std::sqrt(( ( 1- (4*std::pow(fQ, 2)))/ ( std::pow(fQ, 4))) +(4/g1))) / 2);
            bOmm = std::tan(sOmm/2);
            bOms = theta_c *  std::pow( 1 - std::pow( g1, 2), 1/4) / 2;
            bOms = std::fmin(bOms, bOmm);
        }

        //calculate pols / zeros (small omega), gains(g) and Qs:

        sOmp = 2 * std::atan(bOms);
        sOmz = 2 * std::atan(bOms / std::sqrt(g1));

        gp = 1 / ( std::sqrt( std::pow( 1 - std::pow( sOmp / theta_c, 2), 2) + std::pow(sOmp/(fQ*theta_c), 2)));
        gz = 1 / ( std::sqrt( std::pow(1 - std::pow( sOmz / theta_c, 2), 2) + std::pow(sOmz/(fQ*theta_c), 2)));

        Qp = std::sqrt((g1*( std::pow(gp, 2) - std::pow(gz, 2)))/(( g1 + std::pow(gz, 2) ) * std::pow(g1 - 1, 2)));
        Qz = std::sqrt((std::pow(g1, 2)*( std::pow(gp, 2) - std::pow(gz, 2)))/(std::pow(gz, 2)*(g1 + std::pow(gp, 2))*std::pow(g1 - 1, 2)));

        gam0 = std::pow(bOms, 2) + ((1/Qp) * bOms) + 1;

        alp0 = std::pow(bOms, 2) + ((std::sqrt(g1)/Qz) * bOms) + g1;
        alp1 = 2*(std::pow(bOms, 2) - g1);
        alp2 = std::pow(bOms, 2) - ((std::sqrt(g1)/Qz) * bOms) + g1;

        bet1 = 2*(std::pow(bOms, 2) - 1);
        bet2 = std::pow(bOms, 2) - ((1/Qp) * bOms) + 1;

        FilterCoefficients c;
        
        c.b[0] = alp0/gam0;
        c.b[1] = alp1/gam0;
        c.b[2] = alp2/gam0;
        c.a[0] = bet1/gam0;
        c.a[1] = bet2/gam0;

        return c;
    }  

    inline FilterCoefficients ZolzerNotch(DspFloatType f, DspFloatType fs, DspFloatType Q) {
        DspFloatType K  = tan(M_PI*f/fs);
        DspFloatType Kq = Q*(1+K*K) ;
        DspFloatType Kk = (K*K*Q+K+Q); 
        FilterCoefficients c;       
        c.b[0] = Kq/Kk;
        c.b[1] = (2*Kq)/Kk;
        c.b[2] = Kq/Kk;
        c.a[0] = (2*Q*(K*K-1))/Kk;
        c.a[1] = (K*K*Q-K+Q)/Kk;
        return c;
    }
    inline FilterCoefficients ZolzerLowpass1p(DspFloatType f, DspFloatType fs)
    {            
        DspFloatType K = tan(M_PI*f/fs);
        FilterCoefficients c;
        c.b[0] = K/(K+1);
        c.b[1] = K/(K+1);
        c.b[2] = 0;
        c.a[0] = (K-1)/(K+1);
        c.a[1] = 0;
        return c;
    }
    inline FilterCoefficients ZolzerHighpass1p(DspFloatType f,DspFloatType fs)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        FilterCoefficients c;
        c.b[0] = 1/(K+1);
        c.b[1] = -1/(K+1);
        c.b[2] = 0;
        c.a[0] = (K-1)/(K+1);
        c.a[1] = 0;
        return c;
    }
    inline FilterCoefficients ZolzerAllpass1p(DspFloatType f, DspFloatType fs)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        FilterCoefficients c;
        c.b[0] = (K-1)/(K+1);
        c.b[1] = 1;
        c.b[2] = 0;
        c.a[0] = (K-1)/(K+1);
        c.a[1] = 0;
        return c;
    }
    inline FilterCoefficients ZolzerLowpass(DspFloatType f, DspFloatType fs, DspFloatType Q) {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType Kk = (K*K*Q+K+Q);        
        DspFloatType Kq = (K*K*Q);
        FilterCoefficients c;
        c.b[0] = Kq/Kk;
        c.b[1] = (2*Kq) /Kk;
        c.b[2] =  Kq / Kk;
        c.a[0] = (2*Q*(K*K-1))/Kk;
        c.a[1] = (K*K*Q-K+Q)/Kk;
        return c;
    }
    inline FilterCoefficients ZolzerAlpass(DspFloatType f, DspFloatType fs, DspFloatType Q) {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType Kk = (K*K*Q+K+Q);        
        DspFloatType Km = (K*K*Q-K+Q);
        DspFloatType Kq = 2*Q*(K*K-1);
        FilterCoefficients c;
        c.b[0] = Km/Kk;        
        c.b[1] = Kq/Kk;
        c.b[2] = 1.0f;
        c.a[0] = Kq/Kk;
        c.a[1] = Km/Kk;
        return c;
    }
    inline FilterCoefficients ZolzerHighpass(DspFloatType f, DspFloatType fs, DspFloatType Q) {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType Kk = (K*K*Q+K+Q); 
        DspFloatType Kq = 2*Q*(K*K-1);
        DspFloatType Km = (K*K*Q-K+Q);
        FilterCoefficients c;
        c.b[0] = Q / Kk;
        c.b[1] = -(2*Q)/Kk;
        c.b[2] = Q / Kk;
        c.a[1] = Kq/Kk;
        c.a[2] = Km/Kk;
        return c;
    }    
    inline FilterCoefficients ZolzerBandpass(DspFloatType f, DspFloatType fs, DspFloatType Q) {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType Kk = (K*K*Q+K+Q); 
        FilterCoefficients c;
        c.b[0] = K / Kk;
        c.b[1] = 0;
        c.b[2] = -c.b[0];
        c.a[0] = (2*Q*(K*K-1))/Kk;
        c.a[1] = (K*K*Q-K+Q)/Kk;
        return c;
    }
    // lowshelf
    inline FilterCoefficients ZolzerLFBoost(DspFloatType f, DspFloatType fs, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);
        DspFloatType Kaka1 = sqrt(2*V0) * K + V0*K*K;
        DspFloatType Kaka2 = 1 + sqrt(2)*K + K*K;
        FilterCoefficients c;
        c.b[0] = (1+Kaka1)/Kaka2;
        c.b[1] = (2*(V0*K*K-1))/ Kaka2;
        c.b[2] = (1 - Kaka1)/Kaka2;
        c.a[0] = (2*(K*K-1))/Kaka2;
        c.a[1] = (1-sqrt(2)*K+K*K)/Kaka2;
        return c;
    }
    // lowshelf
    inline FilterCoefficients ZolzerLFcut(DspFloatType f, DspFloatType fs, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);
        DspFloatType Kaka = V0 + sqrt(2*V0)*K + K*K;
        FilterCoefficients c;
        c.b[0] = (V0*(1+sqrt(2)*K+K*K))/Kaka;
        c.b[1] = (2*V0*(K*K-1))/ Kaka;
        c.b[2] = (V0*(1-sqrt(2)*K+K*K))/Kaka;
        c.a[0] = (2*(K*K-V0))/Kaka;
        c.a[1] = (V0-sqrt(2*V0)*K+K*K)/Kaka;
        return c;
    }
    // hishelf
    inline FilterCoefficients ZolzerHFboost(DspFloatType f, DspFloatType fs, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);            
        DspFloatType Kaka = 1 + sqrt(2)*K + K*K;
        FilterCoefficients c;
        c.b[0] = (V0 + sqrt(2*V0)*K + K*K)/Kaka;
        c.b[1] = (2*(K*K-V0))/Kaka;
        c.b[2] = (V0 - sqrt(2*V0)*K + K*K)/Kaka;
        c.a[0] = (2*(K*K-1))/Kaka;
        c.a[1] = (1-sqrt(2*K)+K*K)/Kaka;
        return c;
    }
    // hishelf
    inline FilterCoefficients ZolzerHFcut(DspFloatType f, DspFloatType fs, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);            
        DspFloatType Kaka = 1 + sqrt(2*V0)*K + V0*K*K;
        FilterCoefficients c;
        c.b[0] = (V0*(1 + sqrt(2)*K + K*K))/Kaka;
        c.b[1] = (2*V0*(K*K-1))/Kaka;
        c.b[2] = (V0*(1 - sqrt(2)*K + K*K))/Kaka;
        c.a[0] = (2*(V0*K*K-1))/Kaka;
        c.a[1] = (1-sqrt(2*V0)*K + V0*K*K)/Kaka;
        return c;
    }
    // peak
    inline FilterCoefficients ZolzerBoost(DspFloatType f, DspFloatType fs, DspFloatType Q, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);            
        DspFloatType Kaka = 1 + (1/Q)*K + K*K;
        FilterCoefficients c;
        c.b[0] = (1+(V0/Q)*K + K*K)/Kaka;
        c.b[1] = (2*(K*K-1))/Kaka;
        c.b[2] = (1- (V0/Q)*K + K*K)/Kaka;
        c.a[0] = (2*(K*K-1))/Kaka;
        c.a[1] = (1 - (1/Q)*K + K*K)/Kaka;
        return c;
    }
    //peak
    inline FilterCoefficients ZolzerCut(DspFloatType f, DspFloatType fs, DspFloatType Q, DspFloatType G)
    {        
        DspFloatType K = tan(M_PI*f/fs);
        DspFloatType V0= pow(10,G/20.0);            
        DspFloatType Kaka = 1 + (1/(V0*Q)*K + K*K);
        FilterCoefficients c;
        c.b[0] = (1 + (1/Q)*K + K*K)/Kaka;
        c.b[1] = (2*(K*K-1))/Kaka;
        c.b[2] = (1 - (1/Q)*Kaka *K*K)/Kaka;
        c.a[0] = (2*(K*K-1))/Kaka;
        c.a[1] = (1 - (1/(V0*Q)*K +K*K))/Kaka;
        return c;
    }

    struct BiquadFilter : public FilterBase
    {
        DspFloatType a1,a2;
        DspFloatType b0,b1,b2;
        DspFloatType x2,x1;
        DspFloatType y2,y1;
        DspFloatType x;
        DspFloatType y;
        DspFloatType fc,sr,q,gain;
        FilterBase::FilterType type;

        BiquadFilter(DspFloatType sample_rate) : 
        FilterBase(FilterBase::FilterType::LOWPASS,1000,sample_rate,0.5)
        {
            setCoefficients();
        }
        BiquadFilter(FilterType _type, DspFloatType freq, DspFloatType sample_rate, DspFloatType resonance, DspFloatType dbGain=0) 
        : FilterBase(type,freq,sample_rate,resonance)
        {            
            setCoefficients();
        }

        void setCoefficients(FilterCoefficients c)
        {
            a1 = c.a[0];
            a2 = c.a[1];
            b0 = c.b[0];
            b1 = c.b[1];
            b2 = c.b[2];
        }
        void setCoefficients()
        {
            FilterCoefficients c;
            switch(type)
            {
                case LOWPASS: c = LowpassBiquad(fc,sr,q); break;
                case HIGHPASS: c = HighpassBiquad(fc,sr,q); break;
                case BANDPASS: c = BandpassBiquad(fc,sr,q); break;
                case NOTCH: c = NotchBiquad(fc,sr,q); break;
                case PEAK: c = PeakBiquad(fc,sr,q,gain); break;
                case LOWSHELF: c = LowshelfBiquad(fc,sr,q,gain); break;
                case HIGHSHELF: c = HighshelfBiquad(fc,sr,q,gain); break;
                case ALLPASS: c = AllpassBiquad(fc,sr,q); break;
            }
            a1 = c.a[0];
            a2 = c.a[1];
            b0 = c.b[0];
            b1 = c.b[1];
            b2 = c.b[2];
        }        
        void setCutoff(DspFloatType f)
        {
            fc = f;
            if(fc > Fs/2) fc = Fs/2;
            setCoefficients();
        }
        void setQ(DspFloatType Q)
        {
            if(Q < 0.01) Q = 0.01;                  
            if(Q > 999) Q = 999;
            q = Q;
            setCoefficients();
        }
        DspFloatType Tick(DspFloatType in, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0) {
            Undenormal denormal;
            //setCoefficients();
            x = in;
            y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;
            y2 = y1;
            y1 = y;
            x2 = x2;
            x1 = x;
            return y;
        }
        void Process(size_t n, DspFloatType * input, DspFloatType * output)
        {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
        void InplaceProcess(size_t n, DspFloatType * buffer)
        {
            for(size_t i = 0; i < n; i++) buffer[i] = Tick(buffer[i]);
        }
    };


    struct BiquadFilterCascader
    {
        std::vector<BiquadFilter*> filters;
        DspFloatType x,y;

        BiquadFilterCascader() = default;

        DspFloatType Tick(DspFloatType in, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0) {        
            x = in;
            y = x;
            for(size_t i = filters.size()-1; i >= 0; i--)
                y = filters[i]->Tick(y,A,X,Y);
            return y;
        }
    };
    struct BiquadParallelFilters
    {
        std::vector<BiquadFilter*> filters;
        DspFloatType x,y;

        BiquadParallelFilters() = default;

        DspFloatType Tick(DspFloatType in, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0) {        
            x = in;
            y = 0;
            for(size_t i = 0; i < filters.size(); i++)
                y += filters[i]->Tick(x,A,X,Y);
            return y/filters.size();
        }
    };

        struct BiquadSection
    {
        DspFloatType z[3];
        DspFloatType p[3];

        BiquadSection()
        {
            memset(z, 0, sizeof(z));
            memset(p, 0, sizeof(p));
        }
        BiquadSection(const FilterCoefficients &c)
        {
            z[0] = c.b[0];
            z[1] = c.b[1];
            z[2] = c.b[2];
            p[0] = c.a[0];
            p[1] = c.a[1];
        }
        BiquadSection(DspFloatType z1, DspFloatType z2, DspFloatType z3, DspFloatType p1, DspFloatType p2)
        {
            z[0] = z1;
            z[1] = z2;
            z[2] = z3;
            p[0] = p1;
            p[1] = p2;
        }
        BiquadSection(const BiquadSection &b)
        {
            memcpy(z, b.z, sizeof(z));
            memcpy(p, b.p, sizeof(p));
        }
        void setCoefficients(DspFloatType z1, DspFloatType z2, DspFloatType z3, DspFloatType p1, DspFloatType p2)
        {
            z[0] = z1;
            z[1] = z2;
            z[2] = z3;
            p[0] = p1;
            p[1] = p2;
        }
        void setCoefficients(DspFloatType n[3], DspFloatType d[2])
        {
            memcpy(z, n, sizeof(z));
            memcpy(p, d, sizeof(p));
        }
        void setCoefficients(const FilterCoefficients &c)
        {
            z[0] = c.b[0];
            z[1] = c.b[1];
            z[2] = c.b[2];
            p[0] = c.a[0];
            p[1] = c.a[1];
        }
        BiquadSection &operator=(const BiquadSection &b)
        {
            memcpy(z, b.z, sizeof(z));
            memcpy(p, b.p, sizeof(p));
            return *this;
        }

        void print()
        {
            std::cout << z[0] << " + " << z[1] << " z^-1 + " << z[2] << " z^-1\n";
            std::cout << "-------------------------------------------------------------\n";
            std::cout << " 1 + " << p[0] << +" z^-1 + " << p[1] << " z^-2\n";
        }
    };

    // This is digital already transformed into Z
    struct BiquadTypeI : public FilterBase
    {
        BiquadSection biquad;
        DspFloatType x, y, x1, x2, y1, y2;

        BiquadTypeI() : FilterBase()
        {
            x = y = 0;
            x1 = x2 = y1 = y2 = 0;
        }
        BiquadTypeI(const BiquadSection &b) : FilterBase(), biquad(b)
        {
            x = y = 0;
            x1 = x2 = y1 = y2 = 0;
        }
        BiquadTypeI &operator=(const BiquadTypeI &b)
        {
            biquad = b.biquad;
            x = b.x;
            y = b.y;
            x1 = b.x1;
            x2 = b.x2;
            y1 = b.y1;
            y2 = b.y2;
            return *this;
        }
        void setCoefficients(const BiquadSection &b)
        {
            biquad = b;
        }
        void setBiquad(const BiquadSection &b)
        {
            biquad = b;
        }

        // transposed is just flip - to +
        // not really needed unless all you have is addition
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            Undenormal denormal;
            x = I;
            y = biquad.z[0] * x + biquad.z[1] * x1 + biquad.z[2] * x2;
            y = y - biquad.p[0] * y1 - biquad.p[1] * y2;
            x2 = x1;
            x1 = x2;
            y2 = y1;
            y1 = y;
            return A * y;
        }
    };

    struct BiquadTypeII : public FilterBase
    {
        BiquadSection biquad;
        DspFloatType x, y, v, v1, v2;

        BiquadTypeII() : FilterBase()
        {
            x = y = 0;
            v = v1 = v2 = 0;
        }
        BiquadTypeII(const BiquadSection &b) : FilterBase(), biquad(b)
        {
            x = y = 0;
            v = v1 = v2 = 0;
        }
        BiquadTypeII &operator=(const BiquadTypeII &b)
        {
            biquad = b.biquad;
            x = b.x;
            y = b.y;
            v1 = b.v1;
            v2 = b.v2;
            v = b.v;

            return *this;
        }
        void setBiquad(const BiquadSection &b)
        {
            biquad = b;
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            Undenormal denormal;
            x = I;
            v = x - biquad.p[0] * v1 - biquad.p[1] * v2;
            y = biquad.z[0] * v + biquad.z[1] * v1 + biquad.z[2] * v2;
            v2 = v1;
            v1 = v;
            return A * y;
        }
    };

    struct BiquadTransposedTypeI : public FilterBase
    {
        BiquadSection biquad;
        DspFloatType x, y, x1, x2, y1, y2;

        BiquadTransposedTypeI() : FilterBase()
        {
            x = y = 0;
            x1 = x2 = y1 = y2 = 0;
        }
        BiquadTransposedTypeI(const BiquadSection &b) : FilterBase(), biquad(b)
        {
            x = y = 0;
            x1 = x2 = y1 = y2 = 0;
        }
        BiquadTransposedTypeI &operator=(const BiquadTransposedTypeI &b)
        {
            biquad = b.biquad;
            x = b.x;
            y = b.y;
            x1 = b.x1;
            x2 = b.x2;
            y1 = b.y1;
            y2 = b.y2;
            return *this;
        }
        void setBiquad(const BiquadSection &b)
        {
            biquad = b;
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            Undenormal denormal;
            x = I;
            x += -biquad.p[0] * y1 + -biquad.p[1] * y2;
            y = biquad.z[0] * x + biquad.z[1] * x1 + biquad.z[2] * x2;
            x2 = x1;
            x1 = x;
            y2 = y1;
            y1 = y;
            return A * y;
        }
    };

    // This is Transposed Type II
    struct BiquadTransposedTypeII : public FilterBase
    {
        BiquadSection biquad;
        DspFloatType x, y, d1, d2;

        BiquadTransposedTypeII() : FilterBase()
        {
            x = y = 0;
            d1 = d2 = 0;
        }
        BiquadTransposedTypeII(const BiquadSection &b) : FilterBase(), biquad(b)
        {
            x = y = 0;
            d1 = d2 = 0;
        }
        BiquadTransposedTypeII &operator=(const BiquadTransposedTypeII &b)
        {
            biquad = b.biquad;
            x = b.x;
            y = b.y;
            d1 = b.d1;
            d2 = b.d2;
            return *this;
        }
        void setBiquad(const BiquadSection &b)
        {
            biquad = b;
        }

        // transposed is just flip - to +
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            Undenormal denormal;
            x = I;
            y = biquad.z[0] * x + d1;
            d1 = biquad.z[1] * x - biquad.p[0] * y + d2;
            d2 = biquad.z[2] * x - biquad.p[1] * y;
            return A * y;
        }
    };

}