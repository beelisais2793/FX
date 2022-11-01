%{
#include "RtAudio.h"
%}

class RtApiOss: public RtApi
{
public:

  RtApiOss();
  ~RtApiOss();
  RtAudio::Api getCurrentApi() override;
  unsigned int getDeviceCount( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;

  void callbackEvent( void );
};
