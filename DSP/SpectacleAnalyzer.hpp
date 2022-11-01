#pragma once

#include "Spectacle.hpp"

namespace DSP::Spectacle
{
    //------------------------------------------------------------------------------
    FFTW_PP_DEFINE_API(FFTW_MANGLE_DOUBLE, double, fftw_complex);
    FFTW_PP_DEFINE_API(FFTW_MANGLE_FLOAT, float, fftwf_complex);
    FFTW_PP_DEFINE_API(FFTW_MANGLE_LONG_DOUBLE, long double, fftwl_complex);

    class FFTPlanner {
    private:
        FFTPlanner() = default;

    public:
        static FFTPlanner& getInstance();
        fftwf_plan forwardFFT(uint32_t windowSize);

    private:
        std::mutex _mutex;
        std::map<uint32_t, fftwf_plan_u> _forwardPlans;
    };


    FFTPlanner& FFTPlanner::getInstance()
    {
        static FFTPlanner instance;
        return instance;
    }

    fftwf_plan FFTPlanner::forwardFFT(uint32_t windowSize)
    {
        std::unique_lock<std::mutex> lock(_mutex);

        auto it = _forwardPlans.find(windowSize);
        if (it != _forwardPlans.end())
            return it->second.get();

        const uint32_t numBins = windowSize / 2 + 1;
        fftwf_real_vector real(windowSize);
        fftwf_complex_vector cpx(numBins);

        int planFlags = 0;
    #if defined(USE_IMPATIENT_FFT_PLANNING)
        planFlags |= FFTW_ESTIMATE;
    #else
        planFlags |= FFTW_MEASURE;
    #endif
        fftwf_plan plan = fftwf_plan_dft_r2c_1d(windowSize, real.data(), (fftwf_complex *)cpx.data(), planFlags);
        _forwardPlans[windowSize] = fftwf_plan_u(plan);
        return plan;
    }






    ///
    struct Configuration {
        uint32_t windowSize = kStftDefaultSize;
        uint32_t stepSize = kStftDefaultStep;
        double attackTime = kStftDefaultAttackTime;
        double releaseTime = kStftDefaultReleaseTime;
        double sampleRate = 44100.0;
    };

    ///
    class BasicAnalyzer {
    public:
        virtual ~BasicAnalyzer() {}

    protected:
        void configureBasic(uint32_t numBins);

    public:
        virtual void configure(const Configuration &config) = 0;
        virtual void setAttackAndRelease(float attack, float release) = 0;
        virtual void clear();
        virtual void process(const float *input, uint32_t numFrames) = 0;

        const float *getFrequencies() const { return _freqs.data(); }
        float *getFrequencies() { return _freqs.data(); }
        const float *getMagnitudes() const { return _mags.data(); }
        float *getMagnitudes() { return _mags.data(); }
        uint32_t getNumBins() const { return _numBins; }

    private:
        uint32_t _numBins = 0;
        std::vector<float> _freqs;
        std::vector<float> _mags;
    };

    ///
    class SteppingAnalyzer : public BasicAnalyzer {
    public:
        SteppingAnalyzer();
        virtual ~SteppingAnalyzer() {}

    public:
        uint32_t getWindowSize() const { return _windowSize; }
        const uint32_t *getBinRange() const { return _binRange; }

    protected:
        void configureStepping(uint32_t numBins, const Configuration &config);

    public:
        virtual void configureBinRange(uint32_t start, uint32_t end);
        virtual void setAttackAndRelease(float attack, float release) override;
        virtual void clear() override;
        virtual void process(const float *input, uint32_t numFrames) override;

    protected:
        virtual void processNewBlock(float *input) = 0;

    private:
        // window
        std::vector<float> _window;
        uint32_t _windowSize {};

        // analysis step
        uint32_t _stepCounter {};
        uint32_t _stepSize {};

        // input sample accumulation
        uint32_t _ringIndex {};
        std::vector<float> _ring;

        // range
        uint32_t _binRange[2] = { 0u, ~0u };

        // temporary
        std::vector<float> _input;

        // step-by-step smoother
        class Smoother {        
        public:
            void configure(uint32_t numBins, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate);
            void configureBinRange(uint32_t start, uint32_t end);
            void setAttackAndRelease(float attack, float release);
            void clear();
            void process(float *stepData);
        private:
            std::vector<ARFollower> _ar;
            uint32_t _stepSize = 0;
            uint32_t _binRange[2] = { 0u, ~0u };
        };
        Smoother _smoother;
    };

