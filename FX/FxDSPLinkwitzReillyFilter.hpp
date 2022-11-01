#pragma once

namespace DSPFX
{

    struct FxLRFilter
    {
        LRFilter * filter;
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
        DspFloatType cutoff, Q;
        int type = LOWPASS;
        FxLRFilter(int type, DspFloatType cut, DspFloatType q, DspFloatType sr = sampleRate) {
            filter = LRFilterInitD(type,cut,q,sr);
            assert(filter != NULL);
            cutoff = cut;
            Q = q;
            this->type = type;
        }
        ~FxLRFilter() {
            if(filter) LRFilterFreeD(filter);
        }
        void flush() { LRFilterFlushD(filter); }
        void setParams(int type, DspFloatType cut, DspFloatType q) {
            this->type = type;
            cutoff = cut;
            Q = q;
            LRFilterSetParamsD(filter,type,cutoff,Q);
        }
        void setCutoff(DspFloatType cut) {
            cutoff = cut;
            LRFilterSetParamsD(filter,type,cutoff,Q);
        }
        void setQ(DspFloatType q) {
            Q = q;
            LRFilterSetParamsD(filter,type,cutoff,Q);
        }
        void setType(int type) {
            this->type = type;
            LRFilterSetParamsD(filter,type,cutoff,Q);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            LRFilterProcessBlockD(filter,out,in,n);
        }
        enum {
            PORT_CUTOFF,
            PORT_Q,
            PORT_TYPE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_TYPE: setType(v); break;
                default: printf("No Port %d\n",port);
            }
        }
    };
}
