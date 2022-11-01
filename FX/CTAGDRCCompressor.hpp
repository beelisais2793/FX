// https://github.com/p-hlp/CTAGDRC
/*
Copyright (C) 2020 Phillip Lamp
This file is part of the CTAGDRC project <https://github.com/p-hlp/CTAGDRC>.
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>

/*CrestFactor Class:
 * Calculates the average Crest-Factor for a given buffer
 * Crest-Factor is time-variable value calculated from the ratio between peak and rms of the signal
 */

namespace FX::Dynamics::CTAGDRC
{
    namespace Constants
    {
        namespace Colors
        {
            //Backgrounds
            constexpr uint32_t bg_App{0xff424242};
            constexpr uint32_t bg_DarkGrey{0xff212121};
            constexpr uint32_t bg_MidGrey{0xff616161};
            constexpr uint32_t bg_LightGrey{0xff9e9e9e};

            //Gradients
            constexpr uint32_t gd_LightGreyStart{0xffeeeee};
            constexpr uint32_t gd_LightGreyEnd{0xffc4c4c4};
            constexpr uint32_t gd_DarktGreyStart{0xff646464};
            constexpr uint32_t gd_DarkGreyEnd{0xff464646};

            //Misc
            constexpr uint32_t statusOutline{0xff00838f};
            constexpr uint32_t knobShadow{0xff424242};
        }

        namespace Margins
        {
            constexpr DspFloatType small = 2.0f;
            constexpr DspFloatType medium = 4.0f;
            constexpr DspFloatType big = 8.0f;
        }

        namespace Parameter
        {
            constexpr DspFloatType inputStart = -30.0f;
            constexpr DspFloatType inputEnd = 30.0f;
            constexpr DspFloatType inputInterval = 0.1f;

            constexpr DspFloatType thresholdStart = -60.0f;
            constexpr DspFloatType thresholdEnd = 0.0f;
            constexpr DspFloatType thresholdInterval = 0.1f;

            constexpr DspFloatType ratioStart = 1.0f;
            constexpr DspFloatType ratioEnd = 24.0f;
            constexpr DspFloatType ratioInterval = 0.05f;

            constexpr DspFloatType kneeStart = 0.0f;
            constexpr DspFloatType kneeEnd = 24.0f;
            constexpr DspFloatType kneeInterval = 0.1f;

            constexpr DspFloatType attackStart = 0.0f;
            constexpr DspFloatType attackEnd = 100.0f;
            constexpr DspFloatType attackInterval = 0.01f;

            constexpr DspFloatType releaseStart = 5.0f;
            constexpr DspFloatType releaseEnd = 1500.0f;
            constexpr DspFloatType releaseInterval = 0.01f;

            constexpr DspFloatType makeupStart = -40.0f;
            constexpr DspFloatType makeupEnd = 40.0f;
            constexpr DspFloatType makeupInterval = 0.1f;

            constexpr DspFloatType mixStart = 0.0f;
            constexpr DspFloatType mixEnd = 1.0f;
            constexpr DspFloatType mixInterval = 0.001f;
        }
    }
    class CrestFactor
    {
    public:

        CrestFactor() = default;

        // Prepares processor with ProcessSpec-Object and recalculates coefficients for current ballistics
        void prepare(const DspFloatType& fs);

        // Calculates Crest-Factor for given buffer
        void process(const DspFloatType* src, int numSamples);

        // Get average calculated attack time of a buffer, call after proces()
        DspFloatType getAvgAttack();

        // Get average calculated release time of a buffer, call after process()
        DspFloatType getAvgRelease();

    private:
        DspFloatType attackTimeInSeconds{0.0}, releaseTimeInSeconds{0.14};
        DspFloatType avgAttackTime{0.0}, avgReleaseTime{0.14};
        DspFloatType peakState{0.0};
        DspFloatType rmsState{0.0};
        DspFloatType a1{0.0}, b1{0.0};
        DspFloatType sampleRate{0.0};
        DspFloatType maxAttackTime{0.08}, maxReleaseTime{1.0}; //respective 8ms and 1sec
        DspFloatType cFactor{0.0};
    };


    /*Simple exponential moving average filter, also known as 1-pole iir filter
    * This class can be used to smooth values over a certain time frame
    */
    class SmoothingFilter
    {
    public:

        SmoothingFilter() = default;

        // Prepares the SmoothingFilter with a sampleRate
        void prepare(const DspFloatType& fs);

        // Processes a given sample
        void process(const DspFloatType& sample);

        // Sets coefficient manually
        void setAlpha(DspFloatType a);

        // Set time-frame in seconds, recalculates needed coefficients
        void setAlphaWithTime(DspFloatType timeInSeconds);

        // Gets current value
        DspFloatType getState();

    private:
        DspFloatType a1{1.0}, b1{0.0};
        DspFloatType state{0.0};
        DspFloatType sampleRate{0.0};
        bool first{true};
    };

    /*LevelDetector Class:
    * Used to have a smooth representation of the level
    * Might be used in linear or log. domain
    * In this compressor implementation it's used in log. domain after the gain computer to smooth the calculated attenuations,
    * therefore the detector does not have to work on the whole dynamic range of the input signal
    */
    class LevelDetector
    {
    public:
        LevelDetector() = default;

