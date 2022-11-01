#pragma once

namespace FX
{
  struct RoomReverb : public StereoFXProcessor
  {
    DspFloatType sampleRate;

    fv3::iir_1st_ input_lpf_0, input_lpf_1, input_hpf_0, input_hpf_1;

#ifdef DSPFLOATDOUBLE
    fv3::earlyref_ early;
    fv3::progenitor2_ late;
#else
  fv3::earlyref_f early;
    fv3::progenitor2_f late;    
#endif

    DspFloatType HighCut = 5000.0f;
    DspFloatType LowCut  = 100.0f;

    RoomReverb(DspFloatType sampleRate);

    void run(size_t n, DspFloatType** inputs, DspFloatType** outputs);
    void sampleRateChanged(DspFloatType newSampleRate);
    void mute();

    void setEarlyMute(bool value) { early.setMuteOnChange(value); }
    void setEarlyDry(DspFloatType value) { early.setdryr(value); }
    void setEarlyWet(DspFloatType value) { early.setwet(value); }
    void setEarlyWidth(DspFloatType value) { early.setwet(value); }
    void setEarlyLRDelay(DspFloatType value) { early.setLRDelay(value); }
    void setEarlyLRCrossApFreq(DspFloatType f, DspFloatType n) { early.setLRCrossApFreq(f,n); }
    void setEarlyDiffusionApFreq(DspFloatType f, DspFloatType n) { early.setDiffusionApFreq(f,n); }

    void setLateMute(bool value) { late.setMuteOnChange(value); }
    void setLateDry(DspFloatType value) { late.setdryr(value); }
    void setLateWet(DspFloatType value) { late.setwet(value); }
    void setLateWidth(DspFloatType value) { late.setwet(value); }
    
    void setInputLPF(DspFloatType freq);
    void setInputHPF(DspFloatType freq);

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
        PORT_INPUTLPF,
        PORT_INPUTHPF,
        PORT_EARLY_RSFACTOR,
        PORT_EARLY_INITDELAY,
        PORT_EARLY_PREDELAY,        
        PORT_EARLY_FEEDBACK,        
        PORT_EARLY_SIZE,

        PORT_LATE_MUTE,
        PORT_LATE_RY,
        PORT_LATE_WET,
        PORT_LATE_WIDTH,     
    };
    void setPort(int port, DspFloatType v) {
        switch(port) {
            case PORT_EARLY_MUTE: setEarlyMute((bool)v); break;
            case PORT_EARLY_RY: setEarlyDry(v); break;
            //case PORT_EARLY_RYR: setEarlyDryR(v); break;
            case PORT_EARLY_WET: setEarlyWet(v); break;
            //case PORT_EARLY_WETR: setEarlyWetR(v); break;
            case PORT_EARLY_WIDTH: setEarlyWidth(v); break;
            case PORT_EARLY_LRDELAY: setEarlyLRDelay(v); break;
            //case PORT_LRCROSSAPFREQ: setLRCrossAPFreq(v); break;
            //case PORT_IFFUSIONAPFREQ: setDiffusionAPFreq(v); break;
            case PORT_INPUTLPF: setInputLPF(v); break;
            case PORT_INPUTHPF: setInputHPF(v); break;
            //case PORT_EARLY_RSFACTOR: setEarlyRSFactor(v); break;
            //case PORT_EARLY_INITDELAY: setEarlyInitialDelay(v); break;
            //case PORT_EARLY_PREDELAY: setEarlyPreDelay(v); break;
            //case PORT_EARLY_FEEDBACK: setEarlyFeedback(v); break;
            //case PORT_EARLY_SIZE: setEarlySize(v); break;  

            case PORT_LATE_MUTE: setLateMute((bool)v); break;
            case PORT_LATE_RY: setLateDry(v); break;            
            case PORT_LATE_WET: setLateWet(v); break;            
            case PORT_LATE_WIDTH: setLateWidth(v); break;     
            default: printf("No port %d\n",port);     
     
        }
    }
    void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
        run(n,in,out);
    }
  };

  inline RoomReverb::RoomReverb(DspFloatType sampleRate) { //}: StereoFXProcessor() {
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();

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

    sampleRateChanged(sampleRate);
  }


  inline void RoomReverb::run(size_t buffer_frames, DspFloatType** inputs, DspFloatType** outputs) {
    
    for (uint32_t i = 0; i < buffer_frames; i++) {
      outputs[0][i] = input_lpf_0.process(input_hpf_0.process(inputs[0][i]));
      outputs[1][i] = input_lpf_1.process(input_hpf_1.process(inputs[1][i]));
    }

    early.processreplace(
      const_cast<DspFloatType *>(outputs[0]),
      const_cast<DspFloatType *>(outputs[1]),
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


  inline void RoomReverb::sampleRateChanged(DspFloatType newSampleRate) {
    sampleRate = newSampleRate;
    early.setSampleRate(newSampleRate);
    late.setSampleRate(newSampleRate);
    setInputLPF(HighCut);
    setInputHPF(LowCut);
  }

  inline void RoomReverb::mute() {
    early.mute();
    late.mute();
  }

  inline void RoomReverb::setInputLPF(DspFloatType freq) {
    if (freq < 0) {
      freq = 0;
    } else if (freq > sampleRate / 2.0) {
      freq = sampleRate / 2.0;
    }

    input_lpf_0.setLPF_BW(freq, sampleRate);
    input_lpf_1.setLPF_BW(freq, sampleRate);
  }

  inline void RoomReverb::setInputHPF(DspFloatType freq) {
    if (freq < 0) {
      freq = 0;
    } else if (freq > sampleRate / 2.0) {
      freq = sampleRate / 2.0;
    }

    input_hpf_0.setHPF_BW(freq, sampleRate);
    input_hpf_1.setHPF_BW(freq, sampleRate);
  }
}