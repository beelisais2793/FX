#pragma once

#ifndef STK_STK_H
#define STK_STK_H

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
//#include <cstdlib>

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
  #include <unistd.h>
#elif defined(__OS_WINDOWS__)
  #include <windows.h>
#endif

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


template<class T>
class Stk
{
public:

  enum StkFormat {
    STK_SINT8=0x1,
    STK_SINT16=0x2,
    STK_SINT24=0x4,
    STK_SINT32=0x8,
    STK_FLOAT32=0x10,
    STK_FLOAT64=0x20,
  };
  

  //! Static method that returns the current STK sample rate.
  static T sampleRate( void ) { return srate_; }

  //! Static method that sets the STK sample rate.
  /*!
    The sample rate set using this method is queried by all STK
    classes that depend on its value.  It is initialized to the
    default SRATE set in Stk.h.  Many STK classes use the sample rate
    during instantiation.  Therefore, if you wish to use a rate that
    is different from the default rate, it is imperative that it be
    set \e BEFORE STK objects are instantiated.  A few classes that
    make use of the global STK sample rate are automatically notified
    when the rate changes so that internal class data can be
    appropriately updated.  However, this has not been fully
    implemented.  Specifically, classes that appropriately update
    their own data when either a setFrequency() or noteOn() function
    is called do not currently receive the automatic notification of
    rate change.  If the user wants a specific class instance to
    ignore such notifications, perhaps in a multi-rate context, the
    function Stk::ignoreSampleRateChange() should be called.
  */
  static void setSampleRate( T rate )
  {
    if ( rate > 0.0 && rate != srate_ ) {
      T oldRate = srate_;
      srate_ = rate;

      for ( unsigned int i=0; i<alertList_.size(); i++ )
        alertList_[i]->sampleRateChanged( srate_, oldRate );
    }
  }

  //! A function to enable/disable the automatic updating of class data when the STK sample rate changes.
  /*!
    This function allows the user to enable or disable class data
    updates in response to global sample rate changes on a class by
    class basis.
  */
  static void ignoreSampleRateChange( bool ignore = true ) { ignoreSampleRateChange_ = ignore; };
  
  //! Static method that frees memory from alertList_.
  static void  clear_alertList(){std::vector<Stk *>().swap(alertList_);}
  
  //! Static method that returns the current rawwave path.
  static std::string rawwavePath(void) { return rawwavepath_; }

  //! Static method that sets the STK rawwave path.
  static void setRawwavePath( std::string path )
  {
    if ( !path.empty() )
      rawwavepath_ = path;

    // Make sure the path includes a "/"
    if ( rawwavepath_[rawwavepath_.length()-1] != '/' )
      rawwavepath_ += "/";
  }


  //! Static method that byte-swaps a 16-bit data type.
  static void swap16( unsigned char *ptr )
  {
    unsigned char val;

    // Swap 1st and 2nd bytes
    val = *(ptr);
    *(ptr) = *(ptr+1);
    *(ptr+1) = val;
  }


  //! Static method that byte-swaps a 32-bit data type.
  static void swap32( unsigned char *ptr )
  {
    unsigned char val;

    // Swap 1st and 4th bytes
    val = *(ptr);
    *(ptr) = *(ptr+3);
    *(ptr+3) = val;

    //Swap 2nd and 3rd bytes
    ptr += 1;
    val = *(ptr);
    *(ptr) = *(ptr+1);
    *(ptr+1) = val;
  }


