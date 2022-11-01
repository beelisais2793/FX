#pragma once
#include "StkFileWvIn.hpp"
#include <cmath>

namespace stk {

/***************************************************/
/*! \class FileLoop
    \brief STK file looping / oscillator class.

    This class provides audio file looping functionality.  Any audio
    file that can be loaded by FileRead can be looped using this
    class.

    FileLoop supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames<T> object for
    multi-channel and/or multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class FileLoop : protected FileWvIn<T>
{
 public:
  //! Default constructor.
  FileLoop( unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024 );

  //! Class constructor that opens a specified file.
  FileLoop( std::string fileName, bool raw = false, bool doNormalize = true,
            unsigned long chunkThreshold = 1000000, unsigned long chunkSize = 1024,
            bool doInt2FloatScaling = true );

  //! Class destructor.
  ~FileLoop( void );

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
  void openFile( std::string fileName, bool raw = false, bool doNormalize = true, bool doInt2FloatScaling = true );

  //! Close a file if one is open.
  void closeFile( void ) { FileWvIn<T>::closeFile(); };

  //! Clear outputs and reset time (file) pointer to zero.
  void reset( void ) { FileWvIn<T>::reset(); };

  //! Return the number of audio channels in the data or stream.
  unsigned int channelsOut( void ) const { return this->data_.channels(); };

  //! Normalize data to a maximum of +-1.0.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  void normalize( void ) { FileWvIn<T>::normalize( 1.0 ); };

  //! Normalize data to a maximum of \e +-peak.
  /*!
    This function has no effect when data is incrementally loaded
    from disk.
  */
  void normalize( T peak ) { FileWvIn<T>::normalize( peak ); };

  //! Return the file size in sample frames.
  //unsigned long getSize( void ) const { return data_.frames(); };
  unsigned long getSize( void ) const { return this->fileSize_; };

  //! Return the input file sample rate in Hz (not the data read rate).
  /*!
    WAV, SND, and AIF formatted files specify a sample rate in
    their headers.  STK RAW files have a sample rate of 22050 Hz
    by definition.  MAT-files are assumed to have a rate of 44100 Hz.
  */
  T getFileRate( void ) const { return this->data_.dataRate(); };

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  void setRate( T rate );

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e frequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
  */
  void setFrequency( T frequency ) { this->setRate( this-> fileSize_ * frequency / sampleRate() ); };

  //! Increment the read pointer by \e time samples, modulo file size.
  void addTime( T time );

  //! Increment current read pointer by \e angle, relative to a looping frequency.
  /*!
    This function increments the read pointer based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
  */
  void addPhase( T angle );

  //! Add a phase offset to the current read pointer.
  /*!
    This function determines a time offset based on the file
    size and the current Stk::sampleRate.  The \e angle value
    is a multiple of file size.
  */
  void addPhaseOffset( T angle );

  //! Return the specified channel value of the last computed frame.
  /*!
    For multi-channel files, use the lastFrame() function to get
    all values from the last computed frame.  If no file data is
    loaded, the returned value is 0.0.  The \c channel argument must
    be less than the number of channels in the file data (the first
    channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  T lastOut( unsigned int channel = 0 ) { return FileWvIn<T>::lastOut( channel ); };

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
  T tick( unsigned int channel = 0 );

  //! Fill the StkFrames<T> object with computed sample frames, starting at the specified channel and return the same reference.
  /*!
    The \c channel argument plus the number of output channels must
    be less than the number of channels in the StkFrames<T> argument (the
    first channel is specified by 0).  However, range checking is only
    performed if _STK_DEBUG_ is defined during compilation, in which
    case an out-of-range value will trigger an StkError exception.
  */
  virtual StkFrames<T>& tick( StkFrames<T>& frames,unsigned int channel = 0 );

 protected:

  StkFrames<T> firstFrame_;
  T phaseOffset_;

};

