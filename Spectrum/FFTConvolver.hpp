// ==================================================================================
// Copyright (c) 2017 HiFi-LoFi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ==================================================================================

#pragma once

#include "AudioFFT.hpp"

#include <vector>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <new>

#if defined (FFTCONVOLVER_USE_SSE)
  #include <xmmintrin.h>
#endif

namespace fftconvolver
{

#if defined(__SSE__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
  #if !defined(FFTCONVOLVER_USE_SSE) && !defined(FFTCONVOLVER_DONT_USE_SSE)
    #define FFTCONVOLVER_USE_SSE
  #endif
#endif


#if defined (FFTCONVOLVER_USE_SSE)
  #include <xmmintrin.h>
#endif


#if defined(__GNUC__)
  #define FFTCONVOLVER_RESTRICT __restrict__
#else
  #define FFTCONVOLVER_RESTRICT
#endif


/**
* @brief Returns whether SSE optimization for the convolver is enabled
* @return true: Enabled - false: Disabled
*/
bool SSEEnabled();


/**
* @class Buffer
* @brief Simple buffer implementation (uses 16-byte alignment if SSE optimization is enabled)
*/
template<typename T>
class Buffer
{
public:  
  explicit Buffer(size_t initialSize = 0) :
    _data(0),
    _size(0)
  {
    resize(initialSize);
  }

  virtual ~Buffer()
  {
    clear();
  }

  void clear()
  {
    deallocate(_data);
    _data = 0;
    _size = 0;
  }

  void resize(size_t size)
  {
    if (_size != size)
    {
      clear();

      if (size > 0)
      {
        assert(!_data && _size == 0);
        _data = allocate(size);
        _size = size;
      }
    }
    setZero();
  }

  size_t size() const
  {
    return _size;
  }

  void setZero()
  {
    ::memset(_data, 0, _size * sizeof(T));
  }

  void copyFrom(const Buffer<T>& other)
  {
    assert(_size == other._size);
    if (this != &other)
    {
      ::memcpy(_data, other._data, _size * sizeof(T));
    }
  }

  T& operator[](size_t index)
  {
    assert(_data && index < _size);
    return _data[index];
  }

  const T& operator[](size_t index) const
  {
    assert(_data && index < _size);
    return _data[index];
  }

  operator bool() const
  {
    return (_data != 0 && _size > 0);
  }

  T* data()
  {
    return _data;
  }

  const T* data() const
  {
    return _data;
  }

  static void Swap(Buffer<T>& a, Buffer<T>& b)
  {
    std::swap(a._data, b._data);
    std::swap(a._size, b._size);
  }

private:
  T* allocate(size_t size)
  {
#if defined(FFTCONVOLVER_USE_SSE)
    return static_cast<T*>(_mm_malloc(size * sizeof(T), 16));
#else
    return new T[size];
#endif
  }
  
  void deallocate(T* ptr)
  {
#if defined(FFTCONVOLVER_USE_SSE)
    _mm_free(ptr);
#else
    delete [] ptr;
#endif
  }

  T* _data;
  size_t _size;

  // Prevent uncontrolled usage
  Buffer(const Buffer&);
  Buffer& operator=(const Buffer&);
};


/**
* @brief Type of one sample
*/
typedef float Sample;


/**
* @brief Buffer for samples
*/
typedef Buffer<Sample> SampleBuffer;


/**
* @class SplitComplex
* @brief Buffer for split-complex representation of FFT results
*
* The split-complex representation stores the real and imaginary parts
* of FFT results in two different memory buffers which is useful e.g. for
* SIMD optimizations.
*/
class SplitComplex
{
public:
  explicit SplitComplex(size_t initialSize = 0) : 
    _size(0),
    _re(),
    _im()
  {
    resize(initialSize);
  }

  ~SplitComplex()
  {
    clear();
  }

  void clear()
  {
    _re.clear();
    _im.clear();
    _size = 0;
  }

  void resize(size_t newSize)
  {
    _re.resize(newSize);
    _im.resize(newSize);
    _size = newSize;
  }

  void setZero()
  {
    _re.setZero();
    _im.setZero();
  }

  void copyFrom(const SplitComplex& other)
  {
    _re.copyFrom(other._re);
    _im.copyFrom(other._im);
  }

  Sample* re()
  {
    return _re.data();
  }

  const Sample* re() const
  {
    return _re.data();
  }

  Sample* im()
  {
    return _im.data();
  }

  const Sample* im() const
  {
    return _im.data();
  }

  size_t size() const
  {
    return _size;
  }

private:
  size_t _size;
  SampleBuffer _re;
  SampleBuffer _im;

