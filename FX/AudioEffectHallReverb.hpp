#pragma once

namespace FX
{
  struct HallReverb : public StereoFXProcessor
  {
    #ifdef DSPFLOATDOUBLE
    fv3::earlyref_ early;
    fv3::zrev2_ late;
    #else
    fv3::earlyref_f early;
    fv3::zrev2_f late;
    #endif

    HallReverb(DspFloatType sampleRate);

    DspFloatType getParameterValue(uint32_t index) const;
    void  setParameterValue(uint32_t index, DspFloatType value);
    
    void run(size_t n, DspFloatType** inputs, DspFloatType** outputs);
    void sampleRateChanged(DspFloatType newSampleRate);
    void mute();

    void setEarlyMuteOnChange(bool value) { early.setMuteOnChange(value); }
    void setEarlyDry(DspFloatType value) { early.setdry(value); }
    void setEarlyDryR(DspFloatType value) { early.setdryr(value); }
    void setEarlyWet(DspFloatType value) { early.setwet(value); }
    void setEarlyWetR(DspFloatType value) { early.setwetr(value); }
    void setEarlyWidth(DspFloatType value) { early.setwet(value); }
    void setEarlyLRDelay(DspFloatType value) { early.setLRDelay(value); }
    void setEarlyLRCrossApFreq(DspFloatType f, DspFloatType n) { early.setLRCrossApFreq(f,n); }
    void setEarlyDiffusionApFreq(DspFloatType f, DspFloatType n) { early.setDiffusionApFreq(f,n); }
    void setEarlyRSFactor(DspFloatType v) { early.setRSFactor(v); }
    //void setEarlyFSFactors() { early.setFSFactors(); }
    void setEarlyInitialDelay(long n) { early.setInitialDelay(n); }
    void setEarlyPreDelay(DspFloatType ms) { early.setPreDelay(ms); }
    void setEarlyOutputLPF(DspFloatType v) { early.setoutputlpf(v); }
    void setEarlyOutputHPF(DspFloatType v) { early.setoutputhpf(v); }

    void setLateMuteOnChange(bool value) { late.setMuteOnChange(value); }
    void setLateDry(DspFloatType value) { late.setdry(value); }
    void setLateDryR(DspFloatType value) { late.setdryr(value); }
    void setLateWet(DspFloatType value) { late.setwet(value); }
    void setLateWetR(DspFloatType value) { late.setwetr(value); }
    void setLateWidth(DspFloatType value) { late.setwet(value); }
    void setLateRSFactor(DspFloatType v) { late.setRSFactor(v); }
    //void setLateFSFactors() { late.setFSFactors(); }
    void setLateInitialDelay(long n) { late.setInitialDelay(n); }
    void setLatePreDelay(DspFloatType ms) { late.setPreDelay(ms); }

    void setRT60(DspFloatType v) { late.setrt60(v); }
    void setLoopDamp(DspFloatType v) { late.setloopdamp(v); }
    void setRT60_factor_low(DspFloatType v) { late.setrt60_factor_low(v); }
    void setRT60_factor_high(DspFloatType v) { late.setrt60_factor_high(v); }
    void setxover_low(DspFloatType v) { late.setxover_low(v); }
    void setxover_high(DspFloatType v) { late.setxover_high(v); }
    void setIDiffusion1(DspFloatType v) { late.setidiffusion1(v); }
    void setWander(DspFloatType v) { late.setwander(v); }
    void setSpin(DspFloatType v) { late.setspin(v); }
    void setSpinFactor(DspFloatType v) { late.setspinfactor(v); }
        

    enum {
        PORT_EARLY_MUTE,
        PORT_EARLY_RY,
        PORT_EARLY_RYR,
        PORT_EARLY_WET,
        PORT_EARLY_WETR,
        PORT_EARLY_WIDTH,
        PORT_EARLY_LRDELAY,
        PORT_EARLY_LRCROSSAPFREQ,
        PORT_EARLY_IFFUSIONAPFREQ,
        PORT_EARLY_OUTPUTLPF,
        PORT_EARLY_OUTPUTHPF,
        PORT_EARLY_RSFACTOR,
        PORT_EARLY_INITDELAY,
        PORT_EARLY_PREDELAY,        
        PORT_EARLY_FEEDBACK,        
        PORT_EARLY_SIZE,

        PORT_LATE_MUTE,
        PORT_LATE_RY,
        PORT_LATE_RYR,
        PORT_LATE_WET,
        PORT_LATE_WETR,
        PORT_LATE_WIDTH,
        PORT_LATE_RSFACTOR,
        PORT_LATE_INITDELAY,
        PORT_LATE_PREDELAY,        

