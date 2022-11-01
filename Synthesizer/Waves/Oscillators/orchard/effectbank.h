#pragma once

#include "Filters/svf.h"
#include "Effects/reverbsc.h"
#include "Utility/delayline.h"
#include "Utility/dsp.h"

#include "commons.h"
#include "resonator.h"

namespace orchard
{
    using namespace daisysp;

    ReverbSc DSY_SDRAM_BSS reverb;
    DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS leftDelay_Line;
    DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS rightDelay_Line;

    DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS leftResoPoleDelayLine[3];
    DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS rightResoPoleDelayLine[3];

    struct delay
    {
        DelayLine<float, MAX_DELAY> *del;
        float currentDelay;
        float delayTarget;

        void Reset()
        {
            del->Reset();
        }

        float Process(float feedback, float in)
        {
            fonepole(currentDelay, delayTarget, .0002f);
            del->SetDelay(currentDelay);

            float read = del->Read();
            del->Write((feedback * read) + in);

            return read;
        }
    };

    struct EffectConf
    {
        bool active;
        float dryWet;
        float param1;
    };

    enum class FilterType
    {
        HP,
        LP,
        BP,
    };

    class EffectBank
    {
    public:
        EffectBank() {}
        ~EffectBank() {}

        void Init(float sampleRate)
        {
            sampleRate_ = sampleRate;

            leftFilter_.Init(sampleRate_);
            rightFilter_.Init(sampleRate_);

            for (int i = 0; i < 3; i++)
            {
                leftResoPoleDelayLine[i].Init();
                rightResoPoleDelayLine[i].Init();
            }
            resonator_.Init(sampleRate_);
            resonator_.AddPole(&leftResoPoleDelayLine[0], &rightResoPoleDelayLine[0]);
            resonator_.AddPole(&leftResoPoleDelayLine[1], &rightResoPoleDelayLine[1]);
            resonator_.AddPole(&leftResoPoleDelayLine[2], &rightResoPoleDelayLine[2]);

            leftDelay_Line.Init();
            rightDelay_Line.Init();
            leftDelay_.del = &leftDelay_Line;
            rightDelay_.del = &rightDelay_Line;

            reverb.Init(sampleRate_);
        }

        void Randomize()
        {
            // Filter.
            conf_[0].active = true; //1 == std::rand() % 2;
            if (conf_[0].active)
            {
                conf_[0].dryWet = RandomFloat(0.f, 1.f);
                filterType_ = static_cast<FilterType>(std::rand() % 3);
                int pitch;
                switch (filterType_)
                {
                case FilterType::HP:
                    pitch = RandomPitch(Range::HIGH);
                    break;

                case FilterType::LP:
                    pitch = RandomPitch(Range::LOW);
                    break;

                case FilterType::BP:
                    pitch = RandomPitch(Range::FULL);
                    break;

                default:
                    break;
                }
                float freq = mtof(pitch);
                float res{RandomFloat(0.f, 1.f)};
                float drive{RandomFloat(0.f, 1.f)};
                leftFilter_.SetFreq(freq);
                leftFilter_.SetRes(res);
                leftFilter_.SetDrive(drive);
                rightFilter_.SetFreq(freq);
                rightFilter_.SetRes(res);
                rightFilter_.SetDrive(drive);
            }

            // Resonator.
            conf_[1].active = true; //1 == std::rand() % 2;
            if (conf_[1].active)
            {
                conf_[1].dryWet = RandomFloat(0.f, 1.f);
                resonator_.SetDecay(RandomFloat(0.f, 0.4f));
                resonator_.SetDetune(RandomFloat(0.f, 0.1f));
                resonator_.SetReso(RandomFloat(0.f, 0.4f));
                resonator_.SetPitch(0, RandomPitch(Range::FULL));
                resonator_.SetPitch(1, RandomPitch(Range::FULL));
                resonator_.SetPitch(2, RandomPitch(Range::FULL));
                resonator_.SetDamp(RandomFloat(100.f, 5000.f));
                /*
            conf_[1].dryWet = 1.f;
            resonator_.SetDecay(0.4f);
            resonator_.SetDetune(0.f);
            resonator_.SetReso(0.4f);
            resonator_.SetPitch(0, 60.f);
            resonator_.SetPitch(1, 60.f);
            resonator_.SetPitch(2, 60.f);
            resonator_.SetDamp(500.f);
            */
            }

            // Delay.
            conf_[2].active = false; //1 == std::rand() % 2;
            if (conf_[2].active)
            {
                leftDelay_.Reset();
                rightDelay_.Reset();
                conf_[2].dryWet = RandomFloat(0.f, 1.f);
                conf_[2].param1 = RandomFloat(0.f, 0.9f);
                leftDelay_.delayTarget = RandomFloat(sampleRate_ * .05f, MAX_DELAY);
                rightDelay_.delayTarget = RandomFloat(sampleRate_ * .05f, MAX_DELAY);
            }

            // Reverb.
            conf_[3].active = true; //1 == std::rand() % 2;
            if (conf_[3].active)
            {
                conf_[3].dryWet = RandomFloat(0.f, 1.f);
                float fb{RandomFloat(0.f, 0.9f)};
                reverb.SetFeedback(fb);
                reverb.SetLpFreq(RandomFloat(0.f, 5000.f));
            }
        }

        void Process(float &left, float &right)
        {
            // Effects.
            float leftW{0.f};
            float rightW{0.f};

            // Filter.
            if (conf_[0].active)
            {
                leftFilter_.Process(left);
                rightFilter_.Process(right);
                switch (filterType_)
                {
                case FilterType::LP:
                    leftW = leftFilter_.Low();
                    rightW = rightFilter_.Low();
                    break;

                case FilterType::HP:
                    leftW = leftFilter_.High();
                    rightW = rightFilter_.High();
                    break;

                case FilterType::BP:
                    leftW = leftFilter_.Band();
                    rightW = rightFilter_.Band();
                    break;

                default:
                    break;
                }
                left = conf_[0].dryWet * leftW * .3f + (1.0f - conf_[0].dryWet) * left;
                right = conf_[0].dryWet * rightW * .3f + (1.0f - conf_[0].dryWet) * right;
            }

            // Resonator.
            if (conf_[1].active)
            {
                leftW = left;
                rightW = right;
                resonator_.Process(leftW, rightW);
                left = conf_[1].dryWet * SoftClip(leftW) * .3f + (1.0f - conf_[1].dryWet) * left;
                right = conf_[1].dryWet * SoftClip(rightW) * .3f + (1.0f - conf_[1].dryWet) * right;
            }

            // Delay.
            if (conf_[2].active)
            {
                leftW = leftDelay_.Process(conf_[2].param1, left);
                rightW = rightDelay_.Process(conf_[2].param1, right);
                left = conf_[2].dryWet * leftW * .3f + (1.0f - conf_[2].dryWet) * left;
                right = conf_[2].dryWet * rightW * .3f + (1.0f - conf_[2].dryWet) * right;
            }

            // Reverb.
            if (conf_[3].active)
            {
                reverb.Process(left, right, &leftW, &rightW);
                left = conf_[3].dryWet * leftW * .3f + (1.0f - conf_[3].dryWet) * left;
                right = conf_[3].dryWet * rightW * .3f + (1.0f - conf_[3].dryWet) * right;
            }
        }

    private:
        Svf leftFilter_;
        Svf rightFilter_;
        delay leftDelay_;
        delay rightDelay_;
        Resonator resonator_;
        EffectConf conf_[4];
        FilterType filterType_;
        float sampleRate_;
    };
}