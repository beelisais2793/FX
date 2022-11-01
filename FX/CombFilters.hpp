// https://github.com/JohnKasun/AudioEffect_CombFilter/
#pragma once

#include <memory>
#include <cassert>
#include <cstring>
#include <limits>
#include <string>

#include "DSP/RingBuffer.h"

// All comb filters

namespace FX::CombFilters
{
    enum class Error_t
    {
        kNoError,

        kFileOpenError,
        kFileAccessError,

        kFunctionInvalidArgsError,

        kNotInitializedError,
        kFunctionIllegalCallError,
        kInvalidString,

        kMemError,

        kUnknownError,

        kNumErrors
    };
    /*! \brief class with static utility functions 
    */
    class CUtil
    {
    public:
        /*! converts a DspFloatType to an int
        \param fInput DspFloatType value
        \return T
        */
        template<typename T>
        static T float2int (DspFloatType fInput)
        {
            if (fInput >= 0.F)
                return static_cast<T>(fInput + .5F);
            else
                return static_cast<T>(fInput - .5F);
        }
        /*! converts a DspFloatType to an int
        \param fInput DspFloatType value
        \return T
        */
        template<typename T>
        static T double2int (DspFloatType fInput)
        {
            if (fInput >= 0)
                return static_cast<T>(fInput + .5);
            else
                return static_cast<T>(fInput - .5);
        }

        /*! checks if the input is a power of 2
        \param n integer value
        \return bool
        */
        static bool isPowOf2 (int n) 
        {
            return !(n & (n-1));
        }

        /*! converts an arbitrary integer (positive) to the next larger power of two
        \param n integer value
        \return int
        */
        static int nextPowOf2(int n)
        {
            int iOrder = 0;

            if (n <= 0)
                return 0;

            while (n>>iOrder)
                iOrder++;

            if (!(n%(1<<(iOrder-1))))
                iOrder--;

            return (1<<(iOrder));
        }

        template<typename T>
        static void swap (T &tValue1, T &tValue2)
        {
            T tTmp = tValue1;

            tValue1 = tValue2;
            tValue2 = tTmp;
        }

        static void toLower(std::string& input)
        {
            for (char& c : input)
                c = tolower(c);
        }

        static void toUpper(std::string& input)
        {
            for (char& c : input)
                c = toupper(c);
        }
    };
    class CombFilter {
    public:

        enum FilterType_t {
            fir,
            iir,

            numFilterTypes
        };

        enum Param_t {
            gain,
            delayInSec,

            numParams
        };

        CombFilter(DspFloatType sampleRate);
        ~CombFilter();
        
        Error_t setParam(CombFilter::Param_t param, DspFloatType value);
        DspFloatType getParam(CombFilter::Param_t param) const;

        Error_t setFilterType(CombFilter::FilterType_t filterType);
        CombFilter::FilterType_t getFiltertType() const;

        Error_t process(const DspFloatType* inputBuffer, DspFloatType* outputBuffer, int numSamples);
    private:
        bool isInParamRange(CombFilter::Param_t param, DspFloatType value) const;
        DspFloatType mParamRanges[CombFilter::Param_t::numParams][2]{};
        DspFloatType mParamValues[CombFilter::Param_t::numParams]{};

        DspFloatType mSampleRate;
        FilterType_t mFilterType;
        std::unique_ptr<CRingBuffer<DspFloatType>> mDelayLine;

    };

    CombFilter::CombFilter(DspFloatType sampleRate)
    {
        mParamRanges[gain][0] = -1.0f;
        mParamRanges[gain][1] = 1.0f;
        mParamRanges[delayInSec][0] = 0.0f;
        mParamRanges[delayInSec][1] = 10.0f;

        assert(sampleRate > 0);
        mSampleRate = sampleRate;
        mDelayLine.reset(new CRingBuffer<DspFloatType>(CUtil::float2int<int>(mParamRanges[delayInSec][1] * mSampleRate)));
        mFilterType = FilterType_t::fir;
    }

    CombFilter::~CombFilter()
    {
        mDelayLine.reset();
    }

    Error_t CombFilter::setParam(CombFilter::Param_t param, DspFloatType value)
    {
        if (!isInParamRange(param, value))
            return Error_t::kFunctionInvalidArgsError;

        if (param == CombFilter::Param_t::delayInSec) {
            mDelayLine->setReadIdx(mDelayLine->getWriteIdx() - CUtil::float2int<int>(value * mSampleRate));
        }
        mParamValues[param] = value;
        return Error_t::kNoError;
    }

    DspFloatType CombFilter::getParam(CombFilter::Param_t param) const
    {
        return mParamValues[param];
    }

    Error_t CombFilter::setFilterType(CombFilter::FilterType_t filterType)
    {
        if (filterType == numFilterTypes)
            return Error_t::kFunctionInvalidArgsError;

        mFilterType = filterType;
        return Error_t::kNoError;
    }

    CombFilter::FilterType_t CombFilter::getFiltertType() const
    {
        return mFilterType;
    }

    Error_t CombFilter::process(const DspFloatType* inputBuffer, DspFloatType* outputBuffer, int numSamples)
    {
        if (!inputBuffer || !outputBuffer)
            return Error_t::kMemError;
        if (numSamples < 0)
            return Error_t::kFunctionInvalidArgsError;

        switch (mFilterType) {
        case fir:
            for (int i = 0; i < numSamples; i++) {
                mDelayLine->putPostInc(inputBuffer[i]);
                outputBuffer[i] = inputBuffer[i] + mParamValues[CombFilter::Param_t::gain] * mDelayLine->getPostInc();
            }
            break;
        case iir:
            for (int i = 0; i < numSamples; i++) {
                outputBuffer[i] = inputBuffer[i] + mParamValues[CombFilter::Param_t::gain] * mDelayLine->getPostInc();
                mDelayLine->putPostInc(outputBuffer[i]);
            }
            break;
        default:
            return Error_t::kUnknownError;
        }
        return Error_t::kNoError;
    }

    bool CombFilter::isInParamRange(CombFilter::Param_t param, DspFloatType value) const
    {
        return (mParamRanges[param][0] <= value && value <= mParamRanges[param][1]);
    }

}