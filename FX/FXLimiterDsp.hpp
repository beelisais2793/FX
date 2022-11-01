/*
  ==============================================================================
    LimiterDsp.h
    Created: 9 Aug 2019 11:02:34pm
    Author:  Utokusa
  ==============================================================================
*/

#pragma once
#include <cassert>
#include <cmath>
#include <vector>
#include <atomic>

// This is the limiter class used in befor feedback section of BiquadLimiter.
// The implementation is based on the link below.
// https://jp.mathworks.com/help/audio/ref/limiter-system-object.html
class LimiterDsp
{
public:
    LimiterDsp(DspFloatType threasholdDb, DspFloatType kneeWidthDb, DspFloatType attackTime, DspFloatType releaseTime, int numChannel, bool isGainShareMode);
    virtual ~LimiterDsp();
    virtual DspFloatType DoProcessOneSample(DspFloatType inputSigLinear, DspFloatType sampleRate, int channel, DspFloatType sidechainSigLinear);
    DspFloatType DoProcessOneSample(DspFloatType inputSigLinear, DspFloatType sampleRate, int channel);
    void UpdateCounter(DspFloatType inputSigLinear,DspFloatType outputSigLinear, DspFloatType sampleRate, int channel);
    void UpdateWorkingState();
    bool IsWorking();
    
private:
    DspFloatType threasholdDb;
    DspFloatType kneeWidthDb;
    DspFloatType attackTime;
    DspFloatType releaseTime;
    int numChannel;
    std::vector<DspFloatType> prevGainSmoothedDb;
    bool isGainShareMode;
    
    std::atomic<bool> isLimitterWorking;
    std::vector<DspFloatType> decayValMilliSec;  // isLimitterWorking is true when decayValMilliSec > 0.
    const DspFloatType epsilon = 0.0001f;
    const DspFloatType ititDecayValMilliSec = 100.0f;  // Whien limiter starts working, decayValMilliSec will be set to this value.
    
    DspFloatType convertLinearToDb(DspFloatType linearVal);
    DspFloatType convertDbToLinear(DspFloatType dbVal);
};


LimiterDsp::LimiterDsp(DspFloatType threasholdDb, DspFloatType kneeWidthDb, DspFloatType attackTime, DspFloatType releaseTime, int numChannel, bool isGainShareMode)
    : threasholdDb(threasholdDb)
    , kneeWidthDb(kneeWidthDb)
    , attackTime(attackTime)
    , releaseTime(releaseTime)
    , numChannel(numChannel)
    , prevGainSmoothedDb(numChannel)
    , isGainShareMode(isGainShareMode)
    , isLimitterWorking(false)
    , decayValMilliSec(numChannel)
{
}

LimiterDsp::~LimiterDsp(){}

DspFloatType LimiterDsp::DoProcessOneSample(DspFloatType inputSigLinear, DspFloatType sampleRate, int channel, DspFloatType sidechainSigLinear)
{
    //==============================================================================
    // When isGainShareMode is true, the limiter uses one shared gain.
    if (isGainShareMode)
    {
        channel = 0;
    }
    
    //==============================================================================
    // Convert linear side chain input Signal to dB.
    DspFloatType InputScSigdB = convertLinearToDb(sidechainSigLinear);
    
    //==============================================================================
    // The gain computer.
    // Calc static characteristic properties (soft knee).
    DspFloatType staticCharacteristic;

    if (InputScSigdB < threasholdDb - kneeWidthDb / 2.0f)
    {
        staticCharacteristic = InputScSigdB;
    }
    else if ( threasholdDb - kneeWidthDb / 2.0f <= InputScSigdB && InputScSigdB <= threasholdDb + kneeWidthDb / 2.0f)
    {
        staticCharacteristic = InputScSigdB - std::pow(InputScSigdB - threasholdDb + kneeWidthDb / 2.0f, 2.0f) / (2 * kneeWidthDb);
    }
    else
    {
        staticCharacteristic = threasholdDb;
    }
    
    //==============================================================================
    // Calc gain.
    // Gc[n]=Xsc[n] −XdB[n], where Xsc[n] is static characteristic
    // and XdB is input side chain level in dB.
    DspFloatType computedGainDb;
    if (std::isinf(InputScSigdB))
    {
        // In case == - Inf
        computedGainDb = 0.0f;
    }
    else
    {
        computedGainDb = staticCharacteristic - InputScSigdB;
    }
    
    //==============================================================================
    // Gain smoothing section
    
    
    static constexpr DspFloatType forwardVoltage = 90.0f;  // We define forward voltage as 90% of the peak voltage.
    static const DspFloatType kLogBaseConversion = forwardVoltage / 10.0f;
    // alphaAttack : Smoothing gain used in attack state.
    DspFloatType alphaAttack = std::exp(- std::log(kLogBaseConversion) / (static_cast<DspFloatType>(sampleRate) * attackTime));
    // alphaRelease : Smoothing gain used in release state.
    DspFloatType alphaRelease = std::exp(- std::log(kLogBaseConversion) / (static_cast<DspFloatType>(sampleRate) * releaseTime));

    DspFloatType gainSmoothedDb;
    if (computedGainDb <= prevGainSmoothedDb[channel])
    {
        gainSmoothedDb = alphaAttack * prevGainSmoothedDb[channel] + (1.0f - alphaAttack) * computedGainDb;
    }
    else
    {
        gainSmoothedDb = alphaRelease * prevGainSmoothedDb[channel] + (1.0f - alphaRelease) * computedGainDb;
    }
    
    //==============================================================================
    // Conver dB gain to linear gain.
    DspFloatType gain = convertDbToLinear(gainSmoothedDb);
    prevGainSmoothedDb[channel] = gainSmoothedDb;
    
    //==============================================================================
    // Apply gain.
    DspFloatType outputSigLinear = inputSigLinear * gain;
    
    return outputSigLinear;
}

DspFloatType LimiterDsp::DoProcessOneSample(DspFloatType inputSigLinear, DspFloatType sampleRate, int channel)
{
    DspFloatType outputSigLinear = DoProcessOneSample(inputSigLinear, sampleRate, channel, inputSigLinear);
    
    return outputSigLinear;
}

void LimiterDsp::UpdateCounter(DspFloatType inputSigLinear,DspFloatType outputSigLinear, DspFloatType sampleRate, int channel)
{
    // Whien limiter starts working, decayValMilliSec will be set to this value.
    if (std::abs(inputSigLinear - outputSigLinear) > epsilon)
    {
        decayValMilliSec[channel] = ititDecayValMilliSec;
    }
    else if (decayValMilliSec[channel] >= 0.0f)
    {
        assert(sampleRate > 0);
        decayValMilliSec[channel] -= 1.0f / static_cast<DspFloatType>(sampleRate) * 1000.0f;
    }
}

void LimiterDsp::UpdateWorkingState()
{
    // isLimitterWorking is true when decayValMilliSec > 0.
    bool next_state = false;
    for (auto cnt : decayValMilliSec)
    {
        if (cnt > epsilon)
        {
            next_state = true;
            break;
        }
    }
    
    isLimitterWorking.store(next_state);
}

bool LimiterDsp::IsWorking()
{
    return isLimitterWorking.load();
}

DspFloatType LimiterDsp::convertLinearToDb(DspFloatType linearVal)
{
    return 20 * std::log10(std::abs(linearVal));
}

DspFloatType LimiterDsp::convertDbToLinear(DspFloatType dbVal)
{
    return std::pow(10.f, dbVal / 20.f);
}

