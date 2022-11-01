#pragma once

#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__) || defined(__WINDOWS_MM__)
  #define __OS_WINDOWS__
  #define __STK_REALTIME__
#elif defined(__LINUX_OSS__) || defined(__LINUX_ALSA__) || defined(__UNIX_JACK__) || defined(__LINUX_PULSE__)
  #define __OS_LINUX__
  #define __STK_REALTIME__
#elif defined(__IRIX_AL__)
  #define __OS_IRIX__
#elif defined(__MACOSX_CORE__) || defined(__UNIX_JACK__)
  #define __OS_MACOSX__
  #define __STK_REALTIME__
#endif

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
  #include <unistd.h>
#elif defined(__OS_WINDOWS__)
  #include <windows.h>
#endif


#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
//#include <cstdlib>


/*! \namespace stk
    \brief The STK namespace.
    Most Stk classes are defined within the STK namespace.  Exceptions
    to this include the classes RtAudio and RtMidi.
*/
namespace stk {

/***************************************************/
/*! \class Stk
    \brief STK base class
    Nearly all STK classes inherit from this class.
    The global sample rate and rawwave path variables
    can be queried and modified via Stk.  In addition,
    this class provides error handling and
    byte-swapping functions.
    The Synthesis ToolKit in C++ (STK) is a set of open source audio
    signal processing and algorithmic synthesis classes written in the
    C++ programming language. STK was designed to facilitate rapid
    development of music synthesis and audio processing software, with
    an emphasis on cross-platform functionality, realtime control,
    ease of use, and educational example code.  STK currently runs
    with realtime support (audio and MIDI) on Linux, Macintosh OS X,
    and Windows computer platforms. Generic, non-realtime support has
    been tested under NeXTStep, Sun, and other platforms and should
    work with any standard C++ compiler.
    STK WWW site: http://ccrma.stanford.edu/software/stk/
    The Synthesis ToolKit in C++ (STK)
    Copyright (c) 1995--2021 Perry R. Cook and Gary P. Scavone
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:
    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.
    Any person wishing to distribute modifications to the Software is
    asked to send the modifications to the original developer so that
    they can be incorporated into the canonical version.  This is,
    however, not a binding provision of this license.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
/***************************************************/

//#define _STK_DEBUG_

//! STK error handling class.
/*!
  This is a fairly abstract exception handling class.  There could
  be sub-classes to take care of more specific error conditions ... or
  not.
*/
class StkError : public std::exception
{
public:
  enum Type {
    STATUS,
    WARNING,
    DEBUG_PRINT,
    MEMORY_ALLOCATION,
    MEMORY_ACCESS,
    FUNCTION_ARGUMENT,
    FILE_NOT_FOUND,
    FILE_UNKNOWN_FORMAT,
    FILE_ERROR,
    PROCESS_THREAD,
    PROCESS_SOCKET,
    PROCESS_SOCKET_IPADDR,
    AUDIO_SYSTEM,
    MIDI_SYSTEM,
    UNSPECIFIED
  };

protected:
  std::string message_;
  Type type_;

public:
  //! The constructor.
  StkError(const std::string& message, Type type = StkError::UNSPECIFIED)
    : std::exception(), message_(message), type_(type) {}

  //! The destructor.
  virtual ~StkError(void) throw() {};

  //! Prints thrown error message to stderr.
  virtual void printMessage(void) { std::cerr << '\n' << message_ << "\n\n"; }

  //! Returns the thrown error message type.
  virtual const Type& getType(void) { return type_; }

  //! Returns the thrown error message string.
  virtual const std::string& getMessage(void) { return message_; }

  //! Returns the thrown error message as a C string.
  virtual const char *getMessageCString(void) { return message_.c_str(); }

