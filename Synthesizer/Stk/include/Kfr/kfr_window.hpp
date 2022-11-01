#pragma once 

namespace DSP {
    using window_symmetry = kfr::window_symmetry;

    template<typename T>
    kfr::univector<T> make_univec(const T * data, size_t s) {
        return kfr::univector<T>(kfr::univector<T>(data,s)); 
    }

    template<typename T>
    kfr::univector<T> make_window_hann(size_t s) {
        return kfr::univector<T>(kfr::window_hann<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_hamming(size_t s) {
        return kfr::univector<T>(kfr::window_hamming<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_blackman(size_t s, const T alpha=T(0.16), window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return kfr::univector<T>(kfr::window_blackman<T>(s,alpha,symmetry));
    }
    template<typename T>
    kfr::univector<T> make_window_blackman_harris(size_t s, window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return kfr::univector<T>(kfr::window_blackman_harris<T>(s,symmetry));
    }
    template<typename T>
    kfr::univector<T> make_window_gaussian(size_t s, const T alpha=T(0.25)) {
        return kfr::univector<T>(kfr::window_gaussian<T>(s,alpha));
    }
    template<typename T>
    kfr::univector<T> make_window_triangular(size_t s) {
        return kfr::univector<T>(kfr::window_triangular<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_bartlett(size_t s) {
        return kfr::univector<T>(kfr::window_bartlett<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_cosine(size_t s) {
        return kfr::univector<T>(kfr::window_cosine<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_bartlett_hann(size_t s) {
        return kfr::univector<T>(kfr::window_bartlett_hann<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_bohman(size_t s) {
        return kfr::univector<T>(kfr::window_bohman<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_lanczos(size_t s) {
        return kfr::univector<T>(kfr::window_lanczos<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_flattop(size_t s) {
        return kfr::univector<T>(kfr::window_flattop<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_rectangular(size_t s) {
        return kfr::univector<T>(kfr::window_rectangular<T>(s));
    }
    template<typename T>
    kfr::univector<T> make_window_kaiser(size_t s, const T beta = T(0.5)) {
        return kfr::univector<T>(kfr::window_kaiser<T>(s,beta));
    }

    template<typename T>
    kfr::expression_pointer<T> make_window_hann_ptr(size_t s) {
        return to_pointer(kfr::window_hann<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_hamming_ptr(size_t s) {
        return to_pointer(kfr::window_hamming<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_blackman_ptr(size_t s, const T alpha=T(0.16), window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return to_pointer(kfr::window_blackman<T>(s,alpha,symmetry));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_blackman_harris_ptr(size_t s, window_symmetry symmetry = kfr::window_symmetry::symmetric) {
        return to_pointer(kfr::window_blackman_harris<T>(s,symmetry));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_gaussian_ptr(size_t s, const T alpha=T(0.25)) {
        return to_pointer(kfr::window_gaussian<T>(s,alpha));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_triangular_ptr(size_t s) {
        return to_pointer(kfr::window_triangular<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bartlett_ptr(size_t s) {
        return to_pointer(kfr::window_bartlett<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_cosine_ptr(size_t s) {
        return to_pointer(kfr::window_cosine<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bartlett_hann_ptr(size_t s) {
        return to_pointer(kfr::window_bartlett_hann<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_bohman_ptr(size_t s) {
        return to_pointer(kfr::window_bohman<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_lanczos_ptr(size_t s) {
        return to_pointer(kfr::window_lanczos<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_flattop_ptr(size_t s) {
        return to_pointer(kfr::window_flattop<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_rectangular_ptr(size_t s) {
        return to_pointer(kfr::window_rectangular<T>(s));
    }
    template<typename T>
    kfr::expression_pointer<T> make_window_kaiser_ptr(size_t s, const T beta = T(0.5)) {
        return to_pointer(kfr::window_kaiser<T>(s,beta));
    }
}