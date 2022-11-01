#pragma once
#include "StkWvIn.hpp"
#include "StkFileRead.hpp"
#include <cmath>


namespace stk {

/***************************************************/
/*! \class FileWvIn
    \brief STK audio file input class.

    This class inherits from WvIn.  It provides a "tick-level"
    interface to the FileRead class.  It also provides variable-rate
    playback functionality.  Audio file support is provided by the
    FileRead class.  Linear interpolation is used for fractional read
    rates.

    FileWvIn supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames object for
    multi-channel and/or multi-frame data.

    FileWvIn will either load the entire content of an audio file into
    local memory or incrementally read file data from disk in chunks.
    This behavior is controlled by the optional constructor arguments
    \e chunkThreshold and \e chunkSize.  File sizes greater than \e
    chunkThreshold (in sample frames) will be read incrementally in
    chunks of \e chunkSize each (also in sample frames).

    For file data read completely into local memory, the \e doNormalize
    flag can be used to normalize all values with respect to the maximum
    absolute value of the data.

    If the file data format is fixed point, the flag \e doInt2FloatScaling
    can be used to control whether the values are scaled with respect to
    the corresponding fixed-point maximum. For example, if reading 16-bit
    signed integers, the input values will be scaled by 1 / 32768.0. This
    scaling will not happen for floating-point file data formats.

    When the file end is reached, subsequent calls to the tick()
    functions return zeros and isFinished() returns \e true.

    See the FileRead class for a description of the supported audio
    file formats.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class FileWvIn : public WvIn<T>
{
public:
  //! Default constructor.
  FileWvIn( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 );

  //! Overloaded constructor for file input.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  FileWvIn( std::string fileName, bool raw = false, bool doNormalize = true,
            unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024,
            bool doInt2FloatScaling = true );

  //! Class destructor.
  ~FileWvIn( void );

  //! Open the specified file and load its data.
  /*!
    Data from a previously opened file will be overwritten by this
    function.  An StkError will be thrown if the file is not found,
    its format is unknown, or a read error occurs.  If the file length
    is less than the chunkThreshold limit and \e doNormalize is true,
    the file data will be normalized with respect to the maximum absolute
    value of the data. If the \e doInt2FloatScaling flag is true and the
    input data is fixed-point, a scaling will be applied with respect to
    the fixed-point limits.
  */
  virtual void openFile( std::string fileName, bool raw = false, bool doNormalize = true, bool doInt2FloatScaling = true );

  //! Close a file if one is open.
  virtual void closeFile( void );

  //! Clear outputs and reset time (file) pointer to zero.
  virtual void reset( void );

  //! Normalize data to a maximum of +-1.0.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  virtual void normalize( void );

  //! Normalize data to a maximum of \e +-peak.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  virtual void normalize( T peak );

  //! Return the file size in sample frames.
  virtual unsigned long getSize( void ) const { return fileSize_; };

  //! Return the input file sample rate in Hz (not the data read rate).
  /*!
    WAV, SND, and AIF formatted files specify a sample rate in
    their headers.  STK RAW files have a sample rate of 22050 Hz
    by definition.  MAT-files are assumed to have a rate of 44100 Hz.
  */
  virtual T getFileRate( void ) const { return this->data_.dataRate(); };

  //! Query whether a file is open.
  bool isOpen( void ) { return file_.isOpen(); };

  //! Query whether reading is complete.
  bool isFinished( void ) const { return finished_; };

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  virtual void setRate( T rate );

  //! Increment the read pointer by \e time samples.
  /*!
    Note that this function will not modify the interpolation flag status.
   */
  virtual void addTime( T time );

  //! Turn linear interpolation on/off.
  /*!
    Interpolation is automatically off when the read rate is
    an integer value.  If interpolation is turned off for a
    fractional rate, the time index is truncated to an integer
    value.
  */
  void setInterpolate( bool doInterpolate ) { interpolate_ = doInterpolate; };

  //! Return the specified channel value of the last computed frame.
  /*!
    If no file is loaded, the returned value is 0.0.  The \c
    channel argument must be less than the number of output channels,
    which can be determined with the channelsOut() function (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception. \sa
    lastFrame()
  */
  T lastOut( unsigned int channel = 0 );

  //! Compute a sample frame and return the specified \c channel value.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the computed frame.  If no file data is loaded,
    the returned value is 0.0.  The \c channel argument must be less
    than the number of channels in the file data (the first channel is
    specified by 0).  However, range checking is only performed if
    _STK_DEBUG_ is defined during compilation, in which case an
    out-of-range value will trigger an StkError exception.
  */
  virtual T tick( unsigned int channel = 0 );

