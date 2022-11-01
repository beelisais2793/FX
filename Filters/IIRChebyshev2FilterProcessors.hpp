#pragma once
#include "DspFilters/Bessel.h"
namespace Filters::IIR::ChebyshevII
{
    struct LowPassFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::LowPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;

        LowPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType stopBand=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop  = stopBand;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            
            fc = Fc;
            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,stop);
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
            PORT_STOPBAND,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct HighPassFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::HighPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;

        HighPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType stopBand=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop  = stopBand;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            
            fc = Fc;
            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,stop);
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
            PORT_STOPBAND,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
   struct BandPassFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::BandPass<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;
        DspFloatType bw;

        BandPassFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType BW=1.0, DspFloatType stopBand=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop  = stopBand;
            bw    = BW;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            
            fc = Fc;
            
            int total = order;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,bw,stop);
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
            PORT_BANDWIDTH,
            PORT_STOPBAND,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
                case PORT_BANDWIDTH: bw = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
   struct BandStopFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::BandStop<32> prototype;
        BiquadTransposedTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;
        DspFloatType bw;

        BandStopFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType BW=1.0, DspFloatType stopBand=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop  = stopBand;
            bw    = BW;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;
            
            fc = Fc;
            
            int total = order;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,bw,stop);
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
            PORT_BANDWIDTH,
            PORT_STOPBAND,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
                case PORT_BANDWIDTH: bw = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };        
    struct BandShelfFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::BandShelf<32> prototype;
        BiquadTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;
        DspFloatType bw,g;

        BandShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType BW, DspFloatType G, DspFloatType stopdb=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop=stopdb;
            bw = BW;
            g  = G;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            int total = order;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,bw,g,stop);
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
            PORT_BANDWIDTH,
            PORT_STOPBAND,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
                case PORT_BANDWIDTH: bw = v; break;
                case PORT_GAIN: g = v; break;
            }
            setCutoff(fc);
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };        
    struct LowShelfFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::LowShelf<32> prototype;
        BiquadTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;
        DspFloatType bw,g;

        LowShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType G, DspFloatType stopdb=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop=stopdb;            
            g  = G;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,g,stop);
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
            PORT_BANDWIDTH,
            PORT_STOPBAND,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
                case PORT_BANDWIDTH: bw = v; break;
                case PORT_GAIN: g = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };        
    struct HighShelfFilter : public FilterProcessor
    {
        Dsp::ChebyshevII::HighShelf<32> prototype;
        BiquadTypeIICascade biquads;
        size_t order;
        DspFloatType fc,sr;
        DspFloatType stop;
        DspFloatType bw,g;

        HighShelfFilter(size_t Order, DspFloatType Fc, DspFloatType Fs, DspFloatType G, DspFloatType stopdb=1.0)
        {            
            order = Order;
            sr    = Fs;
            stop = stopdb;
            g  = G;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            int total = order/2;
            if(order == 1) { total++; }
            sos.resize(total);
            prototype.setup(order,sr,fc,g,stop);
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
            PORT_BANDWIDTH,
            PORT_STOPBAND,
            PORT_GAIN,
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
                case PORT_ORDER: order = (int)v; break;
                case PORT_CUTOFF: fc = v; break;
                case PORT_STOPBAND: stop = v; break;
                case PORT_BANDWIDTH: bw = v; break;
                case PORT_GAIN: g = v; break;
            }
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };        
}
