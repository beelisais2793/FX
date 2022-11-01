%{
#include "RtAudio.h"
%}

#if defined(__WINDOWS_DS__)

class RtApiDs: public RtApi
{
public:

  RtApiDs();
  ~RtApiDs();
  RtAudio::Api getCurrentApi( void ) override { return RtAudio::WINDOWS_DS; }
  unsigned int getDeviceCount( void ) override;
  unsigned int getDefaultOutputDevice( void ) override;
  unsigned int getDefaultInputDevice( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;

  // This function is intended for internal use only.  It must be
  // public because it is called by the internal callback handler,
  // which is not a member of RtAudio.  External use of this function
  // will most likely produce highly undesirable results!
  void callbackEvent( void );

  private:

  bool coInitialized_;
  bool buffersRolling;
  long duplexPrerollBytes;
  std::vector<struct DsDevice> dsDevices;
  bool probeDeviceOpen( unsigned int device, StreamMode mode, unsigned int channels, 
                        unsigned int firstChannel, unsigned int sampleRate,
                        RtAudioFormat format, unsigned int *bufferSize,
                        RtAudio::StreamOptions *options ) override;
};

