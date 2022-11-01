#pragma once

#include "sst-waveshapers/waveshapers.h"
namespace FX::SstWaveShaper
{
    struct SstWaveshaper
    {
        sst::waveshapers::QuadWaveshaperState    waState;
        sst::waveshapers::QuadWaveshaperPtr      pWaveShaper;
        float Distortion = 0;
        
        enum class WaveshaperType
        {
            wst_none = 0,
            wst_soft,
            wst_hard,
            wst_asym,
            wst_sine,
            wst_digital,

            // XT waves start here
            wst_cheby2,
            wst_cheby3,
            wst_cheby4,
            wst_cheby5,

            wst_fwrectify,
            wst_poswav,
            wst_negwav,
            wst_softrect,

            wst_singlefold,
            wst_dualfold,
            wst_westfold,

            // additive harmonics
            wst_add12,
            wst_add13,
            wst_add14,
            wst_add15,
            wst_add12345,
            wst_addsaw3,
            wst_addsqr3,

            wst_fuzz,
            wst_fuzzsoft,
            wst_fuzzheavy,
            wst_fuzzctr,
            wst_fuzzsoftedge,

            wst_sinpx,
            wst_sin2xpb,
            wst_sin3xpb,
            wst_sin7xpb,
            wst_sin10xpb,

            wst_2cyc,
            wst_7cyc,
            wst_10cyc,

            wst_2cycbound,
            wst_7cycbound,
            wst_10cycbound,

            wst_zamsat,
            wst_ojd,

            wst_softfold,

            n_ws_types,
        };

        SstWaveshaper(WaveshaperType type = WaveshaperType::wst_fuzz)
        {
            float R[sst::waveshapers::n_waveshaper_registers];
            sst::waveshapers::initializeWaveshaperRegister((sst::waveshapers::WaveshaperType)type, R);
            for (int i = 0; i < sst::waveshapers::n_waveshaper_registers; ++i)
            {
                waState.R[i] = _mm_set1_ps(R[i]);
            }
            waState.init = _mm_cmpneq_ps(_mm_setzero_ps(), _mm_setzero_ps());

            pWaveShaper = sst::waveshapers::GetQuadWaveshaper((sst::waveshapers::WaveshaperType)type);
        }
        ~SstWaveshaper()
        {

        }

        void ProcessInplace(size_t framesPerBuffer, float ** buffer)
        {
            __m128 gain = _mm_set1_ps(pow(10,(Distortion/20.0)));
            for(size_t i = 0; i < framesPerBuffer; i++)
            {    
                __m128 x = _mm_load_ps(&buffer[0][i]);                
                x = pWaveShaper(&waState,x,gain);        
                _mm_store_ps(&buffer[0][i],x);

                x = _mm_load_ps(&buffer[1][i]);
                x = pWaveShaper(&waState,x,gain);        
                _mm_store_ps(&buffer[1][i],x);
            }        
        }
        void ProcessBlock(size_t framesPerBuffer, float ** buffer, float ** out)
        {
            __m128 gain = _mm_set1_ps(pow(10,(Distortion/20.0)));
            for(size_t i = 0; i < framesPerBuffer; i++)
            {    
                __m128 x = _mm_load_ps(&buffer[0][i]);                
                x = pWaveShaper(&waState,x,gain);        
                _mm_store_ps(&out[0][i],x);

                x = _mm_load_ps(&buffer[1][i]);
                x = pWaveShaper(&waState,x,gain);        
                _mm_store_ps(&out[1][i],x);
            }        
        }
    };
}