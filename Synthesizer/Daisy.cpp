#include "Daisy.hpp"

using namespace Daisy;

/** PIs
*/



    void DcBlock::Init(float sample_rate)
    {
        output_ = 0.0;
        input_  = 0.0;
        gain_   = 0.99;
    }

    float DcBlock::Process(float in)
    {
        float out;
        out     = in - input_ + (gain_ * output_);
        output_ = out;
        input_  = in;
        return out;
    }


    float Jitter::randGab()
    {
        return (float)((rand() >> 1) & 0x7fffffff)
            * (4.656612875245796924105750827168e-10);
    }

    float Jitter::biRandGab()
    {
        return (float)(rand() & 0x7fffffff)
            * (4.656612875245796924105750827168e-10);
    }

    void Jitter::SetAmp(float amp)
    {
        amp_ = amp;
        Reset();
    }

    void Jitter::SetCpsMin(float cps_min)
    {
        cps_min_ = cps_min;
        Reset();
    }

    void Jitter::SetCpsMax(float cps_max)
    {
        cps_max_ = cps_max;
        Reset();
    }

    void Jitter::Reset()
    {
        num2_      = biRandGab();
        init_flag_ = true;
        phs_       = 0;
    }

    void Jitter::Init(float sample_rate)
    {
        sample_rate_ = sample_rate;
        amp_         = 0.5;
        cps_min_     = 0.5;
        cps_max_     = 4;
        Reset();
    }

    float Jitter::Process()
    {
        float out;
        if(init_flag_)
        {
            init_flag_ = false;
            out        = num2_ * amp_;
            cps_       = randGab() * (cps_max_ - cps_min_) + cps_min_;
            phs_ &= FT_PHMASK;
            num1_    = num2_;
            num2_    = biRandGab();
            dfd_max_ = 1.0 * (num2_ - num1_) / FT_MAXLEN;
            return out;
        }

        out = (num1_ + (float)phs_ * dfd_max_) * amp_;
        phs_ += (int32_t)(cps_ * (float)(FT_MAXLEN / sample_rate_));

        if(phs_ >= FT_MAXLEN)
        {
            cps_ = randGab() * (cps_max_ - cps_min_) + cps_min_;
            phs_ &= FT_PHMASK;
            num1_    = num2_;
            num2_    = biRandGab();
            dfd_max_ = 1.0 * (num2_ - num1_) / FT_MAXLEN;
        }

        return out;
    }

void Metro::Init(float freq, float sample_rate)
    {
        freq_        = freq;
        phs_         = 0.0f;
        sample_rate_ = sample_rate;
        phs_inc_     = (TWOPI_F * freq_) / sample_rate_;
    }

    uint8_t Metro::Process()
    {
        phs_ += phs_inc_;
        if(phs_ >= TWOPI_F)
        {
            phs_ -= TWOPI_F;
            return 1;
        }
        return 0;
    }

    void Metro::SetFreq(float freq)
    {
        freq_    = freq;
        phs_inc_ = (TWOPI_F * freq_) / sample_rate_;
    }


    void Port::Init(float sample_rate, float htime)
    {
        yt1_     = 0;
        prvhtim_ = -100.0;
        htime_   = htime;

        sample_rate_ = sample_rate;
        onedsr_      = 1.0 / sample_rate_;
    }

    float Port::Process(float in)
    {
        if(prvhtim_ != htime_)
        {
            c2_      = pow(0.5, onedsr_ / htime_);
            c1_      = 1.0 - c2_;
            prvhtim_ = htime_;
        }

        return yt1_ = c1_ * in + c2_ * yt1_;
    }


    
    // Private Functions
    inline void AdEnv::Init(float sample_rate)
    {
        sample_rate_     = sample_rate;
        current_segment_ = ADENV_SEG_IDLE;
        curve_scalar_    = 0.0f; // full linear
        phase_           = 0;
        min_             = 0.0f;
        max_             = 1.0f;
        output_          = 0.0001f;
        for(uint8_t i = 0; i < ADENV_SEG_LAST; i++)
        {
            segment_time_[i] = 0.05f;
        }
    }

    inline float AdEnv::Process()
    {
        uint32_t time_samps;
        float    val, out, end, beg, inc;

        // Handle Retriggering
        if(trigger_)
        {
            trigger_         = 0;
            current_segment_ = ADENV_SEG_ATTACK;
            phase_           = 0;
            curve_x_         = 0.0f;
            retrig_val_      = output_;
        }

        time_samps = (uint32_t)(segment_time_[current_segment_] * sample_rate_);

        // Fixed for now, but we could always make this a more flexible multi-segment envelope
        switch(current_segment_)
        {
            case ADENV_SEG_ATTACK:
                beg = retrig_val_;
                end = 1.0f;
                break;
            case ADENV_SEG_DECAY:
                beg = 1.0f;
                end = 0.0f;
                break;
            case ADENV_SEG_IDLE:
            default:
                beg = 0;
                end = 0;
                break;
        }

        if(prev_segment_ != current_segment_)
        {
            //Reset at segment beginning
            curve_x_ = 0;
            phase_   = 0;
        }

        //recalculate increment value
        if(curve_scalar_ == 0.0f)
        {
            c_inc_ = (end - beg) / time_samps;
        }
        else
        {
            c_inc_ = (end - beg) / (1.0f - EXPF(curve_scalar_));
        }


        // update output
        val = output_;
        inc = c_inc_;
        out = val;
        if(curve_scalar_ == 0.0f)
        {
            val += inc;
        }
        else
        {
            curve_x_ += (curve_scalar_ / time_samps);
            val = beg + inc * (1.0f - EXPF(curve_x_));
            if(val != val)
                val = 0.0f; // NaN check
        }

        // Update Segment
        phase_ += 1;
        prev_segment_ = current_segment_;
        if(current_segment_ != ADENV_SEG_IDLE)
        {
            if((out >= 1.f && current_segment_ == ADENV_SEG_ATTACK)
            || (out <= 0.f && current_segment_ == ADENV_SEG_DECAY))
            {
                // Advance segment
                current_segment_++;
                // TODO: Add Cycling feature here.
                if(current_segment_ > ADENV_SEG_DECAY)
                {
                    current_segment_ = ADENV_SEG_IDLE;
                }
            }
        }
        if(current_segment_ == ADENV_SEG_IDLE)
        {
            val = out = 0.0f;
        }
        output_ = val;

        return out * (max_ - min_) + min_;
    }

    inline void Adsr::Init(float sample_rate, int blockSize)
    {
        sample_rate_  = sample_rate / blockSize;
        attackShape_  = -1.f;
        attackTarget_ = 0.0f;
        attackTime_   = -1.f;
        decayTime_    = -1.f;
        releaseTime_  = -1.f;
        sus_level_    = 0.7f;
        x_            = 0.0f;
        gate_         = false;
        mode_         = ADSR_SEG_IDLE;

        SetTime(ADSR_SEG_ATTACK, 0.1f);
        SetTime(ADSR_SEG_DECAY, 0.1f);
        SetTime(ADSR_SEG_RELEASE, 0.1f);
    }

    inline void Adsr::Retrigger(bool hard)
    {
        mode_ = ADSR_SEG_ATTACK;
        if(hard)
            x_ = 0.f;
    }

    inline void Adsr::SetTime(int seg, float time)
    {
        switch(seg)
        {
            case ADSR_SEG_ATTACK: SetAttackTime(time, 0.0f); break;
            case ADSR_SEG_DECAY:
            {
                SetTimeConstant(time, decayTime_, decayD0_);
            }
            break;
            case ADSR_SEG_RELEASE:
            {
                SetTimeConstant(time, releaseTime_, releaseD0_);
            }
            break;
            default: return;
        }
    }

    inline void Adsr::SetAttackTime(float timeInS, float shape)
    {
        if((timeInS != attackTime_) || (shape != attackShape_))
        {
            attackTime_  = timeInS;
            attackShape_ = shape;
            if(timeInS > 0.f)
            {
                float x         = shape;
                float target    = 9.f * powf(x, 10.f) + 0.3f * x + 1.01f;
                attackTarget_   = target;
                float logTarget = logf(1.f - (1.f / target)); // -1 for decay
                attackD0_       = 1.f - expf(logTarget / (timeInS * sample_rate_));
            }
            else
                attackD0_ = 1.f; // instant change
        }
    }
    inline void Adsr::SetDecayTime(float timeInS)
    {
        SetTimeConstant(timeInS, decayTime_, decayD0_);
    }
    inline void Adsr::SetReleaseTime(float timeInS)
    {
        SetTimeConstant(timeInS, releaseTime_, releaseD0_);
    }
    inline void Adsr::SetTimeConstant(float timeInS, float& time, float& coeff)
    {
        if(timeInS != time)
        {
            time = timeInS;
            if(time > 0.f)
            {
                const float target = logf(1. / M_E);
                coeff              = 1.f - expf(target / (time * sample_rate_));
            }
            else
                coeff = 1.f; // instant change
        }
    }
    inline float Adsr::Process(bool gate)
    {
        float out = 0.0f;

        if(gate && !gate_) // rising edge
            mode_ = ADSR_SEG_ATTACK;
        else if(!gate && gate_) // falling edge
            mode_ = ADSR_SEG_RELEASE;
        gate_ = gate;

        float D0(attackD0_);
        if(mode_ == ADSR_SEG_DECAY)
            D0 = decayD0_;
        else if(mode_ == ADSR_SEG_RELEASE)
            D0 = releaseD0_;

        float target = mode_ == ADSR_SEG_DECAY ? sus_level_ : -0.01f;
        switch(mode_)
        {
            case ADSR_SEG_IDLE: out = 0.0f; break;
            case ADSR_SEG_ATTACK:
                x_ += D0 * (attackTarget_ - x_);
                out = x_;
                if(out > 1.f)
                {
                    x_ = out = 1.f;
                    mode_    = ADSR_SEG_DECAY;
                }
                break;
            case ADSR_SEG_DECAY:
            case ADSR_SEG_RELEASE:
                x_ += D0 * (target - x_);
                out = x_;
                if(out < 0.0f)
                {
                    x_ = out = 0.f;
                    mode_    = ADSR_SEG_IDLE;
                }
            default: break;
        }
        return out;
    }

    inline void Line::Init(float sample_rate)
    {
        sample_rate_ = sample_rate;
        dur_         = 0.5f;
        end_         = 0.0f;
        start_       = 1.0f;
        val_         = 1.0f;
    }

    inline void Line::Start(float start, float end, float dur)
    {
        start_    = start;
        end_      = end;
        dur_      = dur;
        inc_      = (end - start) / ((sample_rate_ * dur_));
        val_      = start_;
        finished_ = 0;
    }

    inline float Line::Process(uint8_t *finished)
    {
        float out;
        out = val_;

        if((end_ > start_ && out >= end_) || (end_ < start_ && out <= end_))
        {
            finished_ = 1;
            val_      = end_;
            out       = end_;
        }
        else
        {
            val_ += inc_;
        }
        *finished = finished_;
        return out;
    }

    inline void Phasor::SetFreq(float freq)
    {
        freq_ = freq;
        inc_  = (TWOPI_F * freq_) / sample_rate_;
    }

    inline float Phasor::Process()
    {
        float out;
        out = phs_ / TWOPI_F;
        phs_ += inc_;
        if(phs_ > TWOPI_F)
        {
            phs_ -= TWOPI_F;
        }
        if(phs_ < 0.0f)
        {
            phs_ = 0.0f;
        }
        return out;
    }    


    constexpr float TWO_PI_RECIP = 1.0f / TWOPI_F;

    float Polyblep(float phase_inc, float t)
    {
        float dt = phase_inc * TWO_PI_RECIP;
        if(t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0f;
        }
        else if(t > 1.0f - dt)
        {
            t = (t - 1.0f) / dt;
            return t * t + t + t + 1.0f;
        }
        else
        {
            return 0.0f;
        }
    }    

    float Oscillator::Process()
    {
        float out, t;
        switch(waveform_)
        {
            case WAVE_SIN: out = sinf(phase_); break;
            case WAVE_TRI:
                t   = -1.0f + (2.0f * phase_ * TWO_PI_RECIP);
                out = 2.0f * (fabsf(t) - 0.5f);
                break;
            case WAVE_SAW:
                out = -1.0f * (((phase_ * TWO_PI_RECIP * 2.0f)) - 1.0f);
                break;
            case WAVE_RAMP: out = ((phase_ * TWO_PI_RECIP * 2.0f)) - 1.0f; break;
            case WAVE_SQUARE: out = phase_ < PI_F ? (1.0f) : -1.0f; break;
            case WAVE_POLYBLEP_TRI:
                t   = phase_ * TWO_PI_RECIP;
                out = phase_ < PI_F ? 1.0f : -1.0f;
                out += Polyblep(phase_inc_, t);
                out -= Polyblep(phase_inc_, fmodf(t + 0.5f, 1.0f));
                // Leaky Integrator:
                // y[n] = A + x[n] + (1 - A) * y[n-1]
                out       = phase_inc_ * out + (1.0f - phase_inc_) * last_out_;
                last_out_ = out;
                break;
            case WAVE_POLYBLEP_SAW:
                t   = phase_ * TWO_PI_RECIP;
                out = (2.0f * t) - 1.0f;
                out -= Polyblep(phase_inc_, t);
                out *= -1.0f;
                break;
            case WAVE_POLYBLEP_SQUARE:
                t   = phase_ * TWO_PI_RECIP;
                out = phase_ < PI_F ? 1.0f : -1.0f;
                out += Polyblep(phase_inc_, t);
                out -= Polyblep(phase_inc_, fmodf(t + 0.5f, 1.0f));
                out *= 0.707f; // ?
                break;
            default: out = 0.0f; break;
        }
        phase_ += phase_inc_;
        if(phase_ > TWOPI_F)
        {
            phase_ -= TWOPI_F;
            eoc_ = true;
        }
        else
        {
            eoc_ = false;
        }
        eor_ = (phase_ - phase_inc_ < PI_F && phase_ >= PI_F);

        return out * amp_;
    }

    float Oscillator::CalcPhaseInc(float f)
    {
        return (TWOPI_F * f) * sr_recip_;
    }


