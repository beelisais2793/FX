#pragma once
#include "StkHeaders.hpp
namespace Stk::FX
{
    struct TapDelay : public MonoFXProcessorPlugin<stk::ReedTable>
    {        
        size_t numTaps = 0;

        TapDelay(size_t n) : MonoFXProcessorPlugin<stk::TapDelay>()
        {
            numTaps = n;
            this->setTapDelays(n);
        }
        enum {
            PORT_MAXDELAY,            
        };
        void setPort(int port, double v) {
            if(port == PORT_MAXDELAY) this->setMaximumDelay(v);
        }
        // if callback is set it is called with the StkFrames of each tap
        // returns the average of all taps
        double Tick(double I, double A=1, double X=1, double Y=1)
        {
            StkFrames outputs(numTaps);
            outputs = this->tick(I,outputs);            
            double o = 0;
            for(size_t i = 0; i < numTaps; i++) o += outputs[i];
            return o/(double)numTaps;
        }
         void ProcessBlock(size_t n, float * input, float * output) {
            for(size_t i = 0; i < n; i++) output[i] = Tick(input[i]);
        }
    };

    struct StereoTapDelay : public StereoFXProcessorPlugin<stk::ReedTable>
    {        
        size_t numTaps = 0;

        StereoTapDelay(size_t n) : StereoFXProcessorPlugin<stk::TapDelay>()
        {
            numTaps = n;
            this->setTapDelays(n);
        }
        enum {
            PORT_MAXDELAY,            
        };
        void setPort(int port, double v) {
            if(port == PORT_MAXDELAY) this->setMaximumDelay(v);
        }
        // if callback is set it is called with the StkFrames of each tap
        // returns the average of all taps
        double Tick(double iL, double iR, double &L, double & R, double A=1, double X=1, double Y=1)
        {
            StkFrames outputs(numTaps);
            outputs = this->tick(I,outputs);            
            double o = 0;
            L = 0;
            R = 0;
            for(size_t i = 0; i < numTaps; i++) 
            { 
                o += outputs[i];
                if(i % 2 == 0) R += outputs[i];
                else L += outputs[i];

            return o/(double)numTaps;
        }
        void ProcessBlock(size_t n, float ** input, float ** output) {
            for(size_t i = 0; i < n; i++) 
            {
                double L,R;
                Tick(input[0][i],input[1][i],L,R);
                output[0][i] = L;
                output[1][i] = R;
            }
        }
    };

}