  //! Fill the StkFrames object with computed sample frames, starting at the specified channel and return the same reference.
  /*!
    The \c channel argument plus the number of input channels must
    be less than the number of channels in the StkFrames argument (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  virtual StkFrames<T>& tick( StkFrames<T>& frames, unsigned int channel = 0 );

protected:

  void sampleRateChanged( T newRate, T oldRate );

  FileRead<T> file_;
  bool finished_;
  bool interpolate_;
  bool int2floatscaling_;
  bool chunking_;
  T time_;
  T rate_;
  unsigned long fileSize_;
  unsigned long chunkThreshold_;
  unsigned long chunkSize_;
  long chunkPointer_;

};

template<typename T>
inline T FileWvIn<T>::lastOut( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "FileWvIn::lastOut(): channel argument and soundfile data are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( finished_ ) return 0.0;
  return this->lastFrame_[channel];
}

/***************************************************/
/*! \class FileWvIn
    \brief STK audio file input class.

    This class inherits from WvIn.  It provides a "tick-level"
    interface to the FileRead class.  It also provides variable-rate
    playback functionality.  Audio file support is provided by the
    FileRead class.  Linear interpolation is used for fractional read
    rates.

    FileWvIn supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames object for
    multi-channel and/or multi-frame data.

    FileWvIn will either load the entire content of an audio file into
    local memory or incrementally read file data from disk in chunks.
    This behavior is controlled by the optional constructor arguments
    \e chunkThreshold and \e chunkSize.  File sizes greater than \e
    chunkThreshold (in sample frames) will be read incrementally in
    chunks of \e chunkSize each (also in sample frames).

    When the file end is reached, subsequent calls to the tick()
    functions return zeros and isFinished() returns \e true.

    See the FileRead class for a description of the supported audio
    file formats.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/



template<typename T>
FileWvIn<T>::FileWvIn( unsigned long chunkThreshold, unsigned long chunkSize )
  : finished_(true), interpolate_(false), time_(0.0), rate_(0.0),
    chunkThreshold_(chunkThreshold), chunkSize_(chunkSize)
{
  this->addSampleRateAlert( this );
}

template<typename T>
FileWvIn<T>::FileWvIn( std::string fileName, bool raw, bool doNormalize,
                      unsigned long chunkThreshold, unsigned long chunkSize,
                      bool doInt2FloatScaling )
  : finished_(true), interpolate_(false), time_(0.0), rate_(0.0),
    chunkThreshold_(chunkThreshold), chunkSize_(chunkSize)
{
  openFile( fileName, raw, doNormalize, doInt2FloatScaling );
  this->addSampleRateAlert( this );
}

template<typename T>
FileWvIn<T>::~FileWvIn()
{
  this->closeFile();
  this->removeSampleRateAlert( this );
}

template<typename T>
void FileWvIn<T>::sampleRateChanged( T newRate, T oldRate )
{
  if ( ! this-> ignoreSampleRateChange_ )
    this->setRate( oldRate * rate_ / newRate );
}

template<typename T>
void FileWvIn<T>::closeFile( void )
{
  if ( file_.isOpen() ) file_.close();
  finished_ = true;
  this->lastFrame_.resize( 0, 0 );
}

template<typename T>
void FileWvIn<T>::openFile( std::string fileName, bool raw, bool doNormalize, bool doInt2FloatScaling )
{
  // Call close() in case another file is already open.
  this->closeFile();

  // Attempt to open the file ... an error might be thrown here.
  file_.open( fileName, raw );

  // Determine whether chunking or not.
  if ( file_.fileSize() > chunkThreshold_ ) {
    chunking_ = true;
    chunkPointer_ = 0;
    this->data_.resize( chunkSize_, file_.channels() );
  }
  else {
    chunking_ = false;
    this->data_.resize( (size_t) file_.fileSize(), file_.channels() );
  }

  if ( doInt2FloatScaling )
    int2floatscaling_ = true;
  else
    int2floatscaling_ = false;

  // Load all or part of the data.
  file_.read( this->data_, 0, int2floatscaling_ );

  // Resize our lastFrame container.
  this->lastFrame_.resize( 1, file_.channels() );

  // Close the file unless chunking
  fileSize_ = file_.fileSize();
  if ( !chunking_ ) file_.close();

  // Set default rate based on file sampling rate.
  this->setRate( this->data_.dataRate() / sampleRate() );

  if ( doNormalize & !chunking_ ) this->normalize();

  this->reset();
}

template<typename T>
void FileWvIn<T>::reset(void)
{
  time_ = (T) 0.0;
  for ( unsigned int i=0; i < this->lastFrame_.size(); i++ ) this->lastFrame_[i] = 0.0;
  finished_ = false;
}

template<typename T>
void FileWvIn<T>::normalize(void)
{
  this->normalize( 1.0 );
}

// Normalize all channels equally by the greatest magnitude in all of the data.
template<typename T>
void FileWvIn<T>::normalize( T peak )
{
  // When chunking, the "normalization" scaling is performed by FileRead.
  if ( chunking_ ) return;

  size_t i;
  T max = 0.0;

  for ( i=0; i < this->data_.size(); i++ ) {
    if ( fabs( this->data_[i] ) > max )
      max = (T) fabs((double) this->data_[i]);
  }

  if ( max > 0.0 ) {
    max = 1.0 / max;
    max *= peak;
    for ( i=0; i < this->data_.size(); i++ )
      this->data_[i] *= max;
  }
}

template<typename T>
void FileWvIn<T>::setRate( T rate )
{
  rate_ = rate;

  // If negative rate and at beginning of sound, move pointer to end
  // of sound.
  if ( (rate_ < 0) && (time_ == 0.0) ) time_ = fileSize_ - 1.0;

  if ( fmod( rate_, 1.0 ) != 0.0 ) interpolate_ = true;
  else interpolate_ = false;
}

template<typename T>
void FileWvIn<T>::addTime( T time )   
{
  // Add an absolute time in samples 
  time_ += time;

  if ( time_ < 0.0 ) time_ = 0.0;
  if ( time_ > fileSize_ - 1.0 ) {
    time_ = fileSize_ - 1.0;
    for ( unsigned int i=0; i < this->lastFrame_.size(); i++ ) this->lastFrame_[i] = 0.0;
    finished_ = true;
  }
}

template<typename T>
T FileWvIn<T>::tick( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "FileWvIn::tick(): channel argument and soundfile data are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( finished_ ) return 0.0;

  if ( time_ < 0.0 || time_ > (T) ( fileSize_ - 1.0 ) ) {
    for ( unsigned int i=0; i < this->lastFrame_.size(); i++ ) this->lastFrame_[i] = 0.0;
    finished_ = true;
    return 0.0;
  }

  T tyme = time_;
  if ( chunking_ ) {

    // Check the time address vs. our current buffer limits.
    if ( ( time_ < (T) chunkPointer_ ) ||
         ( time_ > (T) ( chunkPointer_ + chunkSize_ - 1 ) ) ) {

      while ( time_ < (T) chunkPointer_ ) { // negative rate
        chunkPointer_ -= chunkSize_ - 1; // overlap chunks by one frame
        if ( chunkPointer_ < 0 ) chunkPointer_ = 0;
      }
      while ( time_ > (T) ( chunkPointer_ + chunkSize_ - 1 ) ) { // positive rate
        chunkPointer_ += chunkSize_ - 1; // overlap chunks by one frame
        if ( chunkPointer_ + chunkSize_ > fileSize_ ) // at end of file
          chunkPointer_ = fileSize_ - chunkSize_;
      }

      // Load more data.
      file_.read( this->data_, chunkPointer_, int2floatscaling_ );
    }

    // Adjust index for the current buffer.
    tyme -= chunkPointer_;
  }

  if ( interpolate_ ) {
    for ( unsigned int i=0; i < this->lastFrame_.size(); i++ )
      this->lastFrame_[i] = this->data_.interpolate( tyme, i );
  }
  else {
    for ( unsigned int i=0; i < this->lastFrame_.size(); i++ )
      this->lastFrame_[i] = this->data_( (size_t) tyme, i );
  }

  // Increment time, which can be negative.
  time_ += rate_;

  return this->lastFrame_[channel];
}

template<typename T>
StkFrames<T>& FileWvIn<T>::tick( StkFrames<T>& frames, unsigned int channel)
{
  if ( finished_ ) {
#if defined(_STK_DEBUG_)
    oStream_ << "FileWvIn::tick(): end of file or no open file!";
    handleError( StkError::DEBUG_PRINT );
#endif
    return frames;
  }

  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "FileWvIn::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif
    
  T *samples = &frames[channel];
  unsigned int j, hop = frames.channels() - nChannels;
  if ( nChannels == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
      *samples++ = tick();
  }
  else {
    for ( unsigned int i=0; i<frames.frames(); i++, samples += hop ) {
      *samples++ = tick();
      for ( j=1; j<nChannels; j++ )
        *samples++ = this->lastFrame_[j];
    }
  }
  return frames;
}

} // stk namespace

