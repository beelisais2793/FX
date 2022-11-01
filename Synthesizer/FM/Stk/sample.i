%module std_sample
%{
//#include "SampleVector.h"
#include "sample.hpp"
#include "sample_dsp.hpp"
%}

%include "stdint.i"
%include "std_vector.i"

%template(float_vector) std::vector<float>;
%template(double_vector) std::vector<double>;
%template(int8_vector) std::vector<signed char>;
%template(uint8_vector) std::vector<unsigned char>;
%template(int16_vector) std::vector<signed short>;
%template(uint16_vector) std::vector<unsigned short>;
%template(int32_vector) std::vector<signed int>;
%template(uint32_vector) std::vector<unsigned int>;
%template(int64_vector) std::vector<signed long>;
%template(uint64_vector) std::vector<unsigned long>;

//%include "SampleVector.h"
%include "sample.hpp"
%include "sample_dsp.hpp"

//%template(FloatSampleVector) SoundWave::SampleVector<float>;

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
