#pragma once

// Dsp Filters
// DspFilters
#pragma once
#include "DspFilters/RBJ.h"

namespace Filters::IIR::RBJ
{

    struct LowPassFilter : public FilterProcessor
    {
        Dsp::RBJ::LowPass prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        LowPassFilter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct HighPassFilter : public FilterProcessor
    {
        Dsp::RBJ::HighPass prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        HighPassFilter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct AllPassFilter : public FilterProcessor
    {
        Dsp::RBJ::AllPass prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        AllPassFilter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct BandPass1Filter : public FilterProcessor
    {
        Dsp::RBJ::BandPass1 prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        BandPass1Filter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct BandPass2Filter : public FilterProcessor
    {
        Dsp::RBJ::BandPass2 prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        BandPass2Filter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct BandStopFilter : public FilterProcessor
    {
        Dsp::RBJ::BandStopPass prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,Q;
        BandStopFilter(size_t Order, DspFloatType Fc, DspFloatType q, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            Q     = pow(q,(DspFloatType)Order);
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,Q);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setQ(DspFloatType q) {            
            Q = pow(q,(DspFloatType)order);
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct LowShelfFilter : public FilterProcessor
    {
        Dsp::RBJ::LowShelf prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,G,S;
        LowShelfFilter(size_t Order, DspFloatType Fc, DspFloatType g, DspFloatType slope, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            G     = g;
            S     = slope;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,G,S);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setGain(DspFloatType g) {            
            G = g;
            setCutoff(fc);
        }
        void setSlope(DspFloatType s) {            
            S = s;
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct HighShelfFilter : public FilterProcessor
    {
        Dsp::RBJ::HighShelf prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,G,S;
        HighShelfFilter(size_t Order, DspFloatType Fc, DspFloatType g, DspFloatType slope, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            G     = g;
            S     = slope;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,G,S);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setGain(DspFloatType g) {            
            G = g;
            setCutoff(fc);
        }
        void setSlope(DspFloatType s) {            
            S = s;
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };
    struct BandShelfFilter : public FilterProcessor
    {
        Dsp::RBJ::BandShelf prototype;
        BiquadTypeICascade biquads;
        size_t order;
        DspFloatType fc,sr,G,S;
        BandShelfFilter(size_t Order, DspFloatType Fc, DspFloatType g, DspFloatType slope, DspFloatType Fs)
        {            
            order = Order;
            sr    = Fs;
            G     = g;
            S     = slope;
            setCutoff(Fc);            
        }
        void setCutoff(DspFloatType Fc) {
            BiquadSOS sos;            
            fc = Fc;            
            sos.resize(order);
            prototype.setup(sr,fc,G,S);
            for(size_t i = 0; i < total; i++) {                
                sos[i].z[0] = prototype.m_b0;
                sos[i].z[1] = prototype.m_b1;
                sos[i].z[2] = prototype.m_b2;
                sos[i].p[0] = prototype.m_a1;
                sos[i].p[1] = prototype.m_a2;
                sos[i].p[2] = 0;
            }
            biquads.setCoefficients(sos);
        }
        void setGain(DspFloatType g) {            
            G = g;
            setCutoff(fc);
        }
        void setSlope(DspFloatType s) {            
            S = s;
            setCutoff(fc);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            return biquads.Tick(I,A,X,Y);
        }
    };

}
