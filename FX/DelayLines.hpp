#pragma once

#include <vector>
#include <cmath>
#include <cstring>
#include "Undenormal.hpp"

extern DspFloatType sampleRate;

namespace Delay::DelayLines
{
    struct ringbuffer
    {
        std::vector<DspFloatType> buffer;
        int read_cursor,write_cursor;

        ringbuffer() = default;
        ringbuffer(const ringbuffer & r) {
            buffer = r.buffer;
            read_cursor = r.read_cursor;
            write_cursor = r.write_cursor;
        }
        ringbuffer(size_t n) {
            buffer.resize(n);
            //zeros(buffer);
            memset(buffer.data(),0,buffer.size()*sizeof(DspFloatType));
            read_cursor = 0;
            write_cursor = 0;
        }
        size_t size() const { 
            return buffer.size();
        }
        void resize(size_t s) {
            buffer.resize(s);
    //        zeros(buffer);
            memset(buffer.data(),0,buffer.size()*sizeof(DspFloatType));
            read_cursor  %= s;
            write_cursor %= s;
        }
        void calc_delay(DspFloatType time)
        {
            write_cursor = 0;
            read_cursor  = (time * sampleRate);
            read_cursor  %= buffer.size();
        }
        void set_delay(size_t samples)
        {
            write_cursor = 0;
            read_cursor  = samples % buffer.size();
        }
        DspFloatType read(int cursor)
        {
            return buffer[cursor];
        }
        void write(int cursor, DspFloatType value)
        {
            buffer[cursor] = value;
        }
        DspFloatType& operator[](int idx) {
            return buffer[idx];
        }
        ringbuffer& operator = (const ringbuffer & r) {
            buffer = r.buffer;
            read_cursor = r.read_cursor;
            write_cursor = r.write_cursor;
            return *this;
        }
        DspFloatType read() 
        {
            DspFloatType x1 = buffer[read_cursor++];
            read_cursor %= buffer.size();
            DspFloatType x2 = buffer[read_cursor];
            DspFloatType frac = x1 - floor(x1);
            DspFloatType out = x1 + frac * (x2-x1);        
            return out;
        }
        void write(DspFloatType v)
        {
            buffer[write_cursor++] = v;
            write_cursor %= buffer.size();
        }
    };


    struct delayline : public FunctionProcessor
    {
        ringbuffer delay;
        size_t read_cursor,write_cursor;
        DspFloatType feedback;
        DspFloatType mix;
        size_t delayLen;
        DspFloatType delayTime;

        enum InterpType
        {
            None,
            NearestNeighbor,
            Linear,
            Cubic,
            Spline3,
            Spline5,
            Hermite1,
            Hermite2,
            Hermite3,
            Hermite4,
        }
        interpType;

        
        delayline() = default;
        delayline(const delayline & d) : FunctionProcessor() {
            delay = d.delay;
            read_cursor = d.read_cursor;
            write_cursor = d.write_cursor;
            feedback = d.feedback;
            mix = d.mix;
            delayLen = d.delayLen;
            delayTime  = d.delayTime;
        }
        delayline(DspFloatType delay_time, DspFloatType delay_size = 1) : FunctionProcessor() {
            delay.resize(sampleRate*delay_size);        
            feedback=0.5;
            mix = 0.5;
            delayTime = delay_time;        
            delayLen  = delay_time * sampleRate;        
            read_cursor  = 0;
            write_cursor = delayLen;
            interpType = Linear;
        }