  //! Static method that byte-swaps a 64-bit data type.
  static void swap64( unsigned char *ptr ) {
  {
    unsigned char val;

    // Swap 1st and 8th bytes
    val = *(ptr);
    *(ptr) = *(ptr+7);
    *(ptr+7) = val;

    // Swap 2nd and 7th bytes
    ptr += 1;
    val = *(ptr);
    *(ptr) = *(ptr+5);
    *(ptr+5) = val;

    // Swap 3rd and 6th bytes
    ptr += 1;
    val = *(ptr);
    *(ptr) = *(ptr+3);
    *(ptr+3) = val;

    // Swap 4th and 5th bytes
    ptr += 1;
    val = *(ptr);
    *(ptr) = *(ptr+1);
    *(ptr+1) = val;
  }


  //! Static cross-platform method to sleep for a number of milliseconds.
  static void sleep( unsigned long milliseconds )
  {
    #if defined(__OS_WINDOWS__)
      Sleep((DWORD) milliseconds);
    #elif (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
      usleep( (unsigned long) (milliseconds * 1000.0) );
    #endif
  }


  //! Static method to check whether a value is within a specified range.
  static bool inRange( T value, T min, T max ) {
    if ( value < min ) return false;
    else if ( value > max ) return false;
    else return true;
  }

  //! Internal function for error reporting that assumes message in \c oStream_ variable.
  void handleError( StkError::Type type ) const
  {
    handleError( oStream_.str(), type );
    oStream_.str( std::string() ); // reset the ostringstream buffer
  }

  //! Static function for error reporting and handling using c-strings.
  static void handleError( const char *message, StkError::Type type )
  {
    std::string msg( message );
    handleError( msg, type );    
  }


  //! Static function for error reporting and handling using c++ strings.
  static void handleError( std::string message, StkError::Type type )
  {
    if ( type == StkError::WARNING || type == StkError::STATUS ) {
      if ( !showWarnings_ ) return;
      std::cerr << '\n' << message << '\n' << std::endl;
    }
    else if (type == StkError::DEBUG_PRINT) {
    #if defined(_STK_DEBUG_)
        std::cerr << '\n' << message << '\n' << std::endl;
    #endif
    }
    else {
      if ( printErrors_ ) {
        // Print error message before throwing.
        std::cerr << '\n' << message << '\n' << std::endl;
      }
      throw StkError(message, type);
    }
  }



  //! Toggle display of WARNING and STATUS messages.
  static void showWarnings( bool status ) { showWarnings_ = status; }

  //! Toggle display of error messages before throwing exceptions.
  static void printErrors( bool status ) { printErrors_ = status; }

private:

  static T srate_;
  static std::string rawwavepath_;
  static bool showWarnings_;
  static bool printErrors_;
  static std::vector<Stk *> alertList_;

protected:

  static std::ostringstream oStream_;
  bool ignoreSampleRateChange_;

  //! Default constructor.
  Stk( void ): ignoreSampleRateChange_(false)
  {

  }

  virtual ~Stk( void )
  {

  }

  //! This function should be implemented in subclasses that depend on the sample rate.
  virtual void sampleRateChanged( T newRate, T oldRate ) {

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
                Stk::handleError( error, StkError::MEMORY_ALLOCATION );
              }
          #endif
        }

