#pragma once

#include "DaisySP.hpp"

namespace DaisySP::Util
{
    struct Looper : public MonoFXProcessorPlugin<daisysp::Looper>
    {
        float * memory;
        size_t  size;
        enum 
        {
            NORMAL,
            ONETIME_DUB,
            REPLACE,
            FRIPPERTRONICS,
        };

        Looper(size_t n) : MonoFXProcessorPlugin<daisysp::Looper>()
        {
            memory = new float[n];
            size = n;
            this->Init(memory,size);
        }
        ~Looper() {
            if(memory) delete [] memory;
        }

        enum {
            PORT_CLEAR,
            PORT_TRIGRECORD,
            PORT_INCMODE,
            PORT_SETMODE,
            PORT_TOGGLE_REVERSE,
            PORT_SET_REVERSE,
            PORT_TOGGLE_HALFSPEED,
            PORT_SET_HALFSPEED,
            PORT_RECORDING_QUEUED,
            PORT_RECORDING,
            PORT_GETMODE,
            PORT_GET_REVERSE,
            PORT_GET_HALFSPEED,
            PORT_IS_NEAR_BEGINNING,
        };
        void setPort(int port, double v) {
            switch(port) {
                case PORT_CLEAR: this->Clear(); break;
                case PORT_TRIGRECORD: this->TrigRecord(); break;
                case PORT_INCMOD: this->IncrementMode(); break;
                case PORT_SETMODE: this->SetMode((Mode)v); break;
                case PORT_TOGGLE_REVERSE: this->ToggleReverse(); break;
                case PORT_SET_REVERSE: this->SetReverse(v); break;
                case PORT_TOGGLE_HALFSPEED: this->ToggleHalfSpeed(); break;
                case PORT_SET_HALFSPEED: this->SetHalfSpeed(v); break;
            }
        }
        double getPort(int port) {
            switch(port) {
                case PORT_RECORDING: return this->Recording();
                case PORT_RECODING_QUEUED: return this->RecordingQueued();
                case PORT_GETMODE: return this->GetMode();
                case PORT_GET_REVERSE: return this->GetReverse();
                case PORT_GET_HALFSPEED: return this->GetHalfSpeed();
                case PORT_IS_NEAR_BEGINNING: return this->IsNearBeginning(); 
            }
        }
        double Tick(double I, double A=1, double X=1, double Y=1) {
            double out = A*this->Process(I);            
            return out;
        }
         void ProcessBlock(size_t n, float * in, float * out) {
            for(size_t i = 0; i < n; i++) out[i] = Tick(in[i]);
        }
    };
}