        DspFloatType& operator[](size_t i) {
            return delay[i % delay.size()];
        }
        delayline& operator = (const delayline& d) {
            delay = d.delay;
            read_cursor = d.read_cursor;
            write_cursor = d.write_cursor;
            feedback = d.feedback;
            mix = d.mix;
            delayLen = d.delayLen;
            delayTime  = d.delayTime;
            return *this;
        }
        void setDelaySize(DspFloatType size) {
            delay.resize(size);        
        }
        void reset() {
            read_cursor  = 0;
            write_cursor = delayLen;   
        }
        void setDelayTime(DspFloatType f) 
        {
            delayTime = f;
            delayLen  = f;
            write_cursor = (read_cursor + delayLen) % delayLen;
            read_cursor  = 0;
            write_cursor = delayLen;
        }
        void setFeedback(DspFloatType f) {
            feedback = f;
        }
        void setMix(DspFloatType m) {
            mix = std::fmod(m,1);
        }
        void resize(size_t n) {
            delay.resize(n);
        }
        virtual DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 1, DspFloatType Y = 1) {        
            size_t n = read_cursor;
            DspFloatType output = delay.read(read_cursor++);            
            DspFloatType d1 = A*(I - Y*output*feedback);                
            DspFloatType f= d1 - floor(d1); //(int)d1;        
            read_cursor  %= delayLen;
            output = Interpolate(n,f);            
            delay.write(write_cursor++, I*Y + feedback*output);                    
            write_cursor %= delayLen;            
            return mix*I + (1.0-mix)*output;
        }
        size_t size() { return delay.size(); }    