  virtual const char *what(void) const throw() { return message_.c_str(); }
};



enum StkFormat {
    STK_NONE=0,
    STK_SINT8=0x1,
    STK_SINT16=0x2,
    STK_SINT24=0x4,
    STK_SINT32=0x8,
    STK_FLOAT32=0x10,
    STK_FLOAT64=0x20,
  };


extern std::ostringstream oStream_;

// these really dont need to be in the class anymore and the tempalte makes it awkward
double sampleRate( void );
void SetSampleRate( double rate );
std::string rawwavePath(void);
void setRawwavePath( std::string path );
void swap16( unsigned char *ptr );
void swap32( unsigned char *ptr );
void swap64( unsigned char *ptr );
void sleep( unsigned long milliseconds );
void handleError( StkError::Type type );
void handleError( const char *message, StkError::Type type );
void handleError( std::string message, StkError::Type type );
void showWarnings( bool status );
void printErrors( bool status );


//! Static method to check whether a value is within a specified range.
template<typename T>
bool inRange( T value, T min, T max ) {
  if ( value < min ) return false;
  else if ( value > max ) return false;
  else return true;
}



// I've removing all the static members soon it's not really sense with templates at all.
// Then you won't have multiple sample rates


class Stk
{
public:

  
 protected: 
//! Default constructor.
  Stk( void ): ignoreSampleRateChange_(false)
  {

  }

  virtual ~Stk( void )
  {

  }

protected:

  static std::vector<Stk*> alertList_;
  bool   ignoreSampleRateChange_;

  void setSampleRate( double rate )
  {
    double oldRate = stk::sampleRate();
    if ( rate > 0.0 && rate != oldRate ) {
      
      stk::SetSampleRate(rate);

      for ( unsigned int i=0; i<alertList_.size(); i++ )
        alertList_[i]->sampleRateChanged( rate, oldRate );
    }
  }


  void ignoreSampleRateChange( bool ignore = true ) { ignoreSampleRateChange_ = ignore; };
    
  //! Static method that frees memory from alertList_.
  void  clear_alertList()
  {
      std::vector<Stk*>().swap(alertList_);
  }

  //! This function should be implemented in subclasses that depend on the sample rate.
  virtual void sampleRateChanged( double newRate, double oldRate ) {

  }

  //! Add class pointer to list for sample rate change notification.
  void addSampleRateAlert( Stk *ptr )
  {
    for ( unsigned int i=0; i<alertList_.size(); i++ )
      if ( alertList_[i] == ptr ) return;

    alertList_.push_back( ptr );
  }


