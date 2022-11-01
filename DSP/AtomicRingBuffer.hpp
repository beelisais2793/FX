//          Copyright Jean Pierre Cimalando 2018-2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <memory>
#include <atomic>
#include <shared_mutex>
#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <mutex>
#include <cassert>

#if defined(__cpp_lib_atomic_is_always_lock_free)
static_assert(
    std::atomic<size_t>::is_always_lock_free, "atomic<size_t> must be lock free");
#endif

//------------------------------------------------------------------------------
template <bool> class Ring_Buffer_Ex;
typedef Ring_Buffer_Ex<true> Ring_Buffer;

template <class> class Soft_Ring_Buffer_Ex;
#if defined(__cpp_lib_shared_mutex)
typedef Soft_Ring_Buffer_Ex<std::shared_mutex> Soft_Ring_Buffer;
#else
typedef Soft_Ring_Buffer_Ex<std::shared_timed_mutex> Soft_Ring_Buffer;
#endif

//------------------------------------------------------------------------------
template <class RB>
class Basic_Ring_Buffer {
public:
    // read operations
    template <class T> bool get(T &x);
    template <class T> bool get(T *x, size_t n);
    template <class T> bool peek(T &x);
    template <class T> bool peek(T *x, size_t n);
    // write operations
    template <class T> bool put(const T &x);
    template <class T> bool put(const T *x, size_t n);
};

//------------------------------------------------------------------------------
template <bool Atomic>
class Ring_Buffer_Ex final :
    private Basic_Ring_Buffer<Ring_Buffer_Ex<Atomic>> {
private:
    typedef Basic_Ring_Buffer<Ring_Buffer_Ex<Atomic>> Base;
public:
    // initialization and cleanup
    explicit Ring_Buffer_Ex(size_t capacity);
    ~Ring_Buffer_Ex();
    // attributes
    size_t capacity() const;
    // read operations
    size_t size_used() const;
    bool discard(size_t len);
    using Base::get;
    using Base::peek;
    // write operations
    size_t size_free() const;
    using Base::put;

    static constexpr bool can_extend() { return false; }

private:
    size_t cap_{0};
    std::conditional_t<Atomic, std::atomic<size_t>, size_t> rp_{0}, wp_{0};
    std::unique_ptr<uint8_t[]> rbdata_ {};
    friend Base;
    template <class> friend class Soft_Ring_Buffer_Ex;
    bool getbytes_(void *data, size_t len);
    bool peekbytes_(void *data, size_t len) const;
    bool getbytes_ex_(void *data, size_t len, bool advp);
    bool putbytes_(const void *data, size_t len);
};

//------------------------------------------------------------------------------
template <class Mutex>
class Soft_Ring_Buffer_Ex final :
    private Basic_Ring_Buffer<Soft_Ring_Buffer_Ex<Mutex>> {
private:
    typedef Basic_Ring_Buffer<Soft_Ring_Buffer_Ex<Mutex>> Base;
public:
    // initialization and cleanup
    explicit Soft_Ring_Buffer_Ex(size_t capacity);
    ~Soft_Ring_Buffer_Ex();
    // attributes
    size_t capacity() const;
    // read operations
    size_t size_used() const;
    bool discard(size_t len);
    using Base::get;
    using Base::peek;
    // write operations
    size_t size_free() const;
    using Base::put;

    static constexpr bool can_extend() { return true; }

private:
    Ring_Buffer_Ex<false> rb_;
    mutable Mutex shmutex_;
    friend Base;
    bool getbytes_(void *data, size_t len);
    bool peekbytes_(void *data, size_t len) const;
    bool putbytes_(const void *data, size_t len);
    void grow_(size_t newcap);
};

//------------------------------------------------------------------------------



template <bool Atomic>
inline size_t Ring_Buffer_Ex<Atomic>::capacity() const
{
    return cap_ - 1;
}

//------------------------------------------------------------------------------
template <class Mutex>
inline Soft_Ring_Buffer_Ex<Mutex>::Soft_Ring_Buffer_Ex(size_t capacity)
    : rb_(capacity)
{
}

template <class Mutex>
inline Soft_Ring_Buffer_Ex<Mutex>::~Soft_Ring_Buffer_Ex()
{
}

