/**
* First order all-pass filter
* Dimitris Tassopoulos 2016
*
* fc, corner frequency
*/
#pragma once


#ifndef sqrt2
#define sqrt2	(2.0 * 0.707106781186547524401)
#endif

#ifndef sqrt2over2
#define sqrt2over2  0.707106781186547524401
#endif


typedef DspFloatType coef_size_t;

#include <cmath>
#include "Undenormal.hpp"

namespace FX::Filters::cppfilters
{
    struct Biquad : public FilterProcessor 
    {
        Biquad() : FilterProcessor(),sr(sampleRate),fc(1000.0),q(0.5),g(1.0),m_xnz1(0), m_xnz2(0), m_ynz1(0), m_ynz2(0), m_offset(0), m_coeffs{0} {};
        virtual ~Biquad() {};
        
        virtual coef_size_t process(coef_size_t sample)
        {
            Undenormal denormal;
            coef_size_t xn = sample;
            coef_size_t yn = m_coeffs.a0*xn + m_coeffs.a1*m_xnz1 + m_coeffs.a2*m_xnz2
                            - m_coeffs.b1*m_ynz1 - m_coeffs.b2*m_ynz2;  
            m_xnz2 = m_xnz1;
            m_xnz1 = xn;
            m_ynz2 = m_ynz1;
            m_ynz1 = yn;    
            return(yn + m_offset);
        }

        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
            return A*process(I);
        }
        void set_offset(coef_size_t offset)
        {
            m_offset = offset;
        }

        coef_size_t get_offset(void)
        {
            return(m_offset);
        }

        typedef struct {
            coef_size_t a0;
            coef_size_t a1;
            coef_size_t a2;
            coef_size_t b1;
            coef_size_t b2;
            coef_size_t c0;
            coef_size_t d0;
        } tp_coeffs;

