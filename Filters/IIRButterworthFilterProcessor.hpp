
// DspFilters
#pragma once
#include "DspFilters/Butterworth.h"

namespace Filters::IIR::Butterworth
{    
    // todo: get analog prototype and apply Radius/Q
    struct LowPassFilter : public FilterProcessor
    {
        Dsp::Butterworth::LowPass<32> prototype;
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
        void setResonance(DspFloatType r) {
            
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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
    struct HighPassFilter : public FilterProcessor
    {
        Dsp::Butterworth::HighPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,R;
        HighPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            R     = 1.0;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
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
        void setResonance(DspFloatType r) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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
    struct BandPassFilter : public FilterProcessor
    {
        Dsp::Butterworth::BandPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,bw,R;

        BandPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType W)
        {            
            order = Order;
            sr    = Fs;
            bw    = W;
            R     = 1.0;
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
        void setResonance(DspFloatType q) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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
    struct BandStopFilter : public FilterProcessor
    {
        Dsp::Butterworth::BandStop<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,bw,R;
        BandStopFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType W)
        {            
            order = Order;
            sr    = Fs;
            bw    = W;
            R     = 1.0;
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
        void setResonance(DspFloatType q) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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
    struct LowShelfFilter : public FilterProcessor
    {
        Dsp::Butterworth::LowShelf<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,g;
        LowShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType G)
        {            
            order = Order;
            sr    = Fs;
            g     = G;
            R     = 1.0;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            
            fc = Fc;
            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,g);
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
        void setResonance(DspFloatType q) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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

    struct HighShelfFilter : public FilterProcessor
    {
        Dsp::Butterworth::HighShelf<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,g;
        HighShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType G)
        {            
            order = Order;
            sr    = Fs;
            g     = G;
            R     = 1.0;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,g);
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
        void setResonance(DspFloatType q) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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

    struct BandShelfFilter : public FilterProcessor
    {
        Dsp::Butterworth::BandShelf<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr,g,bw;
        HighShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType W, DspFloatType G)
        {            
            order = Order;
            sr    = Fs;
            g     = G;
            bw    = W;
            R     = 1.0;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            int total = order;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,bw,g);
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
        void setResonance(DspFloatType q) {
         
        }
        enum {
            PORT_ORDER,
            PORT_CUTOFF
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
}