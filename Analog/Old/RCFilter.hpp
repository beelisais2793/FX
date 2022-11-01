#pragma once

#define HIGHPASS 0
#define LOWPASS 1

#define MAX_FILTERS 10

namespace Analog::Filters
{
    struct RCFilter
    {
        double          i[MAX_FILTERS],
                        last_sample[MAX_FILTERS];
        double          max,
                        amplify,
                        R,
                        C,
                        invR,
                        dt_div_C;
        double          di[MAX_FILTERS];
        double          out;
        double          freq=440.0;
        enum {
            LOW,
            HIGH,
            BAND,
        };
        int band = HIGH;
        
        RCFilter(int max, double amplify)
        {
            this->max = max;
            this->amplify = amplify;
            setCutoff(440);
            memset(i, 0, sizeof(i));
            memset(di, 0, sizeof(di));
            memset(last_sample, 0, sizeof(last_sample));
        }
        void LC_filter(double sound, int filter_no)
        {
            double          R,
                            L,
                            C,
                            dt_div_L,
                            dt_div_C;
            double          du,
                            d2i;
            int             t,
                            currchannel = 0;

            
            L = 50e-3;
            C = 1.0 / (4.0 * pow(M_PI * freq, 2.0) * L);
            R = 300.0;

            dt_div_C = 1.0 / (C * sampleRate);
            dt_div_L = 1.0 / (L * sampleRate);

            
            du = sound - last_sample[filter_no];
            last_sample[filter_no] = (double) sound;

            d2i = dt_div_L * (du - i[filter_no] * dt_div_C - R * di[filter_no]);
            di[filter_no] += d2i;
            i[filter_no] += di[filter_no];

            out = (int) (i[filter_no] * 500.0);        
        }
        double other(double f, double x)
        {
            return 1.0 / (2 * M_PI * f * x);
        }

        void RC_set_freq(double f)
        {
            R = 1000.0;
            C = other(f, R);
            invR = 1.0 / R;
            dt_div_C = (1.0 / (sampleRate)) / C;
        }
        void setCutoff(double f) {
            freq = f;
            RC_set_freq(f);
        }
        void RC_filter(double sound, int mode, int filter_no)
        {
            double          du,
                            di;
            int             t,
                            currchannel = 0;
            
            du = (double) sound - last_sample[filter_no];
            last_sample[filter_no] = (double) sound;

            di = invR * (du - i[filter_no] * dt_div_C);
            i[filter_no] += di;

            if (mode == HIGHPASS)
                out = ((i[filter_no] * R) * amplify);
            else
                out =
                (((double) sound -
                    i[filter_no] * R) *
                    amplify);
                    
        }
        void RC_bandpass(double sound)
        {
            int             a;

            for (a = 0; a < max; a++) {
                RC_filter(sound, HIGHPASS, a);
                RC_filter(sound, LOWPASS, a);
            }
        }

        void RC_highpass(double sound)
        {
            int             a;

            for (a = 0; a < max; a++)
                RC_filter(sound, HIGHPASS, a);
        }

        void RC_lowpass(double sound)
        {
            int             a;

            for (a = 0; a < max; a++)
                RC_filter(sound, LOWPASS, a);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double f = freq;        
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