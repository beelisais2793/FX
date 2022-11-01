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
    static constexpr DspFloatType pi = 3.14159265358979f;
    static constexpr DspFloatType twoPi = pi * 2.0f;
    static constexpr DspFloatType min3dBAmp = 0.7079457844f;
    
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
    inline void cancelDenormals (DspFloatType& f)
    {
        f += 1E-18;
    }
    
    // From Reaktor
    inline DspFloatType powXGrt0ToY (DspFloatType x, DspFloatType y)
    {
        return std::exp2f (std::log2f (x) * y);
    }
    
    static constexpr DspFloatType c0Freq_ = 8.1757989156f;
    
    inline DspFloatType pitchToFreq (DspFloatType p)
    {
        return std::exp2f (p / 12.0f) * c0Freq_;
    }
    
    inline DspFloatType freqToPitch (DspFloatType f)
    {
        return 12.0f * std::log2f (f / c0Freq_);
    }
    
    inline DspFloatType decibelToAmp (DspFloatType dB, DspFloatType minDB = -100.0f)
    {
        if (dB <= minDB)
            return 0.0f;
        static DspFloatType a = std::log2(10.0f);
        return std::exp2f (a * dB * 0.05f); // see powXGrt0toY
    }
    
    inline DspFloatType ampToDecibel (DspFloatType amp, DspFloatType minDB = -100.0f)
    {
        if (amp > 0.0f)
            return std::max (minDB, std::log10(amp) * 20.0f);
        return minDB;
    }
    
    inline DspFloatType getRMSAmp (DspFloatType* data, int num)
    {
        assert (num > 0);
        DspFloatType sum = 0.0f;
        for (int i = 0; i < num; i++)
            sum += data[i] * data[i];
        return std::sqrt(sum / num);
    }
    
    inline DspFloatType getRMSDB (DspFloatType* data, int num)
    {
        return ampToDecibel (getRMSAmp (data, num));
    }
    
    inline DspFloatType getPeakAmp (DspFloatType* data, int num)
    {
        DspFloatType max = 0.0f;
        for (int i = 0; i < num; i++)
            max = std::max (max, std::fabs (data[i]));
        return max;
    }
    
    inline DspFloatType getPeakDB (DspFloatType* data, int num)
    {
        return ampToDecibel (getPeakAmp (data, num));
    }
    
    // https://en.wikibooks.org/wiki/Digital_Signal_Processing/Bilinear_Transform
    inline DspFloatType bilinearTransformOpt (DspFloatType f, DspFloatType piDivSr)
    {
        DspFloatType a = f * piDivSr;
        assert (a >= 0.0f && a <= 0.5f * pi);
        return std::tan(f * piDivSr);
    }
    
    // pi / sr can be calculated once at sr change, then use the opt version
    inline DspFloatType bilinearTransform (DspFloatType f, DspFloatType sr)
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
        static constexpr DspFloatType calculatePointV1 (DspFloatType x, DspFloatType y0, DspFloatType y1, DspFloatType y2, DspFloatType y3)
        {
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr DspFloatType calculatePointV2 (DspFloatType x, DspFloatType y0, DspFloatType y1, DspFloatType y2, DspFloatType y3)
        {
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType c3 = 1.5f * (y1 - y2) + 0.5f * (y3 - y0);
            DspFloatType c2 = y0 - y1 + c1 - c3;
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr DspFloatType calculatePointV3 (DspFloatType x, DspFloatType y0, DspFloatType y1, DspFloatType y2, DspFloatType y3)
        {
            DspFloatType c0 = y1;
            DspFloatType c1 = 0.5f * (y2 - y0);
            DspFloatType y0my1 = y0 - y1;
            DspFloatType c3 = (y1 - y2) + 0.5f * (y3 - y0my1 - y2);
            DspFloatType c2 = y0my1 + c1 - c3;
            
            return ((c3 * x + c2) * x + c1) * x + c0;
        }
        
        static constexpr DspFloatType calculatePointV4 (DspFloatType frac_pos, DspFloatType xm1, DspFloatType x0, DspFloatType x1, DspFloatType x2)
        {
            const DspFloatType    c     = (x1 - xm1) * 0.5f;
            const DspFloatType    v     = x0 - x1;
            const DspFloatType    w     = c + v;
            const DspFloatType    a     = w + v + (x2 - x0) * 0.5f;
            const DspFloatType    b_neg = w + a;
            
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
        
        inline void setSampleRate (DspFloatType sr)
        {
            sampleRate_ = sr;
            setFreqClipped (freq_);
        }
        
        inline void setFreq (DspFloatType f)
        {
            freq_ = f;
            DspFloatType a = bilinearTransform (f, sampleRate_);
            g_ = a / (1.0f + a);
        }
        
        inline void setFreqClipped (DspFloatType f)
        {
            setFreq (clamp (f, 0.0f, sampleRate_ * 0.49f));
        }
        
        inline DspFloatType tickLP (DspFloatType input)
        {
            auto a = (input - z1_) * g_;
            auto b = a + z1_;
            z1_ = a + b;
            cancelDenormals (z1_);
            return b;
        }
        
        inline DspFloatType tickHP (DspFloatType input)
        {
            return input - tickLP (input);
        }
        
        inline DspFloatType getPhaseDelayLP (DspFloatType freq)
        {
            DspFloatType a = freq / freq_;
            DspFloatType re = 1.0f / (a * a + 1.0f);
            DspFloatType im = -(a * re);
            
            return std::atan2 (im, re);
        }
        
    private:
        DspFloatType freq_ = 1000.0f;
        DspFloatType g_;
        DspFloatType sampleRate_;
        
        DspFloatType z1_ = 0.0f;
    };
    
    class OnePoleZDFHighShelf
    {
    public:
        OnePoleZDFHighShelf()
        {
            setFreqAndGainDB (2000.0f, 0.0f);
        }
        
        void setSampleRate (DspFloatType sr)
        {
            onePole_.setSampleRate (sr);
        }
        
        inline void reset()
        {
            onePole_.reset();
        }
        
        inline void setFreqAndGainDB (DspFloatType f, DspFloatType dB)
        {
            setFreqAndGainAmp (f, decibelToAmp (dB));
        }
        
        inline void setFreqAndGainAmp (DspFloatType f, DspFloatType g)
        {
            gain_ = g - 1.0f;
            onePole_.setFreq (f * std::sqrt (g));
        }

        inline DspFloatType tick (DspFloatType input)
        {
            return input + onePole_.tickHP (input) * gain_;
        }
        
    private:
        OnePoleZDF onePole_;
        DspFloatType gain_;
    };
    
    class OnePoleZDFLowShelf
    {
    public:
        OnePoleZDFLowShelf()
        {
            setFreqAndGainDB (2000.0f, 0.0f);
        }
        
        void setSampleRate (DspFloatType sr)
        {
            onePole_.setSampleRate (sr);
        }
        
        inline void reset()
        {
            onePole_.reset();
        }
        
        inline void setFreqAndGainDB (DspFloatType f, DspFloatType dB)
        {
            setFreqAndGainAmp (f, decibelToAmp (dB));
        }
        
        inline void setFreqAndGainAmp (DspFloatType f, DspFloatType g)
        {
            gain_ = g - 1.0f;
            onePole_.setFreq (f / std::sqrt (g));
        }
        
        inline DspFloatType tick (DspFloatType input)
        {
            return input + onePole_.tickLP (input) * gain_;
        }
        
    private:
        OnePoleZDF onePole_;
        DspFloatType gain_;
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
        
        inline void setSampleRate (DspFloatType sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setFreq (DspFloatType freq)
        {
            assert (false); // not working yet
            assert (freq = clampFreq (freq));
            b1_ = std::sin (freq * twoPi / sampleRate_) * 0.5f;
            b0_ = 1.0f - b1_;
        }
        
        inline void setFreqClipped (DspFloatType freq)
        {
            setFreq (clampFreq (freq));
        }
        
        inline DspFloatType clampFreq (DspFloatType freq) const
        {
            return clamp (freq, getMinFreq(), getMaxFreq());
        }
        
        // a one zero filter cannot have a cutoff freq smaller than samplerate / 4
        inline DspFloatType getMinFreq() const
        {
            return sampleRate_ * 0.25f;
        }
        
        inline DspFloatType getMaxFreq() const
        {
            return sampleRate_ * 0.5f;
        }
        
        inline DspFloatType tickLP (DspFloatType input)
        {
            return 0.0f;
        }
        
        inline DspFloatType tickHP (DspFloatType input)
        {
            return input - tickLP (input);
        }
        
    private:
        DspFloatType sampleRate_;
        DspFloatType z1_;
        DspFloatType b0_;
        DspFloatType b1_;
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
        
        inline void setSampleRate (DspFloatType sr)
        {
            onepole_.setSampleRate (sr);
            onepole_.setFreq (10.0f);
        }
        
        inline DspFloatType tick (DspFloatType input)
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
        
        inline void setSampleRate (DspFloatType sr)
        {
            sampleRate_ = sr;
        }
        
        // it's more efficient to combine the two than calling them seperately
        inline void setFreqAndRes (DspFloatType f, DspFloatType r)
        {
            wDiv2_ = bilinearTransform (f, sampleRate_);
            r = std::min (r, 0.96875f);
            r = 1.0f - r;
            twoR_ = r + r;
            resOrFreqChanged();
        }
        
        inline void setFreq (DspFloatType f)
        {
            wDiv2_ = bilinearTransform (f, sampleRate_);
            resOrFreqChanged();
        }
        
        inline void setRes (DspFloatType r)
        {
            assert (r <= 0.96875f);
            r = 1.0f - r;
            twoR_ = r + r;
            resOrFreqChanged();
        }
        
        inline void tick (DspFloatType input)
        {
            DspFloatType a = buf1_ * d_ + buf2_;
            lastHP_ = (input - a) / a_;
            
            DspFloatType b = lastHP_ * wDiv2_;
            lastBP_ = b + buf1_;
            buf1_ = lastBP_ + b;
            
            DspFloatType c = lastBP_ * wDiv2_;
            lastLP_ = c + buf2_;
            buf2_ = lastLP_ + c;
        }
        
        inline DspFloatType tickLP (DspFloatType input)
        {
            tick (input);
            return lastLP_;
        }
        
        inline DspFloatType tickBP (DspFloatType input)
        {
            tick (input);
            return lastBP_;
        }
        
        inline DspFloatType tickHP (DspFloatType input)
        {
            tick (input);
            return lastHP_;
        }
        
        inline DspFloatType getLastLP() const { return lastLP_; }
        inline DspFloatType getLastBP() const { return lastBP_; }
        inline DspFloatType getLastHP() const { return lastHP_; }
        
        inline DspFloatType getPhaseDelayLP (DspFloatType freq)
        {
            assert (false); // todo
            return 0.0f;
        }
        
    private:
        DspFloatType sampleRate_;
        
        DspFloatType wDiv2_;
        DspFloatType twoR_;
        DspFloatType d_;
        DspFloatType a_;
        
        DspFloatType buf1_;
        DspFloatType buf2_;
        
        DspFloatType lastLP_;
        DspFloatType lastBP_;
        DspFloatType lastHP_;
        
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
        
        void setSampleRate (DspFloatType sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackTime (DspFloatType attackS)
        {
            attCoeff_ = time2Cutoff (attackS);
        }
        
        inline void setReleaseTime (DspFloatType releaseS)
        {
            relCoeff_ = time2Cutoff (releaseS);
        }
        
        inline DspFloatType tick (DspFloatType input)
        {
            return tickGrt0 (std::fabs (input));
        }
        
        inline DspFloatType tickGrt0 (DspFloatType input)
        {
            assert (input >= 0.0f);
            DspFloatType diff = input - z1_;
            cancelDenormals (diff);
            return z1_ = z1_ + diff * (input > z1_ ? attCoeff_ : relCoeff_);
        }
        
    private:
        DspFloatType sampleRate_;
        
        DspFloatType z1_;
        
        DspFloatType attCoeff_;
        DspFloatType relCoeff_;
        
        inline DspFloatType time2Cutoff (DspFloatType seconds)
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
        LogSmoother (DspFloatType initialValue = 0.0f)
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
        
        void setSampleRate (DspFloatType sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackAndReleaseTime (DspFloatType seconds)
        {
            setAttackTime (seconds);
            setReleaseTime (seconds);
        }
        
        inline void setAttackAndReleaseTime (DspFloatType attackS, DspFloatType releaseS)
        {
            setAttackTime (attackS);
            setReleaseTime (releaseS);
        }
        
        inline void setAttackTime (DspFloatType attackS)
        {
            attCoeff_ = time2Cutoff (attackS);
        }
        
        inline void setReleaseTime (DspFloatType releaseS)
        {
            relCoeff_ = time2Cutoff (releaseS);
        }
        
        inline void setTarget (DspFloatType t, bool force = false)
        {
            target_ = t;
            if (force)
                z1_ = t;
            coeff_ = target_ > z1_ ? &attCoeff_ : &relCoeff_;
        }
        
        inline DspFloatType getTarget() const
        {
            return target_;
        }
        
        inline DspFloatType tick()
        {
            DspFloatType diff = target_ - z1_;
            cancelDenormals (diff);
            return z1_ = z1_ + diff * (*coeff_);
        }
        
    private:
        DspFloatType sampleRate_;
        
        DspFloatType target_ = 0.0f;
        DspFloatType z1_;
        
        DspFloatType attCoeff_;
        DspFloatType relCoeff_;
        DspFloatType* coeff_ { &attCoeff_ };
        
        inline DspFloatType time2Cutoff (DspFloatType seconds)
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
        SmootherLin (DspFloatType initialValue = 0.0f)
        {
            setSampleRate (44100.0f);
            setTime (0.01f);
            
            setTargetForced (initialValue);
        }
        
        inline void reset()
        {
            setTargetForced (target_);
        }
        
        void setSampleRate (DspFloatType sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setTime (DspFloatType seconds)
        {
            setNumSteps (int (seconds * sampleRate_));
        }
        
        inline void setNumSteps (int numSteps)
        {
            assert (numSteps >= 1);
            numSteps_ = numSteps;
            numStepsInv_ = 1.0f / numSteps_;
        }
        
        inline void setTarget (DspFloatType t)
        {
            if (target_ != t)
            {
                target_ = t;
                incr_ = (target_ - currentValue_) * numStepsInv_;
                counter_ = numSteps_;
            }
        }
        
        inline void setTargetForced (DspFloatType t)
        {
            currentValue_ = target_ = t;
            counter_ = 0;
        }
        
        inline DspFloatType getTarget() const
        {
            return target_;
        }
        
        inline DspFloatType tick()
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
        
        inline DspFloatType getCurrentValue() const
        {
            return currentValue_;
        }
        
    private:
        DspFloatType sampleRate_;
        
        DspFloatType currentValue_;
        DspFloatType target_;
        
        int counter_;
        int numSteps_;
        DspFloatType numStepsInv_;
        DspFloatType incr_;
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
        inline void push (DspFloatType sample)
        {
            array_ [size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline DspFloatType get (DspFloatType samplesDelay) const
        {
            assert (samplesDelay == clampLength (samplesDelay));
            const int asInt = (int) samplesDelay;
            const DspFloatType x = samplesDelay - asInt;
            const DspFloatType y0 = array_ [size_t ((writeIndex_ + asInt - 1) & mask_)];
            const DspFloatType y1 = array_ [size_t ((writeIndex_ + asInt) & mask_)];
            const DspFloatType y2 = array_ [size_t ((writeIndex_ + asInt + 1) & mask_)];
            const DspFloatType y3 = array_ [size_t ((writeIndex_ + asInt + 2) & mask_)];
            
            return HermiteInterpolation::calculatePointV4 (x, y0, y1, y2, y3);
        }
        
        inline DspFloatType getClipped (DspFloatType samplesDelay) const
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
        
        inline DspFloatType clampLength (DspFloatType samplesDelay) const
        {
            return clamp (samplesDelay, (DspFloatType)getMinDelayLengthSamples(), (DspFloatType)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <DspFloatType> array_;
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
        inline void push (DspFloatType sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline DspFloatType get (DspFloatType samplesDelay) const
        {
            assert (clampLength (samplesDelay) == samplesDelay);
            
            int delayInt = (int)samplesDelay;
            DspFloatType delayFract = samplesDelay - delayInt;
            
            const int lower = (writeIndex_ + delayInt) & mask_;
            const int upper = (lower + 1) & mask_;
            
            return array_[(size_t)lower] + (array_[(size_t)upper] - array_[(size_t)lower]) * delayFract;
        }
        
        inline DspFloatType getClipped (DspFloatType samplesDelay) const
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
        
        inline DspFloatType clampLength (DspFloatType samplesDelay) const
        {
            return clamp (samplesDelay, (DspFloatType)getMinDelayLengthSamples(), (DspFloatType)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <DspFloatType> array_;
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
        inline void push (DspFloatType sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline DspFloatType get (DspFloatType samplesDelay)
        {
            assert (samplesDelay == clampLength (samplesDelay));
            int apM = (int)samplesDelay;
            DspFloatType fract = samplesDelay - apM;
            if (fract < 0.1f)
            {
                fract += 1.0f; // delta range of [0.1, 1.1]
                apM -= 1;
            }
            const DspFloatType apN = (1.0f - fract) / (1.0f + fract);
            
            const DspFloatType a = array_[size_t ((writeIndex_ + apM) & mask_)];
            
            const DspFloatType b = apZ1_ * -apN + a;
            const DspFloatType output = b * apN + apZ1_;
            apZ1_ = b;
            return output;
        }
        
        inline DspFloatType getClipped (DspFloatType samplesDelay)
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
        
        inline DspFloatType clampLength (DspFloatType samplesDelay) const
        {
            return clamp (samplesDelay, (DspFloatType)getMinDelayLengthSamples(), (DspFloatType)getMaxDelayLengthSamples());
        }
        
    private:
        std::vector <DspFloatType> array_;
        
        int mask_;
        int writeIndex_;
        
        DspFloatType apZ1_ = 0.0f;
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
        inline void push (DspFloatType sample)
        {
            array_[size_t (writeIndex_--)] = sample;
            if (writeIndex_ == -1)
                writeIndex_ = (int)array_.size() - 1;
        }
        
        inline DspFloatType get (int samplesDelay) const
        {
            assert (samplesDelay == clampLength (samplesDelay));
            return array_ [size_t ((writeIndex_ + samplesDelay) & mask_)];
        }
        
        inline DspFloatType getClipped (int samplesDelay) const
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
        std::vector <DspFloatType> array_;
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
        
        void setSampleRate (DspFloatType sr)
        {
            assert (sr > 0);
            sampleRate_ = sr;
        }
        
        inline void setAttackTime (DspFloatType seconds)
        {
            attackIncr_ = 1.0f / std::max (1.0f, seconds * sampleRate_);
        }
        
        inline void setDecayTime (DspFloatType seconds)
        {
            decayMult_ = powXGrt0ToY (0.5f, 1.0f / (std::max (1.0f, seconds * sampleRate_)));
        }
        
        inline void setSustainLevel (DspFloatType level0to1)
        {
            assert (level0to1 >= 0.0f && level0to1 <= 1.0f);
            sustainLevel_ = level0to1;
            sustainLevelScaled_ = sustainLevel_ * velocity_;
        }
        
        inline void setReleaseTime (DspFloatType seconds)
        {
            releaseMult_ = powXGrt0ToY (0.5f, 1.0f / (std::max (1.0f, seconds * sampleRate_)));
        }
        
        inline void triggerGate (DspFloatType velocity)
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
        
        inline DspFloatType tick()
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
                    DspFloatType diff = (value_ - sustainLevelScaled_);
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
        DspFloatType sampleRate_;
        
        // params
        DspFloatType attackIncr_;
        DspFloatType decayMult_;
        DspFloatType sustainLevel_;
        DspFloatType releaseMult_;
        
        // state
        DspFloatType velocity_;
        DspFloatType sustainLevelScaled_;
        DspFloatType value_;
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
            static constexpr DspFloatType calculateOutput (DspFloatType input, DspFloatType threshold, DspFloatType ratio)
            {
                if (input <= threshold)
                    return input;
                return threshold + (input - threshold) / ratio;
            }
            
            static constexpr DspFloatType calculateOutput (DspFloatType input, DspFloatType threshold, DspFloatType ratio, DspFloatType knee)
            {
                // no reduction
                if ((input - threshold) <= -0.5f * knee)
                    return input;
                
                // full reduction
                if ((input - threshold) >= 0.5f * knee)
                    return threshold + (input - threshold) / ratio;
                
                // knee range
                // scale input to [0, knee]
                DspFloatType a = input - threshold + knee * 0.5f;
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
        
        void setSampleRate (DspFloatType sr)
        {
            assert (sr > 0);
            follower_.setSampleRate (sr);
        }
        
        inline void setThreshold (DspFloatType thresholdDB)
        {
            threshold_ = thresholdDB;
        }
        
        inline void setRatio (DspFloatType r)
        {
            ratio_ = r;
        }
        
        inline void setKnee (DspFloatType kneeDB)
        {
            knee_ = kneeDB;
        }
        
        inline void setAttackTime (DspFloatType seconds)
        {
            follower_.setAttackTime (seconds);
        }
        
        inline void setReleaseTime (DspFloatType seconds)
        {
            follower_.setReleaseTime (seconds);
        }
        
        inline DspFloatType tick (DspFloatType input)
        {
            const DspFloatType asDB = ampToDecibel (std::fabs (input));
            
            const DspFloatType outDB = Helper::calculateOutput (asDB, threshold_, ratio_, knee_);
            const DspFloatType reductionDB = asDB - outDB;
            
            lastReductionDB_ = -follower_.tickGrt0 (reductionDB);
            lastReductionAmp_ = decibelToAmp (lastReductionDB_);
            
            return input * lastReductionAmp_;
        }
        
        inline DspFloatType getLastReductionDB() const
        {
            return lastReductionDB_;
        }
        
        inline DspFloatType getLastReductionAmp() const
        {
            return lastReductionAmp_;
        }
        
    private:
        EnvFollower follower_;
        DspFloatType threshold_;
        DspFloatType ratio_;
        DspFloatType knee_;
        
        DspFloatType lastReductionDB_;
        DspFloatType lastReductionAmp_;
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
        static inline DspFloatType calculatePoint (std::array <DspFloatType, N> points, DspFloatType p)
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
            DspFloatType sum = points[0];
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
        void init (std::function <DspFloatType (size_t index)> func, size_t tableSize)
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
        
        bool isWithinRange (DspFloatType index) const { return index >= 0.0f && index <= data_.size() - 1; }
        
        inline DspFloatType mapToTable (DspFloatType f, DspFloatType min, DspFloatType max)
        {
            assert (isValid());
            return (data_.size() - 1) * ((f - min) / (max - min));
        }
        
        inline DspFloatType mapToTableClipped (DspFloatType f, DspFloatType min, DspFloatType max)
        {
            assert (isValid());
            return clamp (mapToTable (f, min, max), 0.0f, DspFloatType (data_.size() - 1) - 0.0001f);
        }
        
        inline DspFloatType get (DspFloatType index) const
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
        
        inline DspFloatType getClipped (DspFloatType index) const
        {
            assert (isValid());
            
            if (index < 0.0f) return data_[0];
            if (index >= data_.size() - 1) return data_ [data_.size() - 1];
            
            return get (index);
        }
        
    private:
        std::vector <DspFloatType> data_;
    };
    
    class WaveShaperTable
    {
    public:
        WaveShaperTable()
        {
            init ([&] (DspFloatType audio) { return tanhf (audio); }, 100);
        }
        
        void reset()
        {
            table_.reset();
        }
        
        void init (std::function <DspFloatType (DspFloatType audio)> func,
                   size_t tableSize,
                   DspFloatType audioMin = -1.0f,
                   DspFloatType audioMax = 1.0f)
        {
            if (func != nullptr)
            {
                table_.init (
                [func, tableSize, audioMin, audioMax] (size_t index)
                {
                    return func (map <DspFloatType> (index, 0, tableSize - 1, audioMin, audioMax));
                },
                tableSize);
            }
        }
        
        
        inline DspFloatType get (DspFloatType audio)
        {
            assert (table_.isValid());
            return table_.get (table_.mapToTableClipped (audio, -1.0f, 1.0f));
        }
        
        const LookupTable& getLookupTable() const { return table_; }
        
    private:
        LookupTable table_;
        DspFloatType audioMin_;
        DspFloatType audioMax_;
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
        
        void setSampleRate (DspFloatType sr)
        {
            if (sr > 0)
            {
                sampleRate_ = sr;
                pressure_.setSampleRate (sr);
                pressure_.setAttackAndReleaseTime (0.01f);
            }
        }
        
        inline DspFloatType tick (DspFloatType tubeFeedback)
        {
            // exciter
            DspFloatType p = pressure_.tick();
            DspFloatType a = tubeFeedback - p;
            const DspFloatType output = p + a * (clamp (0.7f - 0.3f * a, -1.0f, 1.0f));
            return output;
        }
        
        inline void setPressure (DspFloatType p)
        {
            pressure_.setTarget (shapePressure (clamp (p, 0.0f, 1.0f)));
        }
        
    private:
        DspFloatType sampleRate_;
        
        jbaudio::LogSmoother pressure_;
        
        static inline DspFloatType shapePressure (DspFloatType p)
        {
            return std::pow (p, 0.2f);
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
        
        void setSampleRate (DspFloatType sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setFreq (DspFloatType f)
        {
            incr_ = (f / sampleRate_) * twoPi;
        }
        
        inline void setVolume (DspFloatType v)
        {
            volume_ = v;
        }
        
        inline DspFloatType tick()
        {
            phase_ += incr_;
            if (phase_ > twoPi)
                phase_ -= twoPi;
            return std::sin (phase_) * volume_;
        }
        
        inline DspFloatType getWithoutTicking()
        {
            return std::sin (phase_) * volume_;
        }
        
    private:
        DspFloatType sampleRate_;
        DspFloatType incr_;
        DspFloatType phase_;
        DspFloatType volume_;
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
            //assert (tapValue_ == 0.0f || tapValue_ == 1.0f);
        }
        
        void setSampleRate (DspFloatType sr)
        {
            sampleRate_ = sr;
        }
        
        inline void setTime (DspFloatType seconds)
        {
            assert (seconds > 0.0f);
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
        
        inline void tick (DspFloatType& tapAMultiplier, DspFloatType& tapBMultiplier)
        {
            
        }
        
    private:
        DspFloatType sampleRate_;
        
        State state_;
        
        bool nextTapNeedsFade_;
        
        DspFloatType incr_;
        DspFloatType xfadeValue_;
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
        
        void setSampleRate (DspFloatType sr)
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
        
        inline void push (DspFloatType input)
        {
            delay_.push (input);
        }
        
        inline DspFloatType get()
        {
            DspFloatType freq = jbaudio::pitchToFreq (length_.tick()); // de freq die je wilt horen
            DspFloatType delayLen = sampleRate_ / freq; // de delay lengte voor die freq
            lowpass_.setFreq (cutoff_.tick());
            DspFloatType phaseDel = lowpass_.getPhaseDelayLP (freq); // de phase delay voor die freq
            DspFloatType samplesDel = delayLen * (-phaseDel / jbaudio::twoPi); // aantal samples delay voor die freq
            delayLen *= 0.5f; // omdat na 1 keer de delay de golf geinverteerd plaats vindt
            delayLen -= samplesDel; // compenseer voor phase delay lengte
            DspFloatType pipeOut = delay_.getClipped (delayLen);
            pipeOut = lowpass_.tickLP (pipeOut);
            pipeOut *= -0.95f;
            return pipeOut;
        }
        
        inline void setLength (DspFloatType pitch)
        {
            length_.setTarget (pitch);
        }
        
        inline void setCutoff (DspFloatType freq)
        {
            cutoff_.setTarget (freq);
        }
        
    private:
        DspFloatType sampleRate_;
        
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
        static DspFloatType gloubiBoulga (DspFloatType input)
        {
            const DspFloatType x = input * 0.686306f;
            const DspFloatType a = 1.0f + std::expf (std::sqrt (std::fabs (x)) * -0.75f);
            return (std::expf (x) - std::expf (-x * a)) / (std::expf (x) + std::expf (-x));
        }
        
        // reaktor
        static DspFloatType tanh (DspFloatType input, DspFloatType level)
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