/***************************************************/
/*! \class FileLoop
    \brief STK file looping / oscillator class.

    This class provides audio file looping functionality.  Any audio
    file that can be loaded by FileRead can be looped using this
    class.

    FileLoop supports multi-channel data.  It is important to
    distinguish the tick() method that computes a single frame (and
    returns only the specified sample of a multi-channel frame) from
    the overloaded one that takes an StkFrames<T> object for
    multi-channel and/or multi-frame data.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
FileLoop<T>::FileLoop( unsigned long chunkThreshold, unsigned long chunkSize )
  : FileWvIn<T>( chunkThreshold, chunkSize ), phaseOffset_(0.0)
{
  this->addSampleRateAlert( this );
}

template<typename T>
FileLoop<T>::FileLoop( std::string fileName, bool raw, bool doNormalize,
                      unsigned long chunkThreshold, unsigned long chunkSize,
                      bool doInt2FloatScaling )
  : FileWvIn<T>( chunkThreshold, chunkSize ), phaseOffset_(0.0)
{
  this->openFile( fileName, raw, doNormalize, doInt2FloatScaling );
  this->addSampleRateAlert( this );
}

template<typename T>
FileLoop<T>::~FileLoop( void )
{
  this->removeSampleRateAlert( this );
}

template<typename T>
void FileLoop<T>::openFile( std::string fileName, bool raw, bool doNormalize, bool doInt2FloatScaling )
{
  // Call close() in case another file is already open.
  this->closeFile();

  // Attempt to open the file ... an error might be thrown here.
  this->file_.open( fileName, raw );

  // Determine whether chunking or not.
  if ( this->file_.fileSize() > this->chunkThreshold_ ) {
    this->chunking_ = true;
    this->chunkPointer_ = 0;
    this->data_.resize( this->chunkSize_ + 1, this->file_.channels() );
  }
  else {
    this->chunking_ = false;
    this->data_.resize( this->file_.fileSize() + 1, this->file_.channels() );
  }

  if ( doInt2FloatScaling )
    this->int2floatscaling_ = true;
  else
    this->int2floatscaling_ = false;

  // Load all or part of the data.
  this->file_.read( this->data_, 0, this->int2floatscaling_ );

  if ( this->chunking_ ) { // If chunking, save the first sample frame for later.
    firstFrame_.resize( 1, this->data_.channels() );
    for ( unsigned int i=0; i < this->data_.channels(); i++ )
      firstFrame_[i] = this->data_[i];
  }
  else {  // If not chunking, copy the first sample frame to the last.
    for ( unsigned int i=0; i < this->data_.channels(); i++ )
      this->data_( this->data_.frames() - 1, i ) = this->data_[i];
  }

  // Resize our lastOutputs container.
  this->lastFrame_.resize( 1, this->file_.channels() );

  // Close the file unless chunking
  this->fileSize_ = this->file_.fileSize();
  if ( ! this->chunking_ ) this->file_.close();

  // Set default rate based on file sampling rate.
  this->setRate( this->data_.dataRate() / sampleRate() );

  if ( doNormalize & ! this->chunking_ ) this->normalize();

  this->reset();
}

template<typename T>
void FileLoop<T>::setRate( T rate )
{
  this->rate_ = rate;

  if ( fmod( this->rate_, 1.0 ) != 0.0 ) this->interpolate_ = true;
  else this->interpolate_ = false;
}

template<typename T>
void FileLoop<T>::addTime( T time )
{
  // Add an absolute time in samples.
  this->time_ += time;

  while ( (this->time_) < 0.0 )
    this->time_ += this->fileSize_;
  while ( this->time_ >= this->fileSize_ )
    this->time_ -= this->fileSize_;
}

template<typename T>
void FileLoop<T>::addPhase( T angle )
{
  // Add a time in cycles (one cycle = fileSize).
  this->time_ += this->fileSize_ * angle;

  while ( (this->time_) < 0.0 )
    this->time_ += this->fileSize_;
  while ( this->time_ >= this->fileSize_ )
    this->time_ -= this->fileSize_;
}

template<typename T>
void FileLoop<T>::addPhaseOffset( T angle )
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset_ = this->fileSize_ * angle;
}

template<typename T>
T FileLoop<T>::tick( unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= data_.channels() ) {
    oStream_ << "FileLoop::tick(): channel argument and soundfile data are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  if ( this->finished_ ) return 0.0;

  // Check limits of time address ... if necessary, recalculate modulo
  // fileSize.
  while ( (this->time_) < 0.0 )
    this->time_ += this->fileSize_;
  while ( this->time_ >= this->fileSize_ )
    this->time_ -= this->fileSize_;

  T tyme = this->time_;
  if ( phaseOffset_ ) {
    tyme += phaseOffset_;
    while ( tyme < 0.0 )
      tyme += this->fileSize_;
    while ( tyme >= this->fileSize_ )
      tyme -= this->fileSize_;
  }

  if ( this->chunking_ ) {

    // Check the time address vs. our current buffer limits.
    if ( ( (this->time_) < (T) this->chunkPointer_ ) ||
         ( (this->time_) > (T) ( this->chunkPointer_ + this->chunkSize_ - 1 ) ) ) {

      while ( (this->time_) < (T) (this->chunkPointer_) ) { // negative rate
        this->chunkPointer_ -= this->chunkSize_ - 1; // overlap chunks by one frame
        if ( (this->chunkPointer_) < 0 ) this->chunkPointer_ = 0;
      }
      while ( (this->time_ )> (T) ( (this->chunkPointer_ + this->chunkSize_) - 1 ) ) { // positive rate
        this->chunkPointer_ += this->chunkSize_ - 1; // overlap chunks by one frame
        if ( (this->chunkPointer_ + this->chunkSize_) > this->fileSize_ ) { // at end of file
          this->chunkPointer_ = this->fileSize_ - this->chunkSize_ + 1; // leave extra frame at end of buffer
          // Now fill extra frame with first frame data.
          for ( unsigned int j=0; j<firstFrame_.channels(); j++ )
            this->data_( this->data_.frames() - 1, j ) = firstFrame_[j];
        }
      }

      // Load more data.
      this->file_.read( this->data_, this->chunkPointer_, this->int2floatscaling_ );
    }

    // Adjust index for the current buffer.
    tyme -= this->chunkPointer_;
  }

  if ( this->interpolate_ ) {
    for ( unsigned int i=0; i < (this->lastFrame_).size(); i++ )
      this->lastFrame_[i] = this->data_.interpolate( tyme, i );
  }
  else {
    for ( unsigned int i=0; i< (this->lastFrame_).size(); i++ )
      this->lastFrame_[i] = this->data_( (size_t) tyme, i );
  }

  // Increment time, which can be negative.
  this->time_ += this->rate_;

  return this->lastFrame_[channel];
}

template<typename T>
StkFrames<T>& FileLoop<T>::tick( StkFrames<T>& frames, unsigned int channel)
{
  if ( this->finished_ ) {
#if defined(_STK_DEBUG_)
    oStream_ << "FileLoop::tick(): no file data is loaded!";
    handleError( StkError::DEBUG_PRINT );
#endif
  return frames;
  }
        
  unsigned int nChannels = this->lastFrame_.channels();
#if defined(_STK_DEBUG_)
  if ( channel > frames.channels() - nChannels ) {
    oStream_ << "FileLoop::tick(): channel and StkFrames<T> arguments are incompatible!";
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