        DspFloatType fc,sr,q,g;
        coef_size_t m_xnz1, m_xnz2, m_ynz1, m_ynz2, m_offset;
        tp_coeffs m_coeffs;
    };

    class BiquadModified : public Biquad {
    public:
        coef_size_t process(coef_size_t sample)
        {
            Undenormal denormal;
            coef_size_t xn = sample;
            coef_size_t ynn = m_coeffs.a0*xn + m_coeffs.a1*m_xnz1 + m_coeffs.a2*m_xnz2
                - m_coeffs.b1*m_ynz1 - m_coeffs.b2*m_xnz2;
            coef_size_t yn = m_coeffs.d0*xn + m_coeffs.c0*ynn;

            m_xnz2 = m_xnz1;
            m_xnz1 = xn;
            m_ynz2 = m_ynz1;
            m_ynz1 = yn;
            return(yn);
        }
    };

    /**
    * First order all-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc, corner frequency
    */
    struct FO_APF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs = 44100)
        {
            coef_size_t a = (tan(M_PI*fc / fs) - 1.0) / (tan(M_PI*fc / fs) + 1.0);
            m_coeffs.a0 = a;
            m_coeffs.a1 = 1.0;
            m_coeffs.a2 = 0.0;
            m_coeffs.b1 = a;
            m_coeffs.b2 = 0.0;        
        }

        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc);
        }    
    };

    /**
    * First order high-pass filter
    * Dimitris Tassopoulos 2016-2020
    */
    struct FO_HPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = 2.0 * M_PI * fc / fs;
            coef_size_t g = cos(th) / (1.0 + sin(th));
            m_coeffs.a0 = (1.0 + g) / 2.0;
            m_coeffs.a1 = -((1.0 + g) / 2.0);
            m_coeffs.a2 = 0.0;
            m_coeffs.b1 = -g;
            m_coeffs.b2 = 0.0;
            
        }

        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    
    };

    /**
    * First order low-pass filter
    * Dimitris Tassopoulos 2016-2020
    */
    struct FO_LPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = 2.0 * M_PI * fc / fs;
            coef_size_t g = cos(th) / (1.0 + sin(th));
            m_coeffs.a0 = (1.0 - g) / 2.0;
            m_coeffs.a1 = (1.0 - g) / 2.0;
            m_coeffs.a2 = 0.0;
            m_coeffs.b1 = -g;
            m_coeffs.b2 = 0.0;
            
        }

        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    

    };

    /**
    * First order high-shelving filter
    * Dimitris Tassopoulos 2016
    *
    * fc , high shelf frequency
    * Low-frequency gain/attenuation in dB
    * Shelving filters are used in many tone controls, especially when there are only two, bass
    * and treble, which are almost always implemented as shelf types. The filters have a corner
    * frequency and gain or attenuation value.
    */
    struct FO_SHELVING_HIGH : public BiquadModified {
    
        void calculate_coeffs(coef_size_t gain_db, coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = 2.0 * M_PI * fc / fs;
            coef_size_t m = pow(10.0, gain_db / 20.0);
            coef_size_t b = (1.0 + m) / 4.0;
            coef_size_t d = b * tan(th / 2.0);
            coef_size_t g = (1.0 - d) / (1.0 + d);
            m_coeffs.a0 = (1.0 + g) / 2.0;
            m_coeffs.a1 = -((1.0 + g) / 2.0);
            m_coeffs.a2 = 0.0;
            m_coeffs.b1 = -g;
            m_coeffs.b2 = 0.0;
            m_coeffs.c0 = m - 1.0;
            m_coeffs.d0 = 1.0;            
        }
        void setGain(coef_size_t G) {
            g = G;
            calculate_coefficients(g,fc,sr);
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(g,fc,sr);
        }    
    };

    /**
    * First order low-shelving filter
    * Dimitris Tassopoulos 2016
    *
    * fc , low shelf frequency
    * Low-frequency gain/attenuation in dB
    * Shelving filters are used in many tone controls, especially when there are only two, bass
    * and treble, which are almost always implemented as shelf types. The filters have a corner
    * frequency and gain or attenuation value.
    */

    struct FO_SHELVING_LOW : public BiquadModified {
    
        void calculate_coeffs(coef_size_t gain_db, coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = 2.0 * M_PI * fc / fs;
            coef_size_t m = pow(10.0, gain_db / 20.0);
            coef_size_t b = 4.0 / (1.0 + m);
            coef_size_t d = b * tan(th / 2.0);
            coef_size_t g = (1.0 - d) / (1.0 + d);
            m_coeffs.a0 = (1.0 - g) / 2.0;
            m_coeffs.a1 = (1.0 - g) / 2.0;
            m_coeffs.a2 = 0.0;
            m_coeffs.b1 = -g;
            m_coeffs.b2 = 0.0;
            m_coeffs.c0 = m - 1.0;
            m_coeffs.d0 = 1.0;            
        }
        void setGain(coef_size_t G) {
            g = G;
            calculate_coefficients(g,fc,sr);
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(g,fc,sr);
        }    
    };


    /**
    * Second order all-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc, corner frequency
    * Q, steepness of phase shift at fc (second-order only)
    */
    struct SO_APF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t Q, coef_size_t fs)
        {
            coef_size_t a = (tan(M_PI*Q *fc/fs) - 1.0) / (tan(M_PI*Q * fc/fs) + 1.0);
            coef_size_t b = -cos(M_PI*Q *fc/fs);
            m_coeffs.a0 = -a;
            m_coeffs.a1 = b*(1.0 - a);
            m_coeffs.a2 = 1.0;
            m_coeffs.b1 = m_coeffs.a1;
            m_coeffs.b2 = m_coeffs.a0;
        }        
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(fc,q,sr);
        }    
    };


    /**
    * Second order band-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency
    * Q , quality factor controlling width of peak or notch = 1/BW
    */
    struct SO_BPF : public Biquad {
        void calculate_coeffs(coef_size_t fc, coef_size_t Q, coef_size_t fs)
        {
            coef_size_t w = 2.0 * M_PI * fc / fs;
            coef_size_t b = 0.5*((1.0 - tan(w / (2.0*Q))) / (1.0 + tan(w / (2.0*Q))));
            coef_size_t g = (0.5 + b)*cos(w);
            m_coeffs.a0 = 0.5 - b;
            m_coeffs.a1 = 0.0;
            m_coeffs.a2 = -(0.5 - b);
            m_coeffs.b1 = -2.0 * g;
            m_coeffs.b2 = 2.0 * b;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(fc,q,sr);
        }    
    };


    /**
    * Second order band-stop filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency
    * Q , quality factor controlling width of peak or notch = 1/BW
    */
    struct SO_BSF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t Q, coef_size_t fs)
        {
            coef_size_t w = 2.0 * M_PI * fc / fs;
            coef_size_t b = 0.5*((1.0 - tan(w / (2.0*Q))) / (1.0 + tan(w / (2.0*Q))));
            coef_size_t g = (0.5 + b)*cos(w);
            m_coeffs.a0 = 0.5 + b;
            m_coeffs.a1 = -2.0 * g;
            m_coeffs.a2 = 0.5 + b;
            m_coeffs.b1 = -2.0 * g;
            m_coeffs.b2 = 2.0 * b;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(fc,q,sr);
        }            
    };


    /**
    * Second order Butterworth band-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency
    * BW , bandwidth of peak/notch = fc/Q
    * Butterworth BPF and BSF are made by cascading (BPF) or paralleling (BSF) a Butterworth
    * LPF and Butterworth HPF.
    */

    struct SO_BUTTERWORTH_BPF : public Biquad {
        void calculate_coeffs(coef_size_t fc,  coef_size_t bw, coef_size_t fs)
        {
            coef_size_t c = 1.0 / (tan(M_PI*fc*bw / fs));
            coef_size_t d = 2.0 * cos(2.0 * M_PI * fc / fs);
            m_coeffs.a0 = 1.0 / (1.0 + c);
            m_coeffs.a1 = 0.0;
            m_coeffs.a2 = - m_coeffs.a0;
            m_coeffs.b1 = -m_coeffs.a0 * (c * d);
            m_coeffs.b2 = m_coeffs.a0 * (c - 1.0);
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setBW(coef_size_t bw)
        {
            q = bw;
            calculate_coefficients(fc,q,sr);
        }    
    };


    /**
    * Second order Butterworth band-stop filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency
    * BW , bandwidth of peak/notch = fc/Q
    * Butterworth BPF and BSF are made by cascading (BPF) or paralleling (BSF) a Butterworth
    * LPF and Butterworth HPF.
    */

    struct SO_BUTTERWORTH_BSF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t bw, coef_size_t fs)
        {
            coef_size_t c = tan(M_PI*fc*bw / fs);
            coef_size_t d = 2.0 * cos(2.0 * M_PI * fc / fs);
            m_coeffs.a0 = 1.0 / (1.0 + c);
            m_coeffs.a1 = -m_coeffs.a0 * d;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = -m_coeffs.a0 * d;
            m_coeffs.b2 = m_coeffs.a0 * (1.0 - c);
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setBW(coef_size_t bw)
        {
            q = bw;
            calculate_coefficients(fc,q,sr);
        }    
    };

    /**
    * Second order Butterworth high-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc, corner frequency
    * Butterworth low-pass and high-pass filters are specialized versions of the ordinary secondorder
    * low-pass filter. Their Q values are fixed at 0.707, which is the largest value it can
    * assume before peaking in the frequency response is observed.
    */

    struct SO_BUTTERWORTH_HPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t c = tan(M_PI*fc / fs);
            m_coeffs.a0 = 1.0 / (1.0 + sqrt2*c + pow(c, 2.0));
            m_coeffs.a1 = -2.0 * m_coeffs.a0;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = 2.0 * m_coeffs.a0*(pow(c, 2.0) - 1.0);
            m_coeffs.b2 = m_coeffs.a0 * (1.0 - sqrt2*c + pow(c, 2.0));
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    
    };


    /**
    * Second order Butterworth low-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc, corner frequency
    * Butterworth low-pass and high-pass filters are specialized versions of the ordinary secondorder
    * low-pass filter. Their Q values are fixed at 0.707, which is the largest value it can
    * assume before peaking in the frequency response is observed.
    */

    struct SO_BUTTERWORTH_LPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t c = 1.0 / (std::tan(M_PI*fc / fs));
            m_coeffs.a0 = 1.0 / (1.0 + sqrt2*c + std::pow(c, 2.0) );
            m_coeffs.a1 = 2.0 * m_coeffs.a0;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = 2.0 * m_coeffs.a0*(1.0 - std::pow(c, 2.0));
            m_coeffs.b2 = m_coeffs.a0 * (1.0 - sqrt2*c + std::pow(c, 2.0) );
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    
    };


    /**
    * Second order high-pass filter
    * Dimitris Tassopoulos 2016-2020
    * 
    * fc , corner frequency
    * Q , quality factor controlling resonant peaking
    */

    struct SO_HPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t Q, coef_size_t fs)
        {
            coef_size_t w = 2.0 * M_PI * fc / fs;
            coef_size_t d = 1.0 / Q;
            coef_size_t b = 0.5*(1.0 - (d / 2)*sin(w)) / (1.0 + (d / 2.0)*sin(w));
            coef_size_t g = (0.5 + b)*cos(w);
            m_coeffs.a0 = (0.5 + b + g) / 2.0;
            m_coeffs.a1 = -(0.5 + b + g);
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = -2.0 * g;
            m_coeffs.b2 = 2.0 * b;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(fc,q,sr);
        }    
    };


    /**
    * Second order Linkwitz-Riley high-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency (-6 dB)
    * Second-order Linkwitz/Riley HPFs are designed to have an attenuation of -6 dB
    * at the corner frequency rather than the standard -3 dB When these
    * filters are placed in parallel with the same cutoff frequency, their outputs sum exactly and
    * the resulting response is flat. They are often used in crossovers.
    */

    struct SO_LINKWITZ_RILEY_HPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = M_PI * fc / fs;
            coef_size_t Wc = M_PI * fc;
            coef_size_t k = Wc / tan(th);

            coef_size_t d = pow(k, 2.0) + pow(Wc, 2.0) + 2.0 * k * Wc;
            m_coeffs.a0 = pow(k, 2.0) / d;
            m_coeffs.a1 = -2.0 * pow(k, 2.0) / d;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = (-2.0 * pow(k, 2.0) + 2.0 * pow(Wc, 2.0)) / d;
            m_coeffs.b2 = (-2.0 * k * Wc + pow(k, 2.0) + pow(Wc, 2.0)) / d;            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    
    };


    /**
    * Second order Linkwitz-Riley low-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency (-6 dB)
    * Second-order Linkwitz/Riley LPFs are designed to have an attenuation of -6 dB
    * at the corner frequency rather than the standard -3 dB When these
    * filters are placed in parallel with the same cutoff frequency, their outputs sum exactly and
    * the resulting response is flat. They are often used in crossovers.
    */

    struct SO_LINKWITZ_RILEY_LPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t fs)
        {
            coef_size_t th = M_PI * fc / fs;
            coef_size_t Wc = M_PI * fc;
            coef_size_t k = Wc / tan(th);

            coef_size_t d = pow(k, 2.0) + pow(Wc, 2.0) + 2.0 * k * Wc;
            m_coeffs.a0 = pow(Wc, 2.0) / d;
            m_coeffs.a1 = 2.0 * pow(Wc, 2.0) / d;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = (-2.0 * pow(k, 2.0) + 2.0 * pow(Wc, 2.0)) / d;
            m_coeffs.b2 = (-2.0 * k * Wc + pow(k, 2.0) + pow(Wc, 2.0)) / d;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,sr);
        }    
    };


    /**
    * Second order Low-pass filter
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , corner frequency
    * Q , quality factor controlling resonant peaking
    */

    struct SO_LPF : public Biquad {
    
        void calculate_coeffs(coef_size_t fc, coef_size_t Q, coef_size_t fs)
        {
            coef_size_t w = 2.0 * M_PI * fc / fs;
            coef_size_t d = 1.0 / Q;
            coef_size_t b = 0.5*(1.0 - (d / 2)*sin(w)) / (1.0 + (d / 2.0)*sin(w));
            coef_size_t g = (0.5 + b)*cos(w);
            m_coeffs.a0 = (0.5 + b - g) / 2.0;
            m_coeffs.a1 = 0.5 + b - g;
            m_coeffs.a2 = m_coeffs.a0;
            m_coeffs.b1 = -2.0 * g;
            m_coeffs.b2 = 2.0 * b;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(fc,q,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(fc,q,sr);
        }    
    };


    /**
    * Second order parametric/peaking boost filter with constant-Q
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , center frequency
    * Q quality factor
    * Gain/attenuation in dB
    * Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
    * arbitrary bumps or notches in the frequency response. The parametric EQ is a
    * variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
    * cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
    * EQ not only has independent controls, but each control only varies one coeffi cient in the
    * fi lter. The parametric EQs in this section afford the same frequency response but adjustments
    * in any parameter require a recalculation of all the coeffi cients. These fi lters are also called
    * peaking fi lters.
    *
    * This design creates an almost perfect constant-Q filter with only a small amount of error
    * for low-boost (or cut) values.
    */

    struct SO_PARAMETRIC_CQ_BOOST : public Biquad {
    
        void calculate_coeffs(coef_size_t gain_db, coef_size_t Q, coef_size_t fc, coef_size_t fs)
        {
            coef_size_t K = 2.0 * M_PI * fc / fs;
            coef_size_t V0 = pow(10.0, gain_db / 20.0);
            coef_size_t d0 = 1.0 + K/Q + pow(K, 2.0);
            coef_size_t a = 1.0 + (V0*K)/Q + pow(K, 2.0);
            coef_size_t b = 2.0*(pow(K, 2.0) - 1.0);
            coef_size_t g = 1.0 - (V0*K)/Q + pow(K, 2.0);
            coef_size_t d = 1.0 - K/Q + pow(K, 2.0);
            m_coeffs.a0 = a/d0;
            m_coeffs.a1 = b/d0;
            m_coeffs.a2 = g/d0;
            m_coeffs.b1 = b/d0;
            m_coeffs.b2 = d/d0;
            m_coeffs.c0 = 1.0;
            m_coeffs.d0 = 0.0;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setGain(coef_size_t G)
        {
            g = G;
            calculate_coefficients(g,q,fc,sr);
        }    
    };


    /**
    * Second order parametric/peaking cut filter with constant-Q
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , center frequency
    * Q quality factor
    * Gain/attenuation in dB
    * Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
    * arbitrary bumps or notches in the frequency response. The parametric EQ is a
    * variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
    * cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
    * EQ not only has independent controls, but each control only varies one coeffi cient in the
    * fi lter. The parametric EQs in this section afford the same frequency response but adjustments
    * in any parameter require a recalculation of all the coeffi cients. These fi lters are also called
    * peaking fi lters.
    *
    * This design creates an almost perfect constant-Q filter with only a small amount of error
    * for low-boost (or cut) values.
    */

    struct SO_PARAMETRIC_CQ_CUT : public Biquad {
    
        tp_coeffs calculate_coeffs(coef_size_t gain_db, coef_size_t Q, coef_size_t fc, coef_size_t fs)
        {
            coef_size_t K = 2.0 * M_PI * fc / fs;
            coef_size_t V0 = pow(10.0, gain_db / 20.0);
            coef_size_t d0 = 1.0 + K / Q + pow(K, 2.0);
            coef_size_t e = 1.0 + K / (V0*Q) + pow(K, 2.0);
            coef_size_t b = 2.0*(pow(K, 2.0) - 1.0);
            coef_size_t d = 1.0 - K / Q + pow(K, 2.0);
            coef_size_t h = 1.0 - K / (V0*Q) + pow(K, 2.0);
            m_coeffs.a0 = d0/e;
            m_coeffs.a1 = b / e;
            m_coeffs.a2 = d / e;
            m_coeffs.b1 = b / e;
            m_coeffs.b2 = h / e;
            m_coeffs.c0 = 1.0;
            m_coeffs.d0 = 0.0;
            return(m_coeffs);
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setGain(coef_size_t G)
        {
            g = G;
            calculate_coefficients(g,q,fc,sr);
        }    
    };


    /**
    * Second order parametric/peaking filter with non-constant-Q
    * Dimitris Tassopoulos 2016-2020
    *
    * fc , center frequency
    * Q quality factor
    * Gain/attenuation in dB
    * Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
    * arbitrary bumps or notches in the frequency response. The parametric EQ is a
    * variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
    * cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
    * EQ not only has independent controls, but each control only varies one coeffi cient in the
    * fi lter. The parametric EQs in this section afford the same frequency response but adjustments
    * in any parameter require a recalculation of all the coeffi cients. These fi lters are also called
    * peaking fi lters.
    * This parametric EQ is not constant-Q, which means the bandwidth varies depending on the
    * boost/cut value. Some analog fi lters have the same issue, although there is occasional debate
    * over whether or not this is desirable in an EQ design.
    */

    struct SO_PARAMETRIC_NCQ : public Biquad {
    
        void calculate_coeffs(coef_size_t gain_db, coef_size_t Q, coef_size_t fc, coef_size_t fs)
        {
            coef_size_t w = 2.0 * M_PI * fc / fs;
            coef_size_t m = pow(10.0, gain_db / 20.0);
            coef_size_t z = 4.0 / (1.0 + m);
            coef_size_t b = 0.5 * ((1.0 - z*tan(w / (2.0*Q)) / (1 + z*tan(w / (2.0*Q)))));
            coef_size_t g = (0.5 + b) * cos(w);
            m_coeffs.a0 = 0.5 - b;
            m_coeffs.a1 = 0.0;
            m_coeffs.a2 = -(0.5 - b);
            m_coeffs.b1 = -2.0*g;
            m_coeffs.b2 = 2.0 * b;
            m_coeffs.c0 = m - 1.0;
            m_coeffs.d0 = 1.0;
            
        }
        void setCutoff(coef_size_t Fc)
        {
            fc = Fc;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setQ(coef_size_t Q)
        {
            q = Q;
            calculate_coefficients(g,q,fc,sr);
        }    
        void setGain(coef_size_t G)
        {
            g = G;
            calculate_coefficients(g,q,fc,sr);
        }    
    };
}