void OscillatorBank::Init(float sample_rate)
    {
        sample_rate_ = sample_rate;

        phase_       = 0.0f;
        next_sample_ = 0.0f;
        segment_     = 0.0f;

        frequency_  = 0.f;
        saw_8_gain_ = 0.0f;
        saw_4_gain_ = 0.0f;
        saw_2_gain_ = 0.0f;
        saw_1_gain_ = 0.0f;

        recalc_ = recalc_gain_ = true;
        SetGain(1.f);

        for(int i = 0; i < 7; i++)
        {
            registration_[i]           = 0.f;
            unshifted_registration_[i] = 0.f;
        }
        SetSingleAmp(1.f, 0);
        SetFreq(440.f);
    }

    float OscillatorBank::Process()
    {
        if(recalc_)
        {
            recalc_ = false;
            frequency_ *= 8.0f;

            // Deal with very high frequencies by shifting everything 1 or 2 octave
            // down: Instead of playing the 1st harmonic of a 8kHz wave, we play the
            // second harmonic of a 4kHz wave.
            size_t shift = 0;
            while(frequency_ > 0.5f)
            {
                shift += 2;
                frequency_ *= 0.5f;
            }

            for(int i = 0; i < 7; i++)
            {
                registration_[i] = 0.f;
            }

            for(size_t i = 0; i < 7 - shift; i++)
            {
                registration_[i + shift] = unshifted_registration_[i];
            }
        }

        if(recalc_gain_ || recalc_)
        {
            saw_8_gain_ = (registration_[0] + 2.0f * registration_[1]) * gain_;
            saw_4_gain_
                = (registration_[2] - registration_[1] + 2.0f * registration_[3])
                * gain_;
            saw_2_gain_
                = (registration_[4] - registration_[3] + 2.0f * registration_[5])
                * gain_;
            saw_1_gain_ = (registration_[6] - registration_[5]) * gain_;
        }

        float this_sample_ = next_sample_;
        next_sample_       = 0.0f;

        phase_ += frequency_;
        int next_segment_ = static_cast<int>(phase_);
        if(next_segment_ != segment_)
        {
            float discontinuity = 0.0f;
            if(next_segment_ == 8)
            {
                phase_ -= 8.0f;
                next_segment_ -= 8;
                discontinuity -= saw_8_gain_;
            }
            if((next_segment_ & 3) == 0)
            {
                discontinuity -= saw_4_gain_;
            }
            if((next_segment_ & 1) == 0)
            {
                discontinuity -= saw_2_gain_;
            }
            discontinuity -= saw_1_gain_;
            if(discontinuity != 0.0f)
            {
                float fraction = phase_ - static_cast<float>(next_segment_);
                float t        = fraction / frequency_;
                this_sample_ += ThisBlepSample(t) * discontinuity;
                next_sample_ += NextBlepSample(t) * discontinuity;
            }
        }
        segment_ = next_segment_;

        next_sample_ += (phase_ - 4.0f) * saw_8_gain_ * 0.125f;
        next_sample_ += (phase_ - float(segment_ & 4) - 2.0f) * saw_4_gain_ * 0.25f;
        next_sample_ += (phase_ - float(segment_ & 6) - 1.0f) * saw_2_gain_ * 0.5f;
        next_sample_ += (phase_ - float(segment_ & 7) - 0.5f) * saw_1_gain_;

        return 2.0f * this_sample_;
    }

    void OscillatorBank::SetFreq(float freq)
    {
        freq       = freq / sample_rate_;
        freq       = freq > 0.5f ? 0.5f : freq;
        recalc_    = cmp(freq, frequency_) || recalc_;
        frequency_ = freq;
    }

    void OscillatorBank::SetAmplitudes(const float* amplitudes)
    {
        for(int i = 0; i < 7; i++)
        {
            recalc_ = cmp(unshifted_registration_[i], amplitudes[i]) || recalc_;
            unshifted_registration_[i] = amplitudes[i];
        }
    }

    void OscillatorBank::SetSingleAmp(float amp, int idx)
    {
        if(idx < 0 || idx > 6)
        {
            return;
        }
        recalc_ = cmp(unshifted_registration_[idx], amp) || recalc_;
        unshifted_registration_[idx] = amp;
    }

    void OscillatorBank::SetGain(float gain)
    {
        gain         = gain > 1.f ? 1.f : gain;
        gain         = gain < 0.f ? 0.f : gain;
        recalc_gain_ = cmp(gain, gain_) || recalc_gain_;
        gain_        = gain;
    }


    void BlOsc::Init(float sample_rate)
    {
        sampling_freq_  = sample_rate;
        half_sr_        = 0.5 * sampling_freq_;
        quarter_sr_     = sampling_freq_ * 0.25;
        sec_per_sample_ = 1.0 / sampling_freq_;
        two_over_sr_    = (float)(2.0 / sampling_freq_);
        four_over_sr_   = 4.0 / sampling_freq_;

        freq_ = 440;
        amp_  = 0.5;
        pw_   = 0.5;
        iota_ = 0;
        mode_ = WAVE_TRIANGLE;

        for(int i = 0; i < 2; i++)
        {
            rec0_[i] = rec1_[i] = vec0_[i] = vec1_[i] = 0.0;
        }

        for(int i = 0; i < 4096; i++)
        {
            vec2_[i] = 0.0;
        }
    }

    float BlOsc::ProcessSquare()
    {
        float out;
        float fSlow2 = fmin(2047.0, sampling_freq_ * (pw_ / freq_));
        float fSlow5 = (float)((int)fSlow2 + 1) - fSlow2;
        float fSlow6 = (quarter_sr_ / freq_);
        float fSlow7 = (sec_per_sample_ * freq_);
        float fSlow8 = fSlow2 - (int)fSlow2;

        rec0_[0]     = fmodf(rec0_[1] + fSlow7, 1.0);
        float fTemp0 = 2.0 * rec0_[0] - 1.0;
        fTemp0 *= fTemp0; //mult faster than fpow for squaring?
        vec1_[0]            = fTemp0;
        float fTemp1        = (fSlow6 * ((fTemp0 - vec1_[1])));
        vec2_[iota_ & 4095] = fTemp1;

        out = amp_
            * (0.0
                - ((fSlow5 * vec2_[(iota_ - (int)fSlow2) & 4095]
                    + fSlow8 * vec2_[(iota_ - ((int)fSlow2 + 1)) & 4095])
                    - fTemp1));
        rec0_[1] = rec0_[0];
        vec1_[1] = vec1_[0];
        iota_++;

        return out;
    }

    float BlOsc::ProcessTriangle()
    {
        float out;
        float fSlow1 = four_over_sr_ * (amp_ * freq_);
        float fSlow3 = half_sr_ / freq_;
        int   iSlow4 = (int)fSlow3;
        int   iSlow5 = 1 + iSlow4;
        float fSlow6 = iSlow5 - fSlow3;
        float fSlow7 = quarter_sr_ / freq_;
        float fSlow8 = sec_per_sample_ * freq_;
        float fSlow9 = fSlow3 - iSlow4; //decimal portion


        rec1_[0]     = fmodf((fSlow8 + rec1_[1]), 1.0);
        float fTemp0 = 2.0 * rec1_[0] - 1.0;
        fTemp0 *= fTemp0; //mult faster than fpow for squaring?
        vec1_[0]            = fTemp0;
        float fTemp1        = fSlow7 * (fTemp0 - vec1_[1]);
        vec2_[iota_ & 4095] = fTemp1;
        rec0_[0]            = 0.0
                - ((fSlow6 * vec2_[(iota_ - iSlow4) & 4095]
                    + fSlow9 * vec2_[(iota_ - iSlow5) & 4095])
                    - (.999 * rec0_[1] + fTemp1));

        out      = (float)(fSlow1 * rec0_[0]);
        rec1_[1] = rec1_[0];
        rec0_[1] = rec0_[0];
        vec1_[1] = vec1_[0];
        iota_++;

        return out;
    }

    float BlOsc::ProcessSaw()
    {
        float out;
        //fSlow0 = Slider1 = freq
        float fSlow1 = sampling_freq_ * (amp_ / freq_);
        float fSlow2 = (two_over_sr_ * freq_);
        float fSlow3 = (sampling_freq_ / freq_);

        rec0_[0]     = fmod((1.0 + rec0_[1]), fSlow3);
        float fTemp0 = fSlow2 * rec0_[0] - 1.0;
        fTemp0 *= fTemp0; //mult faster than fpow for squaring?
        vec0_[0] = fTemp0;
        vec1_[0] = 0.25;
        out      = (float)(fSlow1 * ((fTemp0 - vec0_[1]) * vec1_[1]));
        rec0_[1] = rec0_[0];
        vec0_[1] = vec0_[0];
        vec1_[1] = vec1_[0];

        return out;
    }

    void BlOsc::Reset()
    {
        iota_ = 0;

        for(int i = 0; i < 2; i++)
        {
            rec0_[i] = rec1_[i] = vec0_[i] = vec1_[i] = 0.0;
        }

        for(int i = 0; i < 4096; i++)
        {
            vec2_[i] = 0.0;
        }
    }

    float BlOsc::Process()
    {
        switch(mode_)
        {
            case WAVE_TRIANGLE: return ProcessTriangle();
            case WAVE_SAW: return ProcessSaw();
            case WAVE_SQUARE: return ProcessSquare();
            default: break;
        }

        return 0.0;
    }    


void Fm2::Init(float samplerate)
{
    //init oscillators
    car_.Init(samplerate);
    mod_.Init(samplerate);

    //set some reasonable values
    lfreq_  = 440.f;
    lratio_ = 2.f;
    SetFrequency(lfreq_);
    SetRatio(lratio_);

    car_.SetAmp(1.f);
    mod_.SetAmp(1.f);

    car_.SetWaveform(Oscillator::WAVE_SIN);
    mod_.SetWaveform(Oscillator::WAVE_SIN);

    idx_ = 1.f;
}

float Fm2::Process()
{
    if(lratio_ != ratio_ || lfreq_ != freq_)
    {
        lratio_ = ratio_;
        lfreq_  = freq_;
        car_.SetFreq(lfreq_);
        mod_.SetFreq(lfreq_ * lratio_);
    }

    float modval = mod_.Process();
    car_.PhaseAdd(modval * idx_);
    return car_.Process();
}

void Fm2::SetFrequency(float freq)
{
    freq_ = fabsf(freq);
}

void Fm2::SetRatio(float ratio)
{
    ratio_ = fabsf(ratio);
}

void Fm2::SetIndex(float index)
{
    idx_ = index * kIdxScalar;
}

float Fm2::GetIndex()
{
    return idx_ * kIdxScalarRecip;
}

void Fm2::Reset()
{
    car_.Reset();
    mod_.Reset();
}


void FormantOscillator::Init(float sample_rate)
{
    carrier_phase_ = 0.0f;
    formant_phase_ = 0.0f;
    next_sample_   = 0.0f;

    carrier_frequency_ = 0.0f;
    formant_frequency_ = 100.f;
    phase_shift_       = 0.0f;

    sample_rate_ = sample_rate;
}

float FormantOscillator::Process()
{
    float this_sample = next_sample_;
    float next_sample = 0.0f;
    carrier_phase_ += carrier_frequency_;

    if(carrier_phase_ >= 1.0f)
    {
        carrier_phase_ -= 1.0f;
        float reset_time = carrier_phase_ / carrier_frequency_;

        float formant_phase_at_reset
            = formant_phase_ + (1.0f - reset_time) * formant_frequency_;
        float before        = Sine(formant_phase_at_reset + phase_shift_
                            + (ps_inc_ * (1.0f - reset_time)));
        float after         = Sine(0.0f + phase_shift_ + ps_inc_);
        float discontinuity = after - before;
        this_sample += discontinuity * ThisBlepSample(reset_time);
        next_sample += discontinuity * NextBlepSample(reset_time);
        formant_phase_ = reset_time * formant_frequency_;
    }
    else
    {
        formant_phase_ += formant_frequency_;
        if(formant_phase_ >= 1.0f)
        {
            formant_phase_ -= 1.0f;
        }
    }

    phase_shift_ += ps_inc_;
    ps_inc_ = 0.f;

    next_sample += Sine(formant_phase_ + phase_shift_);

    next_sample_ = next_sample;
    return this_sample;
}

void FormantOscillator::SetFormantFreq(float freq)
{
    //convert from Hz to phase_inc / sample
    formant_frequency_ = freq / sample_rate_;
    formant_frequency_ = formant_frequency_ >= .25f ? .25f : formant_frequency_;
    formant_frequency_
        = formant_frequency_ <= -.25f ? -.25f : formant_frequency_;
}

void FormantOscillator::SetCarrierFreq(float freq)
{
    //convert from Hz to phase_inc / sample
    carrier_frequency_ = freq / sample_rate_;
    carrier_frequency_ = carrier_frequency_ >= .25f ? .25f : carrier_frequency_;
    carrier_frequency_
        = carrier_frequency_ <= -.25f ? -.25f : carrier_frequency_;
}

void FormantOscillator::SetPhaseShift(float ps)
{
    ps_inc_ = ps - phase_shift_;
}

inline float FormantOscillator::Sine(float phase)
{
    return sinf(phase * TWOPI_F);
}

inline float FormantOscillator::ThisBlepSample(float t)
{
    return 0.5f * t * t;
}

inline float FormantOscillator::NextBlepSample(float t)
{
    t = 1.0f - t;
    return -0.5f * t * t;
}

void VariableSawOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    phase_       = 0.0f;
    next_sample_ = 0.0f;
    previous_pw_ = 0.5f;
    high_        = false;

    SetFreq(220.f);
    SetPW(0.f);
    SetWaveshape(1.f);
}

float VariableSawOscillator::Process()
{
    float next_sample = next_sample_;

    float this_sample = next_sample;
    next_sample       = 0.0f;

    const float triangle_amount = waveshape_;
    const float notch_amount    = 1.0f - waveshape_;
    const float slope_up        = 1.0f / (pw_);
    const float slope_down      = 1.0f / (1.0f - pw_);

    phase_ += frequency_;

    if(!high_ && phase_ >= pw_)
    {
        const float triangle_step
            = (slope_up + slope_down) * frequency_ * triangle_amount;
        const float notch
            = (kVariableSawNotchDepth + 1.0f - pw_) * notch_amount;
        const float t = (phase_ - pw_) / (previous_pw_ - pw_ + frequency_);
        this_sample += notch * ThisBlepSample(t);
        next_sample += notch * NextBlepSample(t);
        this_sample -= triangle_step * ThisIntegratedBlepSample(t);
        next_sample -= triangle_step * NextIntegratedBlepSample(t);
        high_ = true;
    }
    else if(phase_ >= 1.0f)
    {
        phase_ -= 1.0f;
        const float triangle_step
            = (slope_up + slope_down) * frequency_ * triangle_amount;
        const float notch = (kVariableSawNotchDepth + 1.0f) * notch_amount;
        const float t     = phase_ / frequency_;
        this_sample -= notch * ThisBlepSample(t);
        next_sample -= notch * NextBlepSample(t);
        this_sample += triangle_step * ThisIntegratedBlepSample(t);
        next_sample += triangle_step * NextIntegratedBlepSample(t);
        high_ = false;
    }

    next_sample += ComputeNaiveSample(
        phase_, pw_, slope_up, slope_down, triangle_amount, notch_amount);
    previous_pw_ = pw_;

    next_sample_ = next_sample;
    return (2.0f * this_sample - 1.0f) / (1.0f + kVariableSawNotchDepth);
}

void VariableSawOscillator::SetFreq(float frequency)
{
    frequency  = frequency / sample_rate_;
    frequency  = frequency >= .25f ? .25f : frequency;
    pw_        = frequency >= .25f ? .5f : pw_;
    frequency_ = frequency;
}

void VariableSawOscillator::SetPW(float pw)
{
    if(frequency_ >= .25f)
    {
        pw_ = .5f;
    }
    else
    {
        pw_ = fclamp(pw, frequency_ * 2.0f, 1.0f - 2.0f * frequency_);
    }
}

void VariableSawOscillator::SetWaveshape(float waveshape)
{
    waveshape_ = waveshape;
}

float VariableSawOscillator::ComputeNaiveSample(float phase,
                                                float pw,
                                                float slope_up,
                                                float slope_down,
                                                float triangle_amount,
                                                float notch_amount)
{
    float notch_saw = phase < pw ? phase : 1.0f + kVariableSawNotchDepth;
    float triangle
        = phase < pw ? phase * slope_up : 1.0f - (phase - pw) * slope_down;
    return notch_saw * notch_amount + triangle * triangle_amount;
}


void VariableShapeOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    master_phase_ = 0.0f;
    slave_phase_  = 0.0f;
    next_sample_  = 0.0f;
    previous_pw_  = 0.5f;
    high_         = false;

    SetFreq(440.f);
    SetWaveshape(0.f);
    SetPW(0.f);
    SetSync(false);
    SetSyncFreq(220.f);
}

float VariableShapeOscillator::Process()
{
    float next_sample = next_sample_;

    bool  reset                   = false;
    bool  transition_during_reset = false;
    float reset_time              = 0.0f;

    float this_sample = next_sample;
    next_sample       = 0.0f;

    const float square_amount   = fmax(waveshape_ - 0.5f, 0.0f) * 2.0f;
    const float triangle_amount = fmax(1.0f - waveshape_ * 2.0f, 0.0f);
    const float slope_up        = 1.0f / (pw_);
    const float slope_down      = 1.0f / (1.0f - pw_);

    if(enable_sync_)
    {
        master_phase_ += master_frequency_;
        if(master_phase_ >= 1.0f)
        {
            master_phase_ -= 1.0f;
            reset_time = master_phase_ / master_frequency_;

            float slave_phase_at_reset
                = slave_phase_ + (1.0f - reset_time) * slave_frequency_;
            reset = true;
            if(slave_phase_at_reset >= 1.0f)
            {
                slave_phase_at_reset -= 1.0f;
                transition_during_reset = true;
            }
            if(!high_ && slave_phase_at_reset >= pw_)
            {
                transition_during_reset = true;
            }
            float value = ComputeNaiveSample(slave_phase_at_reset,
                                             pw_,
                                             slope_up,
                                             slope_down,
                                             triangle_amount,
                                             square_amount);
            this_sample -= value * ThisBlepSample(reset_time);
            next_sample -= value * NextBlepSample(reset_time);
        }
    }

    slave_phase_ += slave_frequency_;
    while(transition_during_reset || !reset)
    {
        if(!high_)
        {
            if(slave_phase_ < pw_)
            {
                break;
            }
            float t = (slave_phase_ - pw_)
                      / (previous_pw_ - pw_ + slave_frequency_);
            float triangle_step = (slope_up + slope_down) * slave_frequency_;
            triangle_step *= triangle_amount;

            this_sample += square_amount * ThisBlepSample(t);
            next_sample += square_amount * NextBlepSample(t);
            this_sample -= triangle_step * ThisIntegratedBlepSample(t);
            next_sample -= triangle_step * NextIntegratedBlepSample(t);
            high_ = true;
        }

        if(high_)
        {
            if(slave_phase_ < 1.0f)
            {
                break;
            }
            slave_phase_ -= 1.0f;
            float t             = slave_phase_ / slave_frequency_;
            float triangle_step = (slope_up + slope_down) * slave_frequency_;
            triangle_step *= triangle_amount;

            this_sample -= (1.0f - triangle_amount) * ThisBlepSample(t);
            next_sample -= (1.0f - triangle_amount) * NextBlepSample(t);
            this_sample += triangle_step * ThisIntegratedBlepSample(t);
            next_sample += triangle_step * NextIntegratedBlepSample(t);
            high_ = false;
        }
    }

    if(enable_sync_ && reset)
    {
        slave_phase_ = reset_time * slave_frequency_;
        high_        = false;
    }

    next_sample += ComputeNaiveSample(slave_phase_,
                                      pw_,
                                      slope_up,
                                      slope_down,
                                      triangle_amount,
                                      square_amount);
    previous_pw_ = pw_;


    next_sample_ = next_sample;
    return (2.0f * this_sample - 1.0f);
}

void VariableShapeOscillator::SetFreq(float frequency)
{
    frequency         = frequency / sample_rate_;
    frequency         = frequency >= .25f ? .25f : frequency;
    master_frequency_ = frequency;
}

void VariableShapeOscillator::SetPW(float pw)
{
    if(slave_frequency_ >= .25f)
    {
        pw_ = .5f;
    }
    else
    {
        pw_ = fclamp(
            pw, slave_frequency_ * 2.0f, 1.0f - 2.0f * slave_frequency_);
    }
}

