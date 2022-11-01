%module sample
%{
#include "SampleVector.h"
#include "samples.hpp"
%}

%include "stdint.i"
%include "std_vector.i"
%include "std_complex.i"

%template(fcomplex) std::complex<float>;
%template(zcomplex) std::complex<double>;

%template(float_vector) std::vector<float>;
%template(double_vector) std::vector<double>;
%template(fcomplex_vector) std::vector<std::complex<float>>;
%template(zcomplex_vector) std::vector<std::complex<double>>;

%template(int8_vector) std::vector<signed char>;
%template(uint8_vector) std::vector<unsigned char>;
%template(int16_vector) std::vector<signed short>;
%template(uint16_vector) std::vector<unsigned short>;
%template(int32_vector) std::vector<signed int>;
%template(uint32_vector) std::vector<unsigned int>;
%template(int64_vector) std::vector<signed long>;
%template(uint64_vector) std::vector<unsigned long>;

%include "SampleVector.h"
%include "samples.hpp"

%extend SoundWave::SampleVector
{
        T     __getitem__(size_t i) { return $self->vector[i]; }
        void  __setitem__(size_t i, T val) { $self->vector[i] = val; }
}
%extend SoundWave::Vector
{
        T __getitem(size_t i) { return $self->vector(i); }
        void __setitem(size_t i, T val) { $self->vector(i) = val; }
}
%extend SoundWave::Matrix
{
        MatrixView<T> __getitem__(int r) { 
            if(r < 0) r += rows();
            return MatrixView<T>(&$self->matrix, r);
        }
        void __setitem__(int r, const Vector<T> & v) { 
            if(r < 0) r += $self->rows();
            $self->matrix.row(r) = v.vector; 
        }
}
%extend SoundWave::ColMatrix
{
        ColMatrixView<T> __getitem__(int r) { 
            if(r < 0) r += $self->rows();
            return ColMatrixView<T>(&$self->matrix, r);
        }
        void __setitem__(int r, const ColVector<T> & v) { 
            if(r < 0) r += $self->rows();
            $self->matrix.row(r) = v.vector; 
        }
}
%extend SoundWave::ColVector
{
        T       __getitem__(size_t i) { return $self->vector(i); }
        void    __setitem__(size_t i, T val) { $self->vector(i) = val; }
}
%extend SoundWave::Window
{
        T    __getitem__(size_t i) { return $self->window[i]; }
        void __setitem__(size_t i, T val) { $self->window[i] = val; }
}
%template(FloatSampleVector) SoundWave::SampleVector<float>;

%template(get_left_channel_float) SoundWave::get_left_channel<float>;
%template(get_right_channel_float) SoundWave::get_right_channel<float>;
%template(get_channel_float) SoundWave::get_channel<float>;

%template(interleave_float) SoundWave::interleave<float>;
%template(deinterleave_float) SoundWave::interleave<float>;
%template(copy_vector_float) SoundWave::copy_vector<float>;
%template(slice_vector_float) SoundWave::slice_vector<float>;
%template(copy_buffer_float) SoundWave::copy_buffer<float>;
%template(slice_buffer_float) SoundWave::slice_buffer<float>;
%template(stereo_split_float) SoundWave::split_stereo<float>;
%template(insert_front_float) SoundWave::insert_front<float>;

%template(containsOnlyZeros_float) SoundWave::containsOnlyZeros<float>;
%template(isAllPositiveOrZero_float) SoundWave::isAllPositiveOrZero<float>;
%template(isAllNegativeOrZero_float) SoundWave::isAllNegativeOrZero<float>;
%template(contains_float) SoundWave::contains<float>;
%template(max_float) SoundWave::max<float>;
%template(min_float) SoundWave::min<float>;
%template(maxIndex_float) SoundWave::maxIndex<float>;
%template(minIndex_float) SoundWave::minIndex<float>;
%template(printVector_float) SoundWave::printVector<float>;
%template(getFirstElement_float) SoundWave::getFirstElement<float>;
%template(getLastElement_float) SoundWave::getLastElement<float>;
%template(getEvenElements_float) SoundWave::getEvenElements<float>;
%template(getOddElements_float) SoundWave::getOddElements<float>;
%template(getEveryNthElementStartingFromK_float) SoundWave::getEveryNthElementStartingFromK<float>;
%template(fillVectorWith_float) SoundWave::fillVectorWith<float>;
%template(countOccurrencesOf_float) SoundWave::countOccurrencesOf<float>;
%template(sum_float) SoundWave::sum<float>;
%template(product_float) SoundWave::product<float>;
%template(mean_float) SoundWave::mean<float>;
%template(median_float) SoundWave::median<float>;
%template(variance_float) SoundWave::variance<float>;
%template(standardDeviation_float) SoundWave::standardDeviation<float>;
%template(norm1_float) SoundWave::norm1<float>;
%template(norm2_float) SoundWave::norm2<float>;
%template(normP_float) SoundWave::normP<float>;
%template(magnitude_float) SoundWave::magnitude<float>;
%template(multiplyInPlace_float) SoundWave::multiplyInPlace<float>;
%template(divideInPlace_float) SoundWave::divideInPlace<float>;
%template(addInPlace_float) SoundWave::addInPlace<float>;
%template(subtractInPlace_float) SoundWave::subtractInPlace<float>;
%template(absInPlace_float) SoundWave::absInPlace<float>;
%template(squareInPlace_float) SoundWave::squareInPlace<float>;
%template(squareRootInPlace_float) SoundWave::squareRootInPlace<float>;
%template(sort_float) SoundWave::sort<float>;
%template(reverse_float) SoundWave::reverse<float>;
%template(multiply_float) SoundWave::multiply<float>;
%template(divide_float) SoundWave::divide<float>;
%template(add_float) SoundWave::add<float>;
%template(subtract_float) SoundWave::subtract<float>;
%template(abs_float) SoundWave::abs<float>;
%template(square_float) SoundWave::square<float>;
%template(squareRoot_float) SoundWave::squareRoot<float>;
%template(scale_float) SoundWave::scale<float>;
%template(difference_float) SoundWave::difference<float>;
%template(zeros_float) SoundWave::zeros<float>;
%template(ones_float) SoundWave::ones<float>;
%template(range_float) SoundWave::range<float>;
%template(dotProduct_float) SoundWave::dotProduct<float>;
%template(euclideanDistance_float) SoundWave::euclideanDistance<float>;
%template(cosineSimilarity_float) SoundWave::cosineSimilarity<float>;
%template(cosineDistance_float) SoundWave::cosineDistance<float>;

