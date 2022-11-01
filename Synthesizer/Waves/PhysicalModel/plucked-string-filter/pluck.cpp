// Elgin Ciani -- Dec. 4, 2019
// pluck.cpp
//
// Usage:
//   ./pluck
// Output:
//   mono wave file at sampling rate 44100 Hz
//    wave file contains impulses according to input provided in header includes

#define _DEBUG_ON 1

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <stdlib.h>
#include <time.h>
#include "pluck.h" //contains <vector>, <queue>, <cmath>
#include "key_freqs.h" //Note definitions for equal tempered keyboard

using std::cerr; using std::cout; using std::endl;

//////////////////// MAIN ////////////////////
int main()
{
  Pluck pM(440.0f, 44100.0f, 0.9995f);
  Pluck pB(440.0f, 44100.0f, 0.99995f);
  //44100 sam/sec * 26 bars + fluff
  AudioData ad(44100*45); //(44100, mono)
  AudioData ad2(44100*45); //(44100, mono)
  srand(time(NULL));

  #include "bassline.h"
  #include "melody.h"

  //Normalize both voices to -1.5
  normalize(ad, -1.5f);
  normalize(ad2, -1.5f);

  //Mix overlapping voices by averaging samples
  for(unsigned i = 0; i < ad.frames() &&  i < ad2.frames(); ++i)
    ad.data()[i] = (ad.data()[i] + ad2.data()[i]) / 2;

  if (waveWrite("output.wav", ad, 16))
  {
    #if _DEBUG_ON
    cout << "file 'output.wav' written" << endl;
    #endif
  }
  else
  {
    cerr << "failed to write file 'output.wav'" << endl;
    return 1;
  }

  return 0;
}
//////////////////// END MAIN ////////////////////

void Pluck::noteOn(AudioData *ad, float note, double dur)
{
  //Generate 100 samples of noise to excite filter
  for (unsigned i = 0; i < 100; ++i)
  {
    noise[i] = float(rand()) / float(RAND_MAX);
    noise[i] = (1 - noise[i]) * -15000.0f + noise[i] * 15000.0f;
  }

  //Run filter for frequency
  setFreq(note);
  for (unsigned j = 0; j < dur * sRate; ++j)
  {
    if(!(j<dur*sRate-1))
      prev += j;

    if (j < 100)
      ad->data()[prev+j] = (*this)(noise[j]);
    else
      ad->data()[prev+j] = (*this)(0);
  }
  return;
}


////////// AUDIO DATA //////////
//(Ctor): Creates an audio data obj using floating point data w/ nframes frames, sampling rate R, and num channels
AudioData::AudioData(const unsigned nframes, const unsigned R, const unsigned nchannels)
  : frame_count(nframes), sampling_rate(R), channel_count(nchannels)
{
  fdata.resize(frame_count * channel_count, 0.0f);
}

//(Ctor): Creates an audio data obj by reading contents of WAVE file fname
AudioData::AudioData(const char *fname)
{
  char header[44];     // for storing WAVE header
  char buff[16] = {0}; // for storing temp values

  // attempt to open file for read
  std::ifstream file(fname, std::ios::in | std::ios::binary);
  if (!file)
    throw std::runtime_error("Unable to open file.");

  file.read(header, 44);
  std::strncpy(buff, header, 4); // copy 4 bytes into buffer
  if (std::string(buff) != "RIFF")
    throw std::runtime_error("File doesn't have valid RIFF ID.");

  std::strncpy(buff, header + 8, 4);
  if (std::string(buff) != "WAVE")
    throw std::runtime_error("File is not a valid WAVE form-type.");

  std::strncpy(buff, header + 12, 4);
  if (std::string(buff) != "fmt ")
    throw std::runtime_error("File does not have a valid wave-form chunk ID.");

  std::strncpy(buff, header + 36, 4);
  if (std::string(buff) != "data")
    throw std::runtime_error("File does not have a valid data chunk ID.");

  std::strncpy(buff, header + 24, 4); // read the 4 bytes storing the sampling rate
  sampling_rate = *reinterpret_cast<unsigned*>(buff);

  std::strncpy(buff, header + 22, 2); // read the 2 bytes storing the number of channels
  channel_count = *reinterpret_cast<unsigned short*>(buff); // get data here as a short
  std::strncpy(buff, header + 34, 2); // read the 2 bytes storing the bits per sample
  const unsigned short bps = *reinterpret_cast<unsigned short*>(buff);

  std::strncpy(buff, header + 40, 4); // read the 4 bytes storing the subchunk2size
  const unsigned Subchunk2Size = *reinterpret_cast<unsigned*>(buff);
#if 0
  file.seekg(0, file.end);
  if (Subchunk2Size != unsigned(file.tellg()) - 44) // get size of data minus header
  {
    char error[256];
    std::sprintf(error, "ERROR: Inconsistent data size.\nExpected: %u\nGot: %u", Subchunk2Size, unsigned(file.tellg()) - 44);
    throw std::runtime_error(error);
  }
#endif
  std::strncpy(buff, header + 32, 4); // read the 4 bytes storing the sampling rate
  const unsigned blockAlign = *reinterpret_cast<unsigned*>(buff);
  frame_count = Subchunk2Size / blockAlign;

  file.seekg(44, std::ifstream::beg); // move past the header
  while (!file.eof()) // fill the data vector with the raw WAVE data
  {
    file.read(buff, bps / 8);
    if (bps == 8)
    {
      const signed char c = *reinterpret_cast<signed char*>(buff);
      fdata.push_back((float(c) - 128) / 127);
    }
    else if (bps == 16)
    {
      const signed short s = *reinterpret_cast<signed short*>(buff);
      fdata.push_back(s >= 0 ? s / float(0x7FFE) : s / float(0x7FFF));
    }
  }
  if (!fdata.empty()) // remove duplicate from end of data vector
    fdata.pop_back();

  file.close();
}