  dataRate_ = Stk::sampleRate();
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
            Stk::handleError( error, StkError::MEMORY_ALLOCATION );
          }
      #endif
      for ( long i=0; i<(long)size_; i++ ) data_[i] = value;
    }

    dataRate_ = Stk::sampleRate();
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
    dataRate_ = Stk::sampleRate();
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
    dataRate_ = Stk::sampleRate();
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

  //! Scaling operator (T * StkFrame)
  friend StkFrames<T> operator*(T v, const StkFrames<T>& f);

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
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
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
          Stk::handleError( error, StkError::MEMORY_ALLOCATION );
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
  StkFrames<T>& getChannel(unsigned int channel,StkFrames<T>& destinationFrames, unsigned int destinationChannel) const
  {
  #if defined(_STK_DEBUG_)
    if (sourceChannel > channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::getChannel invalid sourceChannel (" << sourceChannel << ")";
      Stk::handleError( error.str(), StkError::FUNCTION_ARGUMENT);
    }
    if (destinationChannel > destinationFrames.channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::getChannel invalid destinationChannel (" << destinationChannel << ")";
      Stk::handleError( error.str(), StkError::FUNCTION_ARGUMENT );
    }
    if (destinationFrames.frames() < frames()) {
      std::ostringstream error;
      error << "StkFrames::getChannel destination.frames() < frames()";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS);
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
  void setChannel(unsigned int channel,const StkFrames<T>&sourceFrames,unsigned int sourceChannel)
  {
  #if defined(_STK_DEBUG_)
    if (sourceChannel > sourceFrames.channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::setChannel invalid sourceChannel (" << sourceChannel << ")";
      Stk::handleError( error.str(), StkError::FUNCTION_ARGUMENT);
    }
    if (destinationChannel > channels() - 1) {
      std::ostringstream error;
      error << "StkFrames::setChannel invalid channel (" << destinationChannel << ")";
      Stk::handleError( error.str(), StkError::FUNCTION_ARGUMENT );
    }
    if (sourceFrames.frames() != frames()) {
      std::ostringstream error;
      error << "StkFrames::setChannel sourceFrames.frames() != frames()";
      Stk::handleError( error.str(), StkError::MEMORY_ACCESS);
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
  
  void SetData(T * p) {
      memcpy(data_,p,size_t * sizeof(float); )
  }
  std::vector<T> GetVector() {
      std::vector<T> r(size_);
      memcpy(r.data(),data_,size_ * sizeof(T));
      return r;
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
inline bool StkFrame<T>:: empty() const
{
  if ( size_ > 0 ) return false;
  else return true;
}

template<typename T>
inline T& StkFrame<T>:: operator[] ( size_t n )
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

template<typename T>
inline T StkFrame<T>:: operator[] ( size_t n ) const
{
#if defined(_STK_DEBUG_)
  if ( n >= size_ ) {
    std::ostringstream error;
    error << "StkFrames::operator[]: invalid index (" << n << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[n];
}

template<typename T>
inline T& StkFrame<T>:: operator() ( size_t frame, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}

template<typename T>
inline T StkFrame<T>:: operator() ( size_t frame, unsigned int channel ) const
{
#if defined(_STK_DEBUG_)
  if ( frame >= nFrames_ || channel >= nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator(): invalid frame (" << frame << ") or channel (" << channel << ") value!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  return data_[ frame * nChannels_ + channel ];
}
    
template<typename T>
inline StkFrames StkFrames::operator+(const StkFrames<T>&f) const
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif
  StkFrames sum((unsigned int)nFrames_,nChannels_);
  T *sumPtr = &sum[0];
  const T *fptr = f.data_;
  const T *dPtr = data_;
  for (unsigned int i = 0; i < size_; i++) {
    *sumPtr++ = *fptr++ + *dPtr++;
  }
  return sum;
}

template<typename T>
inline StkFrames<T>& StkFrame<T>:: operator+= ( StkFrames<T>& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator+=: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  T *fptr = &f[0];
  T *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ += *fptr++;
  return *this;
}

template<typename T>
inline StkFrames<T>& StkFrame<T>:: operator*= ( StkFrames<T>& f )
{
#if defined(_STK_DEBUG_)
  if ( f.frames() != nFrames_ || f.channels() != nChannels_ ) {
    std::ostringstream error;
    error << "StkFrames::operator*=: frames argument must be of equal dimensions!";
    Stk::handleError( error.str(), StkError::MEMORY_ACCESS );
  }
#endif

  T *fptr = &f[0];
  T *dptr = data_;
  for ( unsigned int i=0; i<size_; i++ )
    *dptr++ *= *fptr++;
  return *this;
}

template<typename T>
inline StkFrames StkFrames::operator*(T v) const
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
inline StkFrames operator*(T v, const StkFrames<T>& f)
{
  StkFrames res((unsigned int)f.nFrames_, f.nChannels_);
  T *resPtr = &res[0];
  T *dPtr = f.data_;
  for (unsigned int i = 0; i < f.size_; i++) {
    *resPtr++ = v * *dPtr++;
  }
  return res;
}

template<typename T>
inline StkFrames<T>& StkFrame<T>:: operator*= ( T v )
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

// The default sampling rate.
const double SRATE = 44100.0;

// The default real-time audio input and output buffer size.  If
// clicks are occuring in the input and/or output sound stream, a
// larger buffer size may help.  Larger buffer sizes, however, produce
// more latency.
const unsigned int RT_BUFFER_SIZE = 512;

// The default rawwave path value is set with the preprocessor
// definition RAWWAVE_PATH.  This can be specified as an argument to
// the configure script, in an integrated development environment, or
// below.  The global STK rawwave path variable can be dynamically set
// with the Stk::setRawwavePath() function.  This value is
// concatenated to the beginning of all references to rawwave files in
// the various STK core classes (e.g. Clarinet.cpp).  If you wish to
// move the rawwaves directory to a different location in your file
// system, you will need to set this path definition appropriately.
#if !defined(RAWWAVE_PATH)
  #define RAWWAVE_PATH "../../rawwaves/"
#endif

const double PI           = 3.14159265358979;
const double TWO_PI       = 2 * PI;
const double ONE_OVER_128 = 0.0078125;

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

} // stk namespace

#endif