void VariableShapeOscillator::SetWaveshape(float waveshape)
{
    waveshape_ = waveshape;
}

void VariableShapeOscillator::SetSync(bool enable_sync)
{
    enable_sync_ = enable_sync;
}

void VariableShapeOscillator::SetSyncFreq(float frequency)
{
    frequency        = frequency / sample_rate_;
    pw_              = frequency >= .25f ? .5f : pw_;
    frequency        = frequency >= .25f ? .25f : frequency;
    slave_frequency_ = frequency;
}

float VariableShapeOscillator::ComputeNaiveSample(float phase,
                                                  float pw,
                                                  float slope_up,
                                                  float slope_down,
                                                  float triangle_amount,
                                                  float square_amount)
{
    float saw    = phase;
    float square = phase < pw ? 0.0f : 1.0f;
    float triangle
        = phase < pw ? phase * slope_up : 1.0f - (phase - pw) * slope_down;
    saw += (square - saw) * square_amount;
    saw += (triangle - saw) * triangle_amount;
    return saw;
}


void VosimOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    carrier_phase_   = 0.0f;
    formant_1_phase_ = 0.0f;
    formant_2_phase_ = 0.0f;

    SetFreq(105.f);
    SetForm1Freq(1390.f);
    SetForm2Freq(817.f);
    SetShape(.5f);
}

float VosimOscillator::Process()
{
    carrier_phase_ += carrier_frequency_;
    if(carrier_phase_ >= 1.0f)
    {
        carrier_phase_ -= 1.0f;
        float reset_time = carrier_phase_ / carrier_frequency_;
        formant_1_phase_ = reset_time * formant_1_frequency_;
        formant_2_phase_ = reset_time * formant_2_frequency_;
    }
    else
    {
        formant_1_phase_ += formant_1_frequency_;
        if(formant_1_phase_ >= 1.0f)
        {
            formant_1_phase_ -= 1.0f;
        }
        formant_2_phase_ += formant_2_frequency_;
        if(formant_2_phase_ >= 1.0f)
        {
            formant_2_phase_ -= 1.0f;
        }
    }

    float carrier         = Sine(carrier_phase_ * 0.5f + 0.25f) + 1.0f;
    float reset_phase     = 0.75f - 0.25f * carrier_shape_;
    float reset_amplitude = Sine(reset_phase);
    float formant_0 = Sine(formant_1_phase_ + reset_phase) - reset_amplitude;
    float formant_1 = Sine(formant_2_phase_ + reset_phase) - reset_amplitude;
    return carrier * (formant_0 + formant_1) * 0.25f + reset_amplitude;
}

void VosimOscillator::SetFreq(float freq)
{
    carrier_frequency_ = freq / sample_rate_;
    carrier_frequency_ = carrier_frequency_ > .25f ? .25f : carrier_frequency_;
}

void VosimOscillator::SetForm1Freq(float freq)
{
    formant_1_frequency_ = freq / sample_rate_;
    formant_1_frequency_
        = formant_1_frequency_ > .25f ? .25f : formant_1_frequency_;
}

void VosimOscillator::SetForm2Freq(float freq)
{
    formant_2_frequency_ = freq / sample_rate_;
    formant_2_frequency_
        = formant_2_frequency_ > .25f ? .25f : formant_2_frequency_;
}

void VosimOscillator::SetShape(float shape)
{
    carrier_shape_ = shape;
}

float VosimOscillator::Sine(float phase)
{
    return sinf(TWOPI_F * phase);
}

void ZOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    carrier_phase_       = 0.0f;
    discontinuity_phase_ = 0.0f;
    formant_phase_       = 0.0f;
    next_sample_         = 0.0f;

    SetFreq(220.f);
    SetFormantFreq(550.f);
    SetMode(0.f);
    SetShape(1.f);
}

float ZOscillator::Process()
{
    float next_sample = next_sample_;

    bool  reset      = false;
    float reset_time = 0.0f;

    float this_sample = next_sample;
    next_sample       = 0.0f;

    discontinuity_phase_ += 2.0f * carrier_frequency_;
    carrier_phase_ += carrier_frequency_;
    reset = discontinuity_phase_ >= 1.0f;

    if(reset)
    {
        discontinuity_phase_ -= 1.0f;
        reset_time = discontinuity_phase_ / (2.0f * carrier_frequency_);

        float carrier_phase_before = carrier_phase_ >= 1.0f ? 1.0f : 0.5f;
        float carrier_phase_after  = carrier_phase_ >= 1.0f ? 0.0f : 0.5f;

        float mode_sub  = mode_ + (1.f - reset_time) * (mode_ - mode_new_);
        float shape_sub = carrier_shape_
                          + (1.0f - reset_time) * (carrier_shape_ - shape_new_);
        float before
            = Z(carrier_phase_before,
                1.0f,
                formant_phase_ + (1.0f - reset_time) * formant_frequency_,
                shape_sub,
                mode_sub);

        float after = Z(carrier_phase_after, 0.0f, 0.0f, shape_new_, mode_new_);

        float discontinuity = after - before;
        this_sample += discontinuity * ThisBlepSample(reset_time);
        next_sample += discontinuity * NextBlepSample(reset_time);
        formant_phase_ = reset_time * formant_frequency_;

        if(carrier_phase_ > 1.0f)
        {
            carrier_phase_ = discontinuity_phase_ * 0.5f;
        }
    }
    else
    {
        formant_phase_ += formant_frequency_;
        if(formant_phase_ >= 1.0f)
        {
            formant_phase_ -= 1.0f;
        }
    }

    if(carrier_phase_ >= 1.0f)
    {
        carrier_phase_ -= 1.0f;
    }

    carrier_shape_ = shape_new_;
    mode_          = mode_new_;
    next_sample += Z(carrier_phase_,
                     discontinuity_phase_,
                     formant_phase_,
                     carrier_shape_,
                     mode_);

    next_sample_ = next_sample;
    return this_sample;
}

inline float ZOscillator::Sine(float phase)
{
    return sinf(phase * TWOPI_F);
}

void ZOscillator::SetFreq(float freq)
{
    //convert from Hz to phase_inc / sample
    carrier_frequency_ = freq / sample_rate_;
    carrier_frequency_ = carrier_frequency_ >= .25f ? .25f : carrier_frequency_;
}

void ZOscillator::SetFormantFreq(float freq)
{
    //convert from Hz to phase_inc / sample
    formant_frequency_ = freq / sample_rate_;
    formant_frequency_ = formant_frequency_ >= .25f ? .25f : formant_frequency_;
}

void ZOscillator::SetShape(float shape)
{
    shape_new_ = shape;
}

void ZOscillator::SetMode(float mode)
{
    mode_new_ = mode;
}

inline float ZOscillator::Z(float c, float d, float f, float shape, float mode)
{
    float ramp_down = 0.5f * (1.0f + Sine(0.5f * d + 0.25f));

    float offset;
    float phase_shift;
    if(mode < 0.333f)
    {
        offset      = 1.0f;
        phase_shift = 0.25f + mode * 1.50f;
    }
    else if(mode < 0.666f)
    {
        phase_shift = 0.7495f - (mode - 0.33f) * 0.75f;
        offset      = -Sine(phase_shift);
    }
    else
    {
        phase_shift = 0.7495f - (mode - 0.33f) * 0.75f;
        offset      = 0.001f;
    }

    float discontinuity = Sine(f + phase_shift);
    float contour;
    if(shape < 0.5f)
    {
        shape *= 2.0f;
        if(c >= 0.5f)
        {
            ramp_down *= shape;
        }
        contour = 1.0f + (Sine(c + 0.25f) - 1.0f) * shape;
    }
    else
    {
        contour = Sine(c + shape * 0.5f);
    }
    return (ramp_down * (offset + discontinuity) - offset) * contour;
}


void Allpass::Init(float sample_rate, float* buff, size_t size)
{
    sample_rate_   = sample_rate;
    rev_time_      = 3.5;
    max_loop_time_ = ((float)size / sample_rate_) - .01;
    loop_time_     = max_loop_time_;
    mod_           = (int)(loop_time_ * sample_rate_);
    buf_           = buff;
    prvt_          = 0.0;
    coef_          = 0.0;
    buf_pos_       = 0;
}

float Allpass::Process(float in)
{
    float y, z, out;
    if(prvt_ != rev_time_)
    {
        prvt_ = rev_time_;
        coef_ = expf(-6.9078 * loop_time_ / prvt_);
    }

    y              = buf_[buf_pos_];
    z              = coef_ * y + in;
    buf_[buf_pos_] = z;
    out            = y - coef_ * z;

    buf_pos_++;
    buf_pos_ %= mod_;
    return out;
}

void Allpass::SetFreq(float freq)
{
    loop_time_ = fmaxf(fminf(freq, max_loop_time_), .0001);
    mod_       = fmaxf(loop_time_ * sample_rate_, 0);
}

void ATone::Init(float sample_rate)
{
    prevout_     = 0.0f;
    freq_        = 1000.0f;
    c2_          = 0.5f;
    sample_rate_ = sample_rate;
}

float ATone::Process(float &in)
{
    float out;

    out      = c2_ * (prevout_ + in);
    prevout_ = out - in;

    return out;
}

void ATone::CalculateCoefficients()
{
    float b, c2;

    b   = 2.0f - cosf(TWOPI_F * freq_ / sample_rate_);
    c2  = b - sqrtf(b * b - 1.0f);
    c2_ = c2;
}

void Biquad::Init(float sample_rate)
{
    sample_rate_ = sample_rate;
    two_pi_d_sr_ = TWOPI_F / sample_rate_;

    cutoff_ = 500;
    res_    = 0.7;

    Reset();

    xnm1_ = xnm2_ = ynm1_ = ynm2_ = 0.0f;
}

float Biquad::Process(float in)
{
    float xn, yn;
    float a0 = a0_, a1 = a1_, a2 = a2_;
    float b0 = b0_, b1 = b1_, b2 = b2_;
    float xnm1 = xnm1_, xnm2 = xnm2_, ynm1 = ynm1_, ynm2 = ynm2_;

    xn   = in;
    yn   = (b0 * xn + b1 * xnm1 + b2 * xnm2 - a1 * ynm1 - a2 * ynm2) / a0;
    xnm2 = xnm1;
    xnm1 = xn;
    ynm2 = ynm1;
    ynm1 = yn;

    xnm1_ = xnm1;
    xnm2_ = xnm2;
    ynm1_ = ynm1;
    ynm2_ = ynm2;

    return yn;
}

static float log001 = -6.9078f; // log .001

void Comb::Init(float sample_rate, float* buff, size_t size)
{
    sample_rate_   = sample_rate;
    rev_time_      = 3.5;
    max_size_      = size;
    max_loop_time_ = ((float)size / sample_rate_) - .01;
    loop_time_     = max_loop_time_;
    mod_           = sample_rate_ * loop_time_;
    buf_           = buff;
    prvt_          = 0.0f;
    coef_          = 0.0f;
    buf_pos_       = 0;
}

float Comb::Process(float in)
{
    float tmp     = 0;
    float coef    = coef_;
    float outsamp = 0;

    if(prvt_ != rev_time_)
    {
        prvt_         = rev_time_;
        float exp_arg = (float)(log001 * loop_time_ / prvt_);
        if(exp_arg < -36.8413615)
        {
            coef = coef_ = 0;
        }
        else
        {
            coef = coef_ = expf(exp_arg);
        }
    }

    // internal delay line
    outsamp                = buf_[(buf_pos_ + mod_) % max_size_];
    tmp                    = (outsamp * coef) + in;
    buf_[(size_t)buf_pos_] = tmp;
    buf_pos_               = (buf_pos_ - 1 + max_size_) % max_size_;

    return outsamp;
}

void Comb::SetPeriod(float looptime)
{
    if(looptime > 0)
    {
        loop_time_ = fminf(looptime, max_loop_time_);
        mod_       = loop_time_ * sample_rate_;
        if(mod_ > max_size_)
        {
            mod_ = max_size_ - 1;
        }
    }
}


void Mode::Init(float sample_rate)
{
    freq_ = 500.0f;
    q_    = 50;

    xnm1_ = ynm1_ = ynm2_ = 0.0f;
    a0_ = a1_ = a2_ = d_ = 0.0f;
    lfq_ = lq_ = -1.0f;
    sr_        = sample_rate;
}

void Mode::Clear()
{
    xnm1_ = ynm1_ = ynm2_ = 0.0f;
    a0_ = a1_ = a2_ = 0.0f;
    d_              = 0.0f;
    lfq_            = -1.0f;
    lq_             = -1.0f;
}

float Mode::Process(float in)
{
    float out;
    float lfq = lfq_, lq = lq_;
    float xn, yn, a0 = a0_, a1 = a1_, a2 = a2_, d = d_;
    float xnm1 = xnm1_, ynm1 = ynm1_, ynm2 = ynm2_;
    float kfq = freq_;
    float kq  = q_;

    if(lfq != kfq || lq != kq)
    {
        float kfreq  = kfq * (2.0f * (float)M_PI);
        float kalpha = (sr_ / kfreq);
        float kbeta  = kalpha * kalpha;
        d            = 0.5f * kalpha;
        lq           = kq;
        lfq          = kfq;
        a0           = 1.0f / (kbeta + d / kfreq);
        a1           = a0 * (1.0f - 2.0f * kbeta);
        a2           = a0 * (kbeta - d / kq);
    }
    xn = in;

    yn = a0 * xnm1 - a1 * ynm1 - a2 * ynm2;

    xnm1 = xn;
    ynm2 = ynm1;
    ynm1 = yn;

    yn = yn * d;

    out   = yn;
    xnm1_ = xnm1;
    ynm1_ = ynm1;
    ynm2_ = ynm2;
    lfq_  = lfq;
    lq_   = lq;
    d_    = d;
    a0_   = a0;
    a1_   = a1;
    a2_   = a2;
    return out;
}


float MoogLadder::my_tanh(float x)
{
    int sign = 1;
    if(x < 0)
    {
        sign = -1;
        x    = -x;
        return x * sign;
    }
    else if(x >= 4.0f)
    {
        return sign;
    }
    else if(x < 0.5f)
    {
        return x * sign;
    }
    return sign * tanhf(x);
}

void MoogLadder::Init(float sample_rate)
{
    sample_rate_ = sample_rate;
    istor_       = 0.0f;
    res_         = 0.4f;
    freq_        = 1000.0f;

    for(int i = 0; i < 6; i++)
    {
        delay_[i]       = 0.0;
        tanhstg_[i % 3] = 0.0;
    }

    old_freq_ = 0.0f;
    old_res_  = -1.0f;
}

float MoogLadder::Process(float in)
{
    float  freq = freq_;
    float  res  = res_;
    float  res4;
    float* delay   = delay_;
    float* tanhstg = tanhstg_;
    float  stg[4];
    float  acr, tune;

    float THERMAL = 0.000025;

    if(res < 0)
    {
        res = 0;
    }

    if(old_freq_ != freq || old_res_ != res)
    {
        float f, fc, fc2, fc3, fcr;
        old_freq_ = freq;
        fc        = (freq / sample_rate_);
        f         = 0.5f * fc;
        fc2       = fc * fc;
        fc3       = fc2 * fc2;

        fcr  = 1.8730f * fc3 + 0.4955f * fc2 - 0.6490f * fc + 0.9988f;
        acr  = -3.9364f * fc2 + 1.8409f * fc + 0.9968f;
        tune = (1.0f - expf(-((2 * PI_F) * f * fcr))) / THERMAL;

        old_res_  = res;
        old_acr_  = acr;
        old_tune_ = tune;
    }
    else
    {
        res  = old_res_;
        acr  = old_acr_;
        tune = old_tune_;
    }

    res4 = 4.0f * res * acr;

    for(int j = 0; j < 2; j++)
    {
        in -= res4 * delay[5];
        delay[0] = stg[0]
            = delay[0] + tune * (my_tanh(in * THERMAL) - tanhstg[0]);
        for(int k = 1; k < 4; k++)
        {
            in     = stg[k - 1];
            stg[k] = delay[k]
                     + tune
                           * ((tanhstg[k - 1] = my_tanh(in * THERMAL))
                              - (k != 3 ? tanhstg[k]
                                        : my_tanh(delay[k] * THERMAL)));
            delay[k] = stg[k];
        }
        delay[5] = (stg[3] + delay[4]) * 0.5f;
        delay[4] = stg[3];
    }
    return delay[5];
}


void Svf::Init(float sample_rate)
{
    sr_        = sample_rate;
    fc_        = 200.0f;
    res_       = 0.5f;
    drive_     = 0.5f;
    pre_drive_ = 0.5f;
    freq_      = 0.25f;
    damp_      = 0.0f;
    notch_     = 0.0f;
    low_       = 0.0f;
    high_      = 0.0f;
    band_      = 0.0f;
    peak_      = 0.0f;
    input_     = 0.0f;
    out_notch_ = 0.0f;
    out_low_   = 0.0f;
    out_high_  = 0.0f;
    out_peak_  = 0.0f;
    out_band_  = 0.0f;
    fc_max_    = sr_ / 3.f;
}