  // Prevent uncontrolled usage
  SplitComplex(const SplitComplex&);
  SplitComplex& operator=(const SplitComplex&);
};


/**
* @brief Returns the next power of 2 of a given number
* @param val The number
* @return The next power of 2
*/
template<typename T>
T NextPowerOf2(const T& val)
{
  T nextPowerOf2 = 1;
  while (nextPowerOf2 < val)
  {
    nextPowerOf2 *= 2;
  }
  return nextPowerOf2;
}
  

/**
* @brief Sums two given sample arrays
* @param result The result array
* @param a The 1st array
* @param b The 2nd array
* @param len The length of the arrays
*/
void Sum(Sample* FFTCONVOLVER_RESTRICT result,
         const Sample* FFTCONVOLVER_RESTRICT a,
         const Sample* FFTCONVOLVER_RESTRICT b,
         size_t len);


/**
* @brief Copies a source array into a destination buffer and pads the destination buffer with zeros
* @param dest The destination buffer
* @param src The source array
* @param srcSize The size of the source array
*/
template<typename T>
void CopyAndPad(Buffer<T>& dest, const T* src, size_t srcSize)
{
  assert(dest.size() >= srcSize);
  ::memcpy(dest.data(), src, srcSize * sizeof(T));
  ::memset(dest.data() + srcSize, 0, (dest.size()-srcSize) * sizeof(T)); 
}


/**
* @brief Adds the complex product of two split-complex buffers to a result buffer
* @param result The result buffer
* @param a The 1st factor of the complex product
* @param b The 2nd factor of the complex product
*/
void ComplexMultiplyAccumulate(SplitComplex& result, const SplitComplex& a, const SplitComplex& b);


/**
* @brief Adds the complex product of two split-complex arrays to a result array
* @param re The real part of the result buffer
* @param im The imaginary part of the result buffer
* @param reA The real part of the 1st factor of the complex product
* @param imA The imaginary part of the 1st factor of the complex product
* @param reB The real part of the 2nd factor of the complex product
* @param imB The imaginary part of the 2nd factor of the complex product
*/
void ComplexMultiplyAccumulate(Sample* FFTCONVOLVER_RESTRICT re, 
                               Sample* FFTCONVOLVER_RESTRICT im,
                               const Sample* FFTCONVOLVER_RESTRICT reA,
                               const Sample* FFTCONVOLVER_RESTRICT imA,
                               const Sample* FFTCONVOLVER_RESTRICT reB,
                               const Sample* FFTCONVOLVER_RESTRICT imB,
                               const size_t len);
  
} // End of namespace fftconvolver


namespace fftconvolver
{ 

/**
* @class FFTConvolver
* @brief Implementation of a partitioned FFT convolution algorithm with uniform block size
*
* Some notes on how to use it:
*
* - After initialization with an impulse response, subsequent data portions of
*   arbitrary length can be convolved. The convolver internally can handle
*   this by using appropriate buffering.
*
* - The convolver works without "latency" (except for the required
*   processing time, of course), i.e. the output always is the convolved
*   input for each processing call.
*
* - The convolver is suitable for real-time processing which means that no
*   "unpredictable" operations like allocations, locking, API calls, etc. are
*   performed during processing (all necessary allocations and preparations take
*   place during initialization).
*/
class FFTConvolver
{  
public:
  FFTConvolver();  
  virtual ~FFTConvolver();
  
  /**
  * @brief Initializes the convolver
  * @param blockSize Block size internally used by the convolver (partition size)
  * @param ir The impulse response
  * @param irLen Length of the impulse response
  * @return true: Success - false: Failed
  */
  bool init(size_t blockSize, const Sample* ir, size_t irLen);

  /**
  * @brief Convolves the the given input samples and immediately outputs the result
  * @param input The input samples
  * @param output The convolution result
  * @param len Number of input/output samples
  */
  void process(const Sample* input, Sample* output, size_t len);

  /**
  * @brief Resets the convolver and discards the set impulse response
  */
  void reset();
  
private:
  size_t _blockSize;
  size_t _segSize;
  size_t _segCount;
  size_t _fftComplexSize;
  std::vector<SplitComplex*> _segments;
  std::vector<SplitComplex*> _segmentsIR;
  SampleBuffer _fftBuffer;
  audiofft::AudioFFT _fft;
  SplitComplex _preMultiplied;
  SplitComplex _conv;
  SampleBuffer _overlap;
  size_t _current;
  SampleBuffer _inputBuffer;
  size_t _inputBufferFill;

  // Prevent uncontrolled usage
  FFTConvolver(const FFTConvolver&);
  FFTConvolver& operator=(const FFTConvolver&);
};
  
} // End of namespace fftconvolver

