#pragma once

#define GAMMA_H_INC_ALL
#include "Gamma/Gamma.h"
#include "Gamma/Voices.h"


namespace Gamma::Recorder
{
    struct Recorder : public SignalSinkProcessorPlugin<gam::Recorder>
    {
        bool record=false;
        uint32_t channel = 0;
        Recorder(int channels, int frames=8192) 
        : SignalSinkProcessorPlugin<gam::Recorder>(),
        gam::Recorder(channels,frames)
        {

        }
        enum {
            PORT_START,
            PORT_STOP,       
            PORT_CHANNEL,  
            PORT_CHANNELS,
            PORT_FRAMES,
            PORT_SIZE,   
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_START: record = true; break;
                case PORT_STOP: record = false; break;
                case PORT_CHANNEL: channel = v; break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_CHANNELS: return this->channels(); 
                case PORT_FRAMES: return this->frames();
                case PORT_SIZE: return this->size();
            }
        }
        void ProcessBlock(size_t n, float * in) {
            if(record)
                for(size_t i = 0; i < n; i++)
                    this->write(in[i],channel)
        }
        void ProcessBlock(size_t n, float ** in) {
            if(record)
                for(size_t i = 0; i < n; i++)
                    this->write(in[0][i],in[1][i],channel)
        }
        int readBuffer(float * buffer) {
            return this->read(buffer);
        }
        void resize(int ch, int frames) {
            this->resize(ch,frames);
        }
    };
}