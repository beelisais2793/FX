/*
    ==============================================================================
    This file is part of Tal-NoiseMaker by Patrick Kunz.
    Copyright(c) 2005-2010 Patrick Kunz, TAL
    Togu Audio Line, Inc.
    http://kunz.corrupt.ch
    This file may be licensed under the terms of of the
    GNU General Public License Version 2 (the ``GPL'').
    Software distributed under the License is distributed
    on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
    express or implied. See the GPL for the specific language
    governing rights and limitations.
    You should have received a copy of the GPL along with this
    program. If not, go to http://www.gnu.org/licenses/gpl.html
    or write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    ==============================================================================
 */
#pragma once

#include <cmath>

namespace FX::YKChorus
{
class OnePoleLP {
public:
    DspFloatType inputs, outputs, lastOutput;

    OnePoleLP() {
        lastOutput = inputs = outputs = 0.0f;
    }

    ~OnePoleLP() {}

    void tick(DspFloatType *sample, DspFloatType cutoff) {
        DspFloatType p = (cutoff * 0.98f) * (cutoff * 0.98f) * (cutoff * 0.98f) * (cutoff * 0.98f);
        outputs = (1.0f - p) * (*sample) + p * outputs;
        *sample = outputs;
    }
};

class DCBlock {
public:
    DspFloatType inputs, outputs, lastOutput;

    DCBlock() {
        lastOutput = inputs = outputs = 0.0f;
    }

    ~DCBlock() {}

    inline void tick(DspFloatType *sample, DspFloatType cutoff) {
        outputs     = *sample - inputs + (0.999f - cutoff * 0.4f) * outputs;
        inputs      = *sample;
        lastOutput  = outputs;
        *sample     = lastOutput;
    }
};


class Chorus {
public:
    DspFloatType *delayLineStart;
    DspFloatType *delayLineEnd;
    DspFloatType *writePtr;

    int delayLineLength;
    DspFloatType rate;
    DspFloatType delayLineOutput;

    DspFloatType sampleRate;
    DspFloatType delayTime;

    // Runtime variables
    DspFloatType offset, diff, frac, *ptr, *ptr2;

    int readPos;

    OnePoleLP *lp;
    DspFloatType z1;
    DspFloatType mult, sign;

    // lfo
    DspFloatType lfoPhase, lfoStepSize, lfoSign;

    Chorus(DspFloatType sampleRate, DspFloatType phase, DspFloatType rate, DspFloatType delayTime) {
        this->rate = rate;
        this->sampleRate = sampleRate;
        this->delayTime = delayTime;
        z1 = 0.0f;
        sign = 0;
        lfoPhase = phase * 2.0f - 1.0f;
        lfoStepSize = (4.0f * rate / sampleRate);
        lfoSign = 1.0f;

        // Compute required buffer size for desired delay and allocate it
        // Add extra point to aid in interpolation later
        delayLineLength = ((int)floorf(delayTime * sampleRate * 0.001f) * 2);
        delayLineStart = new DspFloatType[delayLineLength];

        // Set up pointers for delay line
        delayLineEnd = delayLineStart + delayLineLength;
        writePtr = delayLineStart;

        // Zero out the buffer (silence)
        do {
            *writePtr = 0.0f;
        }
        while (++writePtr < delayLineEnd);

        // Set read pointer to end of delayline. Setting it to the end
        // ensures the interpolation below works correctly to produce
        // the first non-zero sample.
        writePtr = delayLineStart + delayLineLength -1;
        delayLineOutput = 0.0f;
        lp = new OnePoleLP();
    }

    ~Chorus() {
        delete[] delayLineStart;
        delete lp;
    }

    void setLfoRate(DspFloatType rate) {
        lfoStepSize = (4.0f * rate / sampleRate);
    }

    DspFloatType process(DspFloatType *sample) {
        // Get delay time
        offset = (nextLFO() * 0.3f + 0.4f) * delayTime * sampleRate * 0.001f;

        // Compute the largest read pointer based on the offset.  If ptr
        // is before the first delayline location, wrap around end point
        ptr = writePtr - (int)floorf(offset);
        if (ptr < delayLineStart)
            ptr += delayLineLength;

        ptr2 = ptr - 1;
        if (ptr2 < delayLineStart)
            ptr2 += delayLineLength;

        frac = offset - (int)floorf(offset);
        delayLineOutput = *ptr2 + *ptr * (1 - frac) - (1 - frac) * z1;
        z1 = delayLineOutput;

        // Low pass
        lp->tick(&delayLineOutput, 0.95f);

        // Write the input sample and any feedback to delayline
        *writePtr = *sample;

        // Increment buffer index and wrap if necesary
        if (++writePtr >= delayLineEnd) {
            writePtr = delayLineStart;
        }
        return delayLineOutput;
    }

