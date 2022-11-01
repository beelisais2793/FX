%{
#include "RtAudio.h"
%}

class RtApiDs: public RtApi
{
public:

  RtApiDs();
  ~RtApiDs();
  RtAudio::Api getCurrentApi( void ) override;
  unsigned int getDeviceCount( void ) override;
  unsigned int getDefaultOutputDevice( void ) override;
  unsigned int getDefaultInputDevice( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;

  
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