  //! Remove class pointer from list for sample rate change notification.
  void removeSampleRateAlert( Stk *ptr )
  {
    for ( unsigned int i=0; i<alertList_.size(); i++ ) {
      if ( alertList_[i] == ptr ) {
        alertList_.erase( alertList_.begin() + i );
        return;
      }
    }
  }
};

// The default sampling rate.
const double SRATE = 44100.0;

// The default rawwave path value is set with the preprocessor
// definition RAWWAVE_PATH.  This can be specified as an argument to
// the configure script, in an integrated development environment, or
// below.  The global STK rawwave path variable can be dynamically set
// with the Stk<T>::setRawwavePath() function.  This value is
// concatenated to the beginning of all references to rawwave files in
// the various STK core classes (e.g. Clarinet.cpp).  If you wish to
// move the rawwaves directory to a different location in your file
// system, you will need to set this path definition appropriately.
#if !defined(RAWWAVE_PATH)
  #define RAWWAVE_PATH "../../rawwaves/"
#endif





/***************************************************/
/*! \class StkFrames
    \brief An STK class to handle vectorized audio data.
    This class can hold single- or multi-channel audio data.  The data
    type is always T and the channel format is always
    interleaved.  In an effort to maintain efficiency, no
    out-of-bounds checks are performed in this class unless
    _STK_DEBUG_ is defined.
    Internally, the data is stored in a one-dimensional C array.  An
    indexing operator is available to set and retrieve data values.
    Alternately, one can use pointers to access the data, using the
    index operator to get an address for a particular location in the
    data:
      T* ptr = &myStkFrames[0];
    Note that this class can also be used as a table with interpolating
    lookup.
    Possible future improvements in this class could include functions
    to convert to and return other data types.
    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
class StkFrames
{
public:

  //! The default constructor initializes the frame data structure to size zero.
  StkFrames( unsigned int nFrames = 0, unsigned int nChannels = 0 )
    : data_( 0 ), nFrames_( nFrames ), nChannels_( nChannels )
    {
        size_ = nFrames_ * nChannels_;
        bufferSize_ = size_;

        if ( size_ > 0 ) {
          data_ = (T *) calloc( size_, sizeof( T ) );
          #if defined(_STK_DEBUG_)
              if ( data_ == NULL ) {
                std::string error = "StkFrames: memory allocation error in constructor!";
                Stk<T>::handleError( error, StkError::MEMORY_ALLOCATION );
              }
          #endif
        }

  dataRate_ = stk::sampleRate();
}


  //! Overloaded constructor that initializes the frame data to the specified size with \c value.
  StkFrames( const T& value, unsigned int nFrames, unsigned int nChannels )
  : data_( 0 ), nFrames_( nFrames ), nChannels_( nChannels )
  {
    size_ = nFrames_ * nChannels_;
    bufferSize_ = size_;
    if ( size_ > 0 ) {
      data_ = (T *) malloc( size_ * sizeof( T ) );
      #if defined(_STK_DEBUG_)
          if ( data_ == NULL ) {
            std::string error = "StkFrames: memory allocation error in constructor!";
            Stk<T>::handleError( error, StkError::MEMORY_ALLOCATION );
          }
      #endif
      for ( long i=0; i<(long)size_; i++ ) data_[i] = value;
    }

    dataRate_ = sampleRate();
  }


  //! The destructor.
  virtual ~StkFrames()
  {
    if ( data_ ) free( data_ );
  }

  // A copy constructor.
  StkFrames( const StkFrames<T>& f )
  : data_(0), size_(0), bufferSize_(0)
  {
    resize( f.frames(), f.channels() );
    dataRate_ = sampleRate();
    for ( unsigned int i=0; i<size_; i++ ) data_[i] = f[i];
  }


  // Assignment operator that returns a reference to self.
  virtual StkFrames<T>& operator= ( const StkFrames<T>& f )
  {
    if ( data_ ) free( data_ );
    data_ = 0;
    size_ = 0;
    bufferSize_ = 0;
    resize( f.frames(), f.channels() );
    dataRate_ = sampleRate();
    for ( unsigned int i=0; i<size_; i++ ) data_[i] = f[i];
    return *this;
  }

  //! Subscript operator that returns a reference to element \c n of self.
  /*!
    The result can be used as an lvalue. This reference is valid
    until the resize function is called or the array is destroyed. The
    index \c n must be between 0 and size less one.  No range checking
    is performed unless _STK_DEBUG_ is defined.
  */
  T& operator[] ( size_t n );

  //! Subscript operator that returns the value at element \c n of self.
  /*!
    The index \c n must be between 0 and size less one.  No range
    checking is performed unless _STK_DEBUG_ is defined.
  */
  T operator[] ( size_t n ) const;
    
  //! Sum operator
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  StkFrames<T> operator+(const StkFrames<T>&frames) const;

  //! Assignment by sum operator into self.
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  StkFrames<T>& operator+= ( StkFrames<T>& f );

  //! Assignment by product operator into self.
  /*!
    The dimensions of the argument are expected to be the same as
    self.  No range checking is performed unless _STK_DEBUG_ is
    defined.
  */
  StkFrames<T>& operator*= ( StkFrames<T>& f );

  //! Scaling operator (StkFrames<T>&* T).
  StkFrames<T> operator* ( T v ) const;

  
  //! Scaling operator (inline).
  StkFrames<T>& operator*= ( T v );

  //! Channel / frame subscript operator that returns a reference.
  /*!
    The result can be used as an lvalue. This reference is valid
    until the resize function is called or the array is destroyed. The
    \c frame index must be between 0 and frames() - 1.  The \c channel
    index must be between 0 and channels() - 1.  No range checking is
    performed unless _STK_DEBUG_ is defined.
  */
  T& operator() ( size_t frame, unsigned int channel );

