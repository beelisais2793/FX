#pragma once

namespace DSPFX
{

    struct FxLadderFilter : public MonoFXProcessor
    {
        LadderFilter * filter;

        FxLadderFilter(DspFloatType sr) {
            filter = LadderFilterInitD(sr);
            assert(filter != NULL);
        }
        ~FxLadderFilter() {
            if(filter) LadderFilterFreeD(filter);
        }
        void ProcessBlock(size_t n, DspFloatType * in, DspFloatType * out) {
            LadderFilterProcessD(filter,out,in,n);
        }
        void setCutoff(DspFloatType c) {
            LadderFilterSetCutoffD(filter,c);
        }
        void setResonance(DspFloatType q) {
            LadderFilterSetResonanceD(filter,q);
        }
        void setTemperature(DspFloatType t) {
            LadderFilterSetTemperatureD(filter,t);
        }
        enum {
            PORT_CUTOFF,
            PORT_RESONANCE,
            PORT_TEMP,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_RESONANCE: setResonance(v); break;
                case PORT_TEMP: setTemperature(v); break;
                default: printf("No Port %d\n",port);
            }
        }
    };
}
