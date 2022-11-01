#include <cmath>
#include <vector>
#include <cstdint>
#include <memory>
#include <complex>
#include <limits>
#include <utility>
#include <new>
#include <map>
#include <mutex>
#include <algorithm>

#include <hiir/Upsampler2xFpu.h>
#include <hiir/Downsampler2xFpu.h>
#include <fftw3.h>

namespace Spectrum
{
enum Algorithm {
    kAlgoStft,
    kAlgoMultirateStftX2,
    kAlgoMultirateStftX3,
    kAlgoMultirateStftX4,
    kAlgoMultirateStftX5,
    kAlgoMultirateStftX6,
    kAlgoMultirateStftX7,
    kAlgoMultirateStftX8,
    kNumAlgorithms,
};

static constexpr Algorithm kDefaultAlgorithm = kAlgoMultirateStftX6;

inline const char *getAlgorithmName(Algorithm a)
{
    switch (a) {
    case kAlgoStft: default:
        return "STFT";
    case kAlgoMultirateStftX2:
        return "STFT x2";
    case kAlgoMultirateStftX3:
        return "STFT x3";
    case kAlgoMultirateStftX4:
        return "STFT x4";
    case kAlgoMultirateStftX5:
        return "STFT x5";
    case kAlgoMultirateStftX6:
        return "STFT x6";
    case kAlgoMultirateStftX7:
        return "STFT x7";
    case kAlgoMultirateStftX8:
        return "STFT x8";
    }
}

static constexpr uint32_t kStftMinSizeLog2 = 6;
static constexpr uint32_t kStftMaxSizeLog2 = 14;
static constexpr uint32_t kStftDefaultSizeLog2 = 8;

static constexpr uint32_t kStftNumSizes = 1 + kStftMaxSizeLog2 - kStftMinSizeLog2;

static constexpr uint32_t kStftMinSize = 1u << kStftMinSizeLog2;
static constexpr uint32_t kStftMaxSize = 1u << kStftMaxSizeLog2;
static constexpr uint32_t kStftDefaultSize = 1u << kStftDefaultSizeLog2;

static constexpr uint32_t kStftMinStepLog2 = 6;
static constexpr uint32_t kStftMaxStepLog2 = 12;
static constexpr uint32_t kStftDefaultStepLog2 = 8;

static constexpr uint32_t kStftNumSteps = 1 + kStftMaxStepLog2 - kStftMinStepLog2;

static constexpr uint32_t kStftMinStep = 1u << kStftMinStepLog2;
static constexpr uint32_t kStftMaxStep = 1u << kStftMaxStepLog2;
static constexpr uint32_t kStftDefaultStep = 1u << kStftDefaultStepLog2;

static constexpr double kStftMinAttackTime = 1e-3;
static constexpr double kStftMaxAttackTime = 100e-3;
static constexpr double kStftMinReleaseTime = 1e-3;
static constexpr double kStftMaxReleaseTime = 500e-3;
static constexpr double kStftDefaultAttackTime = 20e-3;
static constexpr double kStftDefaultReleaseTime = 250e-3;

static constexpr double kStftFloorMagnitude = 1e-9;
static constexpr double kStftFloorMagnitudeInDB = -180.0;

static constexpr double kNegligibleDB = 0.01;

class ARFollower {
private:
    float fConst0;
    float fRec1[2];
    float fRec0[2];
    float fControl[2];

public:
    void init(float sample_rate)
    {
        fConst0 = (1.0f / sample_rate);
        clear();
        setAttackTime(0.1);
        setReleaseTime(0.1);
    }

    void clear()
    {
        for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
            fRec1[l0] = 0.0f;
        }
        for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
            fRec0[l1] = 0.0f;
        }
    }

    void setAttackTime(float at)
    {
        fControl[0] = std::exp((0.0f - (fConst0 / float(at))));
    }

    void setReleaseTime(float rt)
    {
        fControl[1] = std::exp((0.0f - (fConst0 / float(rt))));
    }

    void configureLike(const ARFollower &other) {
        fConst0 = other.fConst0;
        fControl[0] = other.fControl[0];
        fControl[1] = other.fControl[1];
    }

    float compute(float x)
    {
        float fTemp0 = float(x);
        float fTemp1 = ((fRec0[1] > fTemp0) ? fControl[1] : fControl[0]);
        fRec1[0] = ((fRec1[1] * fTemp1) + (fTemp0 * (1.0f - fTemp1)));
        fRec0[0] = fRec1[0];
        float y = float(fRec0[0]);
        fRec1[1] = fRec1[0];
        fRec0[1] = fRec0[0];
        return y;
    }
};