  //! Channel / frame subscript operator that returns a value.
  /*!
    The \c frame index must be between 0 and frames() - 1.  The \c
    channel index must be between 0 and channels() - 1.  No range checking
    is performed unless _STK_DEBUG_ is defined.
  */
  T operator() ( size_t frame, unsigned int channel ) const;

  //! Return an interpolated value at the fractional frame index and channel.
  /*!
    This function performs linear interpolation.  The \c frame
    index must be between 0.0 and frames() - 1.  The \c channel index
    must be between 0 and channels() - 1.  No range checking is
    performed unless _STK_DEBUG_ is defined.
  */
  T interpolate( T frame, unsigned int channel = 0 ) const
  {
  #if defined(_STK_DEBUG_)
    if ( frame < 0.0 || frame > (T) ( nFrames_ - 1 ) || channel >= nChannels_ ) {
      std::ostringstream error;
      error << "StkFrames::interpolate: invalid frame (" << frame << ") or channel (" << channel << ") value!";
      Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
    }
  #endif

    size_t iIndex = ( size_t ) frame;                    // integer part of index
    T output, alpha = frame - (T) iIndex;  // fractional part of index

    iIndex = iIndex * nChannels_ + channel;
    output = data_[ iIndex ];
    if ( alpha > 0.0 )
      output += ( alpha * ( data_[ iIndex + nChannels_ ] - output ) );

    return output;
  }

  void fill(size_t c, T v) {
      for(size_t i = c; i < frames()*channels(); i+=channels()) {
          data_[i] = v;
      }
  }

  //! Returns the total number of audio samples represented by the object.
  size_t size() const { return size_; }; 

  //! Returns \e true if the object size is zero and \e false otherwise.
  bool empty() const;

  //! Resize self to represent the specified number of channels and frames.
  /*!
    Changes the size of self based on the number of frames and
    channels.  No element assignment is performed.  No memory
    deallocation occurs if the new size is smaller than the previous
    size.  Further, no new memory is allocated when the new size is
    smaller or equal to a previously allocated size.
  */
  virtual void resize( size_t nFrames, unsigned int nChannels = 1 )
  {
    nFrames_ = nFrames;
    nChannels_ = nChannels;

    size_ = nFrames_ * nChannels_;
    if ( size_ > bufferSize_ ) {
      if ( data_ ) free( data_ );
      data_ = (T *) malloc( size_ * sizeof( T ) );
      #if defined(_STK_DEBUG_)
        if ( data_ == NULL ) {
          std::string error = "StkFrames::resize: memory allocation error!";
          Stk<T>::handleError( error, StkError::MEMORY_ALLOCATION );
        }
      #endif
      bufferSize_ = size_;
    }
  }

  //! Resize self to represent the specified number of channels and frames and perform element initialization.
  /*!
    Changes the size of self based on the number of frames and
    channels, and assigns \c value to every element.  No memory
    deallocation occurs if the new size is smaller than the previous
    size.  Further, no new memory is allocated when the new size is
    smaller or equal to a previously allocated size.
  */
  virtual void resize( size_t nFrames, unsigned int nChannels, T value )
  {
    this->resize( nFrames, nChannels );

    for ( size_t i=0; i<size_; i++ ) data_[i] = value;
  }

