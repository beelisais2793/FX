#pragma once

#include "Synthesis/blosc.h"
#include "Synthesis/oscillator.h"
#include "Synthesis/variablesawosc.h"
#include "Synthesis/variableshapeosc.h"
#include "Noise/whitenoise.h"
#include "Filters/atone.h"
#include "Filters/tone.h"
#include "Control/adsr.h"
#include "Utility/dsp.h"

#include "commons.h"

namespace orchard
{
    using namespace daisysp;

    constexpr int kGenerators{9};

    struct GeneratorConf
    {
        bool active;
        float volume;
        float pan;
        int pitch;
        int interval;
        float character;
        int ringSource;
        float ringAmt;
    };

    class GeneratorBank
    {

    public:
        GeneratorBank() {}
        ~GeneratorBank() {}

        void Init(float sampleRate)
        {
            hOsc1_.Init(sampleRate);
            hOsc1_.SetWaveform(Oscillator::WAVE_SIN);
            hOsc1_.SetAmp(1.f);

            hOsc2_.Init(sampleRate);

            hOsc3_.Init(sampleRate);
            hOsc3_.SetWaveform(BlOsc::WAVE_TRIANGLE);
            hOsc3_.SetAmp(1.f);

            hOsc4_.Init(sampleRate);
            hOsc4_.SetWaveform(BlOsc::WAVE_SQUARE);
            hOsc4_.SetAmp(1.f);

            lOsc1_.Init(sampleRate);
            lOsc1_.SetWaveform(Oscillator::WAVE_SIN);
            lOsc1_.SetAmp(1.f);

            lOsc2_.Init(sampleRate);

            lOsc3_.Init(sampleRate);
            lOsc3_.SetWaveform(BlOsc::WAVE_TRIANGLE);
            lOsc3_.SetAmp(1.f);

            lOsc4_.Init(sampleRate);
            lOsc4_.SetWaveform(BlOsc::WAVE_SQUARE);
            lOsc4_.SetAmp(1.f);

            noise_.Init();
            noise_.SetAmp(1.f);
            noiseFilterHP_.Init(sampleRate);
            noiseFilterLP_.Init(sampleRate);

            for (int i = 0; i < kGenerators; i++)
            {
                envelopes_[i].Init(sampleRate);
            }
        }

        void SetPitch(float pitch)
        {
            basePitch_ = fclamp(pitch, 0, 127);
            SetFrequencies();
        }

        void SetCharacter(float character)
        {
            hOsc2_.SetWaveshape(character);
            hOsc2_.SetPW(1.f - character);
            hOsc3_.SetPw(character);
            hOsc4_.SetPw(character);

            lOsc2_.SetWaveshape(character);
            lOsc2_.SetPW(1.f - character);
            lOsc3_.SetPw(character);
            lOsc4_.SetPw(character);
        }

        void Randomize()
        {
            int actives{0};
            int half{kGenerators / 2};
            for (int i = 0; i < kGenerators; i++)
            {
                bool active{1 == std::rand() % 2};
                // Limit the number of inactive generators to half of their total number.
                if (i >= half && !active && actives < half)
                {
                    active = true;
                }
                conf_[i].active = active;
                if (active)
                {
                    ++actives;
                }
                conf_[i].pan = RandomFloat(0.3f, 0.7f);

                if (i < kGenerators - 1)
                {
                    if (i % 2 == 0)
                    {
                        conf_[i].interval = RandomInterval(Range::HIGH);
                    }
                    else
                    {
                        conf_[i].interval = RandomInterval(Range::LOW);
                    }
                }
                else
                {
                    conf_[i].interval = RandomInterval(Range::FULL);
                }

                envelopes_[i].SetAttackTime(RandomFloat(0.f, 2.f));
                envelopes_[i].SetDecayTime(RandomFloat(0.f, 2.f));
                envelopes_[i].SetSustainLevel(RandomFloat(0.f, 1.f));
                envelopes_[i].SetReleaseTime(RandomFloat(0.f, 2.f));

                //conf_[i].ringSource = std::floor(RandomFloat(0.f, kGenerators - 1));
            }
            for (int i = 0; i < kGenerators; i++)
            {
                if (conf_[i].active)
                {
                    conf_[i].volume = 1.f / actives; //RandomFloat(0.3f, 0.5f);
                }
            }

            hOsc2_.SetWaveshape(RandomFloat(0.f, 1.f));
            hOsc2_.SetPW(RandomFloat(-1.f, 1.f));

            hOsc3_.SetPw(RandomFloat(-1.f, 1.f));

            hOsc4_.SetPw(RandomFloat(-1.f, 1.f));

            lOsc2_.SetWaveshape(RandomFloat(0.f, 1.f));
            lOsc2_.SetPW(RandomFloat(-1.f, 1.f));

            lOsc3_.SetPw(RandomFloat(-1.f, 1.f));

            lOsc4_.SetPw(RandomFloat(-1.f, 1.f));

            conf_[8].character = RandomFloat(1.f, 2.f);

            SetFrequencies();
        }

