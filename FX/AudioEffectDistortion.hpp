#pragma once

namespace FX
{
    ///////////////////////////////////////////////////////////////////////////////
    // Distortion
    ///////////////////////////////////////////////////////////////////////////////

    #define SNAP_TO_ZERO(n)    if (! (n < -1.0e-8 || n > 1.0e-8)) n = 0;

    struct DistortionEffect
    {
        enum Types
        {
            _hardClipping = 0,
            _softClipping,
            _softClippingExp,
            _fullWaveRectifier,
            _halfWaveRectifier,
            _sigmoid,
            _dcSigmoid,
            _tanh,
            _fuzz,
            _sigmoidFuzz,
            _tanhFuzz,
            _numberOfTypes
        };

        int	  _numChannels;
        int	  _numSamples;
        int   _sampleRate;
        DspFloatType _gainIndB;
        size_t numChannels=2;
        Types _typeNumber = _hardClipping;
        
        enum {
            PORT_GAIN,
            PORT_TYPE,
        };
        void setPort(int port, DspFloatType v) {
            switch(port) {
                case PORT_GAIN: _gainIndB = v; break;
                case PORT_TYPE: _typeNumber = (Types)v; break;
                default: printf("No port %d\n",port);
            }
        }
        DistortionEffect(Types type = _hardClipping, DspFloatType gain=6, size_t numChannels=2)
        {
            _gainIndB = gain;
            _typeNumber = type;
            _numChannels = numChannels;
        }
        void ProcessBlock (size_t numSamples, DspFloatType ** input, DspFloatType ** output)
        {
            DspFloatType gain = powf(10.0f, _gainIndB/20.f);
                    
            for (int channel = 0; channel < _numChannels; ++channel)
            {
                DspFloatType * in = input[channel];
                DspFloatType * out = output[channel];
                
                for(size_t i = 0; i < numSamples; i++) 
                    out[i] = in[i]*gain;

                // Apply distortion (sample per sample)
                switch (_typeNumber) {
                    case _hardClipping:
                    {
                        DspFloatType threshold = 0.5f;
                        for (int sample = 0; sample < numSamples; ++sample)
                        {                        
                            if(out[sample]  > threshold) // positive hard clipping
                            {
                                out[sample] = threshold;
                            }
                            else
                            {
                                if(out[sample]  < - threshold) // negative hard clipping
                                {
                                    out[sample] = - threshold;
                                }
                            }
                        }
                        break;
                    }
                    
                    case _softClipping:
                    {
                        DspFloatType threshold1 = 1.0f/3.0f;
                        DspFloatType threshold2 = 2.0f/3.0f;
                        for (int sample = 0; sample < numSamples; ++sample)
                        {
                            if(out[sample]  > threshold1)
                            {
                                if(out[sample] > threshold2) // positive clipping
                                {
                                    out[sample] = 1.0f;
                                }
                                else // soft knee (positive)
                                {
                                    out[sample] = (3.0f - (2.0f - 3.0f*out[sample])*(2.0f - 3.0f*out[sample]))/3.0f;
                                }
                            }
                            else
                            {
                                if(out[sample]  < -threshold1)
                                {
                                    if(out[sample] < -threshold2) // negative clipping
                                    {
                                        out[sample] = -1.0f;
                                    }
                                    else // soft knee (negative)
                                    {
                                        out[sample] = - (3.0f - (2.0f + 3.0f*out[sample])*(2.0f + 3.0f*out[sample]))/3.0f;
                                    }
                                }
                                else // linear region (-1/3..1/3)
                                {
                                    out[sample] *= 2.0f;
                                }
                            }
                            
                            out[sample] /= 2.0f; // divide all by 2 to compensate for extra 6 dB gain boost
                        }
                        break;
                    }
                        
                    case _softClippingExp:
                    {
                        for (int sample = 0; sample < numSamples; ++sample)
                        {
                            if (out[sample] > 0.0f) // positive
                            {
                                out[sample] = 1.0f - expf(-out[sample]);
                            }
                            else // negative
                            {
                                out[sample] = - 1.0f + expf(out[sample]);
                            }
                        }
                        break;
                    }
                        
                    case _fullWaveRectifier:
                    {
                        for (int sample = 0; sample < numSamples; ++sample)
                        {
                            out[sample] = fabs(out[sample]);
                        }
                        break;
                    }
                        
                    case _halfWaveRectifier:
                    {
                        for (int sample = 0; sample < numSamples; ++sample)
                        {
                            out[sample] = 0.5f*(fabs(out[sample])+out[sample]);
                        }
                        break;
                    }
                        
                    default:
                        break;
                }
            }
        }
        DspFloatType Tick(DspFloatType I, DspFloatType A = 1, DspFloatType X = 0, DspFloatType Y = 0)
        {
            DspFloatType gain = powf(10.0f, _gainIndB/20.f);
            DspFloatType out = I * gain;

            // Apply distortion (sample per sample)
            switch (_typeNumber) 
            {
                case _hardClipping:
                {
                    DspFloatType threshold = 0.5f;                
                    if(out  > threshold) // positive hard clipping
                    {
                        out = threshold;
                    }
                    else
                    {
                        if(out  < - threshold) // negative hard clipping
                        {
                            out = - threshold;
                        }
                    }                
                    break;
                }
                
                case _softClipping:
                {
                    DspFloatType threshold1 = 1.0f/3.0f;
                    DspFloatType threshold2 = 2.0f/3.0f;

                    if(out  > threshold1)
                    {
                        if(out > threshold2) // positive clipping
                        {
                            out = 1.0f;
                        }
                        else // soft knee (positive)
                        {
                            out = (3.0f - (2.0f - 3.0f*out)*(2.0f - 3.0f*out))/3.0f;
                        }
                    }
                    else
                    {
                        if(out < -threshold1)
                        {
                            if(out < -threshold2) // negative clipping
                            {
                                out = -1.0f;
                            }
                            else // soft knee (negative)
                            {
                                out = - (3.0f - (2.0f + 3.0f*out)*(2.0f + 3.0f*out))/3.0f;
                            }
                        }
                        else // linear region (-1/3..1/3)
                        {
                            out *= 2.0f;
                        }
                    }
                    
                    out /= 2.0f; // divide all by 2 to compensate for extra 6 dB gain boost
                }
                break;
                    
                case _softClippingExp:
                {
                    if (out > 0.0f) // positive
                    {
                        out = 1.0f - expf(-out);
                    }
                    else // negative
                    {
                        out = - 1.0f + expf(out);
                    }
                }
                break;                
                case _fullWaveRectifier:
                {
                    out = fabs(out);
        
                    break;
                }
                    
                case _halfWaveRectifier:
                {
                    out = 0.5f*(fabs(out)+out);
                    break;
                }
                case _dcSigmoid:
                {             
                    DspFloatType dc;
                    if(out > 0) dc = 0.1;
                    else if(out < 0) dc = -0.1;
                    else dc = 0;
                    out = Distortion::sigmoid(-(out+dc));
                    out -= dc;
                }
                break;
                case _sigmoid:
                {                             
                    out = Distortion::sigmoid(out);                
                }
                break;
                case _tanh:
                    out = std::tanh(out);
                    break;
                case _fuzz:
                    out = Distortion::fuzz(out,0.95,gain);
                    break;
                default:
                    break;
            }
            return out;
        }
        void InplaceProcess(size_t n, DspFloatType ** buffer)
        {
            ProcessBlock(n,buffer,buffer);
        }
    };

    struct MonoDistortion : public MonoFXProcessor
    {
        DistortionEffect * dist;
        DspFloatType  ** ins, ** outs;
        MonoDistortion() : MonoFXProcessor()
        {
            dist = new DistortionEffect;
            dist->_numChannels=1;            
            ins =  (DspFloatType**)calloc(2,sizeof(DspFloatType*));
            outs = (DspFloatType**)calloc(2,sizeof(DspFloatType*));
        }
        ~MonoDistortion() {
            if(dist) delete dist;
            if(ins) free(ins);
            if(outs) free(outs);
        }
        void ProcessBlock(size_t numSamples, DspFloatType * in, DspFloatType * out)
        {
            ins[0] = in;
            ins[1] = in;
            outs[0]= out;
            outs[1]= out;
            dist->ProcessBlock(numSamples,ins,outs);
        }
    };
}