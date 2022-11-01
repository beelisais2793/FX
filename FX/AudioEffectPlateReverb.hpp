#pragma once

namespace FX
{
#ifdef DSPFLOATDOUBLE  
  struct NRev : public fv3::nrev_ 
#else
  struct NRev : public fv3::nrev_f
#endif  
  {
    NRev();
    void setDampLpf(DspFloatType value);
    virtual void mute();
    virtual void setFsFactors();
    virtual void processloop2(long count, DspFloatType *inputL,  DspFloatType *inputR, DspFloatType *outputL, DspFloatType *outputR);

    DspFloatType dampLpf = 20000;
    #ifdef DSPFLOATDOUBLE
    fv3::iir_1st_ dampLpfL, dampLpfR;
    #else
    fv3::iir_1st_f dampLpfL, dampLpfR;
    #endif
  };

#ifdef DSPFLOATDOUBLE
  struct NRevB : public fv3::nrevb_ 
#else
  struct NRevB : public fv3::nrevb_f
#endif  
  {
    NRevB();
    void setDampLpf(DspFloatType value);
    virtual void mute();
    virtual void setFsFactors();
    virtual void processloop2(long count, DspFloatType *inputL,  DspFloatType *inputR, DspFloatType *outputL, DspFloatType *outputR);
    DspFloatType dampLpf = 20000;
    #ifdef DSPFLOATDOUBLE
    fv3::iir_1st_ dampLpfL, dampLpfR;
    #else
    fv3::iir_1st_f dampLpfL, dampLpfR;
    #endif
  };

  struct PlateReverb : public StereoFXProcessor
  {
    #ifdef DSPFLOATDOUBLE
    fv3::iir_1st_ input_lpf_0, input_lpf_1, input_hpf_0, input_hpf_1;
    fv3::revbase_ *model; // points to one of the following:
    fv3::strev_ strev;
    #else
    fv3::iir_1st_f input_lpf_0, input_lpf_1, input_hpf_0, input_hpf_1;
    fv3::revbase_f *model; // points to one of the following:
    fv3::strev_f strev;
    #endif

    NRev nrev;
    NRevB nrevb;
    
    
    
    DspFloatType HighCut=5000.0f;
    DspFloatType LowCut = 100.0f;


    PlateReverb(DspFloatType sampleRate);
    
    void  run(size_t n, DspFloatType** inputs, DspFloatType** outputs);
    void  sampleRateChanged(DspFloatType newSampleRate);
    void  mute();
    void  setModel1() { model = &nrev; }
    void  setModel2() { model = &nrevb; }

    void setDry(DspFloatType d) { model->setdryr(d); }
    void setWet(DspFloatType w) { model->setwetr(w); }
    void setMuteOnChange(bool v) { model->setMuteOnChange(v); }
    
    DspFloatType sampleRate;
    void setInputLPF(DspFloatType freq);
    void setInputHPF(DspFloatType freq);

    enum {
        PORT_MUTE,
        PORT_MODEL1,
        PORT_MODEL2,
        PORT_DRY,
        PORT_WET,
        PORT_MUTEONCHANGE,
        PORT_LPF,
        PORT_HPF
    };
    void setPort(int port, DspFloatType v)
    {
        switch(port)
        {
            case PORT_MUTE: mute(); break;
            case PORT_MODEL1: setModel1(); break;
            case PORT_MODEL2: setModel2(); break;
            case PORT_DRY: setDry(v); break;
            case PORT_WET: setWet(v); break;
            case PORT_MUTEONCHANGE: setMuteOnChange((bool)v); break;
            case PORT_LPF: setInputLPF(v); break;
            case PORT_HPF: setInputHPF(v); break;
            default: printf("No port %d\n",port);
        }
    }
    void ProcessBlock(size_t n, DspFloatType ** in, DspFloatType ** out) {
        run(n,in,out);
      }
  };


#ifdef DSPFLOATDOUBLE
  inline NRev::NRev() : fv3::nrev_() { }
#else
  inline NRev::NRev() : fv3::nrev_f() { }  
#endif  