template <class Mutex>
inline size_t Soft_Ring_Buffer_Ex<Mutex>::capacity() const
{
    return rb_.capacity();
}

//------------------------------------------------------------------------------
template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::get(T &x)
{
    return get(&x, 1);
}

template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::get(T *x, size_t n)
{
    static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
    RB *self = static_cast<RB *>(this);
    return self->getbytes_(x, n * sizeof(T));
}

template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::peek(T &x)
{
    return peek(&x, 1);
}

template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::peek(T *x, size_t n)
{
    static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
    RB *self = static_cast<RB *>(this);
    return self->peekbytes_(x, n * sizeof(T));
}

template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::put(const T &x)
{
    return put(&x, 1);
}

template <class RB>
template <class T>
inline bool Basic_Ring_Buffer<RB>::put(const T *x, size_t n)
{
    static_assert(std::is_trivially_copyable<T>::value, "ring_buffer: T must be trivially copyable");
    RB *self = static_cast<RB *>(this);
    return self->putbytes_(x, n * sizeof(T));
}

#if defined(__cpp_if_constexpr)
#    define if_constexpr if constexpr
#else
#    define if_constexpr if
#endif

namespace {

template <class T>
inline T atomic_load_maybe(const std::atomic<T> &src, std::memory_order order)
{
    return src.load(order);
}

template <class T>
inline T atomic_load_maybe(const T &src, std::memory_order)
{
    return src;
}

template <class T>
inline void atomic_store_maybe(std::atomic<T> &dst, const T &value, std::memory_order order)
{
    return dst.store(value, order);
}

template <class T>
inline void atomic_store_maybe(T &dst, const T &value, std::memory_order)
{
    dst = value;
}

} // namespace

//------------------------------------------------------------------------------
template <bool Atomic>
Ring_Buffer_Ex<Atomic>::Ring_Buffer_Ex(size_t capacity)
    : cap_(capacity + 1),
      rbdata_(new uint8_t[capacity + 1])
{
}

template <bool Atomic>
Ring_Buffer_Ex<Atomic>::~Ring_Buffer_Ex()
{
}

template <bool Atomic>
size_t Ring_Buffer_Ex<Atomic>::size_used() const
{
    const size_t rp = atomic_load_maybe(rp_, std::memory_order_relaxed);
    const size_t wp = atomic_load_maybe(wp_, std::memory_order_relaxed);
    const size_t cap = cap_;
    return wp + ((wp < rp) ? cap : 0) - rp;
}

template <bool Atomic>
bool Ring_Buffer_Ex<Atomic>::discard(size_t len)
{
    return getbytes_ex_(nullptr, len, true);
}

template <bool Atomic>
size_t Ring_Buffer_Ex<Atomic>::size_free() const
{
    const size_t rp = atomic_load_maybe(rp_, std::memory_order_relaxed);
    const size_t wp = atomic_load_maybe(wp_, std::memory_order_relaxed);
    const size_t cap = cap_;
    return rp + ((rp <= wp) ? cap : 0) - wp - 1;
}

template <bool Atomic>
bool Ring_Buffer_Ex<Atomic>::getbytes_(void *data, size_t len)
{
    return getbytes_ex_(data, len, true);
}

template <bool Atomic>
bool Ring_Buffer_Ex<Atomic>::peekbytes_(void *data, size_t len) const
{
    auto *ncthis = const_cast<Ring_Buffer_Ex<Atomic> *>(this);
    return ncthis->getbytes_ex_(data, len, false);
}

template <bool Atomic>
bool Ring_Buffer_Ex<Atomic>::getbytes_ex_(void *data, size_t len, bool advp)
{
    if (len == 0)
        return true;

    const size_t rp = atomic_load_maybe(rp_, std::memory_order_relaxed);
    const size_t wp = atomic_load_maybe(wp_, data ? std::memory_order_acquire : std::memory_order_relaxed);
    const size_t cap = cap_;

    // same as `size_used()` except `wp` memory order to synchronize data
    size_t used = wp + ((wp < rp) ? cap : 0) - rp;

    if (used < len)
        return false;

    const uint8_t *src = rbdata_.get();
    uint8_t *dst = (uint8_t *)data;

    if (data) {
        const size_t taillen = std::min(len, cap - rp);
        std::copy_n(&src[rp], taillen, dst);
        std::copy_n(src, len - taillen, dst + taillen);
    }

    if (advp)
        atomic_store_maybe(rp_, (rp + len < cap) ? (rp + len) : (rp + len - cap), std::memory_order_relaxed);
    return true;
}

