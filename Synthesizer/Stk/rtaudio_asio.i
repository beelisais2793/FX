%{
#include "RtAudio.h"
%}

class RtApiAsio: public RtApi
{
public:

  RtApiAsio();
  ~RtApiAsio();
  RtAudio::Api getCurrentApi( void ) override;
  unsigned int getDeviceCount( void ) override;
  unsigned int getDefaultOutputDevice( void ) override;
  unsigned int getDefaultInputDevice( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;

  bool callbackEvent( long bufferIndex );
};