  //! Retrieves a single channel
  /*!
    Copies the specified \c channel into \c destinationFrames's \c destinationChannel. \c destinationChannel must be between 0 and destination.channels() - 1 and
    \c channel must be between 0 and channels() - 1. destination.frames() must be >= frames().
    No range checking is performed unless _STK_DEBUG_ is defined.
  */
  StkFrames<T>& getChannel(unsigned int sourceChannel,StkFrames<T>& destinationFrames, unsigned int destinationChannel) const
  {
#if defined(_STK_DEBUG_)
  if (sourceChannel > channels() - 1) {
    std::ostringstream error;
    error << "StkFrames::getChannel invalid sourceChannel (" << sourceChannel << ")";
    Stk<T>::handleError( error.str(), StkError::FUNCTION_ARGUMENT);
  }
  if (destinationChannel > destinationFrames.channels() - 1) {
    std::ostringstream error;
    error << "StkFrames::getChannel invalid destinationChannel (" << destinationChannel << ")";
    Stk<T>::handleError( error.str(), StkError::FUNCTION_ARGUMENT );
  }
  if (destinationFrames.frames() < frames()) {
    std::ostringstream error;
    error << "StkFrames::getChannel destination.frames() < frames()";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS);
  }
#endif
  int sourceHop = nChannels_;
  int destinationHop = destinationFrames.nChannels_;
  for (unsigned int i  = sourceChannel, j= destinationChannel;
       i < nFrames_ * nChannels_;
       i+=sourceHop,j+=destinationHop)
  {
    destinationFrames[j] = data_[i];
  }
  return destinationFrames;        
}



  //! Sets a single channel
  /*!
    Copies the \c sourceChannel of \c sourceFrames into the \c channel of self.
    SourceFrames.frames() must be equal to frames().
    No range checking is performed unless _STK_DEBUG_ is defined.
  */
  void setChannel(unsigned int destinationChannel, const StkFrames<T> &sourceFrames,unsigned int sourceChannel)
  {
  #if defined(_STK_DEBUG_)
    if (sourceChannel > sourceFrames.channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::setChannel invalid sourceChannel (" << sourceChannel << ")";
      Stk<T>::handleError( error.str(), StkError::FUNCTION_ARGUMENT);
    }
    if (destinationChannel > channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::setChannel invalid channel (" << destinationChannel << ")";
      Stk<T>::handleError( error.str(), StkError::FUNCTION_ARGUMENT );
    }
    if (sourceFrames.frames() != frames()) {
      std::ostringstream error;
      error << "StkFrames::setChannel sourceFrames.frames() != frames()";
      Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS);
    }
  #endif
    unsigned int sourceHop = sourceFrames.nChannels_;
    unsigned int destinationHop = nChannels_;
    for (unsigned int i  = destinationChannel,j = sourceChannel;
        i < nFrames_ * nChannels_;
        i+=destinationHop,j+=sourceHop)
    {
      data_[i] = sourceFrames[j];
    }
  }


  //! Return the number of channels represented by the data.
  unsigned int channels( void ) const { return nChannels_; };

  //! Return the number of sample frames represented by the data.
  unsigned int frames( void ) const { return (unsigned int)nFrames_; };

  //! Set the sample rate associated with the StkFrames data.
  /*!
    By default, this value is set equal to the current STK sample
    rate at the time of instantiation.
   */
  void setDataRate( T rate ) { dataRate_ = rate; };

  //! Return the sample rate associated with the StkFrames data.
  /*!
    By default, this value is set equal to the current STK sample
    rate at the time of instantiation.
   */
  T dataRate( void ) const { return dataRate_; };

  T * GetData() { return data_; }
  
  void SetData(size_t n, T * p) {
      assert(n <= frames()*channels);
      memcpy(data_,p,n* sizeof(float));
  }
  void SetData(T * p, int channel) {
      StkFrames<T> c(frames());
      memcpy(c.GetData(),p,frames()*sizeof(T));
      setChannel(channel,c,0);
  }
  void SetVectorChannel(std::vector<T> in, int ch = 0) {
      for(size_t i = 0; i < frames(); i++)
        data_[i*channels()+ch] = in[i];
  }
  std::vector<T> GetVectorChannel(int ch = 0) {      
      std::vector<T> r(frames());
      for(size_t i = 0; i < frames(); i++)
        r[i] = data_[i*channels()+ch];      
      return r;
  }
  std::vector<T> GetVector() {          
      std::vector<T> r(frames());
      memcpy(r.data(),GetData(),frames()*channels()*sizeof(T));
      return r;
  }
  StkFrames<T>& operator = (const std::vector<T> & x) {
      memcpy(data_,x.data(),x.size()*sizeof(T));
      return *this;
  }
    void print() {
        std::cout << "Frames[" << channels() << "][" << frames() << "]\n";            
        for(size_t i = 0; i < channels(); i++)
        {
            std::cout << "Channel[" << i << "]=[";
            for(size_t j = i; j < frames()*channels(); j += channels()) {
                std::cout << data_[j] << ",";
            }
            std::cout << "]\n";
        }
    }

    // friends just don't work with templates (can't do this, needs to be closed, blah blah blah)
    StkFrames<T> operator *(T v) {
      StkFrames<T> res((unsigned int)nFrames_, nChannels_);
      T *resPtr = &res[0];
      T *dPtr = data_;
      for (unsigned int i = 0; i < size_; i++) {
        *resPtr++ = v * *dPtr++;
      }
      return res;
    }
          