        // Prepares LevelDetector with a ProcessSpec-Object containing samplerate, blocksize and number of channels
        void prepare(const DspFloatType& fs);

        // Sets attack time constant
        void setAttack(const DspFloatType&);

        // Sets release time constant
        void setRelease(const DspFloatType&);

        // Sets auto attack to enabled/disabled
        void setAutoAttack(bool isEnabled);

        // Sets auto release to enabled/disabled
        void setAutoRelease(bool isEnabled);

        // Gets current attack time constant
        DspFloatType getAttack();

        // Gets current release time constant
        DspFloatType getRelease();

        // Gets calculated attack coefficient
        DspFloatType getAlphaAttack();

        // gets calculated release coefficient
        DspFloatType getAlphaRelease();

        // Processes a sample with smooth branched peak detector
        DspFloatType processPeakBranched(const DspFloatType&);

        // Processes a sample with smooth decoupled peak detector
        DspFloatType processPeakDecoupled(const DspFloatType&);

        // Applies ballistics to given buffer
        void applyBallistics(DspFloatType*, int);

        // Processes crest factor and sets ballistics accordingly
        void processCrestFactor(const DspFloatType* src, int numSamples);

    private:
        CrestFactor crestFactor;
        SmoothingFilter attackSmoothingFilter;
        SmoothingFilter releaseSmoothingFilter;

        DspFloatType attackTimeInSeconds{0.01}, alphaAttack{0.0};
        DspFloatType releaseTimeInSeconds{0.14}, alphaRelease{0.0};
        DspFloatType state01{0.0}, state02{0.0};
        DspFloatType sampleRate{0.0};
        bool autoAttack{false};
        bool autoRelease{false};
    };

    /* GainComputer Class:
    * Calculates the needed attenuation to compress a signal with given characteristics
    */
    class GainComputer
    {
    public:

        GainComputer();

        // Sets the threshold in dB
        void setThreshold(DspFloatType);

        // Sets the ratio in dB
        void setRatio(DspFloatType);

        // Sets the knee-width in dB (if > 0, 2nd order interpolation for soft knee)
        void setKnee(DspFloatType);

        // Applies characteristics to a given sample
        // returns attenuation
        DspFloatType applyCompression(DspFloatType&);

        void applyCompressionToBuffer(DspFloatType*, int);

    private:
        DspFloatType threshold{-20.0f};
        DspFloatType ratio{2.0f};
        DspFloatType knee{6.0f}, kneeHalf{3.0f};
        DspFloatType slope{-0.5f};
    };

    /*Basic delay line implementation with Juce's AudioBuffer class*/
    class DelayLine
    {
    public:

        DelayLine() = default;

        // Prepares delayline with ProcessSpec-Object, resizes delay-buffer, recalculates delayInSamples
        // and resets writePosition
        //void prepare(const dsp::ProcessSpec& spec);

        // Delays given AudioBuffer by delayInSamples samples
        void process(std::vector<DspFloatType>& buffer);

        // Sets delay
        void setDelay(DspFloatType delayInSeconds);

    private:
        void pushSamples(const std::vector<DspFloatType>& srcBuffer, int nSam, int nCh);
        void readSamples(std::vector<DspFloatType>& dstBuffer, int nSam, int nCh);

        inline void getReadBlockSize(int numSamples, int& index, int& block1, int& block2);
        inline void getWriteBlockSize(int numSamples, int& index, int& block1, int& block2);

    private:
        //Directly initialize process spec to avoid debugging problems
        //dsp::ProcessSpec procSpec{-1, 0, 0};

        std::vector<DspFloatType> delayBuffer;
        int delayBufferSize{0};
        int delayInSamples{0};
        DspFloatType delayInSeconds{0.0f};
        int writePosition{0};
        bool isBypassed{true};
    };


    /*Credits to Daniel Rudrich for the idea https://github.com/DanielRudrich/SimpleCompressor/blob/master/docs/lookAheadLimiter.md */
    /*LookAhead-Class:
    *A LookAhead implementation that not only delays the input signal, but also fades in aggressive gain reduction values to avoid distortion*/
    class LookAhead
    {
    public:
        ~LookAhead();
        void prepare(DspFloatType fs, DspFloatType delay, int blockSize);
        void process(DspFloatType* src, int numSamples);

    private:
        void pushSamples(const DspFloatType* src, int numSamples);
        void readSamples(DspFloatType* dst, int numSamples);
        void processSamples();

        inline void fadeIn(int& index, int range, DspFloatType& slope, DspFloatType& nextValue);
        inline void fadeInWithBreak(int& index, int range, DspFloatType& slope, DspFloatType& nextValue, bool& procMinimumFound);

        inline void getReadBlockSize(int numSamples, int& index, int& block1, int& block2);
        inline void getWriteBlockSize(int numSamples, int& index, int& block1, int& block2);
        inline void getProcessBlockSize(int numSamples, int index, int& block1, int& block2);
    private:
        std::vector<DspFloatType> buffer;
        DspFloatType* rawBuffer{nullptr};