void Svf::Process(float in)
{
    input_ = in;
    // first pass
    notch_ = input_ - damp_ * band_;
    low_   = low_ + freq_ * band_;
    high_  = notch_ - low_;
    band_  = freq_ * high_ + band_ - drive_ * band_ * band_ * band_;
    // take first sample of output
    out_low_   = 0.5f * low_;
    out_high_  = 0.5f * high_;
    out_band_  = 0.5f * band_;
    out_peak_  = 0.5f * (low_ - high_);
    out_notch_ = 0.5f * notch_;
    // second pass
    notch_ = input_ - damp_ * band_;
    low_   = low_ + freq_ * band_;
    high_  = notch_ - low_;
    band_  = freq_ * high_ + band_ - drive_ * band_ * band_ * band_;
    // average second pass outputs
    out_low_ += 0.5f * low_;
    out_high_ += 0.5f * high_;
    out_band_ += 0.5f * band_;
    out_peak_ += 0.5f * (low_ - high_);
    out_notch_ += 0.5f * notch_;
}

void Svf::SetFreq(float f)
{
    fc_ = fclamp(f, 1.0e-6, fc_max_);
    // Set Internal Frequency for fc_
    freq_ = 2.0f
            * sinf(PI_F
                   * MIN(0.25f,
                         fc_ / (sr_ * 2.0f))); // fs*2 because double sampled
    // recalculate damp
    damp_ = MIN(2.0f * (1.0f - powf(res_, 0.25f)),
                MIN(2.0f, 2.0f / freq_ - freq_ * 0.5f));
}

void Svf::SetRes(float r)
{
    float res = fclamp(r, 0.f, 1.f);
    res_      = res;
    // recalculate damp
    damp_  = MIN(2.0f * (1.0f - powf(res_, 0.25f)),
                MIN(2.0f, 2.0f / freq_ - freq_ * 0.5f));
    drive_ = pre_drive_ * res_;
}

void Svf::SetDrive(float d)
{
    float drv  = fclamp(d * 0.1f, 0.f, 1.f);
    pre_drive_ = drv;
    drive_     = pre_drive_ * res_;
}

inline float AnalogBassDrum::Diode(float x)
{
    if(x >= 0.0f)
    {
        return x;
    }
    else
    {
        x *= 2.0f;
        return 0.7f * x / (1.0f + fabsf(x));
    }
}

float AnalogBassDrum::Process(bool trigger)
{
    const int kTriggerPulseDuration  = static_cast<int>(1.0e-3f * sample_rate_);
    const int kFMPulseDuration       = static_cast<int>(6.0e-3f * sample_rate_);
    const float kPulseDecayTime      = 0.2e-3f * sample_rate_;
    const float kPulseFilterTime     = 0.1e-3f * sample_rate_;
    const float kRetrigPulseDuration = 0.05f * sample_rate_;

    const float scale = 0.001f / f0_;
    const float q     = 1500.0f * powf(2.f, kOneTwelfth * decay_ * 80.0f);
    const float tone_f
        = fmin(4.0f * f0_ * powf(2.f, kOneTwelfth * tone_ * 108.0f), 1.0f);
    const float exciter_leak = 0.08f * (tone_ + 0.25f);


    if(trigger || trig_)
    {
        trig_ = false;

        pulse_remaining_samples_    = kTriggerPulseDuration;
        fm_pulse_remaining_samples_ = kFMPulseDuration;
        pulse_height_               = 3.0f + 7.0f * accent_;
        lp_out_                     = 0.0f;
    }

    // Q39 / Q40
    float pulse = 0.0f;
    if(pulse_remaining_samples_)
    {
        --pulse_remaining_samples_;
        pulse = pulse_remaining_samples_ ? pulse_height_ : pulse_height_ - 1.0f;
        pulse_ = pulse;
    }
    else
    {
        pulse_ *= 1.0f - 1.0f / kPulseDecayTime;
        pulse = pulse_;
    }
    if(sustain_)
    {
        pulse = 0.0f;
    }

    // C40 / R163 / R162 / D83
    fonepole(pulse_lp_, pulse, 1.0f / kPulseFilterTime);
    pulse = Diode((pulse - pulse_lp_) + pulse * 0.044f);

    // Q41 / Q42
    float fm_pulse = 0.0f;
    if(fm_pulse_remaining_samples_)
    {
        --fm_pulse_remaining_samples_;
        fm_pulse = 1.0f;
        // C39 / C52
        retrig_pulse_ = fm_pulse_remaining_samples_ ? 0.0f : -0.8f;
    }
    else
    {
        // C39 / R161
        retrig_pulse_ *= 1.0f - 1.0f / kRetrigPulseDuration;
    }
    if(sustain_)
    {
        fm_pulse = 0.0f;
    }
    fonepole(fm_pulse_lp_, fm_pulse, 1.0f / kPulseFilterTime);

    // Q43 and R170 leakage
    float punch = 0.7f + Diode(10.0f * lp_out_ - 1.0f);

    // Q43 / R165
    float attack_fm = fm_pulse_lp_ * 1.7f * attack_fm_amount_;
    float self_fm   = punch * 0.08f * self_fm_amount_;
    float f         = f0_ * (1.0f + attack_fm + self_fm);
    f               = fclamp(f, 0.0f, 0.4f);

    float resonator_out;
    if(sustain_)
    {
        sustain_gain_ = accent_ * decay_;
        phase_ += f;
        phase_ = phase_ >= 1.f ? phase_ - 1.f : phase_;

        resonator_out = sin(TWOPI_F * phase_) * sustain_gain_;
        lp_out_       = cos(TWOPI_F * phase_) * sustain_gain_;
    }
    else
    {
        resonator_.SetFreq(f * sample_rate_);
        //resonator_.SetRes(1.0f + q * f);
        resonator_.SetRes(.4f * q * f);

        resonator_.Process((pulse - retrig_pulse_ * 0.2f) * scale);
        resonator_out = resonator_.Band();
        lp_out_       = resonator_.Low();
    }

    fonepole(tone_lp_, pulse * exciter_leak + resonator_out, tone_f);

    return tone_lp_;
}

void AnalogBassDrum::Trig()
{
    trig_ = true;
}

void AnalogBassDrum::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void AnalogBassDrum::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void AnalogBassDrum::SetFreq(float f0)
{
    f0 /= sample_rate_;
    f0_ = fclamp(f0, 0.f, .5f);
}

void AnalogBassDrum::SetTone(float tone)
{
    tone_ = fclamp(tone, 0.f, 1.f);
}

void AnalogBassDrum::SetDecay(float decay)
{
    decay_ = decay * .1f;
    decay_ -= .1f;
}

void AnalogBassDrum::SetAttackFmAmount(float attack_fm_amount)
{
    attack_fm_amount_ = attack_fm_amount * 50.f;
}

void AnalogBassDrum::SetSelfFmAmount(float self_fm_amount)
{
    self_fm_amount_ = self_fm_amount * 50.f;
}

static const int kNumModes = 5;

void AnalogSnareDrum::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    trig_ = false;

    pulse_remaining_samples_ = 0;
    pulse_                   = 0.0f;
    pulse_height_            = 0.0f;
    pulse_lp_                = 0.0f;
    noise_envelope_          = 0.0f;
    sustain_gain_            = 0.0f;

    SetSustain(false);
    SetAccent(.6f);
    SetFreq(200.f);
    SetDecay(.3f);
    SetSnappy(.7f);
    SetTone(.5f);

    for(int i = 0; i < kNumModes; ++i)
    {
        resonator_[i].Init(sample_rate_);
        phase_[i] = 0.f;
    }
    noise_filter_.Init(sample_rate_);
}

/** Trigger the drum */
void AnalogSnareDrum::Trig()
{
    trig_ = true;
}

void AnalogSnareDrum::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void AnalogSnareDrum::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void AnalogSnareDrum::SetFreq(float f0)
{
    f0  = f0 / sample_rate_;
    f0_ = fclamp(f0, 0.f, .4f);
}

void AnalogSnareDrum::SetTone(float tone)
{
    tone_ = fclamp(tone, 0.f, 1.f);
    tone_ *= 2.f;
}

void AnalogSnareDrum::SetDecay(float decay)
{
    decay_ = decay;
    return;
    decay_ = fmax(decay, 0.f);
}

void AnalogSnareDrum::SetSnappy(float snappy)
{
    snappy_ = fclamp(snappy, 0.f, 1.f);
}

float AnalogSnareDrum::Process(bool trigger)
{
    const float decay_xt = decay_ * (1.0f + decay_ * (decay_ - 1.0f));
    const int   kTriggerPulseDuration = 1.0e-3 * sample_rate_;
    const float kPulseDecayTime       = 0.1e-3 * sample_rate_;
    const float q = 2000.0f * powf(2.f, kOneTwelfth * decay_xt * 84.0f);
    const float noise_envelope_decay
        = 1.0f
          - 0.0017f
                * powf(2.f,
                       kOneTwelfth * (-decay_ * (50.0f + snappy_ * 10.0f)));
    const float exciter_leak = snappy_ * (2.0f - snappy_) * 0.1f;

    float snappy = snappy_ * 1.1f - 0.05f;
    snappy       = fclamp(snappy, 0.0f, 1.0f);

    float tone = tone_;

    if(trigger || trig_)
    {
        trig_                    = false;
        pulse_remaining_samples_ = kTriggerPulseDuration;
        pulse_height_            = 3.0f + 7.0f * accent_;
        noise_envelope_          = 2.0f;
    }

    static const float kModeFrequencies[kNumModes]
        = {1.00f, 2.00f, 3.18f, 4.16f, 5.62f};

    float f[kNumModes];
    float gain[kNumModes];

    for(int i = 0; i < kNumModes; ++i)
    {
        f[i] = fmin(f0_ * kModeFrequencies[i], 0.499f);
        resonator_[i].SetFreq(f[i] * sample_rate_);
        //        resonator_[i].SetRes(1.0f + f[i] * (i == 0 ? q : q * 0.25f));
        resonator_[i].SetRes((f[i] * (i == 0 ? q : q * 0.25f)) * .2);
    }

    if(tone < 0.666667f)
    {
        // 808-style (2 modes)
        tone *= 1.5f;
        gain[0] = 1.5f + (1.0f - tone) * (1.0f - tone) * 4.5f;
        gain[1] = 2.0f * tone + 0.15f;
        for(int i = 2; i < kNumModes; i++)
        {
            gain[i] = 0.f;
        }
    }
    else
    {
        // What the 808 could have been if there were extra modes!
        tone    = (tone - 0.666667f) * 3.0f;
        gain[0] = 1.5f - tone * 0.5f;
        gain[1] = 2.15f - tone * 0.7f;
        for(int i = 2; i < kNumModes; ++i)
        {
            gain[i] = tone;
            tone *= tone;
        }
    }

    float f_noise = f0_ * 16.0f;
    fclamp(f_noise, 0.0f, 0.499f);
    noise_filter_.SetFreq(f_noise * sample_rate_);
    //noise_filter_.SetRes(1.0f + f_noise * 1.5f);
    noise_filter_.SetRes(f_noise * 1.5f);

    // Q45 / Q46
    float pulse = 0.0f;
    if(pulse_remaining_samples_)
    {
        --pulse_remaining_samples_;
        pulse = pulse_remaining_samples_ ? pulse_height_ : pulse_height_ - 1.0f;
        pulse_ = pulse;
    }
    else
    {
        pulse_ *= 1.0f - 1.0f / kPulseDecayTime;
        pulse = pulse_;
    }

    float sustain_gain_value = sustain_gain_ = accent_ * decay_;

    // R189 / C57 / R190 + C58 / C59 / R197 / R196 / IC14
    pulse_lp_ = fclamp(pulse_lp_, pulse, 0.75f);

    float shell = 0.0f;
    for(int i = 0; i < kNumModes; ++i)
    {
        float excitation
            = i == 0 ? (pulse - pulse_lp_) + 0.006f * pulse : 0.026f * pulse;

        phase_[i] += f[i];
        phase_[i] = phase_[i] >= 1.f ? phase_[i] - 1.f : phase_[i];

        resonator_[i].Process(excitation);

        shell += gain[i]
                 * (sustain_
                        ? sin(phase_[i] * TWOPI_F) * sustain_gain_value * 0.25f
                        : resonator_[i].Band() + excitation * exciter_leak);
    }
    shell = SoftClip(shell);

    // C56 / R194 / Q48 / C54 / R188 / D54
    float noise = 2.0f * rand() * kRandFrac - 1.0f;
    if(noise < 0.0f)
        noise = 0.0f;
    noise_envelope_ *= noise_envelope_decay;
    noise *= (sustain_ ? sustain_gain_value : noise_envelope_) * snappy * 2.0f;

    // C66 / R201 / C67 / R202 / R203 / Q49
    noise_filter_.Process(noise);
    noise = noise_filter_.Band();

    // IC13
    return noise + shell * (1.0f - snappy);
}

inline float AnalogSnareDrum::SoftLimit(float x)
{
    return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
}

inline float AnalogSnareDrum::SoftClip(float x)
{
    if(x < -3.0f)
    {
        return -1.0f;
    }
    else if(x > 3.0f)
    {
        return 1.0f;
    }
    else
    {
        return SoftLimit(x);
    }
}

void SquareNoise::Init(float sample_rate)
{
    for(int i = 0; i < 6; i++)
    {
        phase_[i] = 0;
    }
}

float SquareNoise::Process(float f0)
{
    const float ratios[6] = {// Nominal f0: 414 Hz
                             1.0f,
                             1.304f,
                             1.466f,
                             1.787f,
                             1.932f,
                             2.536f};

    uint32_t increment[6];
    uint32_t phase[6];
    for(int i = 0; i < 6; ++i)
    {
        float f = f0 * ratios[i];
        if(f >= 0.499f)
            f = 0.499f;
        increment[i] = static_cast<uint32_t>(f * 4294967296.0f);
        phase[i]     = phase_[i];
    }

    phase[0] += increment[0];
    phase[1] += increment[1];
    phase[2] += increment[2];
    phase[3] += increment[3];
    phase[4] += increment[4];
    phase[5] += increment[5];
    uint32_t noise = 0;
    noise += (phase[0] >> 31);
    noise += (phase[1] >> 31);
    noise += (phase[2] >> 31);
    noise += (phase[3] >> 31);
    noise += (phase[4] >> 31);
    noise += (phase[5] >> 31);

    for(int i = 0; i < 6; ++i)
    {
        phase_[i] = phase[i];
    }

    return 0.33f * static_cast<float>(noise) - 1.0f;
}

void RingModNoise::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    for(int i = 0; i < 6; ++i)
    {
        oscillator_[i].Init(sample_rate_);
    }
}

float RingModNoise::Process(float f0)
{
    const float ratio = f0 / (0.01f + f0);
    const float f1a   = 200.0f / sample_rate_ * ratio;
    const float f1b   = 7530.0f / sample_rate_ * ratio;
    const float f2a   = 510.0f / sample_rate_ * ratio;
    const float f2b   = 8075.0f / sample_rate_ * ratio;
    const float f3a   = 730.0f / sample_rate_ * ratio;
    const float f3b   = 10500.0f / sample_rate_ * ratio;

    float out = ProcessPair(&oscillator_[0], f1a, f1b);
    out += ProcessPair(&oscillator_[2], f2a, f2b);
    out += ProcessPair(&oscillator_[4], f3a, f3b);

    return out;
}

float RingModNoise::ProcessPair(Oscillator* osc, float f1, float f2)
{
    osc[0].SetWaveform(Oscillator::WAVE_SQUARE);
    osc[0].SetFreq(f1 * sample_rate_);
    float temp_1 = osc[0].Process();

    osc[1].SetWaveform(Oscillator::WAVE_SAW);
    osc[1].SetFreq(f2 * sample_rate_);
    float temp_2 = osc[1].Process();

    return temp_1 * temp_2;
}

void SyntheticBassDrumClick::Init(float sample_rate)
{
    lp_ = 0.0f;
    hp_ = 0.0f;
    filter_.Init(sample_rate);
    filter_.SetFreq(5000.0f);
    filter_.SetRes(1.f); //2.f
}

float SyntheticBassDrumClick::Process(float in)
{
    //SLOPE(lp_, in, 0.5f, 0.1f);
    float error = in - lp_;
    lp_ += (error > 0 ? .5f : .1f) * error;

    fonepole(hp_, lp_, 0.04f);
    filter_.Process(lp_ - hp_);
    return filter_.Low();
}

void SyntheticBassDrumAttackNoise::Init()
{
    lp_ = 0.0f;
    hp_ = 0.0f;
}

float SyntheticBassDrumAttackNoise::Process()
{
    float sample = rand() * kRandFrac;
    fonepole(lp_, sample, 0.05f);
    fonepole(hp_, lp_, 0.005f);
    return lp_ - hp_;
}

void SyntheticBassDrum::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    trig_ = false;

    phase_                = 0.0f;
    phase_noise_          = 0.0f;
    f0_                   = 0.0f;
    fm_                   = 0.0f;
    fm_lp_                = 0.0f;
    body_env_lp_          = 0.0f;
    body_env_             = 0.0f;
    body_env_pulse_width_ = 0;
    fm_pulse_width_       = 0;
    tone_lp_              = 0.0f;
    sustain_gain_         = 0.0f;

    SetFreq(100.f);
    SetSustain(false);
    SetAccent(.2f);
    SetTone(.6f);
    SetDecay(.7f);
    SetDirtiness(.3f);
    SetFmEnvelopeAmount(.6);
    SetFmEnvelopeDecay(.3);

    click_.Init(sample_rate);
    noise_.Init();
}