        DspFloatType Interpolate(size_t n, DspFloatType frac)
        {
            switch(interpType)
            {
                case None: return delay[n];
                case NearestNeighbor: return NearestNeighborInterpolate(n,frac);
                case Linear: return LinearInterpolate(n,frac);
                case Cubic: return CubicInterpolate(n,frac);
                case Hermite1: return Hermite1Interpolate(n,frac);
                case Hermite2: return Hermite2Interpolate(n,frac);
                case Hermite3: return Hermite3Interpolate(n,frac);
                case Hermite4: return Hermite4Interpolate(n,frac);
                case Spline3:  return Spline3Interpolate(n,frac);
                case Spline5:  return Spline5Interpolate(n,frac);
            }
            return delay[read_cursor];
        }
        DspFloatType NearestNeighborInterpolate(size_t n,DspFloatType frac)
        {
            int   x  = round(frac);
            DspFloatType x1 = delay[ (n + x) % delayLen];
            return x1;
        }
        DspFloatType LinearInterpolate(size_t n,DspFloatType frac)
        {            
            DspFloatType x1 = delay[n];
            DspFloatType x2 = delay[ (n+1) % delayLen];
            //DspFloatType frac = x1 - (int)x1;
            return x1 + ((x2-x1)*frac);
        }    
        // just cubic stuff from musicdsp
        DspFloatType CubicInterpolate(size_t inpos,DspFloatType finpos)
        {            
            DspFloatType xm1 = delay[(inpos - 1) % delayLen];
            DspFloatType x0 = delay[inpos + 0];
            DspFloatType x1  = delay[(inpos + 1) % delayLen];
            DspFloatType x2  = delay[(inpos + 2) % delayLen];
            DspFloatType a = (3 * (x0-x1) - xm1 + x2) / 2;
            DspFloatType b = 2*x1 + xm1 - (5*x0 + x2) / 2;
            DspFloatType c = (x1 - xm1) / 2;
            return (((a * finpos) + b) * finpos + c) * finpos + x0;
        }
        // just another kind of cubials it might be the same kakaloke really
        inline DspFloatType Hermite1Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType y0 = delay[(inpos - 1) % delayLen];
            DspFloatType y1 = delay[inpos + 0];
            DspFloatType y2  = delay[(inpos + 1) % delayLen];
            DspFloatType y3  = delay[(inpos + 2) % delayLen];
            // 4-point, 3rd-order Hermite (x-form)
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);

            return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite2Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType y0 = delay[(inpos - 1) % delayLen];
            DspFloatType y1 = delay[inpos + 0];
            DspFloatType y2  = delay[(inpos + 1) % delayLen];
            DspFloatType y3  = delay[(inpos + 2) % delayLen];
            // 4-point, 3rd-order Hermite (x-form)
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            DspFloatType c2 = y0 - y1 + c1 - c3;

            return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite3Interpolate(size_t inpos, DspFloatType x)
        {                
                DspFloatType y0 = delay[(inpos - 1) % delayLen];
                DspFloatType y1 = delay[inpos + 0];
                DspFloatType y2  = delay[(inpos + 1) % delayLen];
                DspFloatType y3  = delay[(inpos + 2) % delayLen];
                // 4-point, 3rd-order Hermite (x-form)
                DspFloatType c0 = y1;
                DspFloatType c1 = 0.5f * (y2 - y0);
                DspFloatType y0my1 = y0 - y1;
                DspFloatType c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
                DspFloatType c2 = y0my1 + c1 - c3;

                return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite4Interpolate(size_t inpos, DspFloatType frac_pos)
        {            
            DspFloatType xm1 = delay[(inpos - 1) % delayLen];
            DspFloatType x0 = delay[inpos + 0];
            DspFloatType x1  = delay[(inpos + 1) % delayLen];
            DspFloatType x2  = delay[(inpos + 2) % delayLen];
            const DspFloatType    c     = (x1 - xm1) * 0.5f;
            const DspFloatType    v     = x0 - x1;
            const DspFloatType    w     = c + v;
            const DspFloatType    a     = w + v + (x2 - x0) * 0.5f;
            const DspFloatType    b_neg = w + a;

            return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
        }
        DspFloatType Spline3Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType L1 = delay[(inpos-1)%delayLen];
            DspFloatType L0 = delay[inpos];
            DspFloatType H0 = delay[(inpos + 1) % delayLen];
            DspFloatType H1 = delay[(inpos + 2) % delayLen];
            return L0 + .5f* x*(H0-L1 + x*(H0 + L0*(-2) + L1 + x*( (H0 - L0)*9 + (L1 - H1)*3 + x*((L0 - H0)*15 + (H1 -  L1)*5 +  x*((H0 - L0)*6 + (L1 - H1)*2 )))));
        }
        DspFloatType Spline5Interpolate(size_t inpos, DspFloatType x)
        {
            /* 5-point spline*/
            int nearest_sample = delayLen;

            DspFloatType p0=delay[(nearest_sample-2) % delayLen];
            DspFloatType p1=delay[(nearest_sample-1) % delayLen];
            DspFloatType p2=delay[nearest_sample];
            DspFloatType p3=delay[(nearest_sample+1) % delayLen];
            DspFloatType p4=delay[(nearest_sample+2) % delayLen];
            DspFloatType p5=delay[(nearest_sample+3) % delayLen];

            return p2 + 0.04166666666*x*((p3-p1)*16.0+(p0-p4)*2.0
            + x *((p3+p1)*16.0-p0-p2*30.0- p4
            + x *(p3*66.0-p2*70.0-p4*33.0+p1*39.0+ p5*7.0- p0*9.0
            + x *( p2*126.0-p3*124.0+p4*61.0-p1*64.0- p5*12.0+p0*13.0
            + x *((p3-p2)*50.0+(p1-p4)*25.0+(p5-p0)*5.0)))));
        }
    };


    struct multidelayline : public FunctionProcessor
    {
        std::vector<delayline> delays;        
        
        multidelayline() : FunctionProcessor() {

        }
        void addDelay(const delayline& d) {
            delays.push_back(d);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0) {
            DspFloatType r = I;
            for(size_t i = 0; i < delays.size(); i++)
                r = delays[i].Tick(r,A,X,Y);
            return r;
        }
    };

    struct multitapdelayline : public FunctionProcessor
    {
        std::vector<size_t> tap_reads;
        size_t taps;
        size_t write_cursor, read_cursor;
        size_t delayLen;
        ringbuffer delay;        
        DspFloatType feedback;
        DspFloatType mix;

        enum InterpType
        {
            None,
            NearestNeighbor,
            Linear,
            Cubic,
            Spline3,
            Spline5,
            Hermite1,
            Hermite2,
            Hermite3,
            Hermite4,
        }
        interpType;

        multitapdelayline() = default;
        multitapdelayline (DspFloatType delayTime, DspFloatType delayScale = 1.0)
        : FunctionProcessor()
        {            
            delay.resize(delayScale*sampleRate);                
            feedback=0.5;
            mix = 0.5;             
            delayLen   = sampleRate*delayTime;            
            interpType = Linear;
        }
        void setDelaySize(size_t size) {
            delay.resize(size);
        }
        void setDelayTime(DspFloatType time) {
            delayLen = sampleRate*time;
        }
        void addTap(DspFloatType t) {
            size_t d = t*sampleRate;            
            tap_reads.push_back(d);                    
            taps++;
        }        
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) 
        {       
            DspFloatType output = 0;
            DspFloatType sum    = 0;            
            size_t read;
            size_t len;
            for(size_t i = 0; i < taps; i++)
            {                            
                read = tap_reads[i];                                
                output = delay.read(read);                        
                DspFloatType f  = output - floor(output); //(int)output;                                        
                DspFloatType x1 = output;
                read = read  % delayLen;
                DspFloatType x2 = delay[read];
                output = x1 + (f*(x2-x1));
                tap_reads[i] = read;
                sum += output;                
            }         
            if( taps > 0) sum /= taps;        
                
            DspFloatType x1 = 0;
            size_t r = read_cursor;
            x1 = delay.read(read_cursor);  
            output = x1;
            DspFloatType f  = output - (int)output;                                                                
            output = 0.5*(output + Interpolate(r,f));
            read_cursor = read_cursor % delayLen;
            delay.write(write_cursor, I + Y*feedback*output);        
            write_cursor %= delayLen;
            return mix*I + (1.0-mix)*(0.5*(sum+output));            
        }
        DspFloatType Interpolate(size_t n, DspFloatType frac)
        {
            switch(interpType)
            {
                case None: return delay[n];
                case NearestNeighbor: return NearestNeighborInterpolate(n,frac);
                case Linear: return LinearInterpolate(n,frac);
                case Cubic: return CubicInterpolate(n,frac);
                case Hermite1: return Hermite1Interpolate(n,frac);
                case Hermite2: return Hermite2Interpolate(n,frac);
                case Hermite3: return Hermite3Interpolate(n,frac);
                case Hermite4: return Hermite4Interpolate(n,frac);
                case Spline3:  return Spline3Interpolate(n,frac);
                case Spline5:  return Spline5Interpolate(n,frac);
            }
            return delay[read_cursor];
        }
        DspFloatType NearestNeighborInterpolate(size_t n,DspFloatType frac)
        {
            int   x  = round(frac);
            DspFloatType x1 = delay[ (n + x) % delayLen];
            return x1;
        }
        DspFloatType LinearInterpolate(size_t n,DspFloatType frac)
        {            
            DspFloatType x1 = delay[n];
            DspFloatType x2 = delay[ (n+1) % delayLen];
            //DspFloatType frac = x1 - (int)x1;
            return x1 + ((x2-x1)*frac);
        }    
        // just cubic stuff from musicdsp
        DspFloatType CubicInterpolate(size_t inpos,DspFloatType finpos)
        {            
            DspFloatType xm1 = delay[(inpos - 1) % delayLen];
            DspFloatType x0 = delay[inpos + 0];
            DspFloatType x1  = delay[(inpos + 1) % delayLen];
            DspFloatType x2  = delay[(inpos + 2) % delayLen];
            DspFloatType a = (3 * (x0-x1) - xm1 + x2) / 2;
            DspFloatType b = 2*x1 + xm1 - (5*x0 + x2) / 2;
            DspFloatType c = (x1 - xm1) / 2;
            return (((a * finpos) + b) * finpos + c) * finpos + x0;
        }
        // just another kind of cubials it might be the same kakaloke really
        inline DspFloatType Hermite1Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType y0 = delay[(inpos - 1) % delayLen];
            DspFloatType y1 = delay[inpos + 0];
            DspFloatType y2  = delay[(inpos + 1) % delayLen];
            DspFloatType y3  = delay[(inpos + 2) % delayLen];
            // 4-point, 3rd-order Hermite (x-form)
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);

            return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite2Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType y0 = delay[(inpos - 1) % delayLen];
            DspFloatType y1 = delay[inpos + 0];
            DspFloatType y2  = delay[(inpos + 1) % delayLen];
            DspFloatType y3  = delay[(inpos + 2) % delayLen];
            // 4-point, 3rd-order Hermite (x-form)
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            DspFloatType c2 = y0 - y1 + c1 - c3;

            return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite3Interpolate(size_t inpos, DspFloatType x)
        {                
                DspFloatType y0 = delay[(inpos - 1) % delayLen];
                DspFloatType y1 = delay[inpos + 0];
                DspFloatType y2  = delay[(inpos + 1) % delayLen];
                DspFloatType y3  = delay[(inpos + 2) % delayLen];
                // 4-point, 3rd-order Hermite (x-form)
                DspFloatType c0 = y1;
                DspFloatType c1 = 0.5f * (y2 - y0);
                DspFloatType y0my1 = y0 - y1;
                DspFloatType c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
                DspFloatType c2 = y0my1 + c1 - c3;

                return ((c3 * x + c2) * x + c1) * x + c0;
        }    
        inline DspFloatType Hermite4Interpolate(size_t inpos, DspFloatType frac_pos)
        {            
            DspFloatType xm1 = delay[(inpos - 1) % delayLen];
            DspFloatType x0 = delay[inpos + 0];
            DspFloatType x1  = delay[(inpos + 1) % delayLen];
            DspFloatType x2  = delay[(inpos + 2) % delayLen];
            const DspFloatType    c     = (x1 - xm1) * 0.5f;
            const DspFloatType    v     = x0 - x1;
            const DspFloatType    w     = c + v;
            const DspFloatType    a     = w + v + (x2 - x0) * 0.5f;
            const DspFloatType    b_neg = w + a;

            return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
        }
        DspFloatType Spline3Interpolate(size_t inpos, DspFloatType x)
        {            
            DspFloatType L1 = delay[(inpos-1)%delayLen];
            DspFloatType L0 = delay[inpos];
            DspFloatType H0 = delay[(inpos + 1) % delayLen];
            DspFloatType H1 = delay[(inpos + 2) % delayLen];
            return L0 + .5f* x*(H0-L1 + x*(H0 + L0*(-2) + L1 + x*( (H0 - L0)*9 + (L1 - H1)*3 + x*((L0 - H0)*15 + (H1 -  L1)*5 +  x*((H0 - L0)*6 + (L1 - H1)*2 )))));
        }
        DspFloatType Spline5Interpolate(size_t inpos, DspFloatType x)
        {
            /* 5-point spline*/
            int nearest_sample = delayLen;

            DspFloatType p0=delay[(nearest_sample-2) % delayLen];
            DspFloatType p1=delay[(nearest_sample-1) % delayLen];
            DspFloatType p2=delay[nearest_sample];
            DspFloatType p3=delay[(nearest_sample+1) % delayLen];
            DspFloatType p4=delay[(nearest_sample+2) % delayLen];
            DspFloatType p5=delay[(nearest_sample+3) % delayLen];

            return p2 + 0.04166666666*x*((p3-p1)*16.0+(p0-p4)*2.0
            + x *((p3+p1)*16.0-p0-p2*30.0- p4
            + x *(p3*66.0-p2*70.0-p4*33.0+p1*39.0+ p5*7.0- p0*9.0
            + x *( p2*126.0-p3*124.0+p4*61.0-p1*64.0- p5*12.0+p0*13.0
            + x *((p3-p2)*50.0+(p1-p4)*25.0+(p5-p0)*5.0)))));
        }
    };  



    struct combfilter : public FilterProcessor
    {
        delayline delay[2];
        DspFloatType x1,y,y1;
        DspFloatType gain[2];
        DspFloatType delayTime[2];
        
        enum {
            X_index,
            Y_index,
        };

        combfilter(const combfilter& f) {
            delay[0] = f.delay[0];
            delay[1] = f.delay[1];
            x1 = f.x1;
            y  = f.y;
            y1 = f.y1;
            gain[0] = f.gain[0];
            gain[1] = f.gain[1];
            delayTime[0] = f.delayTime[0];
            delayTime[1] = f.delayTime[1];
        }
        combfilter(DspFloatType _g1, DspFloatType _g2, DspFloatType _d1, DspFloatType _d2)
        : FilterProcessor()
        {
            gain[X_index] = _g1;
            gain[Y_index] = _g2;
            delayTime[0] = _d1;
            delayTime[1] = _d2;
            for(size_t i = 0; i < 2; i++)
            {
                delay[i].setDelaySize(44100);
                delay[i].setDelayTime(delayTime[i]);
            }       
            x1=y=y1=0;
        }    

        combfilter& operator = (const combfilter& f) {
            delay[0] = f.delay[0];
            delay[1] = f.delay[1];
            x1 = f.x1;
            y  = f.y;
            y1 = f.y1;
            gain[0] = f.gain[0];
            gain[1] = f.gain[1];
            delayTime[0] = f.delayTime[0];
            delayTime[1] = f.delayTime[1];
            return *this;
        }
        // X modulation * depth
        // Y modulation * depth
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X = 0, DspFloatType Y=0)
        {
            DspFloatType x = I;
            y = x + gain[X_index] * x1 - gain[Y_index] * y1;
            x1 = delay[X_index].Tick(x);
            y1 = delay[Y_index].Tick(y);
            return y;
        }
    };


    struct iircombfilter : public FilterProcessor
    {
        delayline delay;
        DspFloatType g,x,y,y1;

        iircombfilter(const iircombfilter& c) : FilterProcessor()
        {
            delay = c.delay;
            g = c.g;
            x = c.x;
            y = c.y;
            y1 = c.y1;
        }
        iircombfilter(DspFloatType _g, DspFloatType _d) : FilterProcessor()
        {
            delay.setDelaySize(44100);
            delay.setDelayTime(_d);
            g = _g;
            x = y = y1 = 0;
        }
        iircombfilter& operator = (const iircombfilter& f) {
            delay = f.delay;
            g = f.g;
            x = f.x;
            y = f.y;
            y1 = f.y1;
            return *this;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y= 0)
        {
            x = I;
            y1= delay.Tick(y);
            y = x - g*y1;
            
            return y;
        }
    };


    struct fircombfilter : public FilterProcessor
    {
        delayline delay;
        DspFloatType g,x,x1,y;

        fircombfilter(const fircombfilter& c) : FilterProcessor()
        {
            delay = c.delay;
            g = c.g;
            x = c.x;
            y = c.y;
            x1 = c.x1;
        }
        fircombfilter(DspFloatType _g, DspFloatType _d) : FilterProcessor()
        {
            delay.setDelaySize(44100);
            delay.setDelayTime(_d);
            g = _g;
            x = y = x1 = 0;
        }
        fircombfilter& operator = (const fircombfilter& c) {
            delay = c.delay;
            g = c.g;
            x = c.x;
            y = c.y;
            x1 = c.x1;
            return *this;
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y= 0)
        {
            x = I;
            y = x + g*x1;
            x1= delay.Tick(x);
            return y;
        }
    };



    struct multitapcombfilter : public FilterProcessor
    {
        multitapdelayline delay[2];
        DspFloatType x1,y,y1;
        DspFloatType gain[2];
        DspFloatType delayTime[2];
        
        enum {
            X_index,
            Y_index,
        };

        multitapcombfilter(DspFloatType _g1, DspFloatType _g2, DspFloatType _d1, DspFloatType _d2)
        : FilterProcessor()
        {
            gain[X_index] = _g1;
            gain[Y_index] = _g2;
            delayTime[0] = _d1;
            delayTime[1] = _d2;
            for(size_t i = 0; i < 2; i++)
            {
                delay[i].setDelaySize(44100);
                delay[i].setDelayTime(delayTime[i]);
            }       
            x1=y=y1=0;
        }    
        multitapcombfilter(const multitapcombfilter & f) : FilterProcessor() {
            delay[0] = f.delay[0];
            delay[1] = f.delay[1];
            x1 = f.x1;
            y  = f.y;
            y1 = f.y1;
            for(size_t i = 0; i < 2; i++)
            {
                gain[i] = f.gain[i];
                delayTime[i] = f.delayTime[i];
            }
        }
        multitapcombfilter& operator = (const multitapcombfilter & f) {
            delay[0] = f.delay[0];
            delay[1] = f.delay[1];
            x1 = f.x1;
            y  = f.y;
            y1 = f.y1;
            for(size_t i = 0; i < 2; i++)
            {
                gain[i] = f.gain[i];
                delayTime[i] = f.delayTime[i];
            }
            return *this;
        }
        void addTap(DspFloatType t) {
            delay[0].addTap(t);
            delay[1].addTap(t);
        }
        // X modulation * depth
        // Y modulation * depth
        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X = 0, DspFloatType Y=0)
        {
            DspFloatType x = I;
            y = x + gain[X_index] * x1 - gain[Y_index] * y1;
            x1 = delay[X_index].Tick(x);
            y1 = delay[Y_index].Tick(y);
            return y;
        }
    };


    struct multitapiircombfilter : public FilterProcessor
    {
        multitapdelayline delay;
        DspFloatType g,x,y,y1;

        multitapiircombfilter(DspFloatType _g, DspFloatType _d) 
        : FilterProcessor()
        {
            delay.setDelaySize(44100);
            delay.setDelayTime(_d);
            g = _g;
            x = y = y1 = 0;
        }
        multitapiircombfilter(const multitapiircombfilter & f) : FilterProcessor() {
            delay = f.delay;
            g = f.g;
            x = f.x;
            y = f.y;
            y1 = f.y1;
        }
        void addTap(DspFloatType t) {
            delay.addTap(t);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y= 0)
        {
            x = I;
            y = x - g*y1;
            y1= delay.Tick(y);
            return y;
        }
        multitapiircombfilter& operator = (const multitapiircombfilter& f) {
            delay = f.delay;
            g = f.g;
            x = f.x;
            y = f.y;
            y1 = f.y1;
            return *this;
        }
    };


    struct multitapfircombfilter : public FilterProcessor
    {
        multitapdelayline delay;
        DspFloatType g,x,x1,y;

        multitapfircombfilter(DspFloatType _g, DspFloatType _d)  : FilterProcessor()
        {
            delay.setDelaySize(44100);
            delay.setDelayTime(_d);
            g = _g;
            x = y = x1 = 0;
        }
        multitapfircombfilter(const multitapfircombfilter & f) : FilterProcessor() {
            delay = f.delay;
            g = f.g;
            x = f.x;
            y = f.y;
            x1 = f.x1;
        }
        void addTap(DspFloatType t) {
            delay.addTap(t);
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y= 0)
        {
            x = I;
            y = x + g*x1;
            x1= delay.Tick(x);
            return y;
        }
        multitapfircombfilter& operator = (const multitapfircombfilter& f) {
            delay = f.delay;
            g = f.g;
            x = f.x;
            y = f.y;
            x1 = f.x1;
            return *this;
        }
    };

    
    struct biquaddelay : public FilterProcessor
    {
        enum FilterType
        {
            LowPass = 1,
            HighPass,
            BandPass,
            Notch,
            AllPass,
            Peaking,
            LowShelf,
            HighShelf,
        };

        struct Parameters
        {
            FilterType filterType;
            DspFloatType fs;
            DspFloatType f0;
            DspFloatType Q;
            DspFloatType dBGain;
        };

        delayline  delayin,delayout;
        
        Parameters mparams;
        
        DspFloatType ma0, ma1, ma2, mb0, mb1, mb2;        
        DspFloatType mx_z1, mx_z2, my_z1, my_z2;
        
        void calculateCoeffs();
        
        biquaddelay(DspFloatType inDelayTime, DspFloatType outDelayTime)
        :   FilterProcessor(),
            delayin(inDelayTime),
            delayout(outDelayTime)
        {
            ma0 = ma1 = ma2 = mb0 = mb1 = mb2 = 0;
            mx_z1 = mx_z2 = my_z1 = my_z2 = 0;
            mparams.fs = sampleRate;
            mparams.f0 = 1000.0f;
            mparams.Q  = 0.5;
            mparams.dBGain = 1;
            mparams.filterType = LowPass;        
            calculateCoeffs();
        };

        void setCutoff(DspFloatType f0) {
            mparams.f0 = f0;
            calculateCoeffs();
        }
        void setQ(DspFloatType q) {
            mparams.Q = q;
            calculateCoeffs();
        }
        void setGain(DspFloatType g) {
            mparams.dBGain = g;
            calculateCoeffs();
        }
        void setSampleRate(DspFloatType sr)
        {
            mparams.fs = sr;
            calculateCoeffs();
        }
        ~biquaddelay(){};
        void setParams(const Parameters& params);
        Parameters getParams();
        DspFloatType process(DspFloatType x);    

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1)
        {
            return A*process(I);
        }
    };


    void biquaddelay::setParams(const Parameters& params)
    {
        mparams = params;
        calculateCoeffs();
    }

    biquaddelay::Parameters biquaddelay::getParams()
    {
        return mparams;
    }

    void biquaddelay::calculateCoeffs()
    {
        DspFloatType omega0 = 2.0f * M_PI * (mparams.f0 / mparams.fs);
        DspFloatType alpha = std::sin(omega0) / (2.0 * mparams.Q);
        DspFloatType A = std::pow(10, mparams.dBGain / 40.0);
        switch (mparams.filterType)
        {
        case FilterType::LowPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 - std::cos(omega0)) / 2.0;
            mb1 = 1.0 - std::cos(omega0);
            mb2 = (1.0 - std::cos(omega0)) / 2.0;            
            break;
        }
        case FilterType::HighPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = (1.0 + std::cos(omega0)) / 2.0;
            mb1 = -(1.0 + std::cos(omega0));
            mb2 = (1.0 + std::cos(omega0)) / 2.0;
            break;
        }
        case FilterType::BandPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = alpha;
            mb1 = 0;
            mb2 = -alpha;
            break;
        }
        case FilterType::Notch:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0;
            break;
        }
        case FilterType::AllPass:
        {
            ma0 = 1.0 + alpha;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha;
            mb0 = 1.0 - alpha;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 + alpha;
            break;
        }
        case FilterType::Peaking:
        {
            ma0 = 1.0 + alpha / A;
            ma1 = -2.0 * std::cos(omega0);
            ma2 = 1.0 - alpha / A;
            mb0 = 1.0 + alpha * A;
            mb1 = -2.0 * std::cos(omega0);
            mb2 = 1.0 - alpha * A;
            break;
        }
        case FilterType::LowShelf:
        {
            ma0 = (A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = -2.0 * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        case FilterType::HighShelf:
        {
            ma0 = (A + 1.0) - (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha;
            ma1 = 2.0 * ((A - 1.0) - (A + 1.0) * std::cos(omega0));
            ma2 = (A + 1.0) - (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha;
            mb0 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) + 2.0 * std::sqrt(A) * alpha);
            mb1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * std::cos(omega0));
            mb2 = A * ((A + 1.0) + (A - 1.0) * std::cos(omega0) - 2.0 * std::sqrt(A) * alpha);
            break;
        }
        default:
            break;
        }
        if(ma0 != 0.0f)
        {
            mb0 /= ma0;
            mb1 /= ma0;
            mb2 /= ma0;
            ma1 /= ma0;
            ma2 /= ma0;
        }
    }

    DspFloatType biquaddelay::process(DspFloatType x)
    {
        Undenormal denormals;
        x = delayin.Tick(x);
        DspFloatType y = mb0 * x + mb1 * mx_z1 + mb2 * mx_z2 - ma1 * my_z1 - ma2 * my_z2;

        mx_z2 = mx_z1;
        mx_z1 = x;

        my_z2 = my_z1;
        my_z1 = y;
        
        return delayout.Tick(y);
    }

}
