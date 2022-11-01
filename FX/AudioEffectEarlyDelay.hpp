#pragma once

// need to use freeverb DspFloatType
namespace FX
{
  struct EarlyDelayReverb : public StereoFXProcessor
  {

    EarlyDelayReverb(DspFloatType sampleRate);
    
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

    
    void setSize(long s) { 
        delayL.setsize(s); 
        delayR.setsize(s);         
    }
    void setFeedback(DspFloatType v) 
    { 
        delayL.setfeedback(v); 
        delayR.setfeedback(v); 
    }
    void setSizeLeft(long s) { 
        delayL.setsize(s);         
    }
    void setFeedbackLeft(DspFloatType v) 
    { 
        delayL.setfeedback(v);         
    }
    void setSizeRight(long s) { 
        delayR.setsize(s);         
    }
    void setFeedbackRight(DspFloatType v) 
    { 
        delayR.setfeedback(v);         
    }
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
        PORT_FEEDBACK,        
        PORT_SIZE,
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
            case PORT_FEEDBACK: setFeedback(v); break;
            case PORT_SIZE: setSize(v); break;     
            default: printf("No port %d\n",port);       
        }
    }
    DspFloatType mix = 0.5;
    DspFloatType sampleRate;

#ifdef DSPFLOATDOUBLE
    fv3::earlyref_ model;
    fv3::delay_ delayL,delayR;
#else
    fv3::earlyref_f model;
    fv3::delay_f delayL,delayR;
#endif    
    bool pingpong = true;

    void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
        run(n,in,out);
    }
  };

    /////////////////////////////////////////////////////////////////////////////////
    // Early Delay
    /////////////////////////////////////////////////////////////////////////////////
    inline EarlyDelayReverb::EarlyDelayReverb(DspFloatType sampleRate) : StereoFXProcessor() 
    {
        //model.loadPresetReflection(FV3_EARLYREF_PRESET_1);
        model.setMuteOnChange(false);
        model.setdry(0.5); 
        model.setwet(0.5); 
        model.setdryr(0.5); 
        model.setwetr(0.5); 
        model.setwidth(0.8);
        model.setLRDelay(0.3);
        model.setLRCrossApFreq(750, 4);
        model.setDiffusionApFreq(150, 4);  
        delayL.setsize(0.5*sampleRate);
        delayL.setfeedback(0.5);
        delayR.setsize(0.5*sampleRate);
        delayR.setfeedback(0.5);
        sampleRateChanged(sampleRate);
    }

  inline void EarlyDelayReverb::run(size_t numSamples, DspFloatType** inputs, DspFloatType** outputs) 
  {    
      model.processreplace(
          const_cast<DspFloatType *>(inputs[0]),
          const_cast<DspFloatType *>(inputs[1]),
          outputs[0],
          outputs[1],
          numSamples
      );
      for(size_t i = 0; i < numSamples; i++)
      {
          outputs[pingpong? 1:0][i] = mix*outputs[0][i] + (1.0-mix)*delayL.process(outputs[0][i]);
          outputs[pingpong? 0:1][i] = mix*outputs[1][i] + (1.0-mix)*delayR.process(outputs[1][i]);
      }
  }

  inline void EarlyDelayReverb::sampleRateChanged(DspFloatType newSampleRate) {
    sampleRate = newSampleRate;
    model.setSampleRate(newSampleRate);  
  }

  inline void EarlyDelayReverb::mute() {
    model.mute();
    delayL.mute();
    delayR.mute();
  }

}