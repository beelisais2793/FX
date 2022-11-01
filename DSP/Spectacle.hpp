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

namespace Spectacle
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
}