  inline void NRev::setDampLpf(DspFloatType value) {
    dampLpf = limFs2(value);
    dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
    dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
  }

  inline void NRev::mute() {
    #ifdef DSPFLOATDOUBLE
      fv3::nrev_::mute();
    #else
      fv3::nrev_f::mute();
    #endif  
    dampLpfL.mute();
    dampLpfR.mute();
  }

  inline void NRev::setFsFactors() {
    #ifdef DSPFLOATDOUBLE
      fv3::nrev_::setFsFactors();
    #else
      fv3::nrev_f::setFsFactors();
    #endif
    setDampLpf(dampLpf);
  }

  inline void NRev::processloop2(long count, DspFloatType *inputL, DspFloatType *inputR, DspFloatType *outputL, DspFloatType *outputR) {
    DspFloatType outL, outR;
    Undenormal denormals;

    while(count-- > 0)
      {
        outL = outR = 0;
        hpf = damp3_1*inDCC(*inputL + *inputR) - damp3*hpf;
        

        hpf *= FV3_NREV_SCALE_WET;
        
        for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(hpf);
        for(long i = 0;i < 3;i ++) outL = allpassL[i]._process_ov(outL);
        lpfL = dampLpfL(damp2*lpfL + damp2_1*outL); 
        outL = allpassL[3]._process_ov(lpfL);
        outL = allpassL[5]._process_ov(outL);
        outL = delayWL(lLDCC(outL));
        
        for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(hpf);
        for(long i = 0;i < 3;i ++) outR = allpassR[i]._process_ov(outR);
        lpfR = dampLpfR(damp2*lpfR + damp2_1*outR); 
        outR = allpassR[3]._process_ov(lpfR);
        outR = allpassR[6]._process_ov(outR);
        outR = delayWR(lRDCC(outR));
        
        *outputL = outL*wet1 + outR*wet2 + delayL(*inputL)*dry;
        *outputR = outR*wet1 + outL*wet2 + delayR(*inputR)*dry;
        inputL ++; inputR ++; outputL ++; outputR ++;
      }
  }

#ifdef DSPFLOATDOUBLE
  inline NRevB::NRevB() : fv3::nrevb_() { }
#else
  inline NRevB::NRevB() : fv3::nrevb_f() { }  
#endif  

  inline void NRevB::setDampLpf(DspFloatType value)
  {
    dampLpf = limFs2(value);
    dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
    dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
  }

  inline void NRevB::mute() {
    #ifdef DSPFLOATDOUBLE
      fv3::nrevb_::mute();
    #else
      fv3::nrevb_f::mute();
    #endif
    dampLpfL.mute();
    dampLpfR.mute();
  }

  inline void NRevB::setFsFactors() {
    #ifdef DSPFLOATDOUBLE
    fv3::nrevb_::setFsFactors();
    #else
      fv3::nrevb_f::setFsFactors();
    #endif
    setDampLpf(dampLpf);
  }