        PORT_RT60,
        PORT_LOOPDAMP,
        PORT_RT60LOW,
        PORT_RT60HIGH,
        PORT_XOVRLOW,
        PORT_XORHIGH,
        PORT_IDIFFUS,
        PORT_WANDER,
        PORT_SPIN,
        PORT_SPINFACTOR,
    };
    void setPort(int port, DspFloatType v) {
        switch(port) {
            case PORT_EARLY_MUTE: setEarlyMuteOnChange((bool)v); break;
            case PORT_EARLY_RY: setEarlyDry(v); break;
            case PORT_EARLY_RYR: setEarlyDryR(v); break;
            case PORT_EARLY_WET: setEarlyWet(v); break;
            case PORT_EARLY_WETR: setEarlyWetR(v); break;
            case PORT_EARLY_WIDTH: setEarlyWidth(v); break;
            case PORT_EARLY_LRDELAY: setEarlyLRDelay(v); break;
            //case PORT_LRCROSSAPFREQ: setLRCrossAPFreq(v); break;
            //case PORT_IFFUSIONAPFREQ: setDiffusionAPFreq(v); break;
            case PORT_EARLY_OUTPUTLPF: setEarlyOutputLPF(v); break;
            case PORT_EARLY_OUTPUTHPF: setEarlyOutputHPF(v); break;
            case PORT_EARLY_RSFACTOR: setEarlyRSFactor(v); break;
            case PORT_EARLY_INITDELAY: setEarlyInitialDelay(v); break;
            case PORT_EARLY_PREDELAY: setEarlyPreDelay(v); break;
            //case PORT_EARLY_FEEDBACK: setFeedback(v); break;
            case PORT_LATE_MUTE: setLateMuteOnChange((bool)v); break;
            case PORT_LATE_RY: setLateDry(v); break;
            case PORT_LATE_RYR: setLateDryR(v); break;
            case PORT_LATE_WET: setLateWet(v); break;
            case PORT_LATE_WETR: setLateWetR(v); break;
            case PORT_LATE_WIDTH: setLateWidth(v); break;
            case PORT_LATE_RSFACTOR: setLateRSFactor(v); break;
            case PORT_LATE_INITDELAY: setLateInitialDelay(v); break;
            case PORT_LATE_PREDELAY: setLatePreDelay(v); break;

            //case PORT_SIZE: setSize(v); break;            
            case PORT_RT60: setRT60(v); break;
            case PORT_LOOPDAMP: setLoopDamp(v); break;
            case PORT_RT60LOW: setRT60_factor_low(v); break;
            case PORT_RT60HIGH: setRT60_factor_high(v); break;
            case PORT_XOVRLOW: setxover_low(v); break;
            case PORT_XORHIGH: setxover_high(v); break;
            case PORT_IDIFFUS: setIDiffusion1(v); break;
            case PORT_WANDER: setWander(v); break;
            case PORT_SPIN: setSpin(v); break;
            case PORT_SPINFACTOR: setSpinFactor(v); break;

            default: printf("No port %d\n",port);
        }
    }
    void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
      run(n,in,out);
    }
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Hall
  /////////////////////////////////////////////////////////////////////////////////
  inline HallReverb::HallReverb(DspFloatType sampleRate) : StereoFXProcessor() 
  {
    early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    early.setMuteOnChange(false);
    early.setdryr(0.5); // mute dry signal
    early.setwet(0.5); // 0dB
    early.setwidth(0.8);
    early.setLRDelay(0.3);
    early.setLRCrossApFreq(750, 4);
    early.setDiffusionApFreq(150, 4);
    early.setSampleRate(sampleRate);
    
    late.setMuteOnChange(false);
    late.setwet(0.5); // 0dB
    late.setdryr(0.5); // mute dry signal
    late.setwidth(1.0);
    late.setSampleRate(sampleRate);  
  }

  inline void HallReverb::run(size_t buffer_frames, DspFloatType** inputs, DspFloatType** outputs) 
  {  
    early.processreplace(
        const_cast<DspFloatType *>(inputs[0]),
        const_cast<DspFloatType *>(inputs[1]),
        outputs[0],
        outputs[1],
        buffer_frames);
            
    late.processreplace(
        const_cast<DspFloatType *>(outputs[0]),
        const_cast<DspFloatType *>(outputs[1]),
        outputs[0],
        outputs[1],
        buffer_frames);            
  }

  inline void HallReverb::sampleRateChanged(DspFloatType newSampleRate) {
      early.setSampleRate(newSampleRate);
      late.setSampleRate(newSampleRate);
  }

  inline void HallReverb::mute() {
    early.mute();
    late.mute();
  }
}