inline float SyntheticBassDrum::DistortedSine(float phase,
                                              float phase_noise,
                                              float dirtiness)
{
    phase += phase_noise * dirtiness;

    //MAKE_INTEGRAL_FRACTIONAL(phase);
    int32_t phase_integral   = static_cast<int32_t>(phase);
    float   phase_fractional = phase - static_cast<float>(phase_integral);

    phase            = phase_fractional;
    float triangle   = (phase < 0.5f ? phase : 1.0f - phase) * 4.0f - 1.0f;
    float sine       = 2.0f * triangle / (1.0f + fabsf(triangle));
    float clean_sine = sinf(TWOPI_F * (phase + 0.75f));
    return sine + (1.0f - dirtiness) * (clean_sine - sine);
}

inline float SyntheticBassDrum::TransistorVCA(float s, float gain)
{
    s = (s - 0.6f) * gain;
    return 3.0f * s / (2.0f + fabsf(s)) + gain * 0.3f;
}

float SyntheticBassDrum::Process(bool trigger)
{
    float dirtiness = dirtiness_;
    dirtiness *= fmax(1.0f - 8.0f * new_f0_, 0.0f);

    const float fm_decay
        = 1.0f
          - 1.0f / (0.008f * (1.0f + fm_envelope_decay_ * 4.0f) * sample_rate_);

    const float body_env_decay
        = 1.0f
          - 1.0f / (0.02f * sample_rate_)
                * powf(2.f, (-decay_ * 60.0f) * kOneTwelfth);
    const float transient_env_decay = 1.0f - 1.0f / (0.005f * sample_rate_);
    const float tone_f              = fmin(
        4.0f * new_f0_ * powf(2.f, (tone_ * 108.0f) * kOneTwelfth), 1.0f);
    const float transient_level = tone_;

    if(trigger || trig_)
    {
        trig_     = false;
        fm_       = 1.0f;
        body_env_ = transient_env_ = 0.3f + 0.7f * accent_;
        body_env_pulse_width_      = sample_rate_ * 0.001f;
        fm_pulse_width_            = sample_rate_ * 0.0013f;
    }

    sustain_gain_ = accent_ * decay_;

    fonepole(phase_noise_, rand() * kRandFrac - 0.5f, 0.002f);

    float mix = 0.0f;

    if(sustain_)
    {
        f0_ = new_f0_;
        phase_ += f0_;
        if(phase_ >= 1.0f)
        {
            phase_ -= 1.0f;
        }
        float body = DistortedSine(phase_, phase_noise_, dirtiness);
        mix -= TransistorVCA(body, sustain_gain_);
    }
    else
    {
        if(fm_pulse_width_)
        {
            --fm_pulse_width_;
            phase_ = 0.25f;
        }
        else
        {
            fm_ *= fm_decay;
            float fm = 1.0f + fm_envelope_amount_ * 3.5f * fm_lp_;
            f0_      = new_f0_;
            phase_ += fmin(f0_ * fm, 0.5f);
            if(phase_ >= 1.0f)
            {
                phase_ -= 1.0f;
            }
        }

        if(body_env_pulse_width_)
        {
            --body_env_pulse_width_;
        }
        else
        {
            body_env_ *= body_env_decay;
            transient_env_ *= transient_env_decay;
        }

        const float envelope_lp_f = 0.1f;
        fonepole(body_env_lp_, body_env_, envelope_lp_f);
        fonepole(transient_env_lp_, transient_env_, envelope_lp_f);
        fonepole(fm_lp_, fm_, envelope_lp_f);

        float body      = DistortedSine(phase_, phase_noise_, dirtiness);
        float transient = click_.Process(body_env_pulse_width_ ? 0.0f : 1.0f)
                          + noise_.Process();

        mix -= TransistorVCA(body, body_env_lp_);
        mix -= transient * transient_env_lp_ * transient_level;
    }

    fonepole(tone_lp_, mix, tone_f);
    return tone_lp_;
}

void SyntheticBassDrum::Trig()
{
    trig_ = true;
}

void SyntheticBassDrum::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void SyntheticBassDrum::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void SyntheticBassDrum::SetFreq(float freq)
{
    freq /= sample_rate_;
    new_f0_ = fclamp(freq, 0.f, 1.f);
}

void SyntheticBassDrum::SetTone(float tone)
{
    tone_ = fclamp(tone, 0.f, 1.f);
}

void SyntheticBassDrum::SetDecay(float decay)
{
    decay  = fclamp(decay, 0.f, 1.f);
    decay_ = decay * decay;
}

void SyntheticBassDrum::SetDirtiness(float dirtiness)
{
    dirtiness_ = fclamp(dirtiness, 0.f, 1.f);
}

void SyntheticBassDrum::SetFmEnvelopeAmount(float fm_envelope_amount)
{
    fm_envelope_amount_ = fclamp(fm_envelope_amount, 0.f, 1.f);
}

void SyntheticBassDrum::SetFmEnvelopeDecay(float fm_envelope_decay)
{
    fm_envelope_decay  = fclamp(fm_envelope_decay, 0.f, 1.f);
    fm_envelope_decay_ = fm_envelope_decay * fm_envelope_decay;
}


void SyntheticSnareDrum::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    phase_[0]        = 0.0f;
    phase_[1]        = 0.0f;
    drum_amplitude_  = 0.0f;
    snare_amplitude_ = 0.0f;
    fm_              = 0.0f;
    hold_counter_    = 0;
    sustain_gain_    = 0.0f;

    SetSustain(false);
    SetAccent(.6f);
    SetFreq(200.f);
    SetFmAmount(.1f);
    SetDecay(.3f);
    SetSnappy(.7f);

    trig_ = false;

    drum_lp_.Init(sample_rate_);
    snare_hp_.Init(sample_rate_);
    snare_lp_.Init(sample_rate_);
}

inline float SyntheticSnareDrum::DistortedSine(float phase)
{
    float triangle = (phase < 0.5f ? phase : 1.0f - phase) * 4.0f - 1.3f;
    return 2.0f * triangle / (1.0f + fabsf(triangle));
}

bool even = true;

float SyntheticSnareDrum::Process(bool trigger)
{
    const float decay_xt = decay_ * (1.0f + decay_ * (decay_ - 1.0f));
    const float drum_decay
        = 1.0f
          - 1.0f / (0.015f * sample_rate_)
                * powf(2.f,
                       kOneTwelfth
                           * (-decay_xt * 72.0f - fm_amount_ * 12.0f
                              + snappy_ * 7.0f));

    const float snare_decay
        = 1.0f
          - 1.0f / (0.01f * sample_rate_)
                * powf(2.f, kOneTwelfth * (-decay_ * 60.0f - snappy_ * 7.0f));
    const float fm_decay = 1.0f - 1.0f / (0.007f * sample_rate_);

    float snappy = snappy_ * 1.1f - 0.05f;
    snappy       = fclamp(snappy, 0.0f, 1.0f);

    const float drum_level  = sqrtf(1.0f - snappy);
    const float snare_level = sqrtf(snappy);

    const float snare_f_min = fmin(10.0f * f0_, 0.5f);
    const float snare_f_max = fmin(35.0f * f0_, 0.5f);

    snare_hp_.SetFreq(snare_f_min * sample_rate_);
    snare_lp_.SetFreq(snare_f_max * sample_rate_);
    snare_lp_.SetRes(0.5f + 2.0f * snappy);

    drum_lp_.SetFreq(3.0f * f0_ * sample_rate_);

    if(trigger || trig_)
    {
        trig_            = false;
        snare_amplitude_ = drum_amplitude_ = 0.3f + 0.7f * accent_;
        fm_                                = 1.0f;
        phase_[0] = phase_[1] = 0.0f;
        hold_counter_
            = static_cast<int>((0.04f + decay_ * 0.03f) * sample_rate_);
    }

    even = !even;
    if(sustain_)
    {
        sustain_gain_ = snare_amplitude_ = accent_ * decay_;
        drum_amplitude_                  = snare_amplitude_;
        fm_                              = 0.0f;
    }
    else
    {
        // Compute all D envelopes.
        // The envelope for the drum has a very long tail.
        // The envelope for the snare has a "hold" stage which lasts between
        // 40 and 70 ms
        drum_amplitude_
            *= (drum_amplitude_ > 0.03f || even) ? drum_decay : 1.0f;
        if(hold_counter_)
        {
            --hold_counter_;
        }
        else
        {
            snare_amplitude_ *= snare_decay;
        }
        fm_ *= fm_decay;
    }

    // The 909 circuit has a funny kind of oscillator coupling - the signal
    // leaving Q40's collector and resetting all oscillators allow some
    // intermodulation.
    float reset_noise        = 0.0f;
    float reset_noise_amount = (0.125f - f0_) * 8.0f;
    reset_noise_amount       = fclamp(reset_noise_amount, 0.0f, 1.0f);
    reset_noise_amount *= reset_noise_amount;
    reset_noise_amount *= fm_amount_;
    reset_noise += phase_[0] > 0.5f ? -1.0f : 1.0f;
    reset_noise += phase_[1] > 0.5f ? -1.0f : 1.0f;
    reset_noise *= reset_noise_amount * 0.025f;

    float f = f0_ * (1.0f + fm_amount_ * (4.0f * fm_));
    phase_[0] += f;
    phase_[1] += f * 1.47f;
    if(reset_noise_amount > 0.1f)
    {
        if(phase_[0] >= 1.0f + reset_noise)
        {
            phase_[0] = 1.0f - phase_[0];
        }
        if(phase_[1] >= 1.0f + reset_noise)
        {
            phase_[1] = 1.0f - phase_[1];
        }
    }
    else
    {
        if(phase_[0] >= 1.0f)
        {
            phase_[0] -= 1.0f;
        }
        if(phase_[1] >= 1.0f)
        {
            phase_[1] -= 1.0f;
        }
    }

    float drum = -0.1f;
    drum += DistortedSine(phase_[0]) * 0.60f;
    drum += DistortedSine(phase_[1]) * 0.25f;
    drum *= drum_amplitude_ * drum_level;

    drum_lp_.Process(drum);
    drum = drum_lp_.Low();

    float noise = rand() * kRandFrac;
    snare_lp_.Process(noise);
    float snare = snare_lp_.Low();
    snare_hp_.Process(snare);
    snare = snare_hp_.High();
    snare = (snare + 0.1f) * (snare_amplitude_ + fm_) * snare_level;

    return snare + drum; // It's a snare, it's a drum, it's a snare drum.
}

void SyntheticSnareDrum::Trig()
{
    trig_ = true;
}

void SyntheticSnareDrum::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void SyntheticSnareDrum::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void SyntheticSnareDrum::SetFreq(float f0)
{
    f0 /= sample_rate_;
    f0_ = fclamp(f0, 0.f, 1.f);
}

void SyntheticSnareDrum::SetFmAmount(float fm_amount)
{
    fm_amount  = fclamp(fm_amount, 0.f, 1.f);
    fm_amount_ = fm_amount * fm_amount;
}

void SyntheticSnareDrum::SetDecay(float decay)
{
    decay_ = fmax(decay, 0.f);
}

void SyntheticSnareDrum::SetSnappy(float snappy)
{
    snappy_ = fclamp(snappy, 0.f, 1.f);
}




void Balance::Init(float sample_rate)
{
    float b;
    sample_rate_ = sample_rate;
    ihp_         = 10.0f;
    b            = 2.0f - cosf(ihp_ * (TWOPI_F / sample_rate_));
    c2_          = b - sqrtf(b * b - 1.0f);
    c1_          = 1.0f - c2_;
    prvq_ = prvr_ = prva_ = 0.0f;
}

float Balance::Process(float sig, float comp)
{
    float q, r, a, diff, out;
    float c1 = c1_;
    float c2 = c2_;

    q        = prvq_;
    r        = prvr_;
    float as = sig;
    float cs = comp;

    q = c1 * as * as + c2 * q;
    r = c1 * cs * cs + c2 * r;

    prvq_ = q;
    prvr_ = r;

    if(q != 0.0f)
    {
        a = sqrtf(r / q);
    }
    else
    {
        a = sqrtf(r);
    }

    if((diff = a - prva_) != 0.0f)
    {
        out = sig * prva_;
    }
    else
    {
        out = sig * a;
    }

    prva_ = a;

    return out;
}


void Compressor::Init(float sample_rate)
{
    sample_rate_      = min(192000, max(1, sample_rate));
    sample_rate_inv_  = 1.0f / (float)sample_rate_;
    sample_rate_inv2_ = 2.0f / (float)sample_rate_;

    // Initializing the params in this order to avoid dividing by zero

    SetRatio(2.0f);
    SetAttack(0.1f);
    SetRelease(0.1f);
    SetThreshold(-12.0f);
    AutoMakeup(true);

    gain_rec_  = 0.1f;
    slope_rec_ = 0.1f;
}

float Compressor::Process(float in)
{
    float inAbs   = fabsf(in);
    float cur_slo = ((slope_rec_ > inAbs) ? rel_slo_ : atk_slo_);
    slope_rec_    = ((slope_rec_ * cur_slo) + ((1.0f - cur_slo) * inAbs));
    gain_rec_     = ((atk_slo2_ * gain_rec_)
                 + (ratio_mul_
                    * fmax(((20.f * fastlog10f(slope_rec_)) - thresh_), 0.f)));
    gain_         = pow10f(0.05f * (gain_rec_ + makeup_gain_));

    return gain_ * in;
}

void Compressor::ProcessBlock(float *in, float *out, float *key, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        Process(key[i]);
        out[i] = Apply(in[i]);
    }
}

// Multi-channel block processing
void Compressor::ProcessBlock(float **in,
                              float **out,
                              float * key,
                              size_t  channels,
                              size_t  size)
{
    for(size_t i = 0; i < size; i++)
    {
        Process(key[i]);
        for(size_t c = 0; c < channels; c++)
        {
            out[c][i] = Apply(in[c][i]);
        }
    }
}


void Limiter::Init()
{
    peak_ = 0.5f;
}

void Limiter::ProcessBlock(float *in, size_t size, float pre_gain)
{
    while(size--)
    {
        float pre  = *in * pre_gain;
        float peak = fabsf(pre);
        SLOPE(peak_, peak, 0.05f, 0.00002f);
        float gain = (peak_ <= 1.0f ? 1.0f : 1.0f / peak_);
        *in++      = SoftLimit(pre * gain * 0.7f);
    }
}

void Autowah::Init(float sample_rate)
{
    sampling_freq_ = sample_rate;
    const1_        = 1413.72f / sampling_freq_;
    const2_        = expf(0.0f - (100.0f / sampling_freq_));
    const4_        = expf(0.0f - (10.0f / sampling_freq_));

    wet_dry_ = 100.0f;
    level_   = 0.1f;
    wah_     = 0.0;


    for(int i = 0; i < 2; i++)
    {
        rec1_[i] = rec2_[i] = rec3_[i] = rec4_[i] = rec5_[i] = 0.0f;
    }

    for(int i = 0; i < 3; i++)
    {
        rec0_[i] = 0.0f;
    }
}

float Autowah::Process(float in)
{
    float out;
    float fSlow2 = (0.01f * (wet_dry_ * level_));
    float fSlow3 = (1.0f - 0.01f * wet_dry_) + (1.f - wah_);

    float fTemp1 = fabs(in);
    rec3_[0]
        = fmaxf(fTemp1, (const4_ * rec3_[1]) + ((1.0f - const4_) * fTemp1));
    rec2_[0]     = (const2_ * rec2_[1]) + ((1.0f - const2_) * rec3_[0]);
    float fTemp2 = fminf(1.0f, rec2_[0]);
    float fTemp3 = powf(2.0f, (2.3f * fTemp2));
    float fTemp4
        = 1.0f
          - (const1_ * fTemp3 / powf(2.0f, (1.0f + 2.0f * (1.0f - fTemp2))));
    rec1_[0]
        = ((0.999f * rec1_[1])
           + (0.001f
              * (0.0f - (2.0f * (fTemp4 * cosf((const1_ * 2 * fTemp3)))))));
    rec4_[0] = ((0.999f * rec4_[1]) + (0.001f * fTemp4 * fTemp4));
    rec5_[0] = ((0.999f * rec5_[1]) + (0.0001f * powf(4.0f, fTemp2)));
    rec0_[0] = (0.0f
                - (((rec1_[0] * rec0_[1]) + (rec4_[0] * rec0_[2]))
                   - (fSlow2 * (rec5_[0] * in))));

    out      = ((wah_ * (rec0_[0] - rec0_[1])) + (fSlow3 * in));
    rec3_[1] = rec3_[0];
    rec2_[1] = rec2_[0];
    rec1_[1] = rec1_[0];
    rec4_[1] = rec4_[0];
    rec5_[1] = rec5_[0];
    rec0_[2] = rec0_[1];
    rec0_[1] = rec0_[0];

    return out;
}

//ChorusEngine stuff
void ChorusEngine::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    del_.Init();
    lfo_amp_  = 0.f;
    feedback_ = .2f;
    SetDelay(.75);

    lfo_phase_ = 0.f;
    SetLfoFreq(.3f);
    SetLfoDepth(.9f);
}

float ChorusEngine::Process(float in)
{
    float lfo_sig = ProcessLfo();
    del_.SetDelay(lfo_sig + delay_);

    float out = del_.Read();
    del_.Write(in + out * feedback_);

    return (in + out) * .5f; //equal mix
}

