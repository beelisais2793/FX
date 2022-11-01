%{
#include "RtAudio.h"
%}

class RtApiWasapi : public RtApi
{
public:
  RtApiWasapi();
  virtual ~RtApiWasapi();

  RtAudio::Api getCurrentApi( void ) override;
  unsigned int getDeviceCount( void ) override;
  RtAudio::DeviceInfo getDeviceInfo( unsigned int device ) override;
  void closeStream( void ) override;
  void startStream( void ) override;
  void stopStream( void ) override;
  void abortStream( void ) override;
};
