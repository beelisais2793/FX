#pragma once

namespace FX
{
    struct StereoDelayProcessor : public StereoFXProcessor
    {
        bool pingpong = true;
        DspFloatType mix = 0.5;
        fv3::delay_f delayL,delayR;

        StereoDelayProcessor(DspFloatType delayTime) : StereoFXProcessor()
        {
            delayL.setsize(delayTime * sampleRate);
            delayR.setsize(delayTime * sampleRate);
        }

        void setDelayTime(DspFloatType dt) { 
            delayL.setsize(dt*sampleRate); 
            delayR.setsize(dt*sampleRate);         
        }
        void setFeedback(DspFloatType v) 
        { 
            delayL.setfeedback(v); 
            delayR.setfeedback(v); 
        }
        void setDelayTimeLeft(DspFloatType dt) {
            delayL.setsize(dt*sampleRate);         
        }
        void setFeedbackLeft(DspFloatType v) 
        { 
            delayL.setfeedback(v);         
        }
        void setDelayTimeRight(DspFloatType dt) {
            delayR.setsize(dt*sampleRate);         
        }
        void setFeedbackRight(DspFloatType v) 
        { 
            delayR.setfeedback(v);         
        }
        enum {
            PORT_DELAYTIME,
            PORT_FEEDBACK,
            PORT_DELAYTIMELEFT,
            PORT_FEEDBACKLEFT,
            PORT_DELAYTIMERIGHT,
            PORT_FEEDBACKRIGHT
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_DELAYTIME: setDelayTime(v); break;
                case PORT_FEEDBACK: setFeedback(v); break;
                case PORT_DELAYTIMELEFT: setDelayTimeLeft(v); break;
                case PORT_DELAYTIMERIGHT: setDelayTimeRight(v); break;
                case PORT_FEEDBACKLEFT: setFeedbackLeft(v); break;
                case PORT_FEEDBACKRIGHT: setFeedbackRight(v); break;
                default: printf("No port %d\n",port);
            }
        }
        DspFloatType TickLeft(DspFloatType t) {
            return delayL.process(t);
        }
        DspFloatType TickRight(DspFloatType t) {
            return delayR.process(t);
        }
        DspFloatType Tick(DspFloatType t) {
            DspFloatType L = TickLeft(t);
            DspFloatType R = TickRight(t);        
            return 0.5*(L+R);        
        }

        void run(size_t numSamples, DspFloatType** inputs, DspFloatType** outputs) 
        {    
            for(size_t i = 0; i < numSamples; i++)
            {
                outputs[pingpong? 1:0][i] = mix*outputs[0][i] + (1.0-mix)*delayL.process(inputs[0][i]);
                outputs[pingpong? 0:1][i] = mix*outputs[1][i] + (1.0-mix)*delayR.process(inputs[1][i]);
            }
        }

        
    };
}