void ChorusEngine::SetLfoDepth(float depth)
{
    depth    = fclamp(depth, 0.f, .93f);
    lfo_amp_ = depth * delay_;
}

void ChorusEngine::SetLfoFreq(float freq)
{
    freq = 4.f * freq / sample_rate_;
    freq *= lfo_freq_ < 0.f ? -1.f : 1.f;  //if we're headed down, keep going
    lfo_freq_ = fclamp(freq, -.25f, .25f); //clip at +/- .125 * sr
}

void ChorusEngine::SetDelay(float delay)
{
    delay = (.1f + delay * 7.9f); //.1 to 8 ms
    SetDelayMs(delay);
}

void ChorusEngine::SetDelayMs(float ms)
{
    ms     = fmax(.1f, ms);
    delay_ = ms * .001f * sample_rate_; //ms to samples

    lfo_amp_ = fmin(lfo_amp_, delay_); //clip this if needed
}

void ChorusEngine::SetFeedback(float feedback)
{
    feedback_ = fclamp(feedback, 0.f, 1.f);
}

float ChorusEngine::ProcessLfo()
{
    lfo_phase_ += lfo_freq_;

    //wrap around and flip direction
    if(lfo_phase_ > 1.f)
    {
        lfo_phase_ = 1.f - (lfo_phase_ - 1.f);
        lfo_freq_ *= -1.f;
    }
    else if(lfo_phase_ < -1.f)
    {
        lfo_phase_ = -1.f - (lfo_phase_ + 1.f);
        lfo_freq_ *= -1.f;
    }

    return lfo_phase_ * lfo_amp_;
}

//Chorus Stuff
void Chorus::Init(float sample_rate)
{
    engines_[0].Init(sample_rate);
    engines_[1].Init(sample_rate);
    SetPan(.25f, .75f);

    gain_frac_ = .5f;
    sigl_ = sigr_ = 0.f;
}

float Chorus::Process(float in)
{
    sigl_ = 0.f;
    sigr_ = 0.f;

    for(int i = 0; i < 2; i++)
    {
        float sig = engines_[i].Process(in);
        sigl_ += (1.f - pan_[i]) * sig;
        sigr_ += pan_[i] * sig;
    }

    sigl_ *= gain_frac_;
    sigr_ *= gain_frac_;

    return sigl_;
}

float Chorus::GetLeft()
{
    return sigl_;
}

float Chorus::GetRight()
{
    return sigr_;
}

void Chorus::SetPan(float panl, float panr)
{
    pan_[0] = fclamp(panl, 0.f, 1.f);
    pan_[1] = fclamp(panr, 0.f, 1.f);
}

void Chorus::SetPan(float pan)
{
    SetPan(pan, pan);
}

void Chorus::SetLfoDepth(float depthl, float depthr)
{
    engines_[0].SetLfoDepth(depthl);
    engines_[1].SetLfoDepth(depthr);
}

void Chorus::SetLfoDepth(float depth)
{
    SetLfoDepth(depth, depth);
}

void Chorus::SetLfoFreq(float freql, float freqr)
{
    engines_[0].SetLfoFreq(freql);
    engines_[1].SetLfoFreq(freqr);
}

void Chorus::SetLfoFreq(float freq)
{
    SetLfoFreq(freq, freq);
}

void Chorus::SetDelay(float delayl, float delayr)
{
    engines_[0].SetDelay(delayl);
    engines_[1].SetDelay(delayr);
}

void Chorus::SetDelay(float delay)
{
    SetDelay(delay, delay);
}

void Chorus::SetDelayMs(float msl, float msr)
{
    engines_[0].SetDelayMs(msl);
    engines_[1].SetDelayMs(msr);
}

void Chorus::SetDelayMs(float ms)
{
    SetDelayMs(ms, ms);
}

void Chorus::SetFeedback(float feedbackl, float feedbackr)
{
    engines_[0].SetFeedback(feedbackl);
    engines_[1].SetFeedback(feedbackr);
}

void Chorus::SetFeedback(float feedback)
{
    SetFeedback(feedback, feedback);
}


void Decimator::Init()
{
    downsample_factor_ = 1.0f;
    bitcrush_factor_   = 0.0f;
    downsampled_       = 0.0f;
    bitcrushed_        = 0.0f;
    inc_               = 0;
    threshold_         = 0;
}

float Decimator::Process(float input)
{
    int32_t temp;
    //downsample
    threshold_ = (uint32_t)((downsample_factor_ * downsample_factor_) * 96.0f);
    inc_ += 1;
    if(inc_ > threshold_)
    {
        inc_         = 0;
        downsampled_ = input;
    }
    //bitcrush
    temp = (int32_t)(downsampled_ * 65536.0f);
    temp >>= bits_to_crush_; // shift off
    temp <<= bits_to_crush_; // move back with zeros
    bitcrushed_ = (float)temp / 65536.0f;
    return bitcrushed_;
}


void Flanger::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    SetFeedback(.2f);

    del_.Init();
    lfo_amp_ = 0.f;
    SetDelay(.75);

    lfo_phase_ = 0.f;
    SetLfoFreq(.3);
    SetLfoDepth(.9);
}

float Flanger::Process(float in)
{
    float lfo_sig = ProcessLfo();
    del_.SetDelay(1.f + lfo_sig + delay_);

    float out = del_.Read();
    del_.Write(in + out * feedback_);

    return (in + out) * .5f; //equal mix
}

void Flanger::SetFeedback(float feedback)
{
    feedback_ = fclamp(feedback, 0.f, 1.f);
    feedback_ *= .97f;
}

void Flanger::SetLfoDepth(float depth)
{
    depth    = fclamp(depth, 0.f, .93f);
    lfo_amp_ = depth * delay_;
}

void Flanger::SetLfoFreq(float freq)
{
    freq = 4.f * freq / sample_rate_;
    freq *= lfo_freq_ < 0.f ? -1.f : 1.f;  //if we're headed down, keep going
    lfo_freq_ = fclamp(freq, -.25f, .25f); //clip at +/- .125 * sr
}

void Flanger::SetDelay(float delay)
{
    delay = (.1f + delay * 6.9); //.1 to 7 ms
    SetDelayMs(delay);
}

void Flanger::SetDelayMs(float ms)
{
    ms     = fmax(.1, ms);
    delay_ = ms * .001f * sample_rate_; //ms to samples

    lfo_amp_ = fmin(lfo_amp_, delay_); //clip this if needed
}

float Flanger::ProcessLfo()
{
    lfo_phase_ += lfo_freq_;

    //wrap around and flip direction
    if(lfo_phase_ > 1.f)
    {
        lfo_phase_ = 1.f - (lfo_phase_ - 1.f);
        lfo_freq_ *= -1.f;
    }
    else if(lfo_phase_ < -1.f)
    {
        lfo_phase_ = -1.f - (lfo_phase_ + 1.f);
        lfo_freq_ *= -1.f;
    }

    return lfo_phase_ * lfo_amp_;
}


void Fold::Init()
{
    incr_         = 1000.f;
    sample_index_ = 0;
    index_        = 0.0f;
    value_        = 0.0f;
}

float Fold::Process(float in)
{
    float out;

    if(index_ < sample_index_)
    {
        index_ += incr_;
        out = value_ = in;
    }
    else
    {
        out = value_;
    }

    sample_index_++;
    return out;
}

static Fold fold;

void Bitcrush::Init(float sample_rate)
{
    bit_depth_   = 8;
    crush_rate_  = 10000;
    sample_rate_ = sample_rate;
    fold.Init();
}

float Bitcrush::Process(float in)
{
    float bits    = pow(2, bit_depth_);
    float foldamt = sample_rate_ / crush_rate_;
    float out;

    out = in * 65536.0f;
    out += 32768;
    out *= (bits / 65536.0f);
    out = floor(out);
    out *= (65536.0f / bits) - 32768;

    fold.SetIncrement(foldamt);
    out = fold.Process(out);
    out /= 65536.0;

    return out;
}


void Overdrive::Init()
{
    SetDrive(.5f);
}

float Overdrive::Process(float in)
{
    float pre = pre_gain_ * in;
    return SoftClip(pre) * post_gain_;
}

void Overdrive::SetDrive(float drive)
{
    drive  = fclamp(drive, 0.f, 1.f);
    drive_ = 2.f * drive;

    const float drive_2    = drive_ * drive_;
    const float pre_gain_a = drive_ * 0.5f;
    const float pre_gain_b = drive_2 * drive_2 * drive_ * 24.0f;
    pre_gain_              = pre_gain_a + (pre_gain_b - pre_gain_a) * drive_2;

    const float drive_squashed = drive_ * (2.0f - drive_);
    post_gain_ = 1.0f / SoftClip(0.33f + drive_squashed * (pre_gain_ - 0.33f));
}


//PhaserEngine stuff
void PhaserEngine::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    del_.Init();
    lfo_amp_  = 0.f;
    feedback_ = .2f;
    SetFreq(200.f);

    del_.SetDelay(0.f);

    os_ = 30.f; //30 hertz frequency offset, lower than this introduces crunch
    deltime_ = 0.f;

    last_sample_ = 0.f;
    lfo_phase_   = 0.f;
    SetLfoFreq(.3);
    SetLfoDepth(.9);
}

float PhaserEngine::Process(float in)
{
    float lfo_sig = ProcessLfo();
    fonepole(deltime_, sample_rate_ / (lfo_sig + ap_freq_ + os_), .0001f);

    last_sample_ = del_.Allpass(in + feedback_ * last_sample_, deltime_, .3f);

    return (in + last_sample_) * .5f; //equal mix
}

void PhaserEngine::SetLfoDepth(float depth)
{
    lfo_amp_ = fclamp(depth, 0.f, 1.f);
}

void PhaserEngine::SetLfoFreq(float lfo_freq)
{
    lfo_freq = 4.f * lfo_freq / sample_rate_;
    lfo_freq *= lfo_freq_ < 0.f ? -1.f : 1.f; //if we're headed down, keep going
    lfo_freq_ = fclamp(lfo_freq, -.25f, .25f); //clip at +/- .125 * sr
}

void PhaserEngine::SetFreq(float ap_freq)
{
    ap_freq_ = fclamp(ap_freq, 0.f, 20000.f); //0 - 20kHz
}

void PhaserEngine::SetFeedback(float feedback)
{
    feedback_ = fclamp(feedback, 0.f, .75f);
}

float PhaserEngine::ProcessLfo()
{
    lfo_phase_ += lfo_freq_;

    //wrap around and flip direction
    if(lfo_phase_ > 1.f)
    {
        lfo_phase_ = 1.f - (lfo_phase_ - 1.f);
        lfo_freq_ *= -1.f;
    }
    else if(lfo_phase_ < -1.f)
    {
        lfo_phase_ = -1.f - (lfo_phase_ + 1.f);
        lfo_freq_ *= -1.f;
    }

    return lfo_phase_ * lfo_amp_ * ap_freq_;
}

//Phaser Stuff
void Phaser::Init(float sample_rate)
{
    for(size_t i = 0; i < kMaxPoles; i++)
    {
        engines_[i].Init(sample_rate);
    }

    poles_     = 4;
    gain_frac_ = .5f;
}

float Phaser::Process(float in)
{
    float sig = 0.f;

    for(int i = 0; i < poles_; i++)
    {
        sig += engines_[i].Process(in);
    }

    return sig;
}

void Phaser::SetPoles(int poles)
{
    poles_ = DSY_CLAMP(poles, 1, 8);
}

void Phaser::SetLfoDepth(float depth)
{
    for(int i = 0; i < kMaxPoles; i++)
    {
        engines_[i].SetLfoDepth(depth);
    }
}

void Phaser::SetLfoFreq(float lfo_freq)
{
    for(int i = 0; i < kMaxPoles; i++)
    {
        engines_[i].SetLfoFreq(lfo_freq);
    }
}

void Phaser::SetFreq(float ap_freq)
{
    for(int i = 0; i < kMaxPoles; i++)
    {
        engines_[i].SetFreq(ap_freq);
    }
}

void Phaser::SetFeedback(float feedback)
{
    for(int i = 0; i < kMaxPoles; i++)
    {
        engines_[i].SetFeedback(feedback);
    }
}



/* kReverbParams[n][0] = delay time (in seconds)                     */
/* kReverbParams[n][1] = random variation in delay time (in seconds) */
/* kReverbParams[n][2] = random variation frequency (in 1/sec)       */
/* kReverbParams[n][3] = random seed (0 - 32767)                     */

static const float kReverbParams[8][4]
    = {{(2473.0 / DEFAULT_SRATE), 0.0010, 3.100, 1966.0},
       {(2767.0 / DEFAULT_SRATE), 0.0011, 3.500, 29491.0},
       {(3217.0 / DEFAULT_SRATE), 0.0017, 1.110, 22937.0},
       {(3557.0 / DEFAULT_SRATE), 0.0006, 3.973, 9830.0},
       {(3907.0 / DEFAULT_SRATE), 0.0010, 2.341, 20643.0},
       {(4127.0 / DEFAULT_SRATE), 0.0011, 1.897, 22937.0},
       {(2143.0 / DEFAULT_SRATE), 0.0017, 0.891, 29491.0},
       {(1933.0 / DEFAULT_SRATE), 0.0006, 3.221, 14417.0}};

static int DelayLineMaxSamples(float sr, float i_pitch_mod, int n);
//static int InitDelayLine(dsy_reverbsc_dl *lp, int n);
static int         DelayLineBytesAlloc(float sr, float i_pitch_mod, int n);
static const float kOutputGain = 0.35;
static const float kJpScale    = 0.25;

int ReverbSc::Init(float sr)
{
    i_sample_rate_ = sr;
    sample_rate_   = sr;
    feedback_      = 0.97;
    lpfreq_        = 10000;
    i_pitch_mod_   = 1;
    i_skip_init_   = 0;
    damp_fact_     = 1.0;
    prv_lpfreq_    = 0.0;
    init_done_     = 1;
    int i, n_bytes = 0;
    n_bytes = 0;
    for(i = 0; i < 8; i++)
    {
        if(n_bytes > DSY_REVERBSC_MAX_SIZE)
            return 1;
        delay_lines_[i].buf = (aux_) + n_bytes;
        InitDelayLine(&delay_lines_[i], i);
        n_bytes += DelayLineBytesAlloc(sr, 1, i);
    }
    return 0;
}

static int DelayLineMaxSamples(float sr, float i_pitch_mod, int n)
{
    float max_del;

    max_del = kReverbParams[n][0];
    max_del += (kReverbParams[n][1] * (float)i_pitch_mod * 1.125);
    return (int)(max_del * sr + 16.5);
}

static int DelayLineBytesAlloc(float sr, float i_pitch_mod, int n)
{
    int n_bytes = 0;

    n_bytes += (DelayLineMaxSamples(sr, i_pitch_mod, n) * (int)sizeof(float));
    return n_bytes;
}

void ReverbSc::NextRandomLineseg(ReverbScDl *lp, int n)
{
    float prv_del, nxt_del, phs_inc_val;

    /* update random seed */
    if(lp->seed_val < 0)
        lp->seed_val += 0x10000;
    lp->seed_val = (lp->seed_val * 15625 + 1) & 0xFFFF;
    if(lp->seed_val >= 0x8000)
        lp->seed_val -= 0x10000;
    /* length of next segment in samples */
    lp->rand_line_cnt = (int)((sample_rate_ / kReverbParams[n][2]) + 0.5);
    prv_del           = (float)lp->write_pos;
    prv_del -= ((float)lp->read_pos
                + ((float)lp->read_pos_frac / (float)DELAYPOS_SCALE));
    while(prv_del < 0.0)
        prv_del += lp->buffer_size;
    prv_del = prv_del / sample_rate_; /* previous delay time in seconds */
    nxt_del = (float)lp->seed_val * kReverbParams[n][1] / 32768.0;
    /* next delay time in seconds */
    nxt_del = kReverbParams[n][0] + (nxt_del * (float)i_pitch_mod_);
    /* calculate phase increment per sample */
    phs_inc_val           = (prv_del - nxt_del) / (float)lp->rand_line_cnt;
    phs_inc_val           = phs_inc_val * sample_rate_ + 1.0;
    lp->read_pos_frac_inc = (int)(phs_inc_val * DELAYPOS_SCALE + 0.5);
}

int ReverbSc::InitDelayLine(ReverbScDl *lp, int n)
{
    float read_pos;
    /* int     i; */

    /* calculate length of delay line */
    lp->buffer_size = DelayLineMaxSamples(sample_rate_, 1, n);
    lp->dummy       = 0;
    lp->write_pos   = 0;
    /* set random seed */
    lp->seed_val = (int)(kReverbParams[n][3] + 0.5);
    /* set initial delay time */
    read_pos     = (float)lp->seed_val * kReverbParams[n][1] / 32768;
    read_pos     = kReverbParams[n][0] + (read_pos * (float)i_pitch_mod_);
    read_pos     = (float)lp->buffer_size - (read_pos * sample_rate_);
    lp->read_pos = (int)read_pos;
    read_pos     = (read_pos - (float)lp->read_pos) * (float)DELAYPOS_SCALE;
    lp->read_pos_frac = (int)(read_pos + 0.5);
    /* initialise first random line segment */
    NextRandomLineseg(lp, n);
    /* clear delay line to zero */
    lp->filter_state = 0.0;
    for(int i = 0; i < lp->buffer_size; i++)
    {
        lp->buf[i] = 0;
    }
    return REVSC_OK;
}