protected:

  T *data_;
  T dataRate_;
  size_t nFrames_;
  unsigned int nChannels_;
  size_t size_;
  size_t bufferSize_;

  
};

template<typename T>
inline bool StkFrames<T>:: empty() const
{
  if ( size_ > 0 ) return false;
  else return true;
}

template<typename T>
inline T& StkFrames<T>:: operator[] ( size_t n )
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

template<typename T>
inline T StkFrames<T>:: operator[] ( size_t n ) const
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

template<typename T>
inline T& StkFrames<T>:: operator() ( size_t frame, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}

template<typename T>
inline T StkFrames<T>:: operator() ( size_t frame, unsigned int channel ) const
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}
    
template<typename T>
inline StkFrames<T> StkFrames<T>::operator+(const StkFrames<T> &f) const
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+: frames argument must be of equal dimensions!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif
  StkFrames<T> sum((unsigned int)nFrames_,nChannels_);
  T *sumPtr = &sum[0];
  const T *fptr = f.data_;
  const T *dPtr = data_;
  for (unsigned int i = 0; i < size_; i++) {
    *sumPtr++ = *fptr++ + *dPtr++;
  }
  return sum;
}

template<typename T>
inline StkFrames<T>& StkFrames<T>:: operator+= ( StkFrames<T>& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+=: frames argument must be of equal dimensions!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  T *fptr = &f[0];
  T *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ += *fptr++;
  return *this;
}

template<typename T>
inline StkFrames<T>& StkFrames<T>:: operator*= ( StkFrames<T>& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator*=: frames argument must be of equal dimensions!";
    Stk<T>::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  T *fptr = &f[0];
  T *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ *= *fptr++;
  return *this;
}

template<typename T>
inline StkFrames<T> StkFrames<T>::operator*(T v) const
{
  StkFrames res((unsigned int)nFrames_, nChannels_);
  T *resPtr = &res[0];
  const T *dPtr = data_;
  for (unsigned int i = 0; i < size_; i++) {
    *resPtr++ = v * *dPtr++;
  }
  return res;
}

template<typename T>
StkFrames<T> operator*(T v, const StkFrames<T>& f)
{
  /*
  StkFrames<T> res((unsigned int)f.nFrames_, f.nChannels_);
  T *resPtr = &res[0];
  T *dPtr = f.data_;
  for (unsigned int i = 0; i < f.size_; i++) {
    *resPtr++ = v * *dPtr++;
  }
  return res;
  */
  return v * f;
}

template<typename T>
inline StkFrames<T>& StkFrames<T>:: operator*= ( T v )
{
  T *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ *= v;
  return *this;
}


// Here are a few other useful typedefs.
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef signed short SINT16;
typedef signed int SINT32;
typedef float FLOAT32;
typedef double FLOAT64;


// The default real-time audio input and output buffer size.  If
// clicks are occuring in the input and/or output sound stream, a
// larger buffer size may help.  Larger buffer sizes, however, produce
// more latency.
const unsigned int RT_BUFFER_SIZE = 512;


const double PI           = 3.14159265358979;
const double TWO_PI       = 2 * PI;
const double ONE_OVER_128 = 0.0078125;


} // stk namespace