template <bool Atomic>
bool Ring_Buffer_Ex<Atomic>::putbytes_(const void *data, size_t len)
{
    if (len == 0)
        return true;

    if (size_free() < len)
        return false;

    const size_t wp = atomic_load_maybe(wp_, std::memory_order_relaxed);
    const size_t cap = cap_;
    const uint8_t *src = (const uint8_t *)data;
    uint8_t *dst = rbdata_.get();

    const size_t taillen = std::min(len, cap - wp);
    std::copy_n(src, taillen, &dst[wp]);
    std::copy_n(src + taillen, len - taillen, dst);

    atomic_store_maybe(wp_, (wp + len < cap) ? (wp + len) : (wp + len - cap), std::memory_order_release);
    return true;
}

template class Ring_Buffer_Ex<true>;
template class Ring_Buffer_Ex<false>;

//------------------------------------------------------------------------------
template <class Mutex>
size_t Soft_Ring_Buffer_Ex<Mutex>::size_used() const
{
    std::shared_lock<Mutex> lock(shmutex_);
    return rb_.size_used();
}

template <class Mutex>
bool Soft_Ring_Buffer_Ex<Mutex>::discard(size_t len)
{
    std::shared_lock<Mutex> lock(shmutex_);
    return rb_.discard(len);
}

template <class Mutex>
size_t Soft_Ring_Buffer_Ex<Mutex>::size_free() const
{
    std::shared_lock<Mutex> lock(shmutex_);
    return rb_.size_free();
}

template <class Mutex>
bool Soft_Ring_Buffer_Ex<Mutex>::getbytes_(void *data, size_t len)
{
    std::shared_lock<Mutex> lock(shmutex_);
    return rb_.getbytes_(data, len);
}

template <class Mutex>
bool Soft_Ring_Buffer_Ex<Mutex>::peekbytes_(void *data, size_t len) const
{
    std::shared_lock<Mutex> lock(shmutex_);
    auto &ncrb = const_cast<Ring_Buffer_Ex<false> &>(rb_);
    return ncrb.peekbytes_(data, len);
}

template <class Mutex>
bool Soft_Ring_Buffer_Ex<Mutex>::putbytes_(const void *data, size_t len)
{
    bool good;
    size_t oldcap = rb_.capacity();
    size_t atleastcap = size_used() + len;
    if (atleastcap <= oldcap) {
        std::shared_lock<Mutex> lock(shmutex_);
        good = rb_.putbytes_(data, len);
    }
    else {
        std::unique_lock<Mutex> lock(shmutex_);
        grow_(atleastcap);
        good = rb_.putbytes_(data, len);
    }
    assert(good);
    (void)good;
    return true;
}

template <class Mutex>
void Soft_Ring_Buffer_Ex<Mutex>::grow_(size_t atleastcap)
{
    size_t oldcap = rb_.capacity();
    size_t newcap = (oldcap < 16) ? 16 : oldcap;
    while (newcap < atleastcap) {
        if (newcap > std::numeric_limits<size_t>::max() / 3)
            throw std::bad_alloc();
        newcap = newcap * 3 / 2;
    }

    size_t len = rb_.size_used();
    std::unique_ptr<uint8_t[]> newdata(new uint8_t[newcap + 1]);

    {
        const size_t rp = rb_.rp_;
        const uint8_t *src = rb_.rbdata_.get();
        uint8_t *dst = newdata.get();

        const size_t taillen = std::min(len, oldcap + 1 - rp);
        std::copy_n(&src[rp], taillen, dst);
        std::copy_n(src, len - taillen, dst + taillen);
    }

    rb_.cap_ = newcap + 1;
    rb_.rp_ = 0;
    rb_.wp_ = len;
    rb_.rbdata_ = std::move(newdata);
}

#if defined(__cpp_lib_shared_mutex)
template class Soft_Ring_Buffer_Ex<std::shared_mutex>;
#endif
template class Soft_Ring_Buffer_Ex<std::shared_timed_mutex>;