        int delayInSamples{0};
        int bufferSize{0};
        int writePosition{0};
        int numLastPushed{0};
    };

    /*Basic sample-wise envenlope follower with instant attack*/
    class EnvelopeFollower
    {
    public:

        EnvelopeFollower() = default;

        // Prepares the the envelope follower with given sample rate
        void prepare(const DspFloatType& fs);

        // Sets decay/release time constant
        void setDecay(DspFloatType dcInSeconds);

        // Updates envelope follower, use buffer that contains linear values i.e. 0.0-1.0
        DspFloatType update(DspFloatType sample);

        // Updates envelope follower, use when buffer containts logarithmic values i.e. -infdB - 0dB
        DspFloatType updateLog(DspFloatType sample);

    private:
        DspFloatType decayInSecods{0.0};
        int decayInSamples{0};
        DspFloatType a{1.0};
        DspFloatType sampleRate{0.0};
        DspFloatType state{0.0};
    };

    /* Basic envelope-follwer, to track peak & rms signal level with configurable decay time*/
    class LevelEnvelopeFollower
    {
    public:
        LevelEnvelopeFollower() = default;

        // Prepares envelope follower with given sample rate and recalculates decayInSamples
        // as well as the peak/rms coefficient
        void prepare(const DspFloatType& fs);

        // Set peak decay
        void setPeakDecay(DspFloatType dc);

        // Set rms decay
        void setRmsDecay(DspFloatType dc);

        // Updates peak envelope follower from given audio buffer
        void updatePeak(const DspFloatType* const* channelData, int numChannels, int numSamples);

        // Updates rms envelope follower from given audio buffer
        void updateRMS(const DspFloatType* const* channelData, int numChannels, int numSamples);

        // Gets current peak, call after updatePeak
        DspFloatType getPeak();

        // Gets current rms, vall after updateRMS
        DspFloatType getRMS();

    private:
        DspFloatType currMaxPeak{0.0f};
        DspFloatType currMaxRMS{0.0f};
        DspFloatType peakDecay{0.99992f};
        DspFloatType rmsDecay{0.95f};
        DspFloatType peakDecayInSeconds{0.5f};
        DspFloatType rmsDecayInSeconds{0.0f};

        int peakDecayInSamples{0};
        int rmsDecayInSamples{0};

        DspFloatType sampleRate{0.0f};
    };

    /*Simple exponential moving average filter, also known as 1-pole iir filter
    * This class can be used to smooth values over a certain time frame
    */
    class SmoothingFilter
    {
    public:

        SmoothingFilter() = default;

        // Prepares the SmoothingFilter with a sampleRate
        void prepare(const DspFloatType& fs);

        // Processes a given sample
        void process(const DspFloatType& sample);

        // Sets coefficient manually
        void setAlpha(DspFloatType a);

        // Set time-frame in seconds, recalculates needed coefficients
        void setAlphaWithTime(DspFloatType timeInSeconds);

        // Gets current value
        DspFloatType getState();

    private:
        DspFloatType a1{1.0}, b1{0.0};
        DspFloatType state{0.0};
        DspFloatType sampleRate{0.0};
        bool first{true};
    };


    void SmoothingFilter::prepare(const DspFloatType& fs)
    {
        sampleRate = fs;
        a1 = 1;
        b1 = 1 - a1;
    }

    void SmoothingFilter::process(const DspFloatType& sample)
    {
        if (first)
        {
            state = sample;
            first = false;
        }
        state = a1 * sample + b1 * state;
    }

    DspFloatType SmoothingFilter::getState()
    {
        return state;
    }

    void SmoothingFilter::setAlpha(DspFloatType a)
    {
        a1 = a;
        b1 = 1 - a1;
    }

    void SmoothingFilter::setAlphaWithTime(DspFloatType timeInSeconds)
    {
        a1 = exp(-1.0 / (sampleRate * timeInSeconds));
        b1 = 1 - a1;
    }

    /*Credits to Daniel Rudrich for the idea https://github.com/DanielRudrich/SimpleCompressor/blob/master/docs/lookAheadLimiter.md */
    LookAhead::~LookAhead()
    {
        rawBuffer = nullptr;
    }

    void LookAhead::prepare(DspFloatType fs, DspFloatType delay, int blockSize)
    {
        delayInSamples = static_cast<int>(fs * delay);
        bufferSize = blockSize + delayInSamples;
        buffer.resize(bufferSize);
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        rawBuffer = buffer.data();
    }

    void LookAhead::process(DspFloatType* src, int numSamples)
    {
        pushSamples(src, numSamples);
        processSamples();
        readSamples(src, numSamples);
    }

    void LookAhead::pushSamples(const DspFloatType* src, const int numSamples)
    {
        int index, b1, b2;

        // Write in delay line
        getWriteBlockSize(numSamples, index, b1, b2);

        FloatVectorOperations::copy(rawBuffer + index, src, b1);

        if (b2 > 0)
            FloatVectorOperations::copy(rawBuffer, src + b1, b2);

        writePosition += numSamples;
        writePosition = writePosition % bufferSize;

        numLastPushed = numSamples;
    }

