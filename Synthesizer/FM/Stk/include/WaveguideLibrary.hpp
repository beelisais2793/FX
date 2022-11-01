// Waveguide LIbrary
#pragma once
#include <algorithm>
#include <cmath>
#include <array>
#include <vector>
#include <cassert>
#include <functional>
/*
 
 Maths
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    static constexpr float pi = 3.14159265358979f;
    static constexpr float twoPi = pi * 2.0f;
    static constexpr float min3dBAmp = 0.7079457844f;
    
    template <int N>
    struct ConstexprPow2f
    {
        static constexpr int value_ = 2.0f * ConstexprPow2f <N-1>::value_;
    };
    
    template<>
    struct ConstexprPow2f <0>
    {
        static constexpr int value_ = 1;
    };
    
    // Credits to Martijn Zwartjes
    inline void cancelDenormals (float& f)
    {
        f += 1E-18;
    }
    
    // From Reaktor
    inline float powXGrt0ToY (float x, float y)
    {
        return std::exp2f (std::log2f (x) * y);
    }
    
    static constexpr float c0Freq_ = 8.1757989156f;
    
    inline float pitchToFreq (float p)
    {
        return std::exp2f (p / 12.0f) * c0Freq_;
    }
    
    inline float freqToPitch (float f)
    {
        return 12.0f * std::log2f (f / c0Freq_);
    }
    
    inline float decibelToAmp (float dB, float minDB = -100.0f)
    {
        if (dB <= minDB)
            return 0.0f;
        static float a = std::log2f (10.0f);
        return std::exp2f (a * dB * 0.05f); // see powXGrt0toY
    }
    
    inline float ampToDecibel (float amp, float minDB = -100.0f)
    {
        if (amp > 0.0f)
            return std::max (minDB, std::log10f (amp) * 20.0f);
        return minDB;
    }
    
    inline float getRMSAmp (float* data, int num)
    {
        assert (num > 0);
        float sum = 0.0f;
        for (int i = 0; i < num; i++)
            sum += data[i] * data[i];
        return std::sqrtf (sum / num);
    }
    
    inline float getRMSDB (float* data, int num)
    {
        return ampToDecibel (getRMSAmp (data, num));
    }
    
    inline float getPeakAmp (float* data, int num)
    {
        float max = 0.0f;
        for (int i = 0; i < num; i++)
            max = std::max (max, std::fabsf (data[i]));
        return max;
    }
    
    inline float getPeakDB (float* data, int num)
    {
        return ampToDecibel (getPeakAmp (data, num));
    }
    
    // https://en.wikibooks.org/wiki/Digital_Signal_Processing/Bilinear_Transform
    inline float bilinearTransformOpt (float f, float piDivSr)
    {
        float a = f * piDivSr;
        assert (a >= 0.0f && a <= 0.5f * pi);
        return std::tanf (f * piDivSr);
    }
    
    // pi / sr can be calculated once at sr change, then use the opt version
    inline float bilinearTransform (float f, float sr)
    {
        return bilinearTransformOpt (f, pi / sr);
    }
    
    template <typename T>
    inline const T& clamp (const T& value, const T& min, const T& max)
    {
        assert (min <= max);
        return value < min ? min : value > max ? max : value;
    }
    
    template <typename T>
    inline T map (T input, T inputMin, T inputMax, T targetMin, T targetMax)
    {
        return targetMin + (targetMax - targetMin) * ((input - inputMin) / (inputMax - inputMin));
    }
    
    
};

/*
 
 HermiteInterpolation
 Waveguide Library - JB Audio
 
 Description: Hermite interpolation
 https://www.musicdsp.org/en/latest/Other/93-hermite-interpollation.html?highlight=hermite
 
 */