    inline DspFloatType nextLFO() {
        if (lfoPhase >= 1.0f)
        {
            lfoSign = -1.0f;
        }
        else if (lfoPhase <= -1.0f)
        {
            lfoSign = +1.0f;
        }
        lfoPhase += lfoStepSize * lfoSign;
        return lfoPhase;
    }
};

class ChorusEngine {
public:
    Chorus *chorus1L;
    Chorus *chorus1R;
    Chorus *chorus2L;
    Chorus *chorus2R;

    DCBlock *dcBlock1L;
    DCBlock *dcBlock1R;
    DCBlock *dcBlock2L;
    DCBlock *dcBlock2R;

    bool isChorus1Enabled;
    bool isChorus2Enabled;

    ChorusEngine(DspFloatType sampleRate) {
        dcBlock1L = new DCBlock();
        dcBlock1R = new DCBlock();
        dcBlock2L = new DCBlock();
        dcBlock2R = new DCBlock();

        setUpChorus(sampleRate);
        setEnablesChorus(true,true);
        setChorus1LfoRate(2.12);
        setChorus2LfoRate(2.25);
    }

    ~ChorusEngine() {
        delete chorus1L;
        delete chorus1R;
        delete chorus2L;
        delete chorus2R;
        delete dcBlock1L;
        delete dcBlock1R;
        delete dcBlock2L;
        delete dcBlock2R;
    }

    void setSampleRate(DspFloatType sampleRate) {
        delete chorus1L;
        delete chorus1R;
        delete chorus2L;
        delete chorus2R;
        setUpChorus(sampleRate);
    }

    void setEnablesChorus(bool isChorus1Enabled, bool isChorus2Enabled) {
        this->isChorus1Enabled = isChorus1Enabled;
        this->isChorus2Enabled = isChorus2Enabled;
    }

    void setChorus1LfoRate(DspFloatType rate) {
        chorus1L->setLfoRate(rate);
        chorus1R->setLfoRate(rate);
    }

    void setChorus2LfoRate(DspFloatType rate) {
        chorus2L->setLfoRate(rate);
        chorus2R->setLfoRate(rate);
    }

    void setUpChorus(DspFloatType sampleRate) {
        chorus1L = new Chorus(sampleRate, 1.0f, 0.5f, 7.0f);
        chorus1R = new Chorus(sampleRate, 0.0f, 0.5f, 7.0f);
        chorus2L = new Chorus(sampleRate, 0.0f, 0.83f, 7.0f);
        chorus2R = new Chorus(sampleRate, 1.0f, 0.83f, 7.0f);
    }

    inline void process(DspFloatType *sampleL, DspFloatType *sampleR) {
        DspFloatType resultR = 0.0f;
        DspFloatType resultL = 0.0f;
        if (isChorus1Enabled)
        {
            resultL += chorus1L->process(sampleL);
            resultR += chorus1R->process(sampleR);
            dcBlock1L->tick(&resultL, 0.01f);
            dcBlock1R->tick(&resultR, 0.01f);
        }
        if (isChorus2Enabled)
        {
            resultL += chorus2L->process(sampleL);
            resultR += chorus2R->process(sampleR);
            dcBlock2L->tick(&resultL, 0.01f);
            dcBlock2R->tick(&resultR, 0.01f);
        }
        *sampleL = *sampleL + resultL * 1.4f;
        *sampleR = *sampleR + resultR * 1.4f;
    }
    void ProcessBlock(size_t numSamples, DspFloatType ** inputs, DspFloatType ** outputs)
    {
        for(size_t i = 0; i < numSamples; i++)
        {
            DspFloatType L = inputs[0][i];
            DspFloatType R = inputs[1][i];
            process(&L,&R);
            outputs[0][i] = L;
            outputs[1][i] = R;
        }
    }
};
}