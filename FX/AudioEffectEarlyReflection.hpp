#pragma once

// need to use freeverb double
namespace FX
{
    struct EarlyReflectionReverb : public StereoFXProcessor
    {

        EarlyReflectionReverb(DspFloatType sampleRate);
        
        void  run(size_t n, DspFloatType** inputs, DspFloatType** outputs);
        void  sampleRateChanged(DspFloatType newSampleRate);
        void  mute();

        void setMute(bool value) { model.setMuteOnChange(value); }
        void setDry(DspFloatType value) { model.setdry(value); }
        void setDryR(DspFloatType value) { model.setdryr(value); }
        void setWet(DspFloatType value) { model.setwet(value); }
        void setWetR(DspFloatType value) { model.setwetr(value); }
        void setWidth(DspFloatType value) { model.setwet(value); }
        void setLRDelay(DspFloatType value) { model.setLRDelay(value); }
        void setLRCrossApFreq(DspFloatType f, DspFloatType n) { model.setLRCrossApFreq(f,n); }
        void setDiffusionApFreq(DspFloatType f, DspFloatType n) { model.setDiffusionApFreq(f,n); }
        void setOutputLPF(DspFloatType v) { model.setoutputlpf(v); }
        void setOutputHPF(DspFloatType v) { model.setoutputhpf(v); }
        
        void setRSFactor(DspFloatType v) { model.setRSFactor(v); }
        //void setFSFactors() { model.setFSFactors(); }
        void setInitialDelay(long n) { model.setInitialDelay(n); }
        void setPreDelay(DspFloatType ms) { model.setPreDelay(ms); }

        void setPreset1() {
            model.loadPresetReflection(FV3_EARLYREF_PRESET_0);  
        }
        void setPreset2() {
            model.loadPresetReflection(FV3_EARLYREF_PRESET_1);  
        }
        void setPreset3() {
            model.loadPresetReflection(FV3_EARLYREF_PRESET_2);  
        }
        // n = 0 to 11
        void setPreset11to22(int n) {
            model.loadPresetReflection(FV3_EARLYREF_PRESET_11 + n);  
        }

        enum {
            PORT_MUTE,
            PORT_RY,
            PORT_RYR,
            PORT_WET,
            PORT_WETR,
            PORT_WIDTH,
            PORT_LRDELAY,
            //PORT_LRCROSSAPFREQ,
            //PORT_IFFUSIONAPFREQ,
            PORT_OUTPUTLPF,
            PORT_OUTPUTHPF,
            PORT_RSFACTOR,
            PORT_INITDELAY,
            PORT_PREDELAY,        
            //PORT_FEEDBACK,        
            //PORT_SIZE,
            PORT_PRESET,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_MUTE: setMute((bool)v); break;
                case PORT_RY: setDry(v); break;
                case PORT_RYR: setDryR(v); break;
                case PORT_WET: setWet(v); break;
                case PORT_WETR: setWetR(v); break;
                case PORT_WIDTH: setWidth(v); break;
                case PORT_LRDELAY: setLRDelay(v); break;
                //case PORT_LRCROSSAPFREQ: setLRCrossAPFreq(v); break;
                //case PORT_IFFUSIONAPFREQ: setDiffusionAPFreq(v); break;
                case PORT_OUTPUTLPF: setOutputLPF(v); break;
                case PORT_OUTPUTHPF: setOutputHPF(v); break;
                case PORT_RSFACTOR: setRSFactor(v); break;
                case PORT_INITDELAY: setInitialDelay(v); break;
                case PORT_PREDELAY: setPreDelay(v); break;
                //case PORT_FEEDBACK: setFeedback(v); break;
                //case PORT_SIZE: setSize(v); break;       
                default: printf("No port %d\n",port);
            }
        }
        void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
            run(n,in,out);
        }
        
        DspFloatType sampleRate;
#ifdef DSPFLOATDOUBLE        
        fv3::earlyref_ model;
#else
        fv3::earlyref_f model;
#endif
    };


    /////////////////////////////////////////////////////////////////////////////////
    // Early Reflection
    /////////////////////////////////////////////////////////////////////////////////
    inline EarlyReflectionReverb::EarlyReflectionReverb(DspFloatType sampleRate) : StereoFXProcessor() 
    {
        //model.loadPresetReflection(FV3_EARLYREF_PRESET_1);
        model.setMuteOnChange(false);
        model.setdryr(0.5); // mute dry signal
        model.setwet(0.5); // 0dB
        model.setwidth(0.8);
        model.setLRDelay(0.3);
        model.setLRCrossApFreq(750, 4);
        model.setDiffusionApFreq(150, 4);  
        sampleRateChanged(sampleRate);
    }

    inline void EarlyReflectionReverb::run(size_t numSamples, DspFloatType** inputs, DspFloatType** outputs) 
    {    
        model.processreplace(
            const_cast<DspFloatType *>(inputs[0]),
            const_cast<DspFloatType *>(inputs[1]),
            outputs[0],
            outputs[1],
            numSamples
        );        
    }

    inline void EarlyReflectionReverb::sampleRateChanged(DspFloatType newSampleRate) {
        sampleRate = newSampleRate;
        model.setSampleRate(newSampleRate);
    }

    inline void EarlyReflectionReverb::mute() {
        model.mute();
    }
}