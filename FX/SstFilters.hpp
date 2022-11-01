#pragma once

#include "sst-filters/filters.h"
// sst-filters

namespace FX::SstFilter
{
    struct SstFilterState
    {
        sst::filters::FilterUnitQFPtr filter;
        sst::filters::QuadFilterUnitState filterState{};

        static constexpr auto dbLength =
            sst::filters::utilities::MAX_FB_COMB + sst::filters::utilities::SincTable::FIRipol_N;
        
        float delayBufferData[4][dbLength]{};

        SstFilterState() {
            reset();
        }
        void reset()
        {
            
            for (int i = 0; i < 4; ++i)
            {
                std::fill(delayBufferData[i], delayBufferData[i] + dbLength, 0.0f);
                filterState.DB[i] = delayBufferData[i];
                filterState.active[i] = (int)0xffffffff;
                filterState.WP[i] = 0;
            }

            std::fill(filterState.R, &filterState.R[sst::filters::n_filter_registers],
                      _mm_setzero_ps());
        }
    };

    struct SstFilter
    {
        enum
        {
            fut_none = 0,         /**< Off */
            fut_lp12,             /**< Lowpass - 12 dB */
            fut_lp24,             /**< Lowpass - 24 dB */
            fut_lpmoog,           /**< Lowpass - Legacy Ladder */
            fut_hp12,             /**< Highpass - 12 dB */
            fut_hp24,             /**< Highpass - 24 dB */
            fut_bp12,             /**< Bandpass - 12 dB */
            fut_notch12,          /**< Notch - 12 dB */
            fut_comb_pos,         /**< Effect - Comb + */
            fut_SNH,              /**< Effect - Sample & Hold */
            fut_vintageladder,    /**< Lowpass - Vintage Ladder */
            fut_obxd_2pole_lp,    /**< Lowpass - OB-Xd 12 dB */
            fut_obxd_4pole,       /**< Lowpass - OB-Xd 24 dB */
            fut_k35_lp,           /**< Lowpass - K35 */
            fut_k35_hp,           /**< Highpass - K35 */
            fut_diode,            /**< Lowpass - Diode Ladder */
            fut_cutoffwarp_lp,    /**< Lowpass - Cutoff Warp */
            fut_cutoffwarp_hp,    /**< Higghpass - Cutoff Warp */
            fut_cutoffwarp_n,     /**< Notch - Cutoff Warp */
            fut_cutoffwarp_bp,    /**< Bandpass - Cutoff Warp */
            fut_obxd_2pole_hp,    /**< Highpass - OB-Xd 12 dB */
            fut_obxd_2pole_n,     /**< Notch - OB-Xd 12 dB */
            fut_obxd_2pole_bp,    /**< Bandpass - OB-Xd 12 dB */
            fut_bp24,             /**< Bandpass - 24 dB */
            fut_notch24,          /**< Notch - 24 dB */
            fut_comb_neg,         /**< Effect - Comb - */
            fut_apf,              /**< Effect - Allpass */
            fut_cutoffwarp_ap,    /**< Effect - Cutoff Warp Allpass */
            fut_resonancewarp_lp, /**< Lowpass - Resonance Warp */
            fut_resonancewarp_hp, /**< Highpass - Resonance Warp */
            fut_resonancewarp_n,  /**< Notch - Resonance Warp */
            fut_resonancewarp_bp, /**< Bandpass - Resonance Warp */
            fut_resonancewarp_ap, /**< Effect - Resonance Warp Allpass */
            fut_tripole,          /**< Multi - Tri-pole */
            num_filter_types,
        };

        /** Sub-types for each filter are defined here */
        enum
        {
            st_Standard = 0, /**< Standard (SVF) */
            st_Driven = 1,   /**< Driven */
            st_Clean = 2,    /**< Clean */
            st_Medium = 3,   /**< (Unused) */

            st_Notch = 0,     /**< Standard */
            st_NotchMild = 1, /**< Mild */

            st_lpmoog_6dB = 0,  /**< Legacy Ladder - 6 dB */
            st_lpmoog_12dB = 1, /**< Legacy Ladder - 12 dB */
            st_lpmoog_18dB = 2, /**< Legacy Ladder - 18 dB */
            st_lpmoog_24dB = 3, /**< Legacy Ladder - 24 dB */

            st_diode_6dB = 0,  /**< Diode Ladder - 6 dB */
            st_diode_12dB = 1, /**< Diode Ladder - 12 dB */
            st_diode_18dB = 2, /**< Diode Ladder - 18 dB */
            st_diode_24dB = 3, /**< Diode Ladder - 24 dB */