        void SetEnvelopeGate(bool gate)
        {
            envelopeGate_ = gate;
        }

        void Process(float &left, float &right)
        {
            // Process all generators.
            float sigs[kGenerators];
            for (int i = 0; i < kGenerators; i++)
            {
                if (conf_[i].active)
                {
                    if (i == 0)
                    {
                        sigs[i] = hOsc1_.Process();
                    }
                    else if (i == 1)
                    {
                        sigs[i] = lOsc1_.Process();
                    }
                    else if (i == 2)
                    {
                        sigs[i] = hOsc2_.Process();
                    }
                    else if (i == 3)
                    {
                        sigs[i] = lOsc2_.Process();
                    }
                    else if (i == 4)
                    {
                        sigs[i] = hOsc3_.Process();
                    }
                    else if (i == 5)
                    {
                        sigs[i] = lOsc3_.Process();
                    }
                    else if (i == 6)
                    {
                        sigs[i] = hOsc4_.Process();
                    }
                    else if (i == 7)
                    {
                        sigs[i] = lOsc4_.Process();
                    }
                    else if (i == 8)
                    {
                        sigs[i] = noise_.Process();
                        sigs[i] = conf_[i].character * sigs[i];
                        sigs[i] = noiseFilterHP_.Process(sigs[i]);
                        sigs[i] = conf_[i].character * sigs[i];
                        sigs[i] = SoftClip(noiseFilterLP_.Process(sigs[i]));
                    }

                    left += sigs[i] * conf_[i].volume * (1 - conf_[i].pan) * envelopes_[i].Process(envelopeGate_);
                    right += sigs[i] * conf_[i].volume * conf_[i].pan * envelopes_[i].Process(envelopeGate_);
                }
            }

            /*
            // Apply ring modulation.
            for (int i = 0; i < kGenerators; i++)
            {
                if (conf_[i].ringSource > 0 && conf_[i].active && conf_[conf_[i].ringSource].active)
                {
                    sigs[i] = SoftClip(sigs[i] * sigs[conf_[i].ringSource]);
                }
                left += sigs[i] * conf_[i].volume * (1 - conf_[i].pan) * envelopes_[i].Process(envelopeGate);
                right += sigs[i] * conf_[i].volume * conf_[i].pan * envelopes_[i].Process(envelopeGate);
            }
            */
        }

    private:
        float CalcFrequency(int generator, float pitch)
        {
            float midi_nn = fclamp(pitch + conf_[generator].interval, 0.f, 120.f);

            return mtof(midi_nn);
        }

        void SetFrequencies()
        {
            hOsc1_.SetFreq(CalcFrequency(0, basePitch_));
            hOsc2_.SetFreq(CalcFrequency(2, basePitch_));
            hOsc3_.SetFreq(CalcFrequency(4, basePitch_));
            hOsc4_.SetFreq(CalcFrequency(6, basePitch_));

            lOsc1_.SetFreq(CalcFrequency(1, basePitch_));
            lOsc2_.SetFreq(CalcFrequency(3, basePitch_));
            lOsc3_.SetFreq(CalcFrequency(5, basePitch_));
            lOsc4_.SetFreq(CalcFrequency(7, basePitch_));

            float f{CalcFrequency(8, basePitch_)};
            noiseFilterHP_.SetFreq(f);
            noiseFilterLP_.SetFreq(f);
        }

        float basePitch_;
        bool envelopeGate_{false};

        Oscillator hOsc1_;            // Sine
        VariableSawOscillator hOsc2_; // Bipolar ramp
        BlOsc hOsc3_;                 // Triangle
        BlOsc hOsc4_;                 // Square

        Oscillator lOsc1_;            // Sine
        VariableSawOscillator lOsc2_; // Bipolar ramp
        BlOsc lOsc3_;                 // Triangle
        BlOsc lOsc4_;                 // Square

        WhiteNoise noise_;

        ATone noiseFilterHP_;
        Tone noiseFilterLP_;

        Adsr envelopes_[kGenerators];
        // Generators configuration:
        // 0 = hOsc1_
        // 1 = lOsc1_
        // 2 = hOsc2_
        // 3 = lOsc2_
        // 4 = hOsc3_
        // 5 = lOsc3_
        // 6 = hOsc4_
        // 7 = lOsc4_
        // 8 = noise_
        GeneratorConf conf_[kGenerators];
    };
}