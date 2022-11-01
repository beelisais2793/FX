#pragma once 

namespace FX
{
    struct LFO : public GeneratorProcessor
    {
        Waveform lfoWaveform;
        DspFloatType    lfoPhase;  
        DspFloatType    lfoPhaseInc;           
        DspFloatType    lfoFreqHz;
        DspFloatType   sampleRate;
        DspFloatType    duty;

        enum {
            PORT_WAVEFORM,
            PORT_PHASE,
            PORT_FREQ,
            PORT_DUTY
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_PHASE: lfoPhase = v; break;
                case PORT_FREQ: setFrequency(v); break;
                case PORT_WAVEFORM: setWaveform((Waveform)v); break;
                case PORT_DUTY: setDuty(v); break;
                default: printf("No port %d\n",port);
            }
        }
        
        LFO(DspFloatType freq=0.1, Waveform w = kWaveformSine, DspFloatType sr=44100.0f) {
            sampleRate = sr;
            lfoFreqHz  = freq;
            lfoPhaseInc = freq/sr;
            lfoPhase = 0.0f;
            duty = 0.5f;
            lfoWaveform = kWaveformSine;;
        }
        void setFrequency(DspFloatType f)
        {
            lfoFreqHz   = f;
            lfoPhaseInc = f/sampleRate;
        }
        void setPhase(DspFloatType p)
        {
            lfoPhase = p;
        }
        void setDuty(DspFloatType d) {
            duty = d;
        }
        void setWaveform(Waveform w) {
            lfoWaveform = w;
        }
        DspFloatType Tick(DspFloatType I = 1, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            DspFloatType p = lfoPhase;        
            DspFloatType f = lfoFreqHz;
            DspFloatType hz = X*lfoFreqHz;
            DspFloatType phase = fmod(p + hz/sampleRate + Y,1);
            DspFloatType r;
            switch (lfoWaveform)
            {
            case kWaveformRandomPhase:
                r= ((DspFloatType)rand()/(DspFloatType)RAND_MAX)*phase;            
                break;
            case kWaveformRandom:
                r=((DspFloatType)rand()/(DspFloatType)RAND_MAX);
                break;
            case kWaveformTangent:
                r= std::tan(M_PI/2 * (phase * 0.995));
                break;
            case kWaveformSaw:
                r= phase;
                break;
            case kWaveformReverseSaw:
                r= 1.0-phase;            
                break;
            case kWaveformTriangle:
                if (phase < 0.25f)
                    r=0.5f + 2.0f*phase;
                else if (phase < 0.75f)
                    r=1.0f - 2.0f*(phase - 0.25f);
                else
                    r=2.0f*(phase - 0.75f);
                break;
            case kWaveformSquare:
                if (phase <duty)
                    r=1.0f;
                else
                    r=0.0f;
                break;
            case kWaveformSquareSlopedEdges:
                if (phase < 0.48f)
                    r=1.0f;
                else if (phase < 0.5f)
                    r=1.0f - 50.0f*(phase - 0.48f);
                else if (phase < 0.98f)
                    r=0.0f;
                else
                    r=50.0f*(phase - 0.98f);
                break;
            case kWaveformSine:
            default:
                r= 0.5f + 0.5f*sinf(2*M_PI*phase);
            }        
            lfoPhase = fmodf(lfoPhase + lfoPhaseInc,1);
            return r;
        }
        void ProcessBlock(size_t n, DspFloatType * buffer)
        {
            for(size_t i = 0; i < n; i++) buffer[i] = Tick();
        }
    };

    ///////////////////////////////////////////////////////////////////////////////
    // LFO
    // Function for calculating "biased" LFO waveforms with output range [0, 1].
    // Phase range [0, 1], output also [0, 1] (not [-1, +1] as for the ordinary Sine function).
    ///////////////////////////////////////////////////////////////////////////////
    DspFloatType LFO_GetSample(DspFloatType phase, Waveform waveform)
    {
        switch (waveform)
        {
        case kWaveformTriangle:
            if (phase < 0.25f)
                return 0.5f + 2.0f*phase;
            else if (phase < 0.75f)
                return 1.0f - 2.0f*(phase - 0.25f);
            else
                return 2.0f*(phase - 0.75f);
        case kWaveformSquare:
            if (phase < 0.5f)
                return 1.0f;
            else
                return 0.0f;
        case kWaveformSquareSlopedEdges:
            if (phase < 0.48f)
                return 1.0f;
            else if (phase < 0.5f)
                return 1.0f - 50.0f*(phase - 0.48f);
            else if (phase < 0.98f)
                return 0.0f;
            else
                return 50.0f*(phase - 0.98f);
        case kWaveformSine:
        default:
            return 0.5f + 0.5f*sinf(2*M_PI*phase);
        }
    }
}