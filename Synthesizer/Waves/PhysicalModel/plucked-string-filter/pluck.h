#ifndef __PLUCK_H
#define __PLUCK_H
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <queue>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class AudioData;

class Pluck
{
public:
  Pluck(const float F = 440, const float rate = 44100, const float val = 0.99985)
    : fundamental(F), R(val), sRate(rate), prev(0), W1(0), U1(0), Y1(0)
  {
    setFreq(F);
  }

  void setFreq(const float val)
  {
    freq = val;
    float temp = sRate / freq - 0.5f;
    L = static_cast<unsigned>(temp);
    delta = temp - L;
    float w = 2*M_PI*freq/sRate;
    a = (sin((1-delta)*w/2)) / (sin((1+delta)*w/2)); //Set allpass filter coefficient

    //Initialize queue of 'L' zeroes
    while(!feedback.empty())
      feedback.pop();
    while (feedback.size() < L)
      feedback.push(0);
  }

  float operator()(float x)
  {
    float W = x + pow(R, L) * feedback.front();
    feedback.pop();

    float U = 0.5f * (W + W1); //Less multiplication
    //float U = 0.5f*W + 0.5* W1;

    float Y = a*(U - Y1) + U1; //Less multiplication
    //float Y = a*U + U1 - a*Y1;

    W1 = W;
    U1 = U;
    Y1 = Y;
    feedback.push(Y);
    return Y;
  }

  void noteOn(AudioData *ad, float note, double duration);

  const float fundamental;
  const float R;
  const unsigned sRate;
  unsigned L;
  unsigned prev;
  float noise[100];
  float freq, delta;
  float a;
  float W1, U1, Y1;
  std::queue<float> feedback;
};

class AudioData
{
public:
  /*! (Constructor): creates an audio data obj using floating point data w/
  nframes frames, sampling rate R, and nchannels audio channels*/
  AudioData(unsigned nframes, unsigned R = 44100, unsigned nchannels = 1);
  /*! (Constructor): creates an audio data obj by reading the contents of the
  WAVE file named fname */
  AudioData(const char *fname);
  //! Returns value of specified channel within given frame of audio data (getter)
  float sample(unsigned frame, unsigned channel=0) const;
  //! Returns value of specified channel within given frame of audio data (setter)
  float& sample(unsigned frame, unsigned channel=0);

  /*! Returns ptr to floating point array that stores the audio data.
  Channel is interleaved for data that uses >1 audio channels. */
  const float* data(void) const { return &fdata[0]; }
  float* data(void)             { return &fdata[0]; }
  //! Returns the number of frames in the audio data
  unsigned frames(void) const   { return frame_count; }
  //! Returns the sampling rate for the audio data
  unsigned rate(void) const     { return sampling_rate; }
  //! Returns the number of channels in the audio data
  unsigned channels(void) const { return channel_count; }

private:
  std::vector<float> fdata;
  unsigned frame_count,
           sampling_rate,
           channel_count;
};

//! Normalizes audio data 'ad' to given dB level
void normalize(AudioData &ad, float dB=0);
//! Writes to wave file
bool waveWrite(const char *fname, const AudioData &ad, unsigned bits=16);

#endif //__PLUCK_H
