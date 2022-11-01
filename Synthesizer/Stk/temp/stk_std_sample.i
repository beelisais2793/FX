%module kfr_sample
%{
//#include "SampleVector.h"
//#include "SoundAlchemy.hpp"
#include "samples/sample.hpp"
#include "samples/sample_dsp.hpp"
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
//%include "SoundAlchemy.hpp"
%include "samples/sample.hpp"
%include "samples/sample_dsp.hpp"

//%template(FloatSampleVector) SoundAlchemy::SampleVector<float>;

%template(get_left_channel_float) SoundAlchemy::get_left_channel<float>;
%template(get_right_channel_float) SoundAlchemy::get_right_channel<float>;
%template(get_channel_float) SoundAlchemy::get_channel<float>;

%template(interleave_float) SoundAlchemy::interleave<float>;
%template(deinterleave_float) SoundAlchemy::interleave<float>;
%template(copy_vector_float) SoundAlchemy::copy_vector<float>;
%template(slice_vector_float) SoundAlchemy::slice_vector<float>;
%template(copy_buffer_float) SoundAlchemy::copy_buffer<float>;
%template(slice_buffer_float) SoundAlchemy::slice_buffer<float>;
%template(stereo_split_float) SoundAlchemy::split_stereo<float>;
%template(insert_front_float) SoundAlchemy::insert_front<float>;

%template(containsOnlyZeros_float) SoundAlchemy::containsOnlyZeros<float>;
%template(isAllPositiveOrZero_float) SoundAlchemy::isAllPositiveOrZero<float>;
%template(isAllNegativeOrZero_float) SoundAlchemy::isAllNegativeOrZero<float>;
%template(contains_float) SoundAlchemy::contains<float>;
%template(max_float) SoundAlchemy::max<float>;
%template(min_float) SoundAlchemy::min<float>;
%template(maxIndex_float) SoundAlchemy::maxIndex<float>;
%template(minIndex_float) SoundAlchemy::minIndex<float>;
%template(printVector_float) SoundAlchemy::printVector<float>;
%template(getFirstElement_float) SoundAlchemy::getFirstElement<float>;
%template(getLastElement_float) SoundAlchemy::getLastElement<float>;
%template(getEvenElements_float) SoundAlchemy::getEvenElements<float>;
%template(getOddElements_float) SoundAlchemy::getOddElements<float>;
%template(getEveryNthElementStartingFromK_float) SoundAlchemy::getEveryNthElementStartingFromK<float>;
%template(fillVectorWith_float) SoundAlchemy::fillVectorWith<float>;
%template(countOccurrencesOf_float) SoundAlchemy::countOccurrencesOf<float>;
%template(sum_float) SoundAlchemy::sum<float>;
%template(product_float) SoundAlchemy::product<float>;
%template(mean_float) SoundAlchemy::mean<float>;
%template(median_float) SoundAlchemy::median<float>;
%template(variance_float) SoundAlchemy::variance<float>;
%template(standardDeviation_float) SoundAlchemy::standardDeviation<float>;
%template(norm1_float) SoundAlchemy::norm1<float>;
%template(norm2_float) SoundAlchemy::norm2<float>;
%template(normP_float) SoundAlchemy::normP<float>;
%template(magnitude_float) SoundAlchemy::magnitude<float>;
%template(multiplyInPlace_float) SoundAlchemy::multiplyInPlace<float>;
%template(divideInPlace_float) SoundAlchemy::divideInPlace<float>;
%template(addInPlace_float) SoundAlchemy::addInPlace<float>;
%template(subtractInPlace_float) SoundAlchemy::subtractInPlace<float>;
%template(absInPlace_float) SoundAlchemy::absInPlace<float>;
%template(squareInPlace_float) SoundAlchemy::squareInPlace<float>;
%template(squareRootInPlace_float) SoundAlchemy::squareRootInPlace<float>;
%template(sort_float) SoundAlchemy::sort<float>;
%template(reverse_float) SoundAlchemy::reverse<float>;
%template(multiply_float) SoundAlchemy::multiply<float>;
%template(divide_float) SoundAlchemy::divide<float>;
%template(add_float) SoundAlchemy::add<float>;
%template(subtract_float) SoundAlchemy::subtract<float>;
%template(abs_float) SoundAlchemy::abs<float>;
%template(square_float) SoundAlchemy::square<float>;
%template(squareRoot_float) SoundAlchemy::squareRoot<float>;
%template(scale_float) SoundAlchemy::scale<float>;
%template(difference_float) SoundAlchemy::difference<float>;
%template(zeros_float) SoundAlchemy::zeros<float>;
%template(ones_float) SoundAlchemy::ones<float>;
%template(range_float) SoundAlchemy::range<float>;
%template(dotProduct_float) SoundAlchemy::dotProduct<float>;
%template(euclideanDistance_float) SoundAlchemy::euclideanDistance<float>;
%template(cosineSimilarity_float) SoundAlchemy::cosineSimilarity<float>;
%template(cosineDistance_float) SoundAlchemy::cosineDistance<float>;