int ReverbSc::Process(const float &in1,
                      const float &in2,
                      float *      out1,
                      float *      out2)
{
    float       a_in_l, a_in_r, a_out_l, a_out_r;
    float       vm1, v0, v1, v2, am1, a0, a1, a2, frac;
    ReverbScDl *lp;
    int         read_pos;
    uint32_t    n;
    int         buffer_size; /* Local copy */
    float       damp_fact = damp_fact_;

    //if (init_done_ <= 0) return REVSC_NOT_OK;
    if(init_done_ <= 0)
        return REVSC_NOT_OK;

    /* calculate tone filter coefficient if frequency changed */
    if(lpfreq_ != prv_lpfreq_)
    {
        prv_lpfreq_ = lpfreq_;
        damp_fact
            = 2.0f - cosf(prv_lpfreq_ * (2.0f * (float)M_PI) / sample_rate_);
        damp_fact = damp_fact_
            = damp_fact - sqrtf(damp_fact * damp_fact - 1.0f);
    }

    /* calculate "resultant junction pressure" and mix to input signals */

    a_in_l = a_out_l = a_out_r = 0.0;
    for(n = 0; n < 8; n++)
    {
        a_in_l += delay_lines_[n].filter_state;
    }
    a_in_l *= kJpScale;
    a_in_r = a_in_l + in2;
    a_in_l = a_in_l + in1;

    /* loop through all delay lines */

    for(n = 0; n < 8; n++)
    {
        lp          = &delay_lines_[n];
        buffer_size = lp->buffer_size;

        /* send input signal and feedback to delay line */

        lp->buf[lp->write_pos]
            = (float)((n & 1 ? a_in_r : a_in_l) - lp->filter_state);
        if(++lp->write_pos >= buffer_size)
        {
            lp->write_pos -= buffer_size;
        }

        /* read from delay line with cubic interpolation */

        if(lp->read_pos_frac >= DELAYPOS_SCALE)
        {
            lp->read_pos += (lp->read_pos_frac >> DELAYPOS_SHIFT);
            lp->read_pos_frac &= DELAYPOS_MASK;
        }
        if(lp->read_pos >= buffer_size)
            lp->read_pos -= buffer_size;
        read_pos = lp->read_pos;
        frac     = (float)lp->read_pos_frac * (1.0 / (float)DELAYPOS_SCALE);

        /* calculate interpolation coefficients */

        a2 = frac * frac;
        a2 -= 1.0;
        a2 *= (1.0 / 6.0);
        a1 = frac;
        a1 += 1.0;
        a1 *= 0.5;
        am1 = a1 - 1.0;
        a0  = 3.0 * a2;
        a1 -= a0;
        am1 -= a2;
        a0 -= frac;

        /* read four samples for interpolation */

        if(read_pos > 0 && read_pos < (buffer_size - 2))
        {
            vm1 = (float)(lp->buf[read_pos - 1]);
            v0  = (float)(lp->buf[read_pos]);
            v1  = (float)(lp->buf[read_pos + 1]);
            v2  = (float)(lp->buf[read_pos + 2]);
        }
        else
        {
            /* at buffer wrap-around, need to check index */

            if(--read_pos < 0)
                read_pos += buffer_size;
            vm1 = (float)lp->buf[read_pos];
            if(++read_pos >= buffer_size)
                read_pos -= buffer_size;
            v0 = (float)lp->buf[read_pos];
            if(++read_pos >= buffer_size)
                read_pos -= buffer_size;
            v1 = (float)lp->buf[read_pos];
            if(++read_pos >= buffer_size)
                read_pos -= buffer_size;
            v2 = (float)lp->buf[read_pos];
        }
        v0 = (am1 * vm1 + a0 * v0 + a1 * v1 + a2 * v2) * frac + v0;

        /* update buffer read position */

        lp->read_pos_frac += lp->read_pos_frac_inc;

        /* apply feedback gain and lowpass filter */

        v0 *= (float)feedback_;
        v0               = (lp->filter_state - v0) * damp_fact + v0;
        lp->filter_state = v0;

        /* mix to output */

        if(n & 1)
        {
            a_out_r += v0;
        }
        else
        {
            a_out_l += v0;
        }

        /* start next random line segment if current one has reached endpoint */

        if(--(lp->rand_line_cnt) <= 0)
        {
            NextRandomLineseg(lp, n);
        }
    }
    /* someday, use a_out_r for multimono out */

    *out1 = a_out_l * kOutputGain;
    *out2 = a_out_r * kOutputGain;
    return REVSC_OK;
}


void SampleRateReducer::Init()
{
    frequency_       = .2f;
    phase_           = 0.0f;
    sample_          = 0.0f;
    next_sample_     = 0.0f;
    previous_sample_ = 0.0f;
}

float SampleRateReducer::Process(float in)
{
    float this_sample = next_sample_;
    next_sample_      = 0.f;
    phase_ += frequency_;
    if(phase_ >= 1.0f)
    {
        phase_ -= 1.0f;
        float t = phase_ / frequency_;
        // t = 0: the transition occurred right at this sample.
        // t = 1: the transition occurred at the previous sample.
        // Use linear interpolation to recover the fractional sample.
        float new_sample
            = previous_sample_ + (in - previous_sample_) * (1.0f - t);
        float discontinuity = new_sample - sample_;
        this_sample += discontinuity * ThisBlepSample(t);
        next_sample_ = discontinuity * NextBlepSample(t);
        sample_      = new_sample;
    }
    next_sample_ += sample_;
    previous_sample_ = in;

    return this_sample;
}

void SampleRateReducer::SetFreq(float frequency)
{
    frequency_ = fclamp(frequency, 0.f, 1.f);
}


void Tremolo::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    osc_.Init(sample_rate_);
    SetDepth(1.f);
    SetFreq(1.f);
}

float Tremolo::Process(float in)
{
    float modsig = dc_os_ + osc_.Process();
    return in * modsig;
}

void Tremolo::SetFreq(float freq)
{
    osc_.SetFreq(freq);
}

void Tremolo::SetWaveform(int waveform)
{
    osc_.SetWaveform(waveform);
}
void Tremolo::SetDepth(float depth)
{
    depth = fclamp(depth, 0.f, 1.f);
    depth *= .5f;
    osc_.SetAmp(depth);
    dc_os_ = 1.f - depth;
}

void ClockedNoise::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    phase_       = 0.0f;
    sample_      = 0.0f;
    next_sample_ = 0.0f;
    frequency_   = 0.001f;
}

float ClockedNoise::Process()
{
    float next_sample = next_sample_;
    float sample      = sample_;

    float this_sample = next_sample;
    next_sample       = 0.0f;

    const float raw_sample = rand() * kRandFrac * 2.0f - 1.0f;
    float       raw_amount = 4.0f * (frequency_ - 0.25f);
    raw_amount             = fclamp(raw_amount, 0.0f, 1.0f);

    phase_ += frequency_;

    if(phase_ >= 1.0f)
    {
        phase_ -= 1.0f;
        float t             = phase_ / frequency_;
        float new_sample    = raw_sample;
        float discontinuity = new_sample - sample;
        this_sample += discontinuity * ThisBlepSample(t);
        next_sample += discontinuity * NextBlepSample(t);
        sample = new_sample;
    }

    next_sample += sample;
    next_sample_ = next_sample;
    sample_      = sample;

    return this_sample + raw_amount * (raw_sample - this_sample);
}

void ClockedNoise::SetFreq(float freq)
{
    freq       = freq / sample_rate_;
    freq       = fclamp(freq, 0.0f, 1.0f);
    frequency_ = freq;
}

void ClockedNoise::Sync()
{
    phase_ = 1.0f;
}

void GrainletOscillator::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    carrier_phase_ = 0.0f;
    formant_phase_ = 0.0f;
    next_sample_   = 0.0f;

    carrier_shape_ = 0.f;
    carrier_bleed_ = 0.f;

    SetFreq(440.f);
    SetFormantFreq(220.f);
    SetShape(.5f);
    SetBleed(.5f);
}

float GrainletOscillator::Process()
{
    float this_sample = next_sample_;
    float next_sample = 0.0f;

    carrier_phase_ += carrier_frequency_;

    if(carrier_phase_ >= 1.0f)
    {
        carrier_phase_ -= 1.0f;
        float reset_time = carrier_phase_ / carrier_frequency_;

        float shape_inc = new_carrier_shape_ - carrier_shape_;
        float bleed_inc = new_carrier_bleed_ - carrier_bleed_;

        float before = Grainlet(
            1.0f,
            formant_phase_ + (1.0f - reset_time) * formant_frequency_,
            new_carrier_shape_ + shape_inc * (1.0f - reset_time),
            new_carrier_bleed_ + bleed_inc * (1.0f - reset_time));

        float after
            = Grainlet(0.0f, 0.0f, new_carrier_shape_, new_carrier_bleed_);

        float discontinuity = after - before;
        this_sample += discontinuity * ThisBlepSample(reset_time);
        next_sample += discontinuity * NextBlepSample(reset_time);
        formant_phase_ = reset_time * formant_frequency_;
    }
    else
    {
        formant_phase_ += formant_frequency_;
        if(formant_phase_ >= 1.0f)
        {
            formant_phase_ -= 1.0f;
        }
    }

    carrier_bleed_ = new_carrier_bleed_;
    carrier_shape_ = new_carrier_shape_;
    next_sample += Grainlet(
        carrier_phase_, formant_phase_, carrier_shape_, carrier_bleed_);
    next_sample_ = next_sample;
    return this_sample;
}

void GrainletOscillator::SetFreq(float freq)
{
    carrier_frequency_ = freq / sample_rate_;
    carrier_frequency_ = carrier_frequency_ > 0.5f ? 0.5f : carrier_frequency_;
}

void GrainletOscillator::SetFormantFreq(float freq)
{
    formant_frequency_ = freq / sample_rate_;
    formant_frequency_ = formant_frequency_ > 0.5f ? 0.5f : formant_frequency_;
}

void GrainletOscillator::SetShape(float shape)
{
    new_carrier_shape_ = shape;
}

void GrainletOscillator::SetBleed(float bleed)
{
    new_carrier_bleed_ = bleed;
}


float GrainletOscillator::Sine(float phase)
{
    return sinf(phase * TWOPI_F);
}

float GrainletOscillator::Carrier(float phase, float shape)
{
    shape *= 3.0f;
    int   shape_integral   = static_cast<int>(shape);
    float shape_fractional = shape - static_cast<float>(shape_integral);

    float t = 1.0f - shape_fractional;

    if(shape_integral == 0)
    {
        phase = phase * (1.0f + t * t * t * 15.0f);
        if(phase >= 1.0f)
        {
            phase = 1.0f;
        }
        phase += 0.75f;
    }
    else if(shape_integral == 1)
    {
        float breakpoint = 0.001f + 0.499f * t * t * t;
        if(phase < breakpoint)
        {
            phase *= (0.5f / breakpoint);
        }
        else
        {
            phase = 0.5f + (phase - breakpoint) * 0.5f / (1.0f - breakpoint);
        }
        phase += 0.75f;
    }
    else
    {
        t     = 1.0f - t;
        phase = 0.25f + phase * (0.5f + t * t * t * 14.5f);
        if(phase >= 0.75f)
            phase = 0.75f;
    }
    return (Sine(phase) + 1.0f) * 0.25f;
}

float GrainletOscillator::Grainlet(float carrier_phase,
                                   float formant_phase,
                                   float shape,
                                   float bleed)
{
    float carrier = Carrier(carrier_phase, shape);
    float formant = Sine(formant_phase);
    return carrier * (formant + bleed) / (1.0f + bleed);
}

void Particle::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    sync_ = false;
    aux_  = 0.f;
    SetFreq(440.f);
    resonance_ = .9f;
    density_   = .5f;
    gain_      = 1.f;
    spread_    = 1.f;

    SetRandomFreq(sample_rate_ / 48.f); //48 is the default block size
    rand_phase_ = 0.f;

    pre_gain_ = 0.0f;
    filter_.Init(sample_rate_);
    filter_.SetDrive(.7f);
}

float Particle::Process()
{
    float u = rand() * kRandFrac;
    float s = 0.0f;

    if(u <= density_ || sync_)
    {
        s = u <= density_ ? u * gain_ : s;
        rand_phase_ += rand_freq_;

        if(rand_phase_ >= 1.f || sync_)
        {
            rand_phase_ = rand_phase_ >= 1.f ? rand_phase_ - 1.f : rand_phase_;

            const float u = 2.0f * rand() * kRandFrac - 1.0f;
            const float f
                = fmin(powf(2.f, kRatioFrac * spread_ * u) * frequency_, .25f);
            pre_gain_ = 0.5f / sqrtf(resonance_ * f * sqrtf(density_));
            filter_.SetFreq(f * sample_rate_);
            filter_.SetRes(resonance_);
        }
    }
    aux_ = s;

    filter_.Process(pre_gain_ * s);
    return filter_.Band();
}

float Particle::GetNoise()
{
    return aux_;
}

void Particle::SetFreq(float freq)
{
    freq /= sample_rate_;
    frequency_ = fclamp(freq, 0.f, 1.f);
}

void Particle::SetResonance(float resonance)
{
    resonance_ = fclamp(resonance, 0.f, 1.f);
}

void Particle::SetRandomFreq(float freq)
{
    freq /= sample_rate_;
    rand_freq_ = fclamp(freq, 0.f, 1.f);
}

void Particle::SetDensity(float density)
{
    density_ = fclamp(density * .3f, 0.f, 1.f);
}

void Particle::SetGain(float gain)
{
    gain_ = fclamp(gain, 0.f, 1.f);
}

void Particle::SetSpread(float spread)
{
    spread_ = spread < 0.f ? 0.f : spread;
}

void Particle::SetSync(bool sync)
{
    sync_ = sync;
}

int Drip::my_random(int max)
{
    return (rand() % (max + 1));
}

float Drip::noise_tick()
{
    float temp;
    temp = 1.0f * rand() - 1073741823.5f;
    return temp * (1.0f / 1073741823.0f);
}

void Drip::Init(float sample_rate, float dettack)
{
    sample_rate_ = sample_rate;
    float temp;
    dettack_   = dettack;
    num_tubes_ = 10;
    damp_      = 0.2f;
    shake_max_ = 0.0f;
    freq_      = 450.0f;
    freq1_     = 600.0f;
    freq2_     = 720.0f;
    amp_       = 0.3f;

    snd_level_   = 0.0;
    float tpidsr = 2.0 * PI_F / sample_rate_;

    kloop_     = (sample_rate_ * dettack_);
    outputs00_ = 0.0f;
    outputs01_ = 0.0f;
    outputs10_ = 0.0f;
    outputs11_ = 0.0f;
    outputs20_ = 0.0f;
    outputs21_ = 0.0f;

    total_energy_ = 0.0f;

    center_freqs0_ = res_freq0_ = WUTR_CENTER_FREQ0;
    center_freqs1_ = res_freq1_ = WUTR_CENTER_FREQ1;
    center_freqs2_ = res_freq2_ = WUTR_CENTER_FREQ2;
    num_objects_save_ = num_objects_ = WUTR_NUM_SOURCES;
    sound_decay_                     = WUTR_SOUND_DECAY;
    system_decay_                    = WUTR_SYSTEM_DECAY;
    temp    = logf(WUTR_NUM_SOURCES) * WUTR_GAIN / WUTR_NUM_SOURCES;
    gains0_ = gains1_ = gains2_ = temp;
    coeffs01_                   = WUTR_RESON * WUTR_RESON;
    coeffs00_ = -WUTR_RESON * 2.0f * cosf(WUTR_CENTER_FREQ0 * tpidsr);
    coeffs11_ = WUTR_RESON * WUTR_RESON;
    coeffs10_ = -WUTR_RESON * 2.0f * cosf(WUTR_CENTER_FREQ1 * tpidsr);
    coeffs21_ = WUTR_RESON * WUTR_RESON;
    coeffs20_ = -WUTR_RESON * 2.0f * cosf(WUTR_CENTER_FREQ2 * tpidsr);

    shake_energy_ = amp_ * 1.0f * MAX_SHAKE * 0.1f;
    shake_damp_   = 0.0f;
    if(shake_energy_ > MAX_SHAKE)
        shake_energy_ = MAX_SHAKE;
    shake_max_save_ = 0.0f;
    num_objects_    = 10.0f;
    finalZ0_ = finalZ1_ = finalZ2_ = 0.0f;
}

