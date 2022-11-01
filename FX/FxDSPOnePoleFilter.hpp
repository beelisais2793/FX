#pragma once

namespace DSPFX
{

    struct FxOnePoleFilter : FilterProcessor
    {
        OnePole * filter;

        enum Type
        {
        /** Lowpass */
        LOWPASS,

        /** Highpass */
        HIGHPASS,

        /** Bandpass */
        BANDPASS,

        /** Allpass */
        ALLPASS,

        /** Notch */
        NOTCH,

        /** Peaking */
        PEAK,

        /** Low Shelf */
        LOW_SHELF,

        /** High Shelf */
        HIGH_SHELF,

        /** Number of Filter types */
        N_FILTER_TYPES
        };
        DspFloatType cutoff;
        // only low and highpass are valie
        FxOnePoleFilter(DspFloatType cut, int type=LOWPASS, DspFloatType sr=sampleRate)
        : FilterProcessor()
        {
            cutoff = cut;
            filter = OnePoleInitD(cut,sr,type);
            assert(filter != NULL);
        }
        ~FxOnePoleFilter() {
            if(filter) OnePoleFreeD(filter);
        }
        void flush() { OnePoleFlushD(filter); }
        void setType(int type) {
            OnePoleSetTypeD(filter,type);
        }
        void setCutoff(DspFloatType c) {
            cutoff = c;
            OnePoleSetCutoffD(filer,c);
        }
        void setCoefficients(DspFloatType beta, DspFloatType alpha) {
            OnePoleSetCoefficientsD(&beta, &alpha);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out)
        {
            OnePoleProcessD(filter,out,in,n);
        }
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            double c = cutoff;            
            setCutoff(cutoff * X * Y);
            double out = OnePoleTickD(I);
            setCutoff(c);
            return out * A;
        }
        enum {
            PORT_CUTOFF,
            PORT_COEFFS,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;                
                default: printf("no port %d\n",port);
            }
        }
        void setPort(int port, double v) {
            switch(port) {
                case PORT_COEFFICIENTS: setCoefficients(a,b); break;
                default: printf("no port %d\n",port);
            }
        }
        
    };
}
