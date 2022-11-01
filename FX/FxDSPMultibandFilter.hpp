#pragma once

namespace DSPFX
{

    struct FxMultibandFilter
    {
        MultibandFilter * filter;

        FxMultiBandFilter(DspFloatType low, DspFloatType high, DspFloatType sr = sampleRate)
        {
            filter = MultibandFilterInitD(low,high,sr);
            assert(filter != NULL);
        }
        ~FxMultiBandFilter() {
            if(filter) MultibandFilterFreeD(filter);
        }
        void flush() { MultibandFilterFlush(filter); }
        void setHighCutoff(DspFloatType c) {
            MultibandFilterSetHighCutoffD(filter,c);
        }
        void setLowCutoff(DspFloatType c) {
            MultibandFilterSetLowCutoffD(filter,c);
        }
        void update(DspFloatType low, DspFloatType high) {
            MultibandFilterUpdateD(filter,low,high);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * low, DspFloatType * mid, DspFloatType * high) {
            MultibandFilterProcessD(filter,low,mid,high,in,n);
        }
        enum {
            PORT_HIGHCUT,
            PORT_LOWCUT,
            PORT_UPDATE,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_HIGHCUT: setHighCutoff(v); break;
                case PORT_LOWCUT: setLowCutoff(v); break;
                default: printf("No Port %d\n",port);
            }
        }
        void setPort2(int port, double a, double b) {
            switch(port) {
                case PORT_HIGHCUT: update(a,b)); break;                
                default: printf("No Port %d\n",port);
            }
        }
    };

}
