// DspFilters
#pragma once
#include "DspFilters/Bessel.h"

namespace Filters::IIR::Bessel
{
    // bessel filter doesn't preserve group delay in digital   
    struct LowPassFilter : public FilterProcessor
    {
        Dsp::Bessel::LowPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,R;
        
        LowPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            R     = 1.0;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            if(Fc < 0 || Fc >= sr/2.0) return;
            fc = Fc;            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc);
            for(size_t i = 0; i < total; i++) {
                Dsp::Cascade::Stage s = prototype[i];
                sos[i].z[0] = s.m_b0;
                sos[i].z[1] = s.m_b1;
                sos[i].z[2] = s.m_b2;
                sos[i].p[0] = s.m_a1;
                sos[i].p[1] = s.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        // it has to be solved before the bilinear/z transform
        void setQ(DspFloatType q) {            
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) 
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };

    // bessel filter doesn't preserve group delay in digital
    struct HighPassFilter : public FilterProcessor
    {
        Dsp::Bessel::HighPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        
        HighPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            // this filter alises very badly above sr/4
            fc = Fc;
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc);
            for(size_t i = 0; i < total; i++) {
                Dsp::Cascade::Stage s = prototype[i];
                sos[i].z[0] = s.m_b0;
                sos[i].z[1] = s.m_b1;
                sos[i].z[2] = s.m_b2;
                sos[i].p[0] = s.m_a1;
                sos[i].p[1] = s.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {
            // not used yet
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) 
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };

    // bessel filter doesn't preserve group delay in digital
    struct BandPassFilter : public FilterProcessor
    {
        Dsp::Bessel::BandPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,bw;
        
        BandPassFilter(size_t Order, DspFloatType BW,DspFloatType Fc, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            bw    = BW;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;        
            int total = order;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,bw);
            for(size_t i = 0; i < total; i++) {
                Dsp::Cascade::Stage s = prototype[i];
                sos[i].z[0] = s.m_b0;
                sos[i].z[1] = s.m_b1;
                sos[i].z[2] = s.m_b2;
                sos[i].p[0] = s.m_a1;
                sos[i].p[1] = s.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {
            // not used yet
        }
        void setBW(DspFloatType BW) {
            bw = BW;
            setCutoff(fc);
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF,
            PORT_BW,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) 
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_BW: bw = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };

    // bessel filter doesn't preserve group delay in digital
    struct BandStopFilter : public FilterProcessor
    {
        Dsp::Bessel::BandStop<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,bw;
        
        BandStopFilter(size_t Order, DspFloatType BW, DspFloatType Fc, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            bw    = BW;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            // this filter alises very badly above sr/4
            fc = Fc;
            int total = order;            
            sos.resize(total);
            prototype.setup(order,sr,fc,bw);
            for(size_t i = 0; i < total; i++) {
                Dsp::Cascade::Stage s = prototype[i];
                sos[i].z[0] = s.m_b0;
                sos[i].z[1] = s.m_b1;
                sos[i].z[2] = s.m_b2;
                sos[i].p[0] = s.m_a1;
                sos[i].p[1] = s.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {
            // not used yet
        }
        void setBW(DspFloatType BW) {
            bw = BW;
            setCutoff(fc);
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF,
            PORT_BW,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) 
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_BW: bw = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
}