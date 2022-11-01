#pragma once

namespace FX
{
    ///////////////////////////////////////////////////////////////////////////////
    // Ring Modulator
    ///////////////////////////////////////////////////////////////////////////////
    struct RingModulator : public StereoFXProcessor
    {    
        DspFloatType RingMod_lfoPhase[2];
        DspFloatType RingMod_lfoFreqHz[2];
        DspFloatType RingMod_lfoWidthHz[2];
        DspFloatType RingMod_carrierPhase[2];
        DspFloatType RingMod_carrierFreqHz[2];
        Waveform RingMod_lfoWaveform[2];
        size_t numInputChannels=2;
        size_t numOutputChannels=2;

        enum {
            PORT_LFOPHASE1,
            PORT_LFOPHASE2,
            PORT_LFOFREQ1,
            PORT_LFOFREQ2,
            PORT_LFOWIDTH1,
            PORT_LFOWIDTH2,
            PORT_LFOWAVEFORM1,
            PORT_LFOWAVEFORM2,
            PORT_CARRIERPHASE1,
            PORT_CARRIERPHASE2,
            PORT_CARRIERFREQ1,
            PORT_CARRIERFREQ2,            
        };
        void setPort(int port, DspFloatType v) {
            switch(port)
            {
            case PORT_LFOPHASE1: RingMod_lfoPhase[0] = v; break;
            case PORT_LFOPHASE2: RingMod_lfoPhase[1] = v; break;
            case PORT_LFOFREQ1: RingMod_lfoFreqHz[0] = v; break;
            case PORT_LFOFREQ2: RingMod_lfoFreqHz[1] = v; break;
            case PORT_LFOWIDTH1: RingMod_lfoWidthHz[0] = v; break;
            case PORT_LFOWIDTH2: RingMod_lfoWidthHz[1] = v; break;
            case PORT_LFOWAVEFORM1: RingMod_lfoWaveform[0] = (Waveform)v; break;
            case PORT_LFOWAVEFORM2: RingMod_lfoWaveform[1] = (Waveform)v; break;
            case PORT_CARRIERPHASE1: RingMod_carrierPhase[0] = v; break;
            case PORT_CARRIERPHASE2: RingMod_carrierPhase[1] = v; break;
            case PORT_CARRIERFREQ1: RingMod_carrierFreqHz[0] = v; break;
            case PORT_CARRIERFREQ2: RingMod_carrierFreqHz[1] = v; break;
            default: printf("No port %d\n",port);
            }
        }
        RingModulator(Waveform lfo=kWaveformSine, DspFloatType lfoFreq=12.0, DspFloatType lfoWidth=5.0, DspFloatType carrierFreq=10.0) 
        : StereoFXProcessor()
        {
            setData<Waveform>(RingMod_lfoWaveform,lfo);
            setData<DspFloatType>(RingMod_lfoFreqHz,lfoFreq);
            setData<DspFloatType>(RingMod_lfoWidthHz,lfoWidth);
            setData<DspFloatType>(RingMod_carrierFreqHz,carrierFreq);
            setData<DspFloatType>(RingMod_carrierPhase,0);
            setData<DspFloatType>(RingMod_lfoPhase,0);    
        }

        // it's not necessary anymore but it's finished
        void MonoProcessBlock (size_t n, DspFloatType * inputs, DspFloatType * outputs)
        {
            Undenormal noDenormals;
            size_t channel = 0;
            // local copies of state variables carrierPhase and lfoPhase
            DspFloatType cphi = RingMod_carrierPhase[channel];
            DspFloatType lphi = RingMod_lfoPhase[channel];
            DspFloatType dlphi = DspFloatType(RingMod_lfoFreqHz[channel] * inverseSampleRate);
                
            const DspFloatType* pIn = inputs;
            DspFloatType* pOut = outputs;

            for (int i = 0; i < n; i++)
            {
                // Carrier oscillator is a simple sine wave
                const DspFloatType twoPi = 6.283185f;
                DspFloatType carrier = sin(2.0*M_PI * cphi);

                // Ring modulation is just simple multiplication
                *pOut++ = *pIn++ * carrier;

                // Update carrier phase with FM, keeping in range [0, 1]
                DspFloatType lfo = LFO_GetSample(lphi, RingMod_lfoWaveform[channel]);
                DspFloatType deltaCarrierHz = RingMod_lfoWidthHz[channel] * lfo;
                DspFloatType dcphi = DspFloatType((RingMod_carrierFreqHz[channel] + deltaCarrierHz) * inverseSampleRate);
                
                cphi += dcphi;
                while (cphi >= 1.0) cphi -= 1.0;            
                lphi += dlphi;
                while (lphi >= 1.0) lphi -= 1.0;
            }
            // update the main phase state variables, ready for the next processBlock() call
            RingMod_carrierPhase[channel] = cphi;
            RingMod_lfoPhase[channel] = lphi;    
        }

        // n = numer of samples per channel
        void ProcessBlock (size_t n, DspFloatType ** inputs, DspFloatType ** outputs)
        {
            Undenormal noDenormals;

            // local copies of state variables carrierPhase and lfoPhase
            
            // apply the same modulation to all input channels for which there is an output channel
            
            for (int channelIndex = 0; channelIndex < numInputChannels; channelIndex++)
            {
                DspFloatType cphi = RingMod_carrierPhase[channelIndex];
                DspFloatType lphi = RingMod_lfoPhase[channelIndex];
                DspFloatType dlphi = DspFloatType(RingMod_lfoFreqHz[channelIndex] * inverseSampleRate);

                // restart the phase sequence
                cphi = RingMod_carrierPhase[channelIndex];
                lphi = RingMod_lfoPhase[channelIndex];

                const DspFloatType* pIn = inputs[channelIndex];
                DspFloatType* pOut = outputs[channelIndex];

                for (int i = 0; i < n; i++)
                {
                    // Carrier oscillator is a simple sine wave
                    const DspFloatType twoPi = 6.283185f;
                    DspFloatType carrier = sin(2.0*M_PI * cphi);

                    // Ring modulation is just simple multiplication
                    pOut[i] = pIn[i] * carrier;

                    // Update carrier phase with FM, keeping in range [0, 1]
                    DspFloatType lfo = LFO_GetSample(lphi, RingMod_lfoWaveform[channelIndex]);
                    DspFloatType deltaCarrierHz = RingMod_lfoWidthHz[channelIndex] * lfo;
                    DspFloatType dcphi = DspFloatType((RingMod_carrierFreqHz[channelIndex] + deltaCarrierHz) * inverseSampleRate);
                    
                    cphi += dcphi;
                    while (cphi >= 1.0) cphi -= 1.0;            
                    lphi += dlphi;
                    while (lphi >= 1.0) lphi -= 1.0;
                }
                // update the main phase state variables, ready for the next processBlock() call
                RingMod_carrierPhase[channelIndex] = cphi;
                RingMod_lfoPhase[channelIndex] = lphi;    
            }        
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };
}