namespace fftconvolver
{

bool SSEEnabled()
{
#if defined(FFTCONVOLVER_USE_SSE)
  return true;
#else
  return false;
#endif
}


void Sum(Sample* FFTCONVOLVER_RESTRICT result,
         const Sample* FFTCONVOLVER_RESTRICT a,
         const Sample* FFTCONVOLVER_RESTRICT b,
         size_t len)
{
  const size_t end4 = 4 * (len / 4);
  for (size_t i=0; i<end4; i+=4)
  {
    result[i+0] = a[i+0] + b[i+0];
    result[i+1] = a[i+1] + b[i+1];
    result[i+2] = a[i+2] + b[i+2];
    result[i+3] = a[i+3] + b[i+3];
  }
  for (size_t i=end4; i<len; ++i)
  {
    result[i] = a[i] + b[i];
  }
}


void ComplexMultiplyAccumulate(SplitComplex& result, const SplitComplex& a, const SplitComplex& b)
{
  assert(result.size() == a.size());
  assert(result.size() == b.size());
  ComplexMultiplyAccumulate(result.re(), result.im(), a.re(), a.im(), b.re(), b.im(), result.size());
}


void ComplexMultiplyAccumulate(Sample* FFTCONVOLVER_RESTRICT re, 
                               Sample* FFTCONVOLVER_RESTRICT im,
                               const Sample* FFTCONVOLVER_RESTRICT reA,
                               const Sample* FFTCONVOLVER_RESTRICT imA,
                               const Sample* FFTCONVOLVER_RESTRICT reB,
                               const Sample* FFTCONVOLVER_RESTRICT imB,
                               const size_t len)
{
#if defined(FFTCONVOLVER_USE_SSE)
  const size_t end4 = 4 * (len / 4);
  for (size_t i=0; i<end4; i+=4)
  {
    const __m128 ra = _mm_load_ps(&reA[i]);
    const __m128 rb = _mm_load_ps(&reB[i]);
    const __m128 ia = _mm_load_ps(&imA[i]);
    const __m128 ib = _mm_load_ps(&imB[i]);
    __m128 real = _mm_load_ps(&re[i]);
    __m128 imag = _mm_load_ps(&im[i]);
    real = _mm_add_ps(real, _mm_mul_ps(ra, rb));
    real = _mm_sub_ps(real, _mm_mul_ps(ia, ib));
    _mm_store_ps(&re[i], real);
    imag = _mm_add_ps(imag, _mm_mul_ps(ra, ib));
    imag = _mm_add_ps(imag, _mm_mul_ps(ia, rb));
    _mm_store_ps(&im[i], imag);
  }
  for (size_t i=end4; i<len; ++i)
  {
    re[i] += reA[i] * reB[i] - imA[i] * imB[i];
    im[i] += reA[i] * imB[i] + imA[i] * reB[i];
  }
#else
  const size_t end4 = 4 * (len / 4);
  for (size_t i=0; i<end4; i+=4)
  {
    re[i+0] += reA[i+0] * reB[i+0] - imA[i+0] * imB[i+0];
    re[i+1] += reA[i+1] * reB[i+1] - imA[i+1] * imB[i+1];
    re[i+2] += reA[i+2] * reB[i+2] - imA[i+2] * imB[i+2];
    re[i+3] += reA[i+3] * reB[i+3] - imA[i+3] * imB[i+3];
    im[i+0] += reA[i+0] * imB[i+0] + imA[i+0] * reB[i+0];
    im[i+1] += reA[i+1] * imB[i+1] + imA[i+1] * reB[i+1];
    im[i+2] += reA[i+2] * imB[i+2] + imA[i+2] * reB[i+2];
    im[i+3] += reA[i+3] * imB[i+3] + imA[i+3] * reB[i+3];
  }
  for (size_t i=end4; i<len; ++i)
  {
    re[i] += reA[i] * reB[i] - imA[i] * imB[i];
    im[i] += reA[i] * imB[i] + imA[i] * reB[i];
  }
#endif
}

} // End of namespace fftconvolver