// Returns value of specified channel within given frame of audio data (getter)
float AudioData::sample(const unsigned frame, const unsigned channel) const
{
  return fdata[(frame*channel_count) + channel];
}

// Returns value of specified channel within given frame of audio data (setter)
float& AudioData::sample(const unsigned frame, const unsigned channel)
{
  return fdata[(frame*channel_count) + channel];
}
////////// END AUDIO DATA //////////


bool waveWrite(const char *fname, const AudioData &ad, const unsigned bits)
{
  if ((bits != 8 && bits != 16) || ad.channels() > 2)
    return false;

  std::ofstream file;
  file.open(fname, std::ios::binary);

  // WAVE file header
  char header[44];
  strncpy(header, "RIFF", 4);
  *reinterpret_cast<unsigned*>(header + 4) = 36u + ad.frames() * ad.channels();
  strncpy(header + 8, "WAVE", 4);
  strncpy(header + 12, "fmt ", 4);
  *reinterpret_cast<unsigned*>(header + 16) = 16;                  // PCM
  *reinterpret_cast<unsigned short*>(header + 20) = 1;             // uncompressed
  *reinterpret_cast<unsigned short*>(header + 22) = ad.channels(); // channel count
  *reinterpret_cast<unsigned*>(header + 24) = ad.rate();           // sample rate
  *reinterpret_cast<unsigned*>(header + 28) = ad.channels() * ad.rate() * bits/8; // byte rate
  *reinterpret_cast<unsigned*>(header + 32) = ad.channels() * bits / 8;           // block align
  *reinterpret_cast<unsigned short*>(header + 34) = bits;          // bit resolution (given)
  strncpy(header + 36, "data", 4);
  unsigned Subchunk2Size = ad.frames() * ad.channels() * bits / 8;
  *reinterpret_cast<unsigned*>(header + 40) = Subchunk2Size; // Subchunk2Size

  file.write(header, 44);

  for (unsigned i = 0; i < ad.frames() * ad.channels(); ++i)
  {
    float f = ad.data()[i]; // read the current float

    if (bits == 8) // bits = 8
    {
      char c = (f *= 127) + 128; // (float val)*(max voltage) + 128
      file.write(&c, 1);
    }
    else // bits = 16
    {
      unsigned short s = f *= 32768; // (float val)*(2^15)
      file.write(reinterpret_cast<char *>(&s), 2);
    }
  }
  file.close();
  return true; // success
}

// Normalizes audio data to given dB level
void normalize(AudioData &ad, const float dB)
{
  std::vector<float> averages(ad.channels()); // for holding DC offsets
  float largest; // for holding largest absolute sample value of each channel
  float gainFactor = pow(10, dB / 20);

  // for each channel, sum every sample
  for (unsigned i = 0; i < ad.frames() * ad.channels(); ++i)
    averages[i % ad.channels()] += ad.data()[i];

  // find the average for each channel by dividing by the number of frames
  for (unsigned i = 0; i < averages.size(); ++i)
    averages[i] /= ad.frames();

  // remove the DC offset from each sample
  for (unsigned i = 0; i < ad.frames() * ad.channels(); ++i)
    ad.data()[i] -= averages[i % ad.channels()];

  // get the largest (absolute) value of entire data set (regardless of channel count)
  for (unsigned i = 0; i < ad.frames() * ad.channels(); ++i)
    largest = std::max(largest, std::abs(ad.data()[i]));

  gainFactor /= largest; // update the gain value now that we've found the largest sample

    // multiply each sample by the gain factor
  for (unsigned i = 0; i < ad.frames() * ad.channels(); ++i)
    ad.data()[i] *= gainFactor;
}
