#pragma once

#include "FxDSP.hpp"
#include <cassert>

namespace DSPFX
{
    struct FxBiquadFilter : public MonoFXProcessor
    {
        BiquadFilterD * filter;

        FxBiquadFilter(const DspFloatType * Bc, const DspFloatType * Ac) : MonoFXProcessor()
        {
            filter = BiquadFilterInitD(Bc,Ac);
            assert(filter != NULL);
        }    
        ~FxBiquadFilter() {
            if(filter) BiquadFilterFreeD(filter);
        }
        void flush() {
            BiquadFilterFlushD(filter);
        }
        void ProcessBlock(size_t n, const DspFloatType * in, DspFloatType * out)
        {
            BiquadFilterProcessD(filter,out,in,n);
        }
        DspFloatType Tick(DspFloatType In) {
            return BiquadFilterTickD(filter,In);
        }
        void updateCoefficients(const DspFloatType * B, const DspFloatType * A) {
            BiquadFilterUpdateKernelD(filter,B,A);
        }
        enum {
            PORT_COEFF,
        };
        void setPortV(int port, const std::vector<DspFloatType> & c) {
            if(port == PORT_COEFF) updateCoefficients(&c[0],&c[3]);
            else printf("No Port %d\n",port);
        }
    };

    struct FxRBJFilter : public MonoFXProcessor
    {
        RBJFilter * filter;
        DspFloatType Cutoff,Q;
        
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

        FxRBJFilter(int type,DspFloatType cutoff, DspFloatType sampleRate) : MonoFXProcessor()
        {
            Cutoff = cutoff;
            Q      = 0.5;
            filter = RBJFilterInitD((Filter_t)type,cutoff,sampleRate);
            assert(filter != NULL);
        }    
        ~FxRBJFilter() {
            if(filter) RBJFilterFreeD(filter);
        }
        void setType(Type type) {
            RBJFilterSetTypeD(filter,(Filter_t)type);
        }
        void setCutoff(DspFloatType cut) {
            Cutoff = cut;
            RBJFilterSetCutoffD(filter,cut);
        }
        void setQ(DspFloatType q) {
            Q = q;
            RBJFilterSetQD(filter,Q);
        }
        void flush() {
            RBJFilterFlushD(filter);
        }
        enum {
            PORT_TYPE,
            PORT_CUTOFF,
            PORT_Q,
            PORT_FLUSH,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_TYPE: setType((Type)v); break;
                case PORT_CUTOFF: setCutoff(v); break;
                case PORT_Q: setQ(v); break;
                case PORT_FLUSH: flush(); break;
            }
        }
        void ProcessBlock(size_t n, const DspFloatType * in, DspFloatType * out)
        {
            RBJFilterProcessD(filter,out,in,n);
        }          
    };
}