template <class T, class Traits>
struct ordinary_allocator {
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::true_type propagate_on_container_move_assignment;
    template <class U> struct rebind { typedef ordinary_allocator<U, Traits> other; };
    typedef std::true_type is_always_equal;

    ordinary_allocator() noexcept {}
    ordinary_allocator(const ordinary_allocator &) noexcept {}
    template <class U> ordinary_allocator(const ordinary_allocator<U, Traits>&) noexcept {}

    T *address(T &x) const noexcept;
    const T *address(const T &x) const noexcept;

    T *allocate(std::size_t n, const void * = nullptr);
    void deallocate(T *p, std::size_t n) noexcept;
    std::size_t max_size() const noexcept;

    template <class U, class... Args> void construct(U *p, Args &&...args);
    template <class U> void destroy(U *p);
};

template <class T, class Traits>
inline bool operator==(const ordinary_allocator<T, Traits> &, const ordinary_allocator<T, Traits> &) noexcept
{
    return true;
}

template <class T, class Traits>
inline bool operator!=(const ordinary_allocator<T, Traits> &, const ordinary_allocator<T, Traits> &) noexcept
{
    return false;
}

template <class T, class Traits>
inline T *ordinary_allocator<T, Traits>::address(T &x) const noexcept
{
    return &x;
}

template <class T, class Traits>
inline const T *ordinary_allocator<T, Traits>::address(const T &x) const noexcept
{
    return &x;
}