    void LookAhead::readSamples(DspFloatType* dst, int numSamples)
    {
        int index, b1, b2;

        // Get starting index, size of block before and after wrap
        getReadBlockSize(numSamples, index, b1, b2);

        // Read from internal buffer to dst
        FloatVectorOperations::copy(dst, rawBuffer + index, b1);

        if (b2 > 0)
            FloatVectorOperations::copy(dst + b1, rawBuffer, b2);
    }

    void LookAhead::processSamples()
    {
        int index = writePosition - 1;
        if (index < 0) index += bufferSize;

        int b1, b2;
        getProcessBlockSize(numLastPushed, index, b1, b2);

        DspFloatType nextValue = 0.0f;
        DspFloatType slope = 0.0f;

        // first run
        fadeIn(index, b1, slope, nextValue);

        // second run
        if (b2 > 0)
        {
            index = bufferSize - 1;
            fadeIn(index, b2, slope, nextValue);
        }

        if (index < 0) index = bufferSize - 1;

        // process delayInSamples
        getProcessBlockSize(delayInSamples, index, b1, b2);
        bool procMinimumFound = false;

        // first run
        fadeInWithBreak(index, b1, slope, nextValue, procMinimumFound);

        // second run
        if (!procMinimumFound && b2 > 0)
        {
            index = bufferSize - 1;
            fadeInWithBreak(index, b2, slope, nextValue, procMinimumFound);
        }
    }

    inline void LookAhead::fadeIn(int& index, int range, DspFloatType& slope, DspFloatType& nextValue)
    {
        for (int i = 0; i < range; ++i)
        {
            const DspFloatType sample = buffer[index];
            if (sample > nextValue)
            {
                buffer[index] = nextValue;
                nextValue += slope;
            }
            else
            {
                slope = -sample / static_cast<DspFloatType>(delayInSamples);
                nextValue = sample + slope;
            }
            --index;
        }
    }

    inline void LookAhead::fadeInWithBreak(int& index, int range, DspFloatType& slope, DspFloatType& nextValue,
                                        bool& procMinimumFound)
    {
        for (int i = 0; i < range; ++i)
        {
            const DspFloatType sample = buffer[index];
            if (sample > nextValue)
            {
                buffer[index] = nextValue;
                nextValue += slope;
            }
            else
            {
                procMinimumFound = true;
                break;
            }
            --index;
        }
    }

    inline void LookAhead::getReadBlockSize(int numSamples, int& index, int& block1, int& block2)
    {
        int pos = writePosition - numLastPushed - delayInSamples;

        if (pos < 0)
            pos = pos + bufferSize;
        pos = pos % bufferSize;

        index = pos;
        block1 = std::min(bufferSize - pos, numSamples);
        numSamples -= block1;
        block2 = numSamples > 0 ? numSamples : 0;
    }

    inline void LookAhead::getWriteBlockSize(int numSamples, int& index, int& block1, int& block2)
    {
        int pos = writePosition;

        if (pos < 0)
            pos = pos + bufferSize;
        pos = pos % bufferSize;

        index = pos;
        block1 = std::min(bufferSize - pos, numSamples);
        numSamples -= block1;
        block2 = numSamples > 0 ? numSamples : 0;
    }

    inline void LookAhead::getProcessBlockSize(int numSamples, int index, int& block1, int& block2)
    {
        block1 = std::min(index + 1, numSamples);
        numSamples -= block1;
        block2 = numSamples > 0 ? numSamples : 0;
    }

    void LevelEnvelopeFollower::prepare(const DspFloatType& fs)
    {
        sampleRate = fs;

        peakDecayInSamples = static_cast<int>(peakDecayInSeconds * sampleRate);
        peakDecay = 1.0f - 1.0f / static_cast<DspFloatType>(peakDecayInSamples);

        rmsDecayInSamples = static_cast<int>(rmsDecayInSeconds * sampleRate);
        rmsDecay = 1.0f - 1.0f / static_cast<DspFloatType>(rmsDecayInSamples);
    }

    // Set peak decay
    void LevelEnvelopeFollower::setPeakDecay(DspFloatType dc)
    {
        peakDecayInSeconds = dc;
        prepare(sampleRate);
    }

    // Set rms decay
    void LevelEnvelopeFollower::setRmsDecay(DspFloatType dc)
    {
        rmsDecayInSeconds = dc;
        prepare(sampleRate);
    }