            st_cutoffwarp_tanh1 = 0,     /**< Cutoff Warp - 1 Stage tanh */
            st_cutoffwarp_tanh2 = 1,     /**< Cutoff Warp - 2 Stages tanh */
            st_cutoffwarp_tanh3 = 2,     /**< Cutoff Warp - 3 Stages tanh */
            st_cutoffwarp_tanh4 = 3,     /**< Cutoff Warp - 4 Stages tanh */
            st_cutoffwarp_softclip1 = 4, /**< Cutoff Warp - 1 Stage Soft Clip */
            st_cutoffwarp_softclip2 = 5, /**< Cutoff Warp - 2 Stages Soft Clip */
            st_cutoffwarp_softclip3 = 6, /**< Cutoff Warp - 3 Stages Soft Clip */
            st_cutoffwarp_softclip4 = 7, /**< Cutoff Warp - 4 Stages Soft Clip */
            st_cutoffwarp_ojd1 = 8,      /**< Cutoff Warp - 1 Stage OJD */
            st_cutoffwarp_ojd2 = 9,      /**< Cutoff Warp - 2 Stages OJD */
            st_cutoffwarp_ojd3 = 10,     /**< Cutoff Warp - 3 Stages OJD */
            st_cutoffwarp_ojd4 = 11,     /**< Cutoff Warp - 4 Stages OJD */

            st_resonancewarp_tanh1 = 0,     /**< Resonance Warp - 1 Stage tanh */
            st_resonancewarp_tanh2 = 1,     /**< Resonance Warp - 2 Stages tanh */
            st_resonancewarp_tanh3 = 2,     /**< Resonance Warp - 3 Stages tanh */
            st_resonancewarp_tanh4 = 3,     /**< Resonance Warp - 4 Stages tanh */
            st_resonancewarp_softclip1 = 4, /**< Resonance Warp - 1 Stage Soft Clip */
            st_resonancewarp_softclip2 = 5, /**< Resonance Warp - 2 Stages Soft Clip */
            st_resonancewarp_softclip3 = 6, /**< Resonance Warp - 3 Stages Soft Clip */
            st_resonancewarp_softclip4 = 7, /**< Resonance Warp - 4 Stages Soft Clip */

            st_tripole_LLL1 = 0,  /**< Low -> Low -> Low, first */
            st_tripole_LHL1 = 1,  /**< Low -> High -> Low, first */
            st_tripole_HLH1 = 2,  /**< High -> Low -> High, first */
            st_tripole_HHH1 = 3,  /**< High -> High -> High, first */
            st_tripole_LLL2 = 4,  /**< Low -> Low -> Low, second */
            st_tripole_LHL2 = 5,  /**< Low -> High -> Low, second */
            st_tripole_HLH2 = 6,  /**< High -> Low -> High, second */
            st_tripole_HHH2 = 7,  /**< High -> High -> High, second */
            st_tripole_LLL3 = 8,  /**< Low -> Low -> Low, third */
            st_tripole_LHL3 = 9,  /**< Low -> High -> Low, third */
            st_tripole_HLH3 = 10, /**< High -> Low -> High, third */
            st_tripole_HHH3 = 11, /**< High -> High -> High, third */
        };


        SstFilterState filters[2];
        sst::filters::FilterCoefficientMaker<> coeffMaker;
        int filterType,filterSubType;
        double freqParam,resParam;

        SstFilter(int type =fut_lpmoog, int subtype = st_lpmoog_24dB)
        {                
            filterType = type;
            filterSubType = subtype;
            resParam = 0.5;
            filters[0].filter = sst::filters::GetQFPtrFilterUnit((sst::filters::FilterType)type,(sst::filters::FilterSubType) subtype);
            filters[1].filter = sst::filters::GetQFPtrFilterUnit((sst::filters::FilterType)type,(sst::filters::FilterSubType) subtype);        
        }        
        ~SstFilter()
        {

        }
        inline float freq_hz_to_note_num(float freqHz) { return 12.0f * std::log2(freqHz / 440.0f); }
        void setCutoff(float fc)
        {
            freqParam = fc;
            coeffMaker.MakeCoeffs(freq_hz_to_note_num (fc), resParam, (sst::filters::FilterType)filterType, (sst::filters::FilterSubType)filterSubType, nullptr, false);
        }
        void setResonance(float r) {
            resParam = r;
            setCutoff(freqParam);
        }
        void ProcessInplace(size_t framesPerBuffer, float ** buffer)
        {            
            coeffMaker.updateState(filters[0].filterState);
            coeffMaker.updateState(filters[1].filterState);
            for(size_t i = 0; i < framesPerBuffer; i++)
            {                
                __m128 x = _mm_load_ps(&buffer[0][i]);                
                x = filters[0].filter(&filters[0].filterState,x);        
                _mm_store_ps(&buffer[0][i],x);

                x = _mm_load_ps(&buffer[1][i]);
                x = filters[1].filter(&filters[1].filterState,x); 
                _mm_store_ps(&buffer[1][i],x);
            }        
        }
        void ProcessBlock(size_t framesPerBuffer, float ** buffer, float ** out)
        {
         
            coeffMaker.updateState(filters[0].filterState);
            coeffMaker.updateState(filters[1].filterState);
            for(size_t i = 0; i < framesPerBuffer; i++)
            {                
                __m128 x = _mm_load_ps(&buffer[0][i]);                
                x = filters[0].filter(&filters[0].filterState,x);        
                _mm_store_ps(&out[0][i],x);

                x = _mm_load_ps(&buffer[1][i]);
                x = filters[1].filter(&filters[1].filterState,x);        
                _mm_store_ps(&out[1][i],x);
            }        
        }
    };
}