  inline void NRevB::processloop2(long count, DspFloatType *inputL, DspFloatType *inputR, DspFloatType *outputL, DspFloatType *outputR) {
    DspFloatType outL, outR, tmpL, tmpR;
    Undenormal denormals;

    while(count-- > 0)
      {
        hpf = damp3_1*inDCC.process(*inputL + *inputR) - damp3*hpf;
        outL = outR = tmpL = tmpR = hpf;

        outL += apfeedback*lastL;
        lastL += -1*apfeedback*outL;

        for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outL += combL[i]._process(tmpL);
        for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outL += comb2L[i]._process(tmpL);
        for(long i = 0;i < 3;i ++) outL = allpassL[i]._process(outL);
        for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outL = allpass2L[i]._process(outL);
        lpfL = dampLpfL(damp2*lpfL + damp2_1*outL); 
        outL = allpassL[3]._process(lpfL); outL = allpassL[5]._process(outL);
        outL = lLDCC(outL);

        outR += apfeedback*lastR;
        lastR += -1*apfeedback*outR;
        for(long i = 0;i < FV3_NREV_NUM_COMB;i ++) outR += combR[i]._process(tmpR);
        for(long i = 0;i < FV3_NREVB_NUM_COMB_2;i ++) outR += comb2R[i]._process(tmpR);
        for(long i = 0;i < 3;i ++) outR = allpassR[i]._process(outR);
        for(long i = 0;i < FV3_NREVB_NUM_ALLPASS_2;i ++) outR = allpass2R[i]._process(outR);
        lpfR = dampLpfR(damp2*lpfR + damp2_1*outR); 
        outR = allpassR[3]._process(lpfR); outR = allpassL[6]._process(outR);
        outR = lRDCC(outR);
        
        lastL = FV3_NREVB_SCALE_WET*delayWL(lastL);
        lastR = FV3_NREVB_SCALE_WET*delayWR(lastR);
        *outputL = lastL*wet1 + lastR*wet2 + delayL(*inputL)*dry;
        *outputR = lastR*wet1 + lastL*wet2 + delayR(*inputR)*dry;
        lastL = outL; lastR = outR;
        inputL ++; inputR ++; outputL ++; outputR ++;
      }
  }


  inline PlateReverb::PlateReverb(DspFloatType sampleRate) : StereoFXProcessor() {
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();

    nrev.setdryr(0);
    nrev.setwetr(1);
    nrev.setMuteOnChange(false);
    nrev.setSampleRate(sampleRate);

    nrevb.setdryr(0);
    nrevb.setwetr(1);
    nrevb.setMuteOnChange(false);
    nrevb.setSampleRate(sampleRate);

    strev.setdryr(0);
    strev.setwetr(1);
    strev.setMuteOnChange(false);
    strev.setdccutfreq(6);
    strev.setspinlimit(12);
    strev.setspindiff(0.15);
    strev.setSampleRate(sampleRate);

    model = &nrevb;

    sampleRateChanged(sampleRate);
  }


  inline void PlateReverb::run(size_t frames, DspFloatType** inputs, DspFloatType** outputs) {

      for (uint32_t i = 0; i < frames; i++) {
        outputs[0][i] = input_lpf_0.process(input_hpf_0.process(inputs[0][i]));
        outputs[0][i] = input_lpf_1.process(input_hpf_1.process(inputs[1][i]));
      }

      model->processreplace(
          const_cast<DspFloatType *>(outputs[0]),
          const_cast<DspFloatType *>(outputs[1]),
          outputs[0],
          outputs[1],
          frames
      );      
  }

  inline void PlateReverb::sampleRateChanged(DspFloatType newSampleRate) {
    sampleRate = newSampleRate;
    nrev.setSampleRate(newSampleRate);
    nrevb.setSampleRate(newSampleRate);
    strev.setSampleRate(newSampleRate);
    setInputLPF(HighCut);
    setInputHPF(LowCut);  
  }

  inline void PlateReverb::mute() {
    nrev.mute();
    nrevb.mute();
    strev.mute();
  }

  inline void PlateReverb::setInputLPF(DspFloatType freq) {
    if (freq < 0) {
      freq = 0;
    } else if (freq > sampleRate / 2.0) {
      freq = sampleRate / 2.0;
    }

    input_lpf_0.setLPF_BW(freq, sampleRate);
    input_lpf_1.setLPF_BW(freq, sampleRate);
  }

  inline void PlateReverb::setInputHPF(DspFloatType freq) {
    if (freq < 0) {
      freq = 0;
    } else if (freq > sampleRate / 2.0) {
      freq = sampleRate / 2.0;
    }

    input_hpf_0.setHPF_BW(freq, sampleRate);
    input_hpf_1.setHPF_BW(freq, sampleRate);
  }
}