    // Updates peak envelope follower from given audio buffer
    void LevelEnvelopeFollower::updatePeak(const DspFloatType* const* channelData, int numChannels, int numSamples)
    {
        assert(numChannels >= 0 && numSamples >= 0 && channelData != nullptr);
        if (numChannels > 0 && numSamples > 0)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                DspFloatType sum = 0.0f;
                for (int j = 0; j < numChannels; ++j)
                    sum += std::abs(channelData[j][i]);

                sum /= static_cast<DspFloatType>(numChannels);

                if (sum > currMaxPeak)
                    currMaxPeak = sum;
                else if (currMaxPeak > 0.001f)
                    currMaxPeak *= peakDecay;
                else currMaxPeak = 0.0f;
            }
        }
    }

    // Updates rms envelope follower from given audio buffer
    void LevelEnvelopeFollower::updateRMS(const DspFloatType* const* channelData, int numChannels, int numSamples)
    {
        assert(numChannels >= 0 && numSamples >= 0 && channelData != nullptr);
        if (numChannels > 0 && numSamples > 0)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                DspFloatType sum = 0.0f;
                for (int j = 0; j < numChannels; ++j)
                    sum += std::abs(channelData[j][i]);

                sum /= static_cast<DspFloatType>(numChannels);
                sum *= sum;

                if (sum > currMaxRMS)
                    currMaxRMS = sum * rmsDecay;
                else if (currMaxRMS > 0.001f)
                    currMaxRMS *= peakDecay;
                else currMaxRMS = 0.0f;
            }
        }
    }

    // Gets current peak, call after updatePeak
    DspFloatType LevelEnvelopeFollower::getPeak()
    {
        return currMaxPeak;
    }

    // Gets current rms, vall after updateRMS
    DspFloatType LevelEnvelopeFollower::getRMS()
    {
        return sqrt(currMaxRMS);
    }

    void LevelDetector::prepare(const DspFloatType& fs)
    {
        sampleRate = fs;
        crestFactor.prepare(fs);
        attackSmoothingFilter.prepare(fs);
        releaseSmoothingFilter.prepare(fs);

        alphaAttack = exp(-1.0 / (sampleRate * attackTimeInSeconds));
        alphaRelease = exp(-1.0 / (sampleRate * releaseTimeInSeconds));
        state01 = 0.0;
        state02 = 0.0;
    }

    void LevelDetector::setAttack(const DspFloatType& attack)
    {
        if (attack != attackTimeInSeconds)
        {
            attackTimeInSeconds = attack; //Time it takes to reach 1-1/e = 0.63
            alphaAttack = exp(-1.0 / (sampleRate * attackTimeInSeconds)); //aA = e^(-1/TA*fs)
        }
    }

    void LevelDetector::setRelease(const DspFloatType& release)
    {
        if (release != releaseTimeInSeconds)
        {
            releaseTimeInSeconds = release; //Time it takes to reach 1 - (1-1/e) = 0.37
            alphaRelease = exp(-1.0 / (sampleRate * releaseTimeInSeconds)); //aR = e^(-1/TR*fs)
        }
    }

    void LevelDetector::setAutoAttack(bool isEnabled)
    {
        autoAttack = isEnabled;
    }

    void LevelDetector::setAutoRelease(bool isEnabled)
    {
        autoRelease = isEnabled;
    }

    DspFloatType LevelDetector::getAttack()
    {
        return attackTimeInSeconds;
    }

    DspFloatType LevelDetector::getRelease()
    {
        return releaseTimeInSeconds;
    }

    DspFloatType LevelDetector::getAlphaAttack()
    {
        return alphaAttack;
    }

    DspFloatType LevelDetector::getAlphaRelease()
    {
        return alphaRelease;
    }

    DspFloatType LevelDetector::processPeakBranched(const DspFloatType& in)
    {
        //Smooth branched peak detector
        if (in < state01)
            state01 = alphaAttack * state01 + (1 - alphaAttack) * in;
        else
            state01 = alphaRelease * state01 + (1 - alphaRelease) * in;

        return static_cast<DspFloatType>(state01); //y_L
    }


    DspFloatType LevelDetector::processPeakDecoupled(const DspFloatType& in)
    {
        //Smooth decoupled peak detector
        const DspFloatType input = static_cast<DspFloatType>(in);
        state02 = jmax(input, alphaRelease * state02 + (1 - alphaRelease) * input);
        state01 = alphaAttack * state01 + (1 - alphaAttack) * state02;
        return static_cast<DspFloatType>(state01);
    }

    void LevelDetector::applyBallistics(DspFloatType* src, int numSamples)
    {
        // Apply ballistics to src buffer
        for (int i = 0; i < numSamples; ++i)
            src[i] = processPeakBranched(src[i]);
    }

    void LevelDetector::processCrestFactor(const DspFloatType* src, int numSamples)
    {
        if (autoAttack || autoRelease)
        {
            //Crest factor calculation
            crestFactor.process(src, numSamples);
            attackSmoothingFilter.process(crestFactor.getAvgAttack());
            releaseSmoothingFilter.process(crestFactor.getAvgRelease());
            if (autoAttack) setAttack(attackSmoothingFilter.getState());
            if (autoRelease) setRelease(releaseSmoothingFilter.getState());
        }
    }
    GainComputer::GainComputer()
    {
        threshold = -20.0f;
        ratio = 2.0f;
        slope = 1.0f / ratio - 1.0f;
        knee = 6.0f;
        kneeHalf = 3.0f;
    }

    void GainComputer::setThreshold(DspFloatType newTreshold)
    {
        threshold = newTreshold;
    }

    void GainComputer::setRatio(DspFloatType newRatio)
    {
        if (ratio != newRatio)
        {
            ratio = newRatio;
            if (ratio > 23.9f) ratio = -std::numeric_limits<DspFloatType>::infinity();
            slope = 1.0f / newRatio - 1.0f;
        }
    }

    void GainComputer::setKnee(DspFloatType newKnee)
    {
        if (newKnee != knee)
        {
            knee = newKnee;
            kneeHalf = newKnee / 2.0f;
        }
    }

    DspFloatType GainComputer::applyCompression(DspFloatType& input)
    {
        const DspFloatType overshoot = input - threshold;

        if (overshoot <= -kneeHalf)
            return 0.0f;
        if (overshoot > -kneeHalf && overshoot <= kneeHalf)
            return 0.5f * slope * ((overshoot + kneeHalf) * (overshoot + kneeHalf)) / knee;


        return slope * overshoot;
    }

    void GainComputer::applyCompressionToBuffer(DspFloatType* src, int numSamples)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            const DspFloatType level = std::max(abs(src[i]), 1e-6f);
            DspFloatType levelInDecibels = Decibels::gainToDecibels(level);
            src[i] = applyCompression(levelInDecibels);
        }
    }

    void EnvelopeFollower::prepare(const DspFloatType& fs)
    {
        sampleRate = fs;
        decayInSamples = static_cast<int>(sampleRate * decayInSecods);
    }

    void EnvelopeFollower::setDecay(DspFloatType dcInSeconds)
    {
        decayInSecods = dcInSeconds;
        prepare(sampleRate);
    }

    DspFloatType EnvelopeFollower::update(DspFloatType sample)
    {
        const DspFloatType s = static_cast<DspFloatType>(sample);
        if (s > state) state = s;
        else state *= a;
        return static_cast<DspFloatType>(state);
    }

    DspFloatType EnvelopeFollower::updateLog(DspFloatType sample)
    {
        const DspFloatType s = static_cast<DspFloatType>(sample);

        if (s < state) state = s; //attack phase	
        else state *= a; //release phase

        return static_cast<DspFloatType>(state);
    }

    void DelayLine::prepare(const dsp::ProcessSpec& spec)
    {
        procSpec = spec;
        delayInSamples = static_cast<int>(spec.sampleRate * delayInSeconds);
        delayBufferSize = static_cast<int>(spec.maximumBlockSize) + delayInSamples;
        delayBuffer.setSize(spec.numChannels, delayBufferSize);
        delayBuffer.clear();
        writePosition = 0;
    }

    void DelayLine::process(AudioBuffer<DspFloatType>& buffer)
    {
        if (!isBypassed)
        {
            const auto numSamples = buffer.getNumSamples();
            const auto numChannels = buffer.getNumChannels();

            jassert(delayBuffer.getNumChannels() == numChannels);
            jassert(numSamples <= delayBufferSize);

            //write to delayBuffer from buffer
            pushSamples(buffer, numSamples, numChannels);

            //read from delayBuffer to buffer
            readSamples(buffer, numSamples, numChannels);

            //Increase writePosition, wrap if not within bounds of delayBuffer
            writePosition += numSamples;
            writePosition = writePosition % delayBufferSize;
        }
    }

    void DelayLine::setDelay(DspFloatType delay)
    {
        //Check if delay is valid, otherwise bypass
        if (delay > 0.0f)
        {
            delayInSeconds = delay;
            isBypassed = false;
        }
        else
        {
            delayInSeconds = 0.0f;
            isBypassed = true;
        }

        //Prepare delayBuffer with processing specs
        prepare(procSpec);
    }

    void DelayLine::pushSamples(const AudioBuffer<DspFloatType>& srcBuffer, int nSam, int nCh)
    {
        //b1 = samples before wrap, b2 = samples after wrap
        int index, b1, b2;
        getWriteBlockSize(nSam, index, b1, b2);
        //Write to delayBuffer with FloatVectorOperations
        for (int i = 0; i < nCh; ++i)
            FloatVectorOperations::copy(delayBuffer.getWritePointer(i) + index, srcBuffer.getReadPointer(i), b1);

        if (b2 > 0)
            for (int i = 0; i < nCh; ++i)
                //Copy rest of buffer to delayBuffer
                FloatVectorOperations::copy(delayBuffer.getWritePointer(i), srcBuffer.getReadPointer(i) + b1, b2);
    }

    void DelayLine::readSamples(AudioBuffer<DspFloatType>& dstBuffer, int nSam, int nCh)
    {
        //b1 = samples before wrap, b2 = samples after wrap
        int index, b1, b2;
        getReadBlockSize(nSam, index, b1, b2);
        for (int i = 0; i < nCh; i++)
            FloatVectorOperations::copy(dstBuffer.getWritePointer(i), delayBuffer.getReadPointer(i) + index, b1);

        if (b2 > 0)
            for (int i = 0; i < nCh; i++)
                FloatVectorOperations::copy(dstBuffer.getWritePointer(i) + b1, delayBuffer.getReadPointer(i), b2);
    }

    inline void DelayLine::getReadBlockSize(int numSamples, int& index, int& block1, int& block2)
    {
        int pos = writePosition - delayInSamples;
        if (pos < 0)pos += delayBufferSize;
        pos = pos % delayBufferSize;
        //b1 can be max. number of samples in inputBuffer -> numSamples
        block1 = std::min(delayBufferSize - pos, numSamples);
        numSamples -= block1;
        //if b1 is not size of inputBuffer, calculate rest
        block2 = numSamples > 0 ? numSamples : 0;
        //set read/write start position
        index = pos;
    }

    inline void DelayLine::getWriteBlockSize(int numSamples, int& index, int& block1, int& block2)
    {
        int pos = writePosition;
        if (pos < 0) pos += delayBufferSize;
        pos = pos % delayBufferSize;
        //b1 can be max. number of samples in inputBuffer -> numSamples
        block1 = std::min(delayBufferSize - pos, numSamples);
        numSamples -= block1;
        //if b1 is not size of inputBuffer, calculate rest
        block2 = numSamples > 0 ? numSamples : 0;
        //set read/write start position
        index = pos;
    }

    void CrestFactor::prepare(const DspFloatType& fs)
    {
        sampleRate = fs;
        //Calculate alpha for release time of 200ms, same release time for peak & rms detector
        a1 = exp(-1.0 / (sampleRate * 0.2));
        b1 = 1 - a1;
    }


    void CrestFactor::
    process(const DspFloatType* src, const int numSamples)
    {
        //Init accumulators
        if (!peakState) peakState = src[0];
        if (!rmsState) rmsState = src[0];

        //Reset avg attack/release
        avgAttackTime = 0.0;
        avgReleaseTime = 0.0;

        //Calculate averages of auto - attack/release times for a single buffer
        for (int i = 0; i < numSamples; ++i)
        {
            //Square of input signal
            const DspFloatType s = static_cast<DspFloatType>(src[i]) * static_cast<DspFloatType>(src[i]);

            //Update peak state
            peakState = jmax(s, a1 * peakState + b1 * s);

            //Update rms state
            rmsState = a1 * rmsState + b1 * s;

            //calculate squared crest factor
            const DspFloatType c = peakState / rmsState;
            cFactor = c > 0.0 ? c : 0.0;

            //calculate ballistics
            if (cFactor > 0.0)
            {
                attackTimeInSeconds = 2 * (maxAttackTime / cFactor);
                releaseTimeInSeconds = 2 * (maxReleaseTime / cFactor) - attackTimeInSeconds;

                //Update avg ballistics
                avgAttackTime += attackTimeInSeconds;
                avgReleaseTime += releaseTimeInSeconds;
            }
        }

        // Calculate average ballistics & crest factor
        avgAttackTime /= numSamples;
        avgReleaseTime /= numSamples;
    }

    DspFloatType CrestFactor::getAvgAttack()
    {
        return avgAttackTime;
    }

    DspFloatType CrestFactor::getAvgRelease()
    {
        return avgReleaseTime;
    }


    namespace SIMDMath
    {
        inline DspFloatType sum(const DspFloatType* src, int num)
        {
            if (SystemStats::hasSSE3())
            {
                __m128 mmSum = _mm_setzero_ps();

                int i = 0;
                int rounddown = num - (num % 16);

                for (; i < rounddown; i += 16)
                {
                    __m128 v0 = _mm_loadu_ps(src + i + 0);
                    __m128 v1 = _mm_loadu_ps(src + i + 4);
                    __m128 s01 = _mm_add_ps(v0, v1);

                    __m128 v2 = _mm_loadu_ps(src + i + 8);
                    __m128 v3 = _mm_loadu_ps(src + i + 12);
                    __m128 s23 = _mm_add_ps(v2, v3);

                    mmSum = _mm_add_ps(mmSum, s01);
                    mmSum = _mm_add_ps(mmSum, s23);
                }

                for (; i < num; i++)
                    mmSum = _mm_add_ss(mmSum, _mm_load_ss(src + i));

                mmSum = _mm_hadd_ps(mmSum, mmSum);
                mmSum = _mm_hadd_ps(mmSum, mmSum);
                return _mm_cvtss_f32(mmSum);
            }

            // Fallback
            DspFloatType s = 0;
            const DspFloatType* e = src + num;

            while (src != e)
            {
                s += *src;
                src++;
            }
            return s;
        }
    }

    Compressor::~Compressor()
    {
        rawSidechainSignal = nullptr;
    }

    void Compressor::prepare(const dsp::ProcessSpec& ps)
    {
        procSpec = ps;
        ballistics.prepare(ps.sampleRate);
        delay.setDelay(0.005f);
        delay.prepare(ps);
        originalSignal.setSize(2, ps.maximumBlockSize);
        sidechainSignal.resize(ps.maximumBlockSize, 0.0f);
        rawSidechainSignal = sidechainSignal.data();
        originalSignal.clear();
        lookahead.prepare(procSpec.sampleRate, lookaheadDelay, ps.maximumBlockSize);
        smoothedAutoMakeup.prepare(ps.sampleRate);
        smoothedAutoMakeup.setAlpha(0.03);
    }

    void Compressor::setPower(bool newPower)
    {
        bypassed = newPower;
    }

    void Compressor::setInput(DspFloatType newInput)
    {
        input = newInput;
    }

    void Compressor::setAttack(DspFloatType attackTimeInMs)
    {
        ballistics.setAttack(attackTimeInMs * 0.001);
    }

    void Compressor::setRelease(DspFloatType releaseTimeInMs)
    {
        ballistics.setRelease(releaseTimeInMs * 0.001);
    }

    void Compressor::setRatio(DspFloatType rat)
    {
        gainComputer.setRatio(rat);
    }

    void Compressor::setKnee(DspFloatType kneeInDb)
    {
        gainComputer.setKnee(kneeInDb);
    }

    void Compressor::setThreshold(DspFloatType thresholdInDb)
    {
        gainComputer.setThreshold(thresholdInDb);
    }

    void Compressor::setMakeup(DspFloatType makeupGainInDb)
    {
        makeup = makeupGainInDb;
    }

    void Compressor::setMix(DspFloatType newMix)
    {
        mix = newMix;
    }

    void Compressor::setAutoAttack(bool isEnabled)
    {
        ballistics.setAutoAttack(isEnabled);
    }

    void Compressor::setAutoRelease(bool isEnabled)
    {
        ballistics.setAutoRelease(isEnabled);
    }

    void Compressor::setAutoMakeup(bool isEnabled)
    {
        autoMakeupEnabled = isEnabled;
    }

    void Compressor::setLookahead(bool newLookahead)
    {
        lookaheadEnabled = newLookahead;
    }

    DspFloatType Compressor::getMakeup()
    {
        return makeup;
    }

    DspFloatType Compressor::getSampleRate()
    {
        return procSpec.sampleRate;
    }


    DspFloatType Compressor::getMaxGainReduction()
    {
        return maxGainReduction;
    }

    void Compressor::process(AudioBuffer<DspFloatType>& buffer)
    {
        if (!bypassed)
        {
            const auto numSamples = buffer.getNumSamples();
            const auto numChannels = buffer.getNumChannels();

            jassert(numSamples == static_cast<int>(sidechainSignal.size()));

            // Clear any old samples
            originalSignal.clear();
            FloatVectorOperations::fill(rawSidechainSignal, 0.0f, numSamples);
            maxGainReduction = 0.0f;

            // Apply input gain
            applyInputGain(buffer, numSamples);

            // Get max l/r amplitude values and fill sidechain signal
            FloatVectorOperations::abs(rawSidechainSignal, buffer.getReadPointer(0), numSamples);
            FloatVectorOperations::max(rawSidechainSignal, rawSidechainSignal, buffer.getReadPointer(1), numSamples);

            // Calculate crest factor on max. amplitude values of input buffer
            ballistics.processCrestFactor(rawSidechainSignal, numSamples);

            // Compute attenuation - converts side-chain signal from linear to logarithmic domain
            gainComputer.applyCompressionToBuffer(rawSidechainSignal, numSamples);

            // Smooth attenuation - still logarithmic
            ballistics.applyBallistics(rawSidechainSignal, numSamples);

            // Get minimum = max. gain reduction from side chain buffer
            maxGainReduction = FloatVectorOperations::findMinimum(rawSidechainSignal, numSamples);

            // Calculate auto makeup
            autoMakeup = calculateAutoMakeup(rawSidechainSignal, numSamples);

            // Do lookahead if enabled
            if (lookaheadEnabled)
            {
                // Delay input buffer
                delay.process(buffer);

                // Process side-chain (delay + gain reduction fade in)
                lookahead.process(rawSidechainSignal, numSamples);
            }

            // Add makeup gain and convert side-chain to linear domain
            for (int i = 0; i < numSamples; ++i)
                sidechainSignal[i] = Decibels::decibelsToGain(sidechainSignal[i] + makeup + autoMakeup);

            // Copy buffer to original signal
            for (int i = 0; i < numChannels; ++i)
                originalSignal.copyFrom(i, 0, buffer, i, 0, numSamples);

            // Multiply attenuation with buffer - apply compression
            for (int i = 0; i < numChannels; ++i)
                FloatVectorOperations::multiply(buffer.getWritePointer(i), rawSidechainSignal, buffer.getNumSamples());

            // Mix dry & wet signal
            for (int i = 0; i < numChannels; ++i)
            {
                DspFloatType* channelData = buffer.getWritePointer(i); //wet signal
                FloatVectorOperations::multiply(channelData, mix, numSamples);
                FloatVectorOperations::addWithMultiply(channelData, originalSignal.getReadPointer(i), 1 - mix, numSamples);
            }
        }
    }

    inline void Compressor::applyInputGain(AudioBuffer<DspFloatType>& buffer, int numSamples)
    {
        if (prevInput == input)
            buffer.applyGain(0, numSamples, Decibels::decibelsToGain(prevInput));
        else
        {
            buffer.applyGainRamp(0, numSamples, Decibels::decibelsToGain(prevInput), Decibels::decibelsToGain(input));
            prevInput = input;
        }
    }

    inline DspFloatType Compressor::calculateAutoMakeup(const DspFloatType* src, int numSamples)
    {
        const DspFloatType sum = SIMDMath::sum(src, numSamples);
        smoothedAutoMakeup.process(-sum / static_cast<DspFloatType>(numSamples));
        return autoMakeupEnabled ? static_cast<DspFloatType>(smoothedAutoMakeup.getState()) : 0.0f;
    }
}