namespace fftconvolver
{  

FFTConvolver::FFTConvolver() :
  _blockSize(0),
  _segSize(0),
  _segCount(0),
  _fftComplexSize(0),
  _segments(),
  _segmentsIR(),
  _fftBuffer(),
  _fft(),
  _preMultiplied(),
  _conv(),
  _overlap(),
  _current(0),
  _inputBuffer(),
  _inputBufferFill(0)
{
}

  
FFTConvolver::~FFTConvolver()
{
  reset();
}

  
void FFTConvolver::reset()
{  
  for (size_t i=0; i<_segCount; ++i)
  {
    delete _segments[i];
    delete _segmentsIR[i];
  }
  
  _blockSize = 0;
  _segSize = 0;
  _segCount = 0;
  _fftComplexSize = 0;
  _segments.clear();
  _segmentsIR.clear();
  _fftBuffer.clear();
  _fft.init(0);
  _preMultiplied.clear();
  _conv.clear();
  _overlap.clear();
  _current = 0;
  _inputBuffer.clear();
  _inputBufferFill = 0;
}

  
bool FFTConvolver::init(size_t blockSize, const Sample* ir, size_t irLen)
{
  reset();

  if (blockSize == 0)
  {
    return false;
  }
  
  // Ignore zeros at the end of the impulse response because they only waste computation time
  while (irLen > 0 && ::fabs(ir[irLen-1]) < 0.000001f)
  {
    --irLen;
  }

  if (irLen == 0)
  {
    return true;
  }
  
  _blockSize = NextPowerOf2(blockSize);
  _segSize = 2 * _blockSize;
  _segCount = static_cast<size_t>(::ceil(static_cast<float>(irLen) / static_cast<float>(_blockSize)));
  _fftComplexSize = audiofft::AudioFFT::ComplexSize(_segSize);
  
  // FFT
  _fft.init(_segSize);
  _fftBuffer.resize(_segSize);
  
  // Prepare segments
  for (size_t i=0; i<_segCount; ++i)
  {
    _segments.push_back(new SplitComplex(_fftComplexSize));    
  }
  
  // Prepare IR
  for (size_t i=0; i<_segCount; ++i)
  {
    SplitComplex* segment = new SplitComplex(_fftComplexSize);
    const size_t remaining = irLen - (i * _blockSize);
    const size_t sizeCopy = (remaining >= _blockSize) ? _blockSize : remaining;
    CopyAndPad(_fftBuffer, &ir[i*_blockSize], sizeCopy);
    _fft.fft(_fftBuffer.data(), segment->re(), segment->im());
    _segmentsIR.push_back(segment);
  }
  
  // Prepare convolution buffers  
  _preMultiplied.resize(_fftComplexSize);
  _conv.resize(_fftComplexSize);
  _overlap.resize(_blockSize);
  
  // Prepare input buffer
  _inputBuffer.resize(_blockSize);
  _inputBufferFill = 0;

  // Reset current position
  _current = 0;
  
  return true;
}


void FFTConvolver::process(const Sample* input, Sample* output, size_t len)
{
  if (_segCount == 0)
  {
    ::memset(output, 0, len * sizeof(Sample));
    return;
  }

  size_t processed = 0;
  while (processed < len)
  {
    const bool inputBufferWasEmpty = (_inputBufferFill == 0);
    const size_t processing = std::min(len-processed, _blockSize-_inputBufferFill);
    const size_t inputBufferPos = _inputBufferFill;
    ::memcpy(_inputBuffer.data()+inputBufferPos, input+processed, processing * sizeof(Sample));

    // Forward FFT
    CopyAndPad(_fftBuffer, &_inputBuffer[0], _blockSize); 
    _fft.fft(_fftBuffer.data(), _segments[_current]->re(), _segments[_current]->im());

    // Complex multiplication
    if (inputBufferWasEmpty)
    {
      _preMultiplied.setZero();
      for (size_t i=1; i<_segCount; ++i)
      {
        const size_t indexIr = i;
        const size_t indexAudio = (_current + i) % _segCount;
        ComplexMultiplyAccumulate(_preMultiplied, *_segmentsIR[indexIr], *_segments[indexAudio]);
      }
    }
    _conv.copyFrom(_preMultiplied);
    ComplexMultiplyAccumulate(_conv, *_segments[_current], *_segmentsIR[0]);

    // Backward FFT
    _fft.ifft(_fftBuffer.data(), _conv.re(), _conv.im());

    // Add overlap
    Sum(output+processed, _fftBuffer.data()+inputBufferPos, _overlap.data()+inputBufferPos, processing);

    // Input buffer full => Next block
    _inputBufferFill += processing;
    if (_inputBufferFill == _blockSize)
    {
      // Input buffer is empty again now
      _inputBuffer.setZero();
      _inputBufferFill = 0;

      // Save the overlap
      ::memcpy(_overlap.data(), _fftBuffer.data()+_blockSize, _blockSize * sizeof(Sample));

      // Update current segment
      _current = (_current > 0) ? (_current - 1) : (_segCount - 1);
    }

    processed += processing;
  }
}
  
} // End of namespace fftconvolver