float Drip::Process(bool trig)
{
    float data;
    float lastOutput;

    float tpidsr = 2.0f * PI_F / sample_rate_;

    if(trig)
    {
        Init(sample_rate_, dettack_);
    }
    if(num_tubes_ != 0.0f && num_tubes_ != num_objects_)
    {
        num_objects_ = num_tubes_;
        if(num_objects_ < 1.0f)
        {
            num_objects_ = 1.0f;
        }
    }
    if(freq_ != 0.0f && freq_ != res_freq0_)
    {
        res_freq0_ = freq_;
        coeffs00_  = -WUTR_RESON * 2.0f * cosf(res_freq0_ * tpidsr);
    }
    if(damp_ != 0.0f && damp_ != shake_damp_)
    {
        shake_damp_   = damp_;
        system_decay_ = WUTR_SYSTEM_DECAY + (shake_damp_ * 0.002f);
    }
    if(shake_max_ != 0.0f && shake_max_ != shake_max_save_)
    {
        shake_max_save_ = shake_max_;
        shake_energy_ += shake_max_save_ * MAX_SHAKE * 0.1f;
        if(shake_energy_ > MAX_SHAKE)
            shake_energy_ = MAX_SHAKE;
    }
    if(freq1_ != 0.0f && freq1_ != res_freq1_)
    {
        res_freq1_ = freq1_;
        coeffs10_  = -WUTR_RESON * 2.0f * cosf(res_freq1_ * tpidsr);
    }
    if(freq2_ != 0.0f && freq2_ != res_freq2_)
    {
        res_freq2_ = freq2_;
        coeffs20_  = -WUTR_RESON * 2.0f * cosf(res_freq2_ * tpidsr);
    }
    if((--kloop_) == 0.0f)
    {
        shake_energy_ = 0.0f;
    }

    float shakeEnergy = shake_energy_;
    float systemDecay = system_decay_;
    float sndLevel    = snd_level_;
    float num_objects = num_objects_;
    float soundDecay  = sound_decay_;
    float inputs0, inputs1, inputs2;

    shakeEnergy *= systemDecay; /* Exponential system decay */

    sndLevel = shakeEnergy;
    if(my_random(32767) < num_objects)
    {
        int j;
        j = my_random(3);
        if(j == 0)
        {
            center_freqs0_ = res_freq1_ * (0.75f + (0.25f * noise_tick()));
            gains0_        = fabsf(noise_tick());
        }
        else if(j == 1)
        {
            center_freqs1_ = res_freq1_ * (1.0f + (0.25f * noise_tick()));
            gains1_        = fabsf(noise_tick());
        }
        else
        {
            center_freqs2_ = res_freq1_ * (1.25f + (0.25f * noise_tick()));
            gains2_        = fabsf(noise_tick());
        }
    }

    gains0_ *= WUTR_RESON;
    if(gains0_ > 0.001f)
    {
        center_freqs0_ *= WUTR_FREQ_SWEEP;
        coeffs00_ = -WUTR_RESON * 2.0f * cosf(center_freqs0_ * tpidsr);
    }
    gains1_ *= WUTR_RESON;
    if(gains1_ > 0.00f)
    {
        center_freqs1_ *= WUTR_FREQ_SWEEP;
        coeffs10_ = -WUTR_RESON * 2.0f * cosf(center_freqs1_ * tpidsr);
    }
    gains2_ *= WUTR_RESON;
    if(gains2_ > 0.001f)
    {
        center_freqs2_ *= WUTR_FREQ_SWEEP;
        coeffs20_ = -WUTR_RESON * 2.0f * cosf(center_freqs2_ * tpidsr);
    }

    sndLevel *= soundDecay;
    inputs0 = sndLevel;
    inputs0 *= noise_tick();
    inputs1 = inputs0 * gains1_;
    inputs2 = inputs0 * gains2_;
    inputs0 *= gains0_;
    inputs0 -= outputs00_ * coeffs00_;
    inputs0 -= outputs01_ * coeffs01_;
    outputs01_ = outputs00_;
    outputs00_ = inputs0;
    data       = gains0_ * outputs00_;
    inputs1 -= outputs10_ * coeffs10_;
    inputs1 -= outputs11_ * coeffs11_;
    outputs11_ = outputs10_;
    outputs10_ = inputs1_;
    data += gains1_ * outputs10_;
    inputs2 -= outputs20_ * coeffs20_;
    inputs2 -= outputs21_ * coeffs21_;
    outputs21_ = outputs20_;
    outputs20_ = inputs2_;
    data += gains2_ * outputs20_;

    finalZ2_ = finalZ1_;
    finalZ1_ = finalZ0_;
    finalZ0_ = data * 4.0f;

    lastOutput = finalZ2_ - finalZ0_;
    lastOutput *= 0.005f;
    shake_energy_ = shakeEnergy;
    snd_level_    = sndLevel;
    return lastOutput;
}



void Resonator::Init(float position, int resolution, float sample_rate)
{
    sample_rate_ = sample_rate;

    SetFreq(440.f);
    SetStructure(.5f);
    SetBrightness(.5f);
    SetDamping(.5f);

    resolution_ = fmin(resolution, kMaxNumModes);

    for(int i = 0; i < resolution; ++i)
    {
        mode_amplitude_[i] = cos(position * TWOPI_F) * 0.25f;
    }

    for(int i = 0; i < kMaxNumModes / kModeBatchSize; ++i)
    {
        mode_filters_[i].Init();
    }
}

inline float NthHarmonicCompensation(int n, float stiffness)
{
    float stretch_factor = 1.0f;
    for(int i = 0; i < n - 1; ++i)
    {
        stretch_factor += stiffness;
        if(stiffness < 0.0f)
        {
            stiffness *= 0.93f;
        }
        else
        {
            stiffness *= 0.98f;
        }
    }
    return 1.0f / stretch_factor;
}

float Resonator::Process(const float in)
{
    //convert Hz to cycles / sample
    float out = 0.f;

    float stiffness  = CalcStiff(structure_);
    float f0         = frequency_ * NthHarmonicCompensation(3, stiffness);
    float brightness = brightness_;

    float harmonic       = f0;
    float stretch_factor = 1.0f;

    float input  = damping_ * 79.7f;
    float q_sqrt = powf(2.f, input * ratiofrac_);

    float q = 500.0f * q_sqrt * q_sqrt;
    brightness *= 1.0f - structure_ * 0.3f;
    brightness *= 1.0f - damping_ * 0.3f;
    float q_loss = brightness * (2.0f - brightness) * 0.85f + 0.15f;

    float mode_q[kModeBatchSize];
    float mode_f[kModeBatchSize];
    float mode_a[kModeBatchSize];
    int   batch_counter = 0;

    ResonatorSvf<kModeBatchSize>* batch_processor = &mode_filters_[0];

    for(int i = 0; i < resolution_; ++i)
    {
        float mode_frequency = harmonic * stretch_factor;
        if(mode_frequency >= 0.499f)
        {
            mode_frequency = 0.499f;
        }
        const float mode_attenuation = 1.0f - mode_frequency * 2.0f;

        mode_f[batch_counter] = mode_frequency;
        mode_q[batch_counter] = 1.0f + mode_frequency * q;
        mode_a[batch_counter] = mode_amplitude_[i] * mode_attenuation;
        ++batch_counter;

        if(batch_counter == kModeBatchSize)
        {
            batch_counter = 0;
            batch_processor
                ->Process<ResonatorSvf<kModeBatchSize>::BAND_PASS, true>(
                    mode_f, mode_q, mode_a, in, &out);
            ++batch_processor;
        }

        stretch_factor += stiffness;
        if(stiffness < 0.0f)
        {
            // Make sure that the partials do not fold back into negative frequencies.
            stiffness *= 0.93f;
        }
        else
        {
            // This helps adding a few extra partials in the highest frequencies.
            stiffness *= 0.98f;
        }
        harmonic += f0;
        q *= q_loss;
    }

    return out;
}

void Resonator::SetFreq(float freq)
{
    frequency_ = freq / sample_rate_;
}

void Resonator::SetStructure(float structure)
{
    structure_ = fmax(fmin(structure, 1.f), 0.f);
}

void Resonator::SetBrightness(float brightness)
{
    brightness_ = fmax(fmin(brightness, 1.f), 0.f);
}

void Resonator::SetDamping(float damping)
{
    damping_ = fmax(fmin(damping, 1.f), 0.f);
}

float Resonator::CalcStiff(float sig)
{
    if(sig < .25f)
    {
        sig = .25 - sig;
        sig = -sig * .25;
    }
    else if(sig < .3f)
    {
        sig = 0.f;
    }
    else if(sig < .9f)
    {
        sig -= .3f;
        sig *= stiff_frac_2;
    }
    else
    {
        sig -= .9f;
        sig *= 10; // div by .1
        sig *= sig;
        sig = 1.5 - cos(sig * PI_F) * .5f;
    }
    return sig;
}


void ModalVoice::Init(float sample_rate)
{
    sample_rate_ = sample_rate;
    aux_         = 0.f;

    excitation_filter_.Init();
    resonator_.Init(0.015f, 24, sample_rate_);
    excitation_filter_.Init();
    dust_.Init();

    SetSustain(false);
    SetFreq(440.f);
    SetAccent(.3f);
    SetStructure(.6f);
    SetBrightness(.8f);
    SetDamping(.6f);
}

void ModalVoice::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void ModalVoice::Trig()
{
    trig_ = true;
}

void ModalVoice::SetFreq(float freq)
{
    resonator_.SetFreq(freq);
    f0_ = freq / sample_rate_;
    f0_ = fclamp(f0_, 0.f, .25f);
}

void ModalVoice::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void ModalVoice::SetStructure(float structure)
{
    resonator_.SetStructure(structure);
}

void ModalVoice::SetBrightness(float brightness)
{
    brightness_ = fclamp(brightness, 0.f, 1.f);
    density_    = brightness_ * brightness_;
}

void ModalVoice::SetDamping(float damping)
{
    damping_ = fclamp(damping, 0.f, 1.f);
}

float ModalVoice::GetAux()
{
    return aux_;
}

float ModalVoice::Process(bool trigger)
{
    float brightness = brightness_ + 0.25f * accent_ * (1.0f - brightness_);
    float damping    = damping_ + 0.25f * accent_ * (1.0f - damping_);

    const float range  = sustain_ ? 36.0f : 60.0f;
    const float f      = sustain_ ? 4.0f * f0_ : 2.0f * f0_;
    const float cutoff = fmin(
        f
            * powf(2.f,
                   kOneTwelfth
                       * ((brightness * (2.0f - brightness) - 0.5f) * range)),
        0.499f);
    const float q = sustain_ ? 0.7f : 1.5f;

    float temp = 0.f;
    // Synthesize excitation signal.
    if(sustain_)
    {
        const float dust_f = 0.00005f + 0.99995f * density_ * density_;
        dust_.SetDensity(dust_f);
        temp = dust_.Process() * (4.0f - dust_f * 3.0f) * accent_;
    }
    else if(trigger || trig_)
    {
        const float attenuation = 1.0f - damping * 0.5f;
        const float amplitude   = (0.12f + 0.08f * accent_) * attenuation;
        temp = amplitude * powf(2.f, kOneTwelfth * (cutoff * cutoff * 24.0f))
               / cutoff;
        trig_ = false;
    }

    const float one = 1.0f;
    excitation_filter_.Process<ResonatorSvf<1>::LOW_PASS, false>(
        &cutoff, &q, &one, temp, &temp);

    aux_ = temp;

    resonator_.SetBrightness(brightness);
    resonator_.SetDamping(damping);

    return resonator_.Process(temp);
}

void Pluck::Reinit()
{
    int    n;
    float  val = 0;
    float *ap  = buf_;
    //npts_ = (int32_t)roundf(decay_ * (float)(maxpts_ - PLUKMIN) + PLUKMIN);
    npts_ = (int32_t)(decay_ * (float)(maxpts_ - PLUKMIN) + PLUKMIN);
    //sicps_ = ((float)npts_ * INTERPFACTOR + INTERPFACTOR/2.0f) * (1.0f / _sr);
    sicps_ = ((float)npts_ * 256.0f + 128.0f) * (1.0f / sample_rate_);
    for(n = npts_; n--;)
    {
        val   = (float)((float)rand() / (float)RAND_MAX);
        *ap++ = (val * 2.0f) - 1.0f;
    }
    phs256_ = 0;
}

void Pluck::Init(float sample_rate, float *buf, int32_t npts, int32_t mode)
{
    amp_         = 0.5f;
    freq_        = 300;
    decay_       = 1.0f;
    sample_rate_ = sample_rate;
    mode_        = mode;

    maxpts_ = npts;
    npts_   = npts;
    buf_    = buf;

    Reinit();
    /* tuned pitch convt */
    sicps_ = (npts * 256.0f + 128.0f) * (1.0f / sample_rate_);
    init_  = 1;
}

float Pluck::Process(float &trig)
{
    float * fp, out;
    int32_t phs256, phsinc, ltwopi, offset;
    float   coeff;

    // unused variable
    // float inv_coeff;

    float frac, diff;
    float dampmin = 0.42f;

    if(trig != 0)
    {
        init_ = 0;
        Reinit();
    }

    if(init_)
    {
        return 0;
    }
    // Set Coeff for mode.
    switch(mode_)
    {
        case PLUCK_MODE_RECURSIVE:
            coeff = ((0.5f - dampmin) * damp_) + dampmin;
            break;
        case PLUCK_MODE_WEIGHTED_AVERAGE:
            coeff = 0.05f + (damp_ * 0.90f);
            break;
        default: coeff = 0.5f; break;
    }

    // variable set but not used
    //inv_coeff = 1.0f - coeff;

    phsinc = (int32_t)(freq_ * sicps_);
    phs256 = phs256_;
    ltwopi = npts_ << 8;
    offset = phs256 >> 8;
    fp     = (float *)buf_ + offset; /* lookup position   */
    diff   = fp[1] - fp[0];
    frac   = (float)(phs256 & 255) / 256.0f; /*  w. interpolation */
    out    = (fp[0] + diff * frac) * amp_;   /*  gives output val */
    if((phs256 += phsinc) >= ltwopi)
    {
        int   nn;
        float preval;
        phs256 -= ltwopi;
        fp     = buf_;
        preval = fp[0];
        fp[0]  = fp[npts_];
        fp++;
        nn = npts_;
        do
        {
            /* 1st order recursive filter*/
            //preval = (*fp + preval) * coeff;
            /* weighted average - stretches decay times */
            switch(mode_)
            {
                case PLUCK_MODE_RECURSIVE:
                    preval = (*fp + preval) * coeff;
                    break;
                case PLUCK_MODE_WEIGHTED_AVERAGE:
                    preval = (*fp * coeff) + (preval * (1.0f - coeff));
                    break;
                default: break;
            }
            *fp++ = preval;
        } while(--nn);
    }
    phs256_ = phs256;
    return out;
}

void StringVoice::Init(float sample_rate)
{
    sample_rate_ = sample_rate;

    excitation_filter_.Init(sample_rate);
    string_.Init(sample_rate_);
    dust_.Init();
    remaining_noise_samples_ = 0;

    SetSustain(false);
    SetFreq(440.f);
    SetAccent(.8f);
    SetStructure(.7f);
    SetBrightness(.2f);
    SetDamping(.7f);
}

void StringVoice::Reset()
{
    string_.Reset();
}

void StringVoice::SetSustain(bool sustain)
{
    sustain_ = sustain;
}

void StringVoice::Trig()
{
    trig_ = true;
}

void StringVoice::SetFreq(float freq)
{
    string_.SetFreq(freq);
    f0_ = freq / sample_rate_;
    f0_ = fclamp(f0_, 0.f, .25f);
}

void StringVoice::SetAccent(float accent)
{
    accent_ = fclamp(accent, 0.f, 1.f);
}

void StringVoice::SetStructure(float structure)
{
    structure = fclamp(structure, 0.f, 1.f);
    const float non_linearity
        = structure < 0.24f
              ? (structure - 0.24f) * 4.166f
              : (structure > 0.26f ? (structure - 0.26f) * 1.35135f : 0.0f);
    string_.SetNonLinearity(non_linearity);
}

void StringVoice::SetBrightness(float brightness)
{
    brightness_ = fclamp(brightness, 0.f, 1.f);
    density_    = brightness_ * brightness_;
}

void StringVoice::SetDamping(float damping)
{
    damping_ = fclamp(damping, 0.f, 1.f);
}

float StringVoice::GetAux()
{
    return aux_;
}

float StringVoice::Process(bool trigger)
{
    const float brightness = brightness_ + .25 * accent_ * (1.f - brightness_);
    const float damping    = damping_ + .25 * accent_ * (1.f - damping_);

    // Synthesize excitation signal.
    if(trigger || trig_ || sustain_)
    {
        trig_              = false;
        const float range  = 72.0f;
        const float f      = 4.0f * f0_;
        const float cutoff = fmin(
            f
                * powf(2.f,
                       kOneTwelfth * (brightness * (2.0f - brightness) - 0.5f)
                           * range),
            0.499f);
        const float q            = sustain_ ? 1.0f : 0.5f;
        remaining_noise_samples_ = static_cast<size_t>(1.0f / f0_);
        excitation_filter_.SetFreq(cutoff * sample_rate_);
        excitation_filter_.SetRes(q);
    }

    float temp = 0.f;

    if(sustain_)
    {
        const float dust_f = 0.00005f + 0.99995f * density_ * density_;
        dust_.SetDensity(dust_f);
        temp = dust_.Process() * (8.0f - dust_f * 6.0f) * accent_;
    }
    else if(remaining_noise_samples_)
    {
        temp = 2.0f * rand() * kRandFrac - 1.0f;
        remaining_noise_samples_--;
        remaining_noise_samples_ = DSY_MAX(remaining_noise_samples_, 0.f);
    }

    excitation_filter_.Process(temp);
    temp = excitation_filter_.Low();

    aux_ = temp;

    string_.SetBrightness(brightness);
    string_.SetDamping(damping);

    return string_.Process(temp);
}