template <class T, class Traits>
T *ordinary_allocator<T, Traits>::allocate(std::size_t n, const void *)
{
    T *ptr = (T *)Traits::allocate(n * sizeof(T));
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

template <class T, class Traits>
void ordinary_allocator<T, Traits>::deallocate(T *p, std::size_t n) noexcept
{
    Traits::deallocate(p, n * sizeof(T));
}

template <class T, class Traits>
std::size_t ordinary_allocator<T, Traits>::max_size() const noexcept
{
    return std::numeric_limits<std::size_t>::max() / sizeof(T);
}

template <class T, class Traits>
template <class U, class... Args>
void ordinary_allocator<T, Traits>::construct(U *p, Args &&...args)
{
    ::new((void *)p) U(std::forward<Args>(args)...);
}

template <class T, class Traits>
template <class U>
void ordinary_allocator<T, Traits>::destroy(U *p)
{
    p->~U();
}

#define FFTW_PP_DEFINE_API(X, R, C)                                     \
    struct X(allocator_traits) {                                        \
        static void *allocate(std::size_t n)                            \
            /**/{ return ::X(malloc)(n); }                              \
        static void deallocate(void *p, std::size_t)                    \
            /**/{ ::X(free)(p); }                                       \
    };                                                                  \
                                                                        \
    template <class T> using X(allocator) =                             \
        ordinary_allocator<T, X(allocator_traits)>;                     \
                                                                        \
    template <class T> using X(vector) =                                \
        std::vector<T, X(allocator)<T>>;                                \
                                                                        \
    typedef std::vector<R, X(allocator)<R>> X(real_vector);             \
    typedef std::vector<std::complex<R>, X(allocator)<std::complex<R>>> X(complex_vector); \
                                                                        \
    struct X(plan_deleter) {                                            \
        void operator()(X(plan) p)                                      \
            /**/{ ::X(destroy_plan)(p); }                               \
    };                                                                  \
                                                                        \
    typedef std::unique_ptr<X(plan_s), X(plan_deleter)> X(plan_u);      \

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


// 2x <-> 1x: TBW = 0.01
static constexpr double C2x[12] = {
	0.036681502163648017,
	0.136547624631957715,
	0.274631759379454110,
	0.423138617436566666,
	0.561098697879194752,
	0.677540049974161618,
	0.769741833863226588,
	0.839889624849638028,
	0.892260818003878908,
	0.931541959963183896,
	0.962094548378083947,
	0.987816370732897076,
};
// 4x <-> 2x: TBW = 0.255
static constexpr double C4x[4] = {
	0.041893991997656171,
	0.168903482439952013,
	0.390560772921165922,
	0.743895748268478152,
};
// 8x <-> 4x: TBW = 0.3775
static constexpr double C8x[3] = {
	0.055748680811302048,
	0.243051195741530918,
	0.646699131192682297,
};
// 16x <-> 8x: TBW = 0.43875
static constexpr double C16x[2] = {
	0.107172166664564611,
	0.530904350331903085,
};
// 32x <-> 16x: TBW = 0.469375
static constexpr double C32x[2] = {
	0.105969237763476387,
	0.528620279623742473,
};
// 64x <-> 32x: TBW = 0.484687
static constexpr double C64x[1] = {
	0.333526281707771211,
};
// 128x <-> 64x: TBW = 0.492344
static constexpr double C128x[1] = {
	0.333381553051105561,
};

static constexpr uint32_t NC2x = sizeof(C2x) / sizeof(double);
static constexpr uint32_t NC4x = sizeof(C4x) / sizeof(double);
static constexpr uint32_t NC8x = sizeof(C8x) / sizeof(double);
static constexpr uint32_t NC16x = sizeof(C16x) / sizeof(double);
static constexpr uint32_t NC32x = sizeof(C32x) / sizeof(double);
static constexpr uint32_t NC64x = sizeof(C64x) / sizeof(double);
static constexpr uint32_t NC128x = sizeof(C128x) / sizeof(double);

///
class BasicUpsampler {
public:
    virtual ~BasicUpsampler() {}
    virtual void upsample(uint32_t count, const float *input, float **outputs) = 0;
    virtual void clear() = 0;
};

///
template <uint32_t Log2Factor> class Upsampler;

///
class BasicDownsampler {
public:
    virtual ~BasicDownsampler() {}
    virtual void downsample(uint32_t count, const float *input, float **outputs) = 0;
    virtual void clear() = 0;
};

///
template <uint32_t Log2Factor> class Downsampler;


///
struct UpsamplerStage2x : public hiir::Upsampler2xFpu<NC2x> {
    UpsamplerStage2x() { set_coefs(C2x); }
};

///
template <>
class Upsampler<1> final : public BasicUpsampler {
public:
    enum {
        Factor = 2,
        Log2Factor = 1,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
};

using Upsampler2x = Upsampler<1>;

///
struct UpsamplerStage4x : public hiir::Upsampler2xFpu<NC4x> {
    UpsamplerStage4x() { set_coefs(C4x); }
};

///
template <>
class Upsampler<2> final : public BasicUpsampler {
public:
    enum {
        Factor = 4,
        Log2Factor = 2,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
};

using Upsampler4x = Upsampler<2>;

///
struct UpsamplerStage8x : public hiir::Upsampler2xFpu<NC8x> {
    UpsamplerStage8x() { set_coefs(C8x); }
};

///
template <>
class Upsampler<3> final : public BasicUpsampler {
public:
    enum {
        Factor = 8,
        Log2Factor = 3,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
};

using Upsampler8x = Upsampler<3>;

///
struct UpsamplerStage16x : public hiir::Upsampler2xFpu<NC16x> {
    UpsamplerStage16x() { set_coefs(C16x); }
};

///
template <>
class Upsampler<4> final : public BasicUpsampler {
public:
    enum {
        Factor = 16,
        Log2Factor = 4,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
};

using Upsampler16x = Upsampler<4>;

///
struct UpsamplerStage32x : public hiir::Upsampler2xFpu<NC32x> {
    UpsamplerStage32x() { set_coefs(C32x); }
};

///
template <>
class Upsampler<5> final : public BasicUpsampler {
public:
    enum {
        Factor = 32,
        Log2Factor = 5,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
};

using Upsampler32x = Upsampler<5>;

///
struct UpsamplerStage64x : public hiir::Upsampler2xFpu<NC64x> {
    UpsamplerStage64x() { set_coefs(C64x); }
};

///
template <>
class Upsampler<6> final : public BasicUpsampler {
public:
    enum {
        Factor = 64,
        Log2Factor = 6,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
        _stage64x.process_block(outputs[5], outputs[4], count * 32);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
    UpsamplerStage64x _stage64x;
};

using Upsampler64x = Upsampler<6>;

///
struct UpsamplerStage128x : public hiir::Upsampler2xFpu<NC128x> {
    UpsamplerStage128x() { set_coefs(C128x); }
};

///
template <>
class Upsampler<7> final : public BasicUpsampler {
public:
    enum {
        Factor = 128,
        Log2Factor = 7,
    };

    void upsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 16);
        _stage64x.process_block(outputs[5], outputs[4], count * 32);
        _stage128x.process_block(outputs[6], outputs[5], count * 64);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
        _stage128x.clear_buffers();
    }

private:
    UpsamplerStage2x _stage2x;
    UpsamplerStage4x _stage4x;
    UpsamplerStage8x _stage8x;
    UpsamplerStage16x _stage16x;
    UpsamplerStage32x _stage32x;
    UpsamplerStage64x _stage64x;
    UpsamplerStage128x _stage128x;
};

using Upsampler128x = Upsampler<7>;

///
struct DownsamplerStage2x : public hiir::Downsampler2xFpu<NC2x> {
    DownsamplerStage2x() { set_coefs(C2x); }
};

template <>
class Downsampler<1> final : public BasicDownsampler {
public:
    enum {
        Factor = 2,
        Log2Factor = 1,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
};

using Downsampler2x = Downsampler<1>;

///
struct DownsamplerStage4x : public hiir::Downsampler2xFpu<NC4x> {
    DownsamplerStage4x() { set_coefs(C4x); }
};

template <>
class Downsampler<2> final : public BasicDownsampler {
public:
    enum {
        Factor = 4,
        Log2Factor = 2,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 2);
        _stage4x.process_block(outputs[1], outputs[0], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
};

using Downsampler4x = Downsampler<2>;

///
struct DownsamplerStage8x : public hiir::Downsampler2xFpu<NC8x> {
    DownsamplerStage8x() { set_coefs(C8x); }
};

template <>
class Downsampler<3> final : public BasicDownsampler {
public:
    enum {
        Factor = 8,
        Log2Factor = 3,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 4);
        _stage4x.process_block(outputs[1], outputs[0], count * 2);
        _stage8x.process_block(outputs[2], outputs[1], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
};

using Downsampler8x = Downsampler<3>;

///
struct DownsamplerStage16x : public hiir::Downsampler2xFpu<NC16x> {
    DownsamplerStage16x() { set_coefs(C16x); }
};

template <>
class Downsampler<4> final : public BasicDownsampler {
public:
    enum {
        Factor = 16,
        Log2Factor = 4,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 8);
        _stage4x.process_block(outputs[1], outputs[0], count * 4);
        _stage8x.process_block(outputs[2], outputs[1], count * 2);
        _stage16x.process_block(outputs[3], outputs[2], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
};

using Downsampler16x = Downsampler<4>;

///
struct DownsamplerStage32x : public hiir::Downsampler2xFpu<NC32x> {
    DownsamplerStage32x() { set_coefs(C32x); }
};

template <>
class Downsampler<5> final : public BasicDownsampler {
public:
    enum {
        Factor = 32,
        Log2Factor = 5,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 16);
        _stage4x.process_block(outputs[1], outputs[0], count * 8);
        _stage8x.process_block(outputs[2], outputs[1], count * 4);
        _stage16x.process_block(outputs[3], outputs[2], count * 2);
        _stage32x.process_block(outputs[4], outputs[3], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
};

using Downsampler32x = Downsampler<5>;

///
struct DownsamplerStage64x : public hiir::Downsampler2xFpu<NC64x> {
    DownsamplerStage64x() { set_coefs(C64x); }
};

template <>
class Downsampler<6> final : public BasicDownsampler {
public:
    enum {
        Factor = 64,
        Log2Factor = 6,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 32);
        _stage4x.process_block(outputs[1], outputs[0], count * 16);
        _stage8x.process_block(outputs[2], outputs[1], count * 8);
        _stage16x.process_block(outputs[3], outputs[2], count * 4);
        _stage32x.process_block(outputs[4], outputs[3], count * 2);
        _stage64x.process_block(outputs[5], outputs[4], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
    DownsamplerStage64x _stage64x;
};

using Downsampler64x = Downsampler<6>;

///
struct DownsamplerStage128x : public hiir::Downsampler2xFpu<NC128x> {
    DownsamplerStage128x() { set_coefs(C128x); }
};

template <>
class Downsampler<7> final : public BasicDownsampler {
public:
    enum {
        Factor = 128,
        Log2Factor = 7,
    };

    void downsample(uint32_t count, const float *input, float **outputs) override
    {
        _stage2x.process_block(outputs[0], input, count * 64);
        _stage4x.process_block(outputs[1], outputs[0], count * 32);
        _stage8x.process_block(outputs[2], outputs[1], count * 16);
        _stage16x.process_block(outputs[3], outputs[2], count * 8);
        _stage32x.process_block(outputs[4], outputs[3], count * 4);
        _stage64x.process_block(outputs[5], outputs[4], count * 2);
        _stage128x.process_block(outputs[6], outputs[5], count);
    }

    void clear() override
    {
        _stage2x.clear_buffers();
        _stage4x.clear_buffers();
        _stage8x.clear_buffers();
        _stage16x.clear_buffers();
        _stage32x.clear_buffers();
        _stage64x.clear_buffers();
        _stage128x.clear_buffers();
    }

private:
    DownsamplerStage2x _stage2x;
    DownsamplerStage4x _stage4x;
    DownsamplerStage8x _stage8x;
    DownsamplerStage16x _stage16x;
    DownsamplerStage32x _stage32x;
    DownsamplerStage64x _stage64x;
    DownsamplerStage128x _stage128x;
};

using Downsampler128x = Downsampler<7>;


class STFT final : public SteppingAnalyzer {
public:
    void configure(const Configuration &config) override;

private:
    void processNewBlock(float *input) override;

private:
    fftwf_plan _fftPlan {};
    double _sampleRate {};

    // temporary
    fftwf_complex_vector _cpx;
};


template <uint32_t Rates>
class MultirateSTFT final : public BasicAnalyzer {
public:
    void configure(const Configuration &config) override;
    void setAttackAndRelease(float attack, float release) override;
    void clear() override;
    void process(const float *input, uint32_t numFrames) override;

private:
    void processMultirate(const float *input, uint32_t numFrames);
    void processOutputBins();

private:
    static constexpr uint32_t Log2Factor = Rates - 1;
    static constexpr uint32_t Factor = 1u << Log2Factor;

    STFT fStft[Rates];
    Downsampler<Log2Factor> fDownsampler;

    struct BinMapping { uint32_t rate; uint32_t bin; };
    std::vector<BinMapping> fBinMapping;

    uint32_t fNumRemainder = 0;
    float fRemainder[Factor] = {};

    static constexpr uint32_t TempSamples = 1024u;
    float fTemp[TempSamples];
};

template <uint32_t Rates>
void MultirateSTFT<Rates>::configure(const Configuration &config)
{
    const uint32_t windowSize = config.windowSize;
    const uint32_t specSize = windowSize / 2;

    // one full-spectrum (lower), half-spectrum others
    const uint32_t numBins = specSize + (Rates-1) * specSize / 2;

    configureBasic(numBins);

    //
    Configuration rateConfig[Rates];
    for (uint32_t r = 0; r < Rates; ++r) {
        rateConfig[r] = config;
        rateConfig[r].stepSize = config.stepSize / (1u << r);
        rateConfig[r].sampleRate = config.sampleRate / (1u << r);
        fStft[r].configure(rateConfig[r]);

        // skip processing the bins we don't need and their smoothers
        if (r == Rates - 1)
            fStft[r].configureBinRange(0, specSize);
        else
            fStft[r].configureBinRange(specSize / 2, specSize);
    }

    //
    float *frequencies = getFrequencies();
    uint32_t frequencyIndex = 0;

    fBinMapping.resize(numBins);
    BinMapping *binMappings = fBinMapping.data();

    for (uint32_t r = Rates; r-- > 0;) {
        double sampleRate = rateConfig[r].sampleRate;
        if (r == Rates - 1) {
            for (uint32_t b = 0; b < specSize; ++b) {
                frequencies[frequencyIndex] = b * sampleRate / windowSize;
                binMappings[frequencyIndex].rate = r;
                binMappings[frequencyIndex].bin = b;
                ++frequencyIndex;
            }
        }
        else {
            for (uint32_t b = specSize / 2; b < specSize; ++b) {
                frequencies[frequencyIndex] = b * sampleRate / windowSize;
                binMappings[frequencyIndex].rate = r;
                binMappings[frequencyIndex].bin = b;
                ++frequencyIndex;
            }
        }
    }
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::setAttackAndRelease(float attack, float release)
{
    for (uint32_t r = 0; r < Rates; ++r)
        fStft[r].setAttackAndRelease(attack, release);
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::clear()
{
    BasicAnalyzer::clear();

    for (uint32_t r = 0; r < Rates; ++r)
        fStft[r].clear();

    fDownsampler.clear();

    fNumRemainder = 0;

    std::fill_n(getMagnitudes(), getNumBins(), 20.0 * std::log10(kStftFloorMagnitude));
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::process(const float *input, uint32_t numFrames)
{
    uint32_t numRemainder = fNumRemainder;
    float *remainder = fRemainder;

    //
    if (numRemainder > 0) {
        uint32_t numMissing = Factor - numRemainder;
        uint32_t numAvail = (numFrames < numMissing) ? numFrames : numMissing;

        std::copy_n(input, numAvail, &remainder[numRemainder]);
        numRemainder += numAvail;

        input += numAvail;
        numFrames -= numAvail;

        if (numRemainder < Factor)
            return;
        numRemainder = 0;

        processMultirate(remainder, Factor);
    }

    //
    while (numFrames > 0) {
        uint32_t currentFrames = (numFrames < TempSamples) ? numFrames : TempSamples;
        currentFrames &= ~(uint32_t)(Factor - 1);

        if (currentFrames > 0)
            processMultirate(input, currentFrames);
        else {
            currentFrames = numFrames;
            std::copy_n(input, currentFrames, remainder);
            numRemainder = currentFrames;
        }

        input += currentFrames;
        numFrames -= currentFrames;
    }

    processOutputBins();

    fNumRemainder = numRemainder;
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::processMultirate(const float *input, uint32_t numFrames)
{
    assert(numFrames < TempSamples);
    assert(numFrames % Factor == 0);

    float *downsampledInputs[Rates - 1];
    downsampledInputs[0] = fTemp;
    for (uint32_t r = 1, l = TempSamples / 2; r < Rates - 1; ++r, l /= 2)
        downsampledInputs[r] = downsampledInputs[r - 1] + l;

    fDownsampler.downsample(numFrames / Factor, input, downsampledInputs);

    fStft[0].process(input, numFrames);
    for (uint32_t r = 1; r < Rates; ++r)
        fStft[r].process(downsampledInputs[r - 1], numFrames / (1u << r));
}

template <uint32_t Rates>
void MultirateSTFT<Rates>::processOutputBins()
{
    const uint32_t numBins = getNumBins();

    const float* multirateMags[Rates];
    for (uint32_t r = 0; r < Rates; ++r)
        multirateMags[r] = fStft[r].getMagnitudes();

    float* mags = getMagnitudes();
    const BinMapping *binMappings = fBinMapping.data();
    for (uint32_t b = 0; b < numBins; ++b)
        mags[b] = multirateMags[binMappings[b].rate][binMappings[b].bin];
}

///
template <uint32_t Rates> constexpr uint32_t MultirateSTFT<Rates>::Log2Factor;
template <uint32_t Rates> constexpr uint32_t MultirateSTFT<Rates>::Factor;
template <uint32_t Rates> constexpr uint32_t MultirateSTFT<Rates>::TempSamples;

///
template class MultirateSTFT<2>;
template class MultirateSTFT<3>;
template class MultirateSTFT<4>;
template class MultirateSTFT<5>;
template class MultirateSTFT<6>;
template class MultirateSTFT<7>;
template class MultirateSTFT<8>;


}