namespace jbaudio
{
    class HermiteInterpolation
    {
    public:
        static constexpr float calculatePointV1 (float x, float y0, float y1, float y2, float y3)
        {
            float c0 = y1;
            float c1 = 0.5f * (y2 - y0);
            float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
            float c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr float calculatePointV2 (float x, float y0, float y1, float y2, float y3)
        {
            float c0 = y1;
            float c1 = 0.5f * (y2 - y0);
            float c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            float c2 = y0 - y1 + c1 - c3;
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr float calculatePointV3 (float x, float y0, float y1, float y2, float y3)
        {
            float c0 = y1;
            float c1 = 0.5f * (y2 - y0);
            float y0my1 = y0 - y1;
            float c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
            float c2 = y0my1 + c1 - c3;
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr float calculatePointV4 (float frac_pos, float xm1, float x0, float x1, float x2)
        {
            const float    c     = (x1 - xm1) * 0.5f;
            const float    v     = x0 - x1;
            const float    w     = c + v;
            const float    a     = w + v + (x2 - x0) * 0.5f;
            const float    b_neg = w + a;
            
            return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
        }
        
    private:
    };
}



/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 OnePoleZDF
 Waveguide Library - JB Audio
 
 */



namespace jbaudio
{
    class OnePoleZDF
    {
    public:
        OnePoleZDF()
        {
            setSampleRate (44100.0f);
        }
        
        inline void reset()
        {
            z1_ = 0.0f;
        }
        
        inline void setSampleRate (float sr)
        {
            sampleRate_ = sr;
            setFreqClipped (freq_);
        }
        
        inline void setFreq (float f)
        {
            freq_ = f;
            float a = bilinearTransform (f, sampleRate_);
            g_ = a / (1.0f + a);
        }
        
        inline void setFreqClipped (float f)
        {
            setFreq (clamp (f, 0.0f, sampleRate_ * 0.49f));
        }
        
        inline float tickLP (float input)
        {
            auto a = (input - z1_) * g_;
            auto b = a + z1_;
            z1_ = a + b;
            cancelDenormals (z1_);
            return b;
        }
        
        inline float tickHP (float input)
        {
            return input - tickLP (input);
        }
        
        inline float getPhaseDelayLP (float freq)
        {
            float a = freq / freq_;
            float re = 1.0f / (a * a + 1.0f);
            float im = -(a * re);
            
            return std::atan2f (im, re);
        }
        
    private:
        float freq_ = 1000.0f;
        float g_;
        float sampleRate_;
        
        float z1_ = 0.0f;
    };
    
    class OnePoleZDFHighShelf
    {
    public:
        OnePoleZDFHighShelf()
        {
            setFreqAndGainDB (2000.0f, 0.0f);
        }
        
        void setSampleRate (float sr)
        {
            onePole_.setSampleRate (sr);
        }
        
        inline void reset()
        {
            onePole_.reset();
        }
        
        inline void setFreqAndGainDB (float f, float dB)
        {
            setFreqAndGainAmp (f, decibelToAmp (dB));
        }
        
        inline void setFreqAndGainAmp (float f, float g)
        {
            gain_ = g - 1.0f;
            onePole_.setFreq (f * std::sqrtf (g));
        }

        inline float tick (float input)
        {
            return input + onePole_.tickHP (input) * gain_;
        }
        
    private:
        OnePoleZDF onePole_;
        float gain_;
    };
    
    class OnePoleZDFLowShelf
    {
    public:
        OnePoleZDFLowShelf()
        {
            setFreqAndGainDB (2000.0f, 0.0f);
        }
        
        void setSampleRate (float sr)
        {
            onePole_.setSampleRate (sr);
        }
        
        inline void reset()
        {
            onePole_.reset();
        }
        
        inline void setFreqAndGainDB (float f, float dB)
        {
            setFreqAndGainAmp (f, decibelToAmp (dB));
        }
        
        inline void setFreqAndGainAmp (float f, float g)
        {
            gain_ = g - 1.0f;
            onePole_.setFreq (f / std::sqrtf (g));
        }
        
        inline float tick (float input)
        {
            return input + onePole_.tickLP (input) * gain_;
        }
        
    private:
        OnePoleZDF onePole_;
        float gain_;
    };
}

/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 OneZero
 Waveguide Library - JB Audio
 
 */

// TODO
namespace jbaudio
{
    class OneZero
    {
    public:
        OneZero()
        {
            setSampleRate (44100.0f);
            setFreq (22040.0f);
        }
        
        inline void reset()
        {
            z1_ = 0.0f;
        }
        
        inline void setSampleRate (float sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setFreq (float freq)
        {
            assert (false); // not working yet
            assert (freq = clampFreq (freq));
            b1_ = std::sinf (freq * twoPi / sampleRate_) * 0.5f;
            b0_ = 1.0f - b1_;
        }
        
        inline void setFreqClipped (float freq)
        {
            setFreq (clampFreq (freq));
        }
        
        inline float clampFreq (float freq) const
        {
            return clamp (freq, getMinFreq(), getMaxFreq());
        }
        
        // a one zero filter cannot have a cutoff freq smaller than samplerate / 4
        inline float getMinFreq() const
        {
            return sampleRate_ * 0.25f;
        }
        
        inline float getMaxFreq() const
        {
            return sampleRate_ * 0.5f;
        }
        
        inline float tickLP (float input)
        {
            return 0.0f;
        }
        
        inline float tickHP (float input)
        {
            return input - tickLP (input);
        }
        
    private:
        float sampleRate_;
        float z1_;
        float b0_;
        float b1_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 OnePoleZDF
 Waveguide Library - JB Audio
 
 */

namespace jbaudio
{
    // Waveguides can create dc offsets so this object can be used to get rid of the dc offset
    // It's basically a highpass filter with a very low freq
    class DCBlock
    {
    public:
        DCBlock()
        {
            setSampleRate (44100.0f);
        }
        
        inline void reset()
        {
            onepole_.reset();
        }
        
        inline void setSampleRate (float sr)
        {
            onepole_.setSampleRate (sr);
            onepole_.setFreq (10.0f);
        }
        
        inline float tick (float input)
        {
            return onepole_.tickHP (input);
        }
        
    private:
        OnePoleZDF onepole_;
    };
}

/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 SVF
 Waveguide Library - JB Audio
 
 */



namespace jbaudio
{
    class SVF
    {
    public:
        SVF()
        {
            setSampleRate (44100.0f);
            setFreqAndRes (1000.0f, 0.5f);
            reset();
        }
        
        inline void reset()
        {
            buf1_ = 0.0f;
            buf2_ = 0.0f;
            lastLP_ = 0.0f;
            lastBP_ = 0.0f;
            lastHP_ = 0.0f;
        }
        
        inline void setSampleRate (float sr)
        {
            sampleRate_ = sr;
        }
        
        // it's more efficient to combine the two than calling them seperately
        inline void setFreqAndRes (float f, float r)
        {
            wDiv2_ = bilinearTransform (f, sampleRate_);
            r = std::min (r, 0.96875f);
            r = 1.0f - r;
            twoR_ = r + r;
            resOrFreqChanged();
        }
        
        inline void setFreq (float f)
        {
            wDiv2_ = bilinearTransform (f, sampleRate_);
            resOrFreqChanged();
        }
        
        inline void setRes (float r)
        {
            assert (r <= 0.96875f);
            r = 1.0f - r;
            twoR_ = r + r;
            resOrFreqChanged();
        }
        
        inline void tick (float input)
        {
            float a = buf1_ * d_ + buf2_;
            lastHP_ = (input - a) / a_;
            
            float b = lastHP_ * wDiv2_;
            lastBP_ = b + buf1_;
            buf1_ = lastBP_ + b;
            
            float c = lastBP_ * wDiv2_;
            lastLP_ = c + buf2_;
            buf2_ = lastLP_ + c;
        }
        
        inline float tickLP (float input)
        {
            tick (input);
            return lastLP_;
        }
        
        inline float tickBP (float input)
        {
            tick (input);
            return lastBP_;
        }
        
        inline float tickHP (float input)
        {
            tick (input);
            return lastHP_;
        }
        
        inline float getLastLP() const { return lastLP_; }
        inline float getLastBP() const { return lastBP_; }
        inline float getLastHP() const { return lastHP_; }
        
        inline float getPhaseDelayLP (float freq)
        {
            assert (false); // todo
            return 0.0f;
        }
        
    private:
        float sampleRate_;
        
        float wDiv2_;
        float twoR_;
        float d_;
        float a_;
        
        float buf1_;
        float buf2_;
        
        float lastLP_;
        float lastBP_;
        float lastHP_;
        
        inline void resOrFreqChanged()
        {
            d_ = twoR_ + wDiv2_;
            a_ = d_ * wDiv2_ + 1.0f;
        }
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 LogSmoother
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    // Based on Reaktor Env Follower
    class EnvFollower
    {
    public:
        EnvFollower()
        {
            setSampleRate (44100.0f);
            setAttackTime (0.01f);
            setReleaseTime (0.05f);
            reset();
        }
        
        inline void reset()
        {
            z1_ = 0.0f;
        }
        
        void setSampleRate (float sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackTime (float attackS)
        {
            attCoeff_ = time2Cutoff (attackS);
        }
        
        inline void setReleaseTime (float releaseS)
        {
            relCoeff_ = time2Cutoff (releaseS);
        }
        
        inline float tick (float input)
        {
            return tickGrt0 (std::fabsf (input));
        }
        
        inline float tickGrt0 (float input)
        {
            assert (input >= 0.0f);
            float diff = input - z1_;
            cancelDenormals (diff);
            return z1_ = z1_ + diff * (input > z1_ ? attCoeff_ : relCoeff_);
        }
        
    private:
        float sampleRate_;
        
        float z1_;
        
        float attCoeff_;
        float relCoeff_;
        
        inline float time2Cutoff (float seconds)
        {
            return 1.0f - std::exp2f (-1.0f / std::max (0.02f, sampleRate_ * seconds));
        }
    };
}

/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 LogSmoother
 Waveguide Library - JB Audio
 
 */



namespace jbaudio
{
    // Based on Reaktor Env Follower
    class LogSmoother
    {
    public:
        LogSmoother (float initialValue = 0.0f)
        {
            setSampleRate (44100.0f);
            setAttackTime (0.01f);
            setReleaseTime (0.05f);
            setTarget (initialValue, true);
        }
        
        inline void reset()
        {
            z1_ = target_;
        }
        
        void setSampleRate (float sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackAndReleaseTime (float seconds)
        {
            setAttackTime (seconds);
            setReleaseTime (seconds);
        }
        
        inline void setAttackAndReleaseTime (float attackS, float releaseS)
        {
            setAttackTime (attackS);
            setReleaseTime (releaseS);
        }
        
        inline void setAttackTime (float attackS)
        {
            attCoeff_ = time2Cutoff (attackS);
        }
        
        inline void setReleaseTime (float releaseS)
        {
            relCoeff_ = time2Cutoff (releaseS);
        }
        
        inline void setTarget (float t, bool force = false)
        {
            target_ = t;
            if (force)
                z1_ = t;
            coeff_ = target_ > z1_ ? &attCoeff_ : &relCoeff_;
        }
        
        inline float getTarget() const
        {
            return target_;
        }
        
        inline float tick()
        {
            float diff = target_ - z1_;
            cancelDenormals (diff);
            return z1_ = z1_ + diff * (*coeff_);
        }
        
    private:
        float sampleRate_;
        
        float target_ = 0.0f;
        float z1_;
        
        float attCoeff_;
        float relCoeff_;
        float* coeff_ { &attCoeff_ };
        
        inline float time2Cutoff (float seconds)
        {
            return 1.0f - std::exp2f (-1.0f / std::max (0.02f, sampleRate_ * seconds));
        }
    };
}

/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 SmootherLin
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    // based on juce LinearSmoothedValue
    class SmootherLin
    {
    public:
        SmootherLin (float initialValue = 0.0f)
        {
            setSampleRate (44100.0f);
            setTime (0.01f);
            
            setTargetForced (initialValue);
        }
        
        inline void reset()
        {
            setTargetForced (target_);
        }
        
        void setSampleRate (float sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setTime (float seconds)
        {
            setNumSteps (int (seconds * sampleRate_));
        }
        
        inline void setNumSteps (int numSteps)
        {
            assert (numSteps >= 1);
            numSteps_ = numSteps;
            numStepsInv_ = 1.0f / numSteps_;
        }
        
        inline void setTarget (float t)
        {
            if (target_ != t)
            {
                target_ = t;
                incr_ = (target_ - currentValue_) * numStepsInv_;
                counter_ = numSteps_;
            }
        }
        
        inline void setTargetForced (float t)
        {
            currentValue_ = target_ = t;
            counter_ = 0;
        }
        
        inline float getTarget() const
        {
            return target_;
        }
        
        inline float tick()
        {
            if (counter_ > 0)
            {
                currentValue_ += incr_;
                counter_--;
                return currentValue_;
            }
            else return target_;
        }
        
        inline bool isSmoothing() const
        {
            return counter_ > 0;
        }
        
        inline float getCurrentValue() const
        {
            return currentValue_;
        }
        
    private:
        float sampleRate_;
        
        float currentValue_;
        float target_;
        
        int counter_;
        int numSteps_;
        float numStepsInv_;
        float incr_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 DelayHermite
 Waveguide Library - JB Audio
 
 */





namespace jbaudio
{
    class DelayHermite
    {
    public:
        DelayHermite()
        {
            setMaxSize (16);
        }
        
        // ================================================================================
        inline void reset()
        {
            std::fill (array_.begin(), array_.end(), 0.0f);
        }
        
        // ================================================================================
        void setMaxSize (int powerOf2)
        {
            int size = std::max ((int)std::exp2f (powerOf2), 4);
            array_.resize ((size_t)size, 0.0f);
            reset();
            mask_ = size - 1;
            writeIndex_ = 0;
        }
        
        // ================================================================================
        inline void push (float sample)
        {
            array_ [size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline float get (float samplesDelay) const
        {
            assert (samplesDelay == clampLength (samplesDelay));
            const int asInt = (int) samplesDelay;
            const float x = samplesDelay - asInt;
            const float y0 = array_ [size_t ((writeIndex_ + asInt - 1) & mask_)];
            const float y1 = array_ [size_t ((writeIndex_ + asInt) & mask_)];
            const float y2 = array_ [size_t ((writeIndex_ + asInt + 1) & mask_)];
            const float y3 = array_ [size_t ((writeIndex_ + asInt + 2) & mask_)];
            
            return HermiteInterpolation::calculatePointV4 (x, y0, y1, y2, y3);
        }
        
        inline float getClipped (float samplesDelay) const
        {
            return get (clampLength (samplesDelay));
        }
        
        // ================================================================================
        inline int getMinDelayLengthSamples() const
        {
            return 2;
        }
        
        inline int getMaxDelayLengthSamples() const
        {
            return (int)array_.size() - 3;
        }
        
        inline float clampLength (float samplesDelay) const
        {
            return clamp (samplesDelay, (float)getMinDelayLengthSamples(), (float)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <float> array_;
        int mask_;
        int writeIndex_ = 0;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 DelayLin
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    class DelayLin
    {
    public:
        DelayLin()
        {
            setMaxSize (16);
        }
        
        // ================================================================================
        inline void reset()
        {
            std::fill (array_.begin(), array_.end(), 0.0f);
        }
        
        // ================================================================================
        void setMaxSize (int powerOfTwo)
        {
            assert (powerOfTwo >= 0);
            array_.resize ((size_t)std::exp2f (powerOfTwo), 0.0f);
            reset();
            mask_ = (int)array_.size() - 1;
            writeIndex_ = 0;
        }
        
        // ================================================================================
        inline void push (float sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline float get (float samplesDelay) const
        {
            assert (clampLength (samplesDelay) == samplesDelay);
            
            int delayInt = (int)samplesDelay;
            float delayFract = samplesDelay - delayInt;
            
            const int lower = (writeIndex_ + delayInt) & mask_;
            const int upper = (lower + 1) & mask_;
            
            return array_[(size_t)lower] + (array_[(size_t)upper] - array_[(size_t)lower]) * delayFract;
        }
        
        inline float getClipped (float samplesDelay) const
        {
            return get (clampLength (samplesDelay));
        }
        
        // ================================================================================
        inline int getMinDelayLengthSamples() const
        {
            return 1;
        }
        
        inline int getMaxDelayLengthSamples() const
        {
            return (int)array_.size();
        }
        
        inline float clampLength (float samplesDelay) const
        {
            return clamp (samplesDelay, (float)getMinDelayLengthSamples(), (float)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <float> array_;
        int mask_;
        int writeIndex_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 DelayAP
 Waveguide Library - JB Audio
 
 */
namespace jbaudio
{
    // https://ccrma.stanford.edu/~jos/pasp/First_Order_Allpass_Interpolation.html
    class DelayAP
    {
    public:
        DelayAP()
        {
            setMaxSize (16);
        }
        
        // ================================================================================
        inline void reset()
        {
            std::fill (array_.begin(), array_.end(), 0.0f);
            apZ1_ = 0.0f;
        }
        
        // ================================================================================
        void setMaxSize (int powerOfTwo)
        {
            assert (powerOfTwo >= 0);
            array_.resize ((size_t)std::exp2f (powerOfTwo), 0.0f);
            reset();
            mask_ = (int)array_.size() - 1;
            writeIndex_ = 0;
        }
        
        // ================================================================================
        inline void push (float sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline float get (float samplesDelay)
        {
            assert (samplesDelay == clampLength (samplesDelay));
            int apM = (int)samplesDelay;
            float fract = samplesDelay - apM;
            if (fract < 0.1f)
            {
                fract += 1.0f; // delta range of [0.1, 1.1]
                apM -= 1;
            }
            const float apN = (1.0f - fract) / (1.0f + fract);
            
            const float a = array_[size_t ((writeIndex_ + apM) & mask_)];
            
            const float b = apZ1_ * -apN + a;
            const float output = b * apN + apZ1_;
            apZ1_ = b;
            return output;
        }
        
        inline float getClipped (float samplesDelay)
        {
            return get (clampLength (samplesDelay));
        }
        
        // ================================================================================
        inline int getMinDelayLengthSamples() const
        {
            return 1;
        }
        
        inline int getMaxDelayLengthSamples() const
        {
            return (int)array_.size();
        }
        
        inline float clampLength (float samplesDelay) const
        {
            return clamp (samplesDelay, (float)getMinDelayLengthSamples(), (float)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <float> array_;
        
        int mask_;
        int writeIndex_;
        
        float apZ1_ = 0.0f;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 DelayNoInt
 Waveguide Library - JB Audio
 
 Description: Delay without interpolation
 
 */


namespace jbaudio
{
    class DelayNoInt
    {
    public:
        DelayNoInt()
        {
            setMaxSize (16);
        }
        
        // ================================================================================
        inline void reset()
        {
            std::fill (array_.begin(), array_.end(), 0.0f);
        }
        
        // ================================================================================
        void setMaxSize (int powerOfTwo)
        {
            assert (powerOfTwo >= 0);
            array_.resize ((size_t)std::exp2f (powerOfTwo), 0.0f);
            reset();
            mask_ = (int)array_.size() - 1;
            writeIndex_ = 0;
        }
        
        // ================================================================================
        inline void push (float sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline float get (int samplesDelay) const
        {
            assert (samplesDelay == clampLength (samplesDelay));
            return array_ [size_t ((writeIndex_ + samplesDelay) & mask_)];
        }
        
        inline float getClipped (int samplesDelay) const
        {
            return get (clampLength (samplesDelay));
        }
        
        // ================================================================================
        inline int getMinDelayLengthSamples() const
        {
            return 1;
        }
        
        inline int getMaxDelayLengthSamples() const
        {
            return (int)array_.size();
        }
        
        inline int clampLength (int samples) const
        {
            return clamp (samples, getMinDelayLengthSamples(), getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <float> array_;
        int mask_;
        int writeIndex_;
    };
}



/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 ADSR
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    class ADSR
    {
    public:
        enum class Stage
        {
            Idle,
            Attack,
            Decay,
            Release
        };
        
        ADSR()
        {
            reset();
            setSampleRate (44100.0f);
            setAttackTime (0.01f);
            setDecayTime (0.05f);
            setSustainLevel (0.7f);
            setReleaseTime (0.1f);
        }
        
        inline void reset()
        {
            stage_ = Stage::Idle;
            value_ = 0.0f;
        }
        
        void setSampleRate (float sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackTime (float seconds)
        {
            attackIncr_ = 1.0f / std::max (1.0f, seconds * sampleRate_);
        }
        
        inline void setDecayTime (float seconds)
        {
            decayMult_ = powXGrt0ToY (0.5f, 1.0f / (std::max (1.0f, seconds * sampleRate_)));
        }
        
        inline void setSustainLevel (float level0to1)
        {
            assert (level0to1 >= 0.0f && level0to1 <= 1.0f);
            sustainLevel_ = level0to1;
            sustainLevelScaled_ = sustainLevel_ * velocity_;
        }
        
        inline void setReleaseTime (float seconds)
        {
            releaseMult_ = powXGrt0ToY (0.5f, 1.0f / (std::max (1.0f, seconds * sampleRate_)));
        }
        
        inline void triggerGate (float velocity)
        {
            if (velocity > 0.0f)
            {
                velocity_ = velocity;
                sustainLevelScaled_ = sustainLevel_ * velocity_;
                // start
                if (value_ > velocity)
                    stage_ = Stage::Decay;
                else
                    stage_ = Stage::Attack;
            }
            else
            {
                // stop
                stage_ = Stage::Release;
            }
        }
        
        inline float tick()
        {
            switch (stage_)
            {
                case Stage::Attack:
                    value_ += attackIncr_;
                    if (value_ >= velocity_)
                    {
                        value_ = velocity_;
                        stage_ = Stage::Decay;
                    }
                    break;
                    
                case Stage::Decay:
                {
                    float diff = (value_ - sustainLevelScaled_);
                    cancelDenormals (diff);
                    value_ = sustainLevelScaled_ + diff * decayMult_;
                    break;
                }
                    
                case Stage::Release:
                    value_ *= releaseMult_;
                    if (value_ < 0.0001f)
                    {
                        value_ = 0.0f;
                        stage_ = Stage::Idle;
                    }
                    break;
                    
                case Stage::Idle:
                    break;
            };
            
            return value_;
        }
        
        inline Stage getStage() const
        {
            return stage_;
        }
        
    private:
        float sampleRate_;
        
        // params
        float attackIncr_;
        float decayMult_;
        float sustainLevel_;
        float releaseMult_;
        
        // state
        float velocity_;
        float sustainLevelScaled_;
        float value_;
        Stage stage_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 Compressor
 Waveguide Library - JB Audio
 
 */



namespace jbaudio
{
    // Thanks to the great paper "Digital Dynamic Range Compressor Design— A Tutorial and Analysis"
    class Compressor
    {
    public:
        class Helper
        {
        public:
            static constexpr float calculateOutput (float input, float threshold, float ratio)
            {
                if (input <= threshold)
                    return input;
                return threshold + (input - threshold) / ratio;
            }
            
            static constexpr float calculateOutput (float input, float threshold, float ratio, float knee)
            {
                // no reduction
                if ((input - threshold) <= -0.5f * knee)
                    return input;
                
                // full reduction
                if ((input - threshold) >= 0.5f * knee)
                    return threshold + (input - threshold) / ratio;
                
                // knee range
                // scale input to [0, knee]
                float a = input - threshold + knee * 0.5f;
                // quadratic as [0, knee * knee]
                a *= a;
                // quadratic as [0, knee * 0.5]
                a /= 2.0f * knee;
                
                return input + (1 / ratio - 1.0f) * a;
            }
        };
        
        Compressor()
        {
            setSampleRate (44100.0f);
            setThreshold (-10.0f);
            setRatio (3.0f);
            setKnee (5.0f);
            setAttackTime (0.01f);
            setReleaseTime (0.1f);
            reset();
        }
        
        inline void reset()
        {
            follower_.reset();
            lastReductionDB_ = 0.0f;
            lastReductionAmp_ = 1.0f;
        }
        
        void setSampleRate (float sr)
        {
            assert (sr > 0);
            follower_.setSampleRate (sr);
        }
        
        inline void setThreshold (float thresholdDB)
        {
            threshold_ = thresholdDB;
        }
        
        inline void setRatio (float r)
        {
            ratio_ = r;
        }
        
        inline void setKnee (float kneeDB)
        {
            knee_ = kneeDB;
        }
        
        inline void setAttackTime (float seconds)
        {
            follower_.setAttackTime (seconds);
        }
        
        inline void setReleaseTime (float seconds)
        {
            follower_.setReleaseTime (seconds);
        }
        
        inline float tick (float input)
        {
            const float asDB = ampToDecibel (std::fabsf (input));
            
            const float outDB = Helper::calculateOutput (asDB, threshold_, ratio_, knee_);
            const float reductionDB = asDB - outDB;
            
            lastReductionDB_ = -follower_.tickGrt0 (reductionDB);
            lastReductionAmp_ = decibelToAmp (lastReductionDB_);
            
            return input * lastReductionAmp_;
        }
        
        inline float getLastReductionDB() const
        {
            return lastReductionDB_;
        }
        
        inline float getLastReductionAmp() const
        {
            return lastReductionAmp_;
        }
        
    private:
        EnvFollower follower_;
        float threshold_;
        float ratio_;
        float knee_;
        
        float lastReductionDB_;
        float lastReductionAmp_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 LagrangeInterpolation
 Waveguide Library - JB Audio
 
 Description: Lagrange interpolation
 https://en.wikipedia.org/wiki/Lagrange_polynomial
 https://ccrma.stanford.edu/~jos/Interpolation/Lagrange_Interpolation.html
 
 */


namespace jbaudio
{
    class LagrangeInterpolation
    {
    public:
        // e.g calculatePoint ({1, 4, 9}, 2.5) returns 2.5 ^ 2 = 6,25
        template <int N>
        static inline float calculatePoint (std::array <float, N> points, float p)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    if (i != j)
                    {
                        points[i] *= (p - (j + 1)) / ((i + 1) - (j + 1));
                    }
                }
            }
            float sum = points[0];
            for (int i = 1; i < N; i++)
                sum += points[i];
            return sum;
        }
        
    private:
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 LookupTable
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    class LookupTable
    {
    public:
        LookupTable() = default;
        
        void reset() { data_.clear(); }
        
        inline bool isValid() const { return getTableSize() >= 2; }
        
        // make sure to call this from a non-real time thread cause this will allocate
        void init (std::function <float (size_t index)> func, size_t tableSize)
        {
            if (func != nullptr && tableSize >= 2)
            {
                data_.resize (tableSize);
                for (size_t i = 0; i < tableSize; i++)
                {
                    data_[i] = func (i);
                }
            }
            else assert (false);
        }
        
        inline size_t getTableSize() const { return data_.size(); }
        
        bool isWithinRange (float index) const { return index >= 0.0f && index <= data_.size() - 1; }
        
        inline float mapToTable (float f, float min, float max)
        {
            assert (isValid());
            return (data_.size() - 1) * ((f - min) / (max - min));
        }
        
        inline float mapToTableClipped (float f, float min, float max)
        {
            assert (isValid());
            return clamp (mapToTable (f, min, max), 0.0f, float (data_.size() - 1) - 0.0001f);
        }
        
        inline float get (float index) const
        {
            assert (isValid());
            assert (isWithinRange (index));
            
            const auto underIndex = int (index);
            const auto upperIndex = underIndex + 1;
            assert (isWithinRange (underIndex));
            assert (isWithinRange (upperIndex));
            
            const auto coeff = index - underIndex;
            return (1.0f - coeff) * data_[underIndex] + coeff * data_[upperIndex];
        }
        
        inline float getClipped (float index) const
        {
            assert (isValid());
            
            if (index < 0.0f) return data_[0];
            if (index >= data_.size() - 1) return data_ [data_.size() - 1];
            
            return get (index);
        }
        
    private:
        std::vector <float> data_;
    };
    
    class WaveShaperTable
    {
    public:
        WaveShaperTable()
        {
            init ([&] (float audio) { return tanhf (audio); }, 100);
        }
        
        void reset()
        {
            table_.reset();
        }
        
        void init (std::function <float (float audio)> func,
                   size_t tableSize,
                   float audioMin = -1.0f,
                   float audioMax = 1.0f)
        {
            if (func != nullptr)
            {
                table_.init (
                [func, tableSize, audioMin, audioMax] (size_t index)
                {
                    return func (map <float> (index, 0, tableSize - 1, audioMin, audioMax));
                },
                tableSize);
            }
        }
        
        
        inline float get (float audio)
        {
            assert (table_.isValid());
            return table_.get (table_.mapToTableClipped (audio, -1.0f, 1.0f));
        }
        
        const LookupTable& getLookupTable() const { return table_; }
        
    private:
        LookupTable table_;
        float audioMin_;
        float audioMax_;
    };
    
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 MouthPiece
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    // Simple single reed mouth piece
    class MouthPiece
    {
    public:
        MouthPiece()
        {
            setSampleRate (44100.0f);
            reset();
        }
        
        inline void reset()
        {
            pressure_.reset();
        }
        
        void setSampleRate (float sr)
        {
            if (sr > 0)
            {
                sampleRate_ = sr;
                pressure_.setSampleRate (sr);
                pressure_.setAttackAndReleaseTime (0.01f);
            }
        }
        
        inline float tick (float tubeFeedback)
        {
            // exciter
            float p = pressure_.tick();
            float a = tubeFeedback - p;
            const float output = p + a * (clamp (0.7f - 0.3f * a, -1.0f, 1.0f));
            return output;
        }
        
        inline void setPressure (float p)
        {
            pressure_.setTarget (shapePressure (clamp (p, 0.0f, 1.0f)));
        }
        
    private:
        float sampleRate_;
        
        jbaudio::LogSmoother pressure_;
        
        static inline float shapePressure (float p)
        {
            return std::powf (p, 0.2f);
            return p;
        }
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 OscSine
 Waveguide Library - JB Audio
 
 */


namespace jbaudio
{
    class OscSine
    {
    public:
        OscSine()
        {
            setSampleRate (44100.0f);
            setFreq (440.0f);
            setVolume (1.0f);
            reset();
        }
        
        void reset()
        {
            phase_ = 0.0f;
        }
        
        void setSampleRate (float sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setFreq (float f)
        {
            incr_ = (f / sampleRate_) * twoPi;
        }
        
        inline void setVolume (float v)
        {
            volume_ = v;
        }
        
        inline float tick()
        {
            phase_ += incr_;
            if (phase_ > twoPi)
                phase_ -= twoPi;
            return std::sinf (phase_) * volume_;
        }
        
        inline float getWithoutTicking()
        {
            return std::sinf (phase_) * volume_;
        }
        
    private:
        float sampleRate_;
        float incr_;
        float phase_;
        float volume_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 TransitionXFade
 Waveguide Library - JB Audio
 
 */


// Based on the method described in patent:
// https://patentimages.storage.googleapis.com/49/7f/81/c134de24dd69b8/WO1998010355A1.pdf
namespace jbaudio
{
    class TransitionXFade
    {
    public:
        enum class State { Idle, FadingToB, FadingToA };
        
        TransitionXFade()
        {
            setSampleRate (44100.0f);
            setTime (0.02f);
            reset();
        }
        
        inline void reset()
        {
            nextTapNeedsFade_ = false;
            if (state_ == State::FadingToB)
                xfadeValue_ = 1.0f;
            else if (state_ == State::FadingToA)
                xfadeValue_ = 0.0f;
            state_ = State::Idle;
            assert (tapValue_ == 0.0f || tapValue_ == 1.0f);
        }
        
        void setSampleRate (float sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setTime (float seconds)
        {
            assert (seconds > 0.0f;)
            incr_ = 1.0f / (seconds * sampleRate_);
        }
        
        // this class doesn't care what unit this is, it can be samples, pitch whatever it needs to x-fade between
        inline void gotoNextTapForced()
        {
            nextTapNeedsFade_ = false;
            setNextTap (target);
        }
        
        inline void gotoNextTap()
        {
            if (nextTapNeedsFade_)
            {
                if (xfadeValue_ == 0.0f)
                {
                    assert (state_ == State::Idle);
                    state_ == State::FadingToB;
                }
                else if (xfadeValue_ == 1.0f)
                {
                    assert (state_ == State::Idle);
                    state_ == State::FadingToA;
                }
                else
                {
                    // not finished yet.. try again later :)
                }
            }
            else
            {
                if (state_ != State::Idle)
                {
                    
                }
            }
        }
        
        inline void tick (float& tapAMultiplier, float& tapBMultiplier)
        {
            
        }
        
    private:
        float sampleRate_;
        
        State state_;
        
        bool nextTapNeedsFade_;
        
        float incr_;
        float xfadeValue_;
    };
}


/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 Tube
 Waveguide Library - JB Audio
 
 */

namespace jbaudio
{
    // Simple tube
    class Tube
    {
    public:
        Tube()
        {
            setSampleRate (44100.0f);
            reset();
        }
        
        inline void reset()
        {
            delay_.reset();
            lowpass_.reset();
            length_.reset();
            cutoff_.reset();
        }
        
        void setSampleRate (float sr)
        {
            if (sr > 0)
            {
                sampleRate_ = sr;
                lowpass_.setSampleRate (sr);
                length_.setSampleRate (sr);
                length_.setAttackAndReleaseTime (0.003f);
                cutoff_.setSampleRate (sr);
                cutoff_.setAttackAndReleaseTime (0.003f);
            }
        }
        
        inline void push (float input)
        {
            delay_.push (input);
        }
        
        inline float get()
        {
            float freq = jbaudio::pitchToFreq (length_.tick()); // de freq die je wilt horen
            float delayLen = sampleRate_ / freq; // de delay lengte voor die freq
            lowpass_.setFreq (cutoff_.tick());
            float phaseDel = lowpass_.getPhaseDelayLP (freq); // de phase delay voor die freq
            float samplesDel = delayLen * (-phaseDel / jbaudio::twoPi); // aantal samples delay voor die freq
            delayLen *= 0.5f; // omdat na 1 keer de delay de golf geinverteerd plaats vindt
            delayLen -= samplesDel; // compenseer voor phase delay lengte
            float pipeOut = delay_.getClipped (delayLen);
            pipeOut = lowpass_.tickLP (pipeOut);
            pipeOut *= -0.95f;
            return pipeOut;
        }
        
        inline void setLength (float pitch)
        {
            length_.setTarget (pitch);
        }
        
        inline void setCutoff (float freq)
        {
            cutoff_.setTarget (freq);
        }
        
    private:
        float sampleRate_;
        
        jbaudio::DelayHermite delay_;
        
        jbaudio::LogSmoother cutoff_ {5000.0f};
        jbaudio::OnePoleZDF lowpass_;
        
        jbaudio::LogSmoother length_ {60.0f};
        
    };
}

/*
 
 This file is part of the JB Audio Waveguide C++ Library
 
 =============================================
 MIT License
 
 Copyright (c) 2019 JB Audio
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 =============================================
 
 */

/*
 
 WaveShaper
 Waveguide Library - JB Audio
 
 */

namespace jbaudio
{
    class WaveShaper
    {
    public:
        // Laurent de Soras
        // http://www.musicdsp.org/en/latest/Effects/86-waveshaper-gloubi-boulga.html
        static float gloubiBoulga (float input)
        {
            const float x = input * 0.686306f;
            const float a = 1.0f + std::expf (std::sqrtf (std::fabs (x)) * -0.75f);
            return (std::expf (x) - std::expf (-x * a)) / (std::expf (x) + std::expf (-x));
        }
        
        // reaktor
        static float tanh (float input, float level)
        {
            assert (level > 0.0f);
            input /= level;
            input = std::tanhf (input); // todo: approximation
            input *= level;
            return input;
        }
        
    private:
    };
}