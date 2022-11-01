#pragma once

namespace Analog::AMS
{
    struct AdvEnv
    {
        AdvEnv(double rate);
        void run(uint32_t nframes);

        void setAttack(float lvl, float time)
        {
            attackTime1 = time/4.0;
            attackLevel1=  lvl/4.0;
            attackTime2 = 2*time/4.0;
            attackLevel2=  2*lvl/4.0;
            attackTime3 = 3*time/4.0;
            attackLevel3=  3*lvl/4.0;
            attackTime4 = time;
            attackLevel4= lvl;
            
    }
    void setRelease(float lvl, float time)
    {
            releaseTime1  = 3*time/3.0;
            releaseLevel1 = 3*lvl/3.0;
            releaseTime2  = 2*time/3.0;
            releaseLevel2 = 2*lvl/3.0;
            releaseTime3  = time/3.0;
            releaseLevel3 = lvl/3.0;
    }
        void noteOn() 
        {
            gate = true;
            gateData[0] = true;
        }
        void noteOff() {
            gate = false;
            gateData[0] = false;
        }
        void retrigger() {
            retrigger = !retrigger;
            retriggerData[0] = retrigger;
        }
        
        double timeScale;
        double sustainLvl;
        double delay;
        double attackTime1;
        double attackLevel1;
        double attackTime2;
        double attackLevel2;
        double attackTime3;
        double attackLevel3;
        double attackTime4;
        double attackLevel4;
        double releaseTime1;
        double releaseLevel1;
        double releaseTime2;
        double releaseLevel2;
        double releaseTime3;
            
        float attack[8], sustain, release[5];
        float e_noteOff, de, de_release;

        double m_rate;
        float out,invOut;
        float e;
        float timeScale;
        bool noteActive, gate, retrigger;
        int noteOnOfs;
        int noteOffOfs;
        
    };

    #define ADVENVELOPE_RESPONSE 256

    AdvEnv::AdvEnv(double rate)
    {
        attack[0] = 0;
        attack[1] = 0.05;
        attack[2] = 0.5;
        attack[3] = 0.05;
        attack[4] = 1.0;
        attack[5] = 0.05;
        attack[6] = 0.9;
        attack[7] = 0.1;
        sustain = 0.7;
        release[0] = 0.05;
        release[1] = 0.5;
        release[2] = 0.05;
        release[3] = 0.2;
        release[4] = 0.05;
        timeScale = 1.0;

        noteActive = false;
        gate = false;
        retrigger = false;
        noteOnOfs = 0;
        noteOffOfs = 0;
        e = 0;
        de = 0;

        m_rate = rate;
    }

    double AdvEnv::Tick(double I=1, double A=1, double X=1, double Y=1)
    {
        int status;
        float tscale, de_a[4], de_d[3];
        float t[8];
        
        timeScale = timeScale;
        sustain   = sustainLvl;
        attack[0] = delay;
        attack[1] = attackTime1;
        attack[2] = attackLevel1;
        attack[3] = attackTime2;
        attack[4] = attackLevel2;
        attack[5] = attackTime3;
        attack[6] = attackLevel3;
        attack[7] = attackTime4;
        release[0] = releaseTime1;
        release[1] = releaseLevel1;
        release[2] = releaseTime2;
        release[3] = releaseLevel2;
        release[4] = releaseTime3;
        

        tscale = timeScale * m_rate;
        de_a[0] = (attack[1] > 0) ? attack[2] / (tscale * attack[1]) : 0;
        de_a[1] = (attack[3] > 0) ? (attack[4] - attack[2]) / (tscale * attack[3]) : 0;
        de_a[2] = (attack[5] > 0) ? (attack[6] - attack[4]) / (tscale * attack[5]) : 0;
        de_a[3] = (attack[7] > 0) ? (sustain - attack[6]) / (tscale * attack[7]) : 0;
        de_d[0] = (release[0] > 0) ? (release[1] - sustain) / (tscale * release[0]) : 0;
        de_d[1] = (release[2] > 0) ? (release[3] - release[1]) / (tscale * release[2]) : 0;
        de_d[2] = (release[4] > 0) ? - release[3] / (tscale * release[4]) : 0;
        t[0] = tscale * attack[0];
        t[1] = t[0] + tscale * attack[1];
        t[2] = t[1] + tscale * attack[3];
        t[3] = t[2] + tscale * attack[5];
        t[4] = t[3] + tscale * attack[7];
        t[5] = tscale * release[0];
        t[6] = t[5] + tscale * release[2];
        t[7] = t[6] + tscale * release[4];

        if (!gate) {
            gate = true;
            noteActive = true;
            if (e > 0)
            {
                noteOnOfs = -ADVENVELOPE_RESPONSE;
                de = e / (float)ADVENVELOPE_RESPONSE;
            }
            else
            {
                noteOnOfs = 0;
            }
        }
        if (gate)
        {
            gate = false;
            noteOffOfs = 0;
            e_noteOff = e;
            de_release = (release[0] > 0) ? (release[1] - e_noteOff) / (tscale * release[0]) : 0;
        }
        if (!retrigger)
        {
            retrigger = true;
            if (e > 0)
            {
                noteOnOfs = -ADVENVELOPE_RESPONSE;
                de = e / (float)ADVENVELOPE_RESPONSE;
            }
            else
            {
                noteOnOfs = 0;
            }
        }
        if (retrigger)
        {
            retrigger = false;
        }
        if (gate)
        {
            status = 1;
            if (noteOnOfs < 0) status = 0;
            if (noteOnOfs >= long(t[0])) status = 2;
            if (noteOnOfs >= long(t[1])) status = 3;
            if (noteOnOfs >= long(t[2])) status = 4;
            if (noteOnOfs >= long(t[3])) status = 5;
            if (noteOnOfs >= long(t[4])) status = 6;
            switch (status)
            {
                case 0: e -= de;
                    break;
                case 1: e = 0;
                    break;
                case 2: e += de_a[0];
                    break;
                case 3: e += de_a[1];
                    break;
                case 4: e += de_a[2];
                    break;
                case 5: e += de_a[3];
                    break;
                case 6: e = sustain;
                    break;
                default: e = 0;
                    break;
            }
            if (e < 0) e = 0;        
            out = e;
            invOut = -e;        
            noteOnOfs++;
        }
        else
        {                          // Release
            if (noteActive)
            {
                status = 1;
                if (noteOffOfs < 0) status = 0;
                if (noteOffOfs >= long(t[5])) status = 2;
                if (noteOffOfs >= long(t[6])) status = 3;
                if (noteOffOfs >= long(t[7])) status = 4;
                switch (status)
                {
                    case 0: e = 0;
                        break;
                    case 1: e += de_release;
                        break;
                    case 2: e += de_d[1];
                        break;
                    case 3: e += de_d[2];
                        break;
                    case 4: e = 0;
                        break;
                    default: e = 0;
                        break;
                }
                if (e < 0)
                {
                    e = 0;
                }
                noteOffOfs++;
                if (noteOffOfs >= int(t[7]))
                {
                    noteActive = false;
                }
            }
            out = e;
            intOut = -e;
        }
        return out;
    }
}