%template(absf) SoundWave::abs<float>;
%template(abs2f) SoundWave::abs2<float>;
%template(inversef) SoundWave::inverse<float>;
%template(expf) SoundWave::exp<float>;
%template(logf) SoundWave::log<float>;
%template(log1pf) SoundWave::log1p<float>;
%template(log10f) SoundWave::log10<float>;
%template(powf) SoundWave::pow<float>;
%template(sqrtf) SoundWave::sqrt<float>;
%template(rsqrtf) SoundWave::rsqrt<float>;
%template(square) SoundWave::square<float>;
%template(cube) SoundWave::cube<float>;
%template(sinf) SoundWave::sin<float>;
%template(cosf) SoundWave::cos<float>;
%template(tanf) SoundWave::tan<float>;
%template(asinf) SoundWave::asin<float>;
%template(acosf) SoundWave::acos<float>;
%template(atanf) SoundWave::atan<float>;
%template(sinhf) SoundWave::sinh<float>;
%template(coshf) SoundWave::cosh<float>;
%template(tanhf) SoundWave::tanh<float>;
%template(asinhf) SoundWave::asinh<float>;
%template(acoshf) SoundWave::acosh<float>;
%template(atanhf) SoundWave::atanh<float>;
%template(floorf) SoundWave::floor<float>;
%template(ceilf) SoundWave::ceil<float>;
%template(roundf) SoundWave::round<float>;
%template(rintf) SoundWave::rint<float>;
%template(sizef) SoundWave::size<float>;
%template(randomf) SoundWave::random<float>;
%template(fillf) SoundWave::fill<float>;
%template(resizef) SoundWave::resize<float>;


%template(cfreal) std::real<float>;
%template(czreal) std::real<double>;

%template(cfimag) std::imag<float>;
%template(czimag) std::imag<double>;

%template(cfabs) std::abs<float>;
%template(czabs) std::abs<double>;

%template(cfarg) std::arg<float>;
%template(czarg) std::arg<double>;

%template(cfnorm) std::norm<float>;
%template(cznorm) std::norm<double>;

%template(cfproj) std::proj<float>;
%template(czproj) std::proj<double>;

%template(cfpolar) std::polar<float>;
%template(czpolar) std::polar<double>;

%template(cfexp) std::exp<float>;
%template(czexp) std::exp<double>;

%template(cflog) std::log<float>;
%template(czlog) std::log<double>;

%template(cflog10) std::log10<float>;
%template(czlog10) std::log10<double>;

%template(cfpow) std::pow<float>;
%template(czpow) std::pow<double>;

%template(cfsqrt) std::sqrt<float>;
%template(czsqrt) std::sqrt<double>;

%template(cfsin) std::sin<float>;
%template(czsin) std::sin<double>;
    
%template(cfcos) std::cos<float>;
%template(czcos) std::cos<double>;

%template(cftan) std::tan<float>;
%template(cztan) std::tan<double>;

%template(cfasin) std::asin<float>;
%template(czasin) std::asin<double>;
    
%template(cfacos) std::acos<float>;
%template(czacos) std::acos<double>;

%template(cfatan) std::atan<float>;
%template(czatan) std::atan<double>;

%template(cfsinh) std::sinh<float>;
%template(czsinh) std::sinh<double>;
    
%template(cfcosh) std::cosh<float>;
%template(czcosh) std::cosh<double>;

%template(cftanh) std::tanh<float>;
%template(cztanh) std::tanh<double>;

%template(cfasinh) std::asinh<float>;
%template(czasinh) std::asinh<double>;
    
%template(cfacosh) std::acosh<float>;
%template(czacosh) std::acosh<double>;

%template(cfatanh) std::atanh<float>;
%template(czatanh) std::atanh<double>;