    void BasicAnalyzer::configureBasic(uint32_t numBins)
    {
        _numBins = numBins;
        _freqs.resize(numBins);
        _mags.resize(numBins);
    }

    void BasicAnalyzer::clear()
    {
        std::fill_n(_mags.begin(), _numBins, 20.0 * std::log10(kStftFloorMagnitude));
    }

    ///
    SteppingAnalyzer::SteppingAnalyzer()
    {
    }

    void SteppingAnalyzer::configureStepping(uint32_t numBins, const Configuration &config)
    {
        configureBasic(numBins);

        const uint32_t windowSize = _windowSize = config.windowSize;
        _stepSize = config.stepSize;
        _ring.resize(2 * windowSize);
        _window.resize(windowSize);
        _input.resize(windowSize);

        float *window = _window.data();
        for (uint32_t i = 0; i < windowSize; ++i)
            window[i] = 0.5 * (1.0 - std::cos(2.0 * M_PI * i / (windowSize - 1)));

        _smoother.configure(numBins, config.stepSize, config.attackTime, config.releaseTime, config.sampleRate);
    }

    void SteppingAnalyzer::configureBinRange(uint32_t start, uint32_t end)
    {
        _binRange[0] = start;
        _binRange[1] = end;
        _smoother.configureBinRange(start, end);
    }

    void SteppingAnalyzer::setAttackAndRelease(float attack, float release)
    {
        _smoother.setAttackAndRelease(attack, release);
    }

    void SteppingAnalyzer::clear()
    {
        BasicAnalyzer::clear();

        _stepCounter = 0;
        _ringIndex = 0;
        std::fill(_ring.begin(), _ring.end(), 0.0f);

        _smoother.clear();
    }

    void SteppingAnalyzer::process(const float *input, uint32_t numFrames)
    {
        const float* window = _window.data();
        const uint32_t windowSize = _windowSize;

        uint32_t stepCounter = _stepCounter;
        const uint32_t stepSize = _stepSize;

        float *ring = _ring.data();
        uint32_t ringIndex = _ringIndex;

        for (uint32_t i = 0; i < numFrames; ++i) {
            ring[ringIndex] = ring[ringIndex + windowSize] = input[i];
            ringIndex = (ringIndex + 1 != windowSize) ? (ringIndex + 1) : 0;
            if (++stepCounter == stepSize) {
                stepCounter = 0;

                float* windowedBlock = _input.data();
                for (uint32_t i = 0; i < windowSize; ++i)
                    windowedBlock[i] = ring[ringIndex + i] * window[i];

                processNewBlock(windowedBlock);

                _smoother.process(getMagnitudes());
            }
        }

        _stepCounter = stepCounter;
        _ringIndex = ringIndex;
    }

    void SteppingAnalyzer::Smoother::configure(uint32_t numBins, uint32_t stepSize, double attackTime, double releaseTime, double sampleRate)
    {
        _ar.resize(numBins);
        _stepSize = stepSize;
        ARFollower *ar = _ar.data();
        ar[0].init(sampleRate);
        setAttackAndRelease(attackTime, releaseTime);
    }

    void SteppingAnalyzer::Smoother::configureBinRange(uint32_t start, uint32_t end)
    {
        _binRange[0] = start;
        _binRange[1] = end;
    }

    void SteppingAnalyzer::Smoother::setAttackAndRelease(float attack, float release)
    {
        ARFollower *ar = _ar.data();
        uint32_t numBins = (uint32_t)_ar.size();
        uint32_t stepSize = (uint32_t)_stepSize;
        ar[0].setAttackTime(attack / stepSize);
        ar[0].setReleaseTime(release / stepSize);
        for (uint32_t i = 1; i < numBins; ++i)
            ar[i].configureLike(ar[0]);
    }

    void SteppingAnalyzer::Smoother::clear()
    {
        ARFollower *ar = _ar.data();
        uint32_t numBins = (uint32_t)_ar.size();
        for (uint32_t i = 0; i < numBins; ++i)
            ar[i].clear();
    }

    void SteppingAnalyzer::Smoother::process(float *stepData)
    {
        ARFollower *ar = _ar.data();
        uint32_t numBins = (uint32_t)_ar.size();

        uint32_t start = _binRange[0];
        uint32_t end = std::min(_binRange[1], numBins);

        for (uint32_t i = start; i < end; ++i)
            stepData[i] = ar[i].compute(stepData[i]);
    }
}