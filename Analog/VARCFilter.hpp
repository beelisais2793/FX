#pragma once

#define HIGHPASS 0
#define LOWPASS 1
#define MAX_FILTERS 10

namespace Analog::Filters
{
    struct RCFilter : public FilterProcessor
    {
        DspFloatType          i[MAX_FILTERS],
                        last_sample[MAX_FILTERS];
        DspFloatType          max,
                        amplify,
                        R,
                        C,
                        invR,
                        dt_div_C;
        DspFloatType          di[MAX_FILTERS];
        DspFloatType          out;
        DspFloatType          freq=440.0;
        enum {
            LOW,
            HIGH,
            BAND,
        };
        int band = HIGH;
        
        RCFilter(int max, DspFloatType amplify) : FilterProcessor()
        {
            this->max = max;
            this->amplify = amplify;
            setCutoff(440);
            memset(i, 0, sizeof(i));
            memset(di, 0, sizeof(di));
            memset(last_sample, 0, sizeof(last_sample));
        }
        void LC_filter(DspFloatType sound, int filter_no)
        {
            DspFloatType          R,
                            L,
                            C,
                            dt_div_L,
                            dt_div_C;
            DspFloatType          du,
                            d2i;
            int             t,
                            currchannel = 0;

            
            L = 50e-3;
            C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
            R = 300.0;

            dt_div_C = 1.0 / (C * sampleRate);
            dt_div_L = 1.0 / (L * sampleRate);

            
            du = sound - last_sample[filter_no];
            last_sample[filter_no] = (DspFloatType) sound;

            d2i = dt_div_L * (du - i[filter_no] * dt_div_C - R * di[filter_no]);
            di[filter_no] += d2i;
            i[filter_no] += di[filter_no];

            out = (int) (i[filter_no] * 500.0);        
        }
        DspFloatType other(DspFloatType f, DspFloatType x)
        {
            return 1.0 / (2 * M_PI * f * x);
        }

        void RC_set_freq(DspFloatType f)
        {
            R = 1000.0;
            C = other(f, R);
            invR = 1.0 / R;
            dt_div_C = (1.0 / (sampleRate)) / C;
        }
        void setCutoff(DspFloatType f) {
            freq = f;
            RC_set_freq(f);
        }
        
        void RC_filter(DspFloatType sound, int mode, int filter_no)
        {
            DspFloatType          du,
                            di;
            int             t,
                            currchannel = 0;
            
            du = (DspFloatType) sound - last_sample[filter_no];
            last_sample[filter_no] = (DspFloatType) sound;

            di = invR * (du - i[filter_no] * dt_div_C);
            i[filter_no] += di;

            if (mode == HIGHPASS)
                out = ((i[filter_no] * R) * amplify);
            else
                out =
                (((DspFloatType) sound -
                    i[filter_no] * R) *
                    amplify);
                    
        }
        void RC_bandpass(DspFloatType sound)
        {
            int             a;

            for (a = 0; a < max; a++) {
                RC_filter(sound, HIGHPASS, a);
                RC_filter(sound, LOWPASS, a);
            }
        }

        void RC_highpass(DspFloatType sound)
        {
            int             a;

            for (a = 0; a < max; a++)
                RC_filter(sound, HIGHPASS, a);
        }

        void RC_lowpass(DspFloatType sound)
        {
            int             a;

            for (a = 0; a < max; a++)
                RC_filter(sound, LOWPASS, a);
        }
        enum {
            PORT_CUTOFF,
            PORT_LP,
            PORT_HP,
            PORT_BP,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_LP: band = LOW; break;
                case PORT_HP: band = HIGH; break;
                case PORT_BP: band = BAND; break;
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            DspFloatType f = freq;        
            setCutoff(freq*X);
            switch(band) {
                case LOW: RC_lowpass(I); break;
                case HIGH: RC_highpass(I); break;
                case BAND: RC_bandpass(I); break;
            }        
            setCutoff(f);
            return A*out;
        }
    };
}
#undef HIGHPASS 
#undef LOWPASS 
#undef MAX_FILTERS
