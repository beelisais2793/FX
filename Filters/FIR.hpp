#pragma once

namespace Filters::FIR
{
    struct FIR
    {
        std::vector<DspFloatType> taps;
        std::vector<DspFloatType> sr;
        DspFloatType fs,f,fu;
        int    filter_type;
        DspFloatType lambda,phi;

        enum {
            Lowpass,
            Highpass,
            Bandpass,
            Bandstop,
            Peak,
            Notch,
            LowShelf,
            HighShelf,
        };
        FIR(size_t n, int type, DspFloatType sampleRate, DspFloatType fc, DspFloatType _fu = 0, DspFloatType g=1)
        {
            fs = sampleRate;
            f  = fc;
            fu = _fu;
            filter_type = type;
            lambda = M_PI * fc / (fs/2.0);        
            phi = M_PI * fu * (fs/2.0);
            taps.resize(n);
            sr.resize(n);
            switch(type)
            {
                case Lowpass: DesignLowPass(); break;
                case Highpass: DesignHighPass(); break;
                case Bandpass: DesignBandPass(); break;
                case Bandstop: DesignBandStop(); break;
                case Peak: DesignPeak(g); break;
                case Notch: DesignNotch(g); break;
                case LowShelf: DesignLowShelf(g); break;
                case HighShelf: DesignHighShelf(g); break;
            }
        }
        void DesignLowPass()
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = lambda / M_PI;
                else taps[n] = sin( mm * lambda ) / (mm * M_PI);
            }
        }
        void DesignHighPass()
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = 1.0 - lambda / M_PI;
                else taps[n] = -sin( mm * lambda ) / (mm * M_PI);
            }    
        }
        void DesignBandPass()
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = (   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);
            }
        }
        void DesignBandStop()
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = -(   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);
            }
        }
        void DesignPeak(DspFloatType g)
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                DspFloatType bs = -(   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);
                DspFloatType bp = (   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);

                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = g*bp + bs;
            }
        }
        void DesignNotch(DspFloatType g)
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                DspFloatType bs = -(   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);
                DspFloatType bp = (   sin( mm * phi ) -
                                    sin( mm * lambda )   ) / (mm * M_PI);

                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = bp + g*bs;
            }
        }
        void DesignLowShelf(DspFloatType g)
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = g*(sin( mm * lambda ) / (mm * M_PI)) - (sin( mm * lambda ) / (mm * M_PI)); 
            }
        }
        void DesignHighShelf(DspFloatType g)
        {
            int n;
            DspFloatType mm;

            for(n = 0; n < taps.size(); n++){
                mm = n - (taps.size() - 1.0) / 2.0;
                if( mm == 0.0 ) taps[n] = (phi - lambda) / M_PI;
                else taps[n] = (sin( mm * lambda ) / (mm * M_PI)) - g*(sin( mm * lambda ) / (mm * M_PI)); 
            }
        }
        DspFloatType Tick(DspFloatType data_sample)
        {
            int i;
            DspFloatType result;
            Undenormal denormals;

            for(i = taps.size() - 1; i >= 1; i--){
                sr[i] = sr[i-1];
            }	
            sr[0] = data_sample;

            result = 0;
            for(i = 0; i < taps.size(); i++) result += sr[i] * taps[i];

            return result;
        }
        DspFloatType Tick2x(DspFloatType x) {
            Tick(x);
            return Tick(0);
        }
        // todo: gnuplot
        std::complex<DspFloatType> freqResponse(DspFloatType w) {
            std::complex<DspFloatType> r = 0;
            for(size_t i = 0; i < taps.size(); i++)
                r += taps[i]*exp(0,-w*i);
            return r;
        }
        DspFloatType magResponse(DspFloatType w) {
            std::complex<DspFloatType> r = 0;
            for(size_t i = 0; i < taps.size(); i++)
                r += taps[i]*exp(0,-w*i);
            return abs(r);
        }
        DspFloatType phaseResponse(DspFloatType w) {
            std::complex<DspFloatType> r = 0;
            for(size_t i = 0; i < taps.size(); i++)
                r += taps[i]*exp(0,-w*i);
            return arg(r);
        }
        std::vector<DspFloatType> impResponse(size_t len) {
            std::vector<DspFloatType> r(len);
            r[0] = Tick(1);
            for(size_t i = 1; i < len; i++)
                r[i] = Tick(0);
            return r;
        }
    };
}