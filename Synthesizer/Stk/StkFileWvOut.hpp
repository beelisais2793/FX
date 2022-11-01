#pragma once
#include "StkWvOut.hpp"
#include "StkFileWrite.hpp"

namespace stk {

/***************************************************/
/*! \class FileWvOut
    \brief STK audio file output class.

    This class inherits from WvOut.  It provides a "tick-level"
    interface to the FileWrite class.

    FileWvOut writes samples to an audio file and supports
    multi-channel data.  It is important to distinguish the tick()
    method that outputs a single sample to all channels in a sample
    frame from the overloaded one that takes a reference to an
    StkFrames<T> object for multi-channel and/or multi-frame data.

    See the FileWrite class for a description of the supported audio
    file formats.

    Currently, FileWvOut is non-interpolating and the output rate is
    always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

template<typename T>
class FileWvOut : public WvOut<T>
{
 public:

  //! Default constructor with optional output buffer size argument.
  /*!
    The output buffer size defines the number of frames that are
    accumulated between writes to disk.
  */
  FileWvOut( unsigned int bufferFrames = 1024 );

  //! Overloaded constructor used to specify a file name, type, and data format with this object.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  FileWvOut( std::string fileName,
             unsigned int nChannels = 1,
             FILE_TYPE type = FILE_WAV,
             StkFormat format = STK_SINT16,
             unsigned int bufferFrames = 1024 );

  //! Class destructor.
  virtual ~FileWvOut();

  //! Open a new file with the specified parameters.
  /*!
    If a file was previously open, it will be closed.  An StkError
    will be thrown if any of the specified arguments are invalid or a
    file error occurs during opening.
  */
  void openFile( std::string fileName,
                 unsigned int nChannels,
                 FILE_TYPE type,
                 StkFormat format );

  //! Close a file if one is open.
  /*!
    Any data remaining in the internal buffer will be written to
    the file before closing.
  */
  void closeFile( void );

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if an output error occurs.
  */
  void tick( const T sample );

  //! Output the StkFrames<T> data.
  /*!
    An StkError will be thrown if an output error occurs.  An
    StkError will also be thrown if _STK_DEBUG_ is defined during
    compilation and there is an incompatability between the number of
    channels in the FileWvOut object and that in the StkFrames<T> object.
  */
  void tick( const StkFrames<T>& frames );

 protected:

  void incrementFrame( void );

  FileWrite<T> file_;
  unsigned int bufferFrames_;
  unsigned int bufferIndex_;
  unsigned int iData_;

};

/***************************************************/
/*! \class FileWvOut
    \brief STK audio file output class.

    This class inherits from WvOut.  It provides a "tick-level"
    interface to the FileWrite class.

    FileWvOut writes samples to an audio file and supports
    multi-channel data.  It is important to distinguish the tick()
    method that outputs a single sample to all channels in a sample
    frame from the overloaded one that takes a reference to an
    StkFrames<T> object for multi-channel and/or multi-frame data.

    See the FileWrite class for a description of the supported audio
    file formats.

    Currently, FileWvOut is non-interpolating and the output rate is
    always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/


template<typename T>
FileWvOut <T>:: FileWvOut( unsigned int bufferFrames )
  :bufferFrames_( bufferFrames )
{
}

template<typename T>
FileWvOut<T>::FileWvOut( std::string fileName, unsigned int nChannels, FILE_TYPE type, StkFormat format, unsigned int bufferFrames )
  :bufferFrames_( bufferFrames )
{
  this->openFile( fileName, nChannels, type, format );
}

template<typename T>
FileWvOut <T>:: ~FileWvOut()
{
  this->closeFile();
}

template<typename T>
void FileWvOut <T>:: closeFile( void )
{
  if ( file_.isOpen() ) {

    // Output any remaining samples in the buffer before closing.
    if ( bufferIndex_ > 0 ) {
      this->data_.resize( bufferIndex_, this->data_.channels() );
      file_.write( this->data_ );
    }

    file_.close();
    this->frameCounter_ = 0;
  }
}

template<typename T>
void FileWvOut <T>:: openFile( std::string fileName,
                            unsigned int nChannels,
                            FILE_TYPE type,
                            StkFormat format )
{
  closeFile();

  if ( nChannels < 1 ) {
    oStream_ << "FileWvOut::openFile: the channels argument must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // An StkError can be thrown by the FileWrite class here.
  file_.open( fileName, nChannels, type, format );

  // Allocate new memory if necessary.
  this->data_.resize( bufferFrames_, nChannels );

  bufferIndex_ = 0;
  iData_ = 0;
}

template<typename T>
void FileWvOut <T>:: incrementFrame( void )
{
  this->frameCounter_++;
  bufferIndex_++;

  if ( bufferIndex_ == bufferFrames_ ) {
    file_.write( this->data_ );
    bufferIndex_ = 0;
    iData_ = 0;
  }
}

template<typename T>
void FileWvOut <T>:: tick( const T sample )
{
#if defined(_STK_DEBUG_)
  if ( !file_.isOpen() ) {
    oStream_ << "FileWvOut::tick(): no file open!";
    handleError( StkError::WARNING );
    return;
  }
#endif

  unsigned int nChannels = this->data_.channels();
  T input = sample;
  clipTest( input );
  for ( unsigned int j=0; j<nChannels; j++ )
    this->data_[iData_++] = input;

  this->incrementFrame();
}

template<typename T>
void FileWvOut <T>:: tick( const StkFrames<T>& frames )
{
#if defined(_STK_DEBUG_)
  if ( !file_.isOpen() ) {
    oStream_ << "FileWvOut::tick(): no file open!";
    handleError( StkError::WARNING );
    return;
  }

  if ( data_.channels() != frames.channels() ) {
    oStream_ << "FileWvOut::tick(): incompatible channel value in StkFrames<T> argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  unsigned int iFrames = 0;
  unsigned int j, nChannels = this->data_.channels();
  for ( unsigned int i=0; i<frames.frames(); i++ ) {

    for ( j=0; j<nChannels; j++ ) {
      this->data_[iData_] = frames[iFrames++];
      clipTest( this->data_[iData_++] );
    }

    this->incrementFrame();
  }
}

} // stk namespace

