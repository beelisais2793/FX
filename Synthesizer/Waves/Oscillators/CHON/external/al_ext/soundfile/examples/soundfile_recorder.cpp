#include <memory>

#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al_ext/soundfile/al_SoundfileBufferedRecord.hpp"

using namespace al;

// This example shows usage of the SoundfileBufferedRecord class
// that buffers output from the audio callback to a separate writer
// thread. This is the best way to write to a sound file to avoid
// dropouts on playback

struct MyApp : public App {
  SoundFileBufferedRecord soundFile;
  gam::Sine<> wave{440.0f};

  void onInit() override {
    // Set Gamma sampling rate. Needed for the gam::Square oscillator
    gam::sampleRate(audioIO().framesPerSecond());

    // Set writing time to 2 seconds. This will end recording automatically
    // You can also end recording programmatically by using soundfile.close()
    // if setMaxWriteTime() is not used to set max time, record will continue
    // until disk space runs out.
    soundFile.setMaxWriteTime(2.0f);
    // Open the sound file for writing. This will be a two channel soundfile
    // The default is to use 16-bit int WAV files.
    soundFile.open("output.wav", audioIO().framesPerSecond(), 2,
                   audioIO().framesPerSecond() * 8);
  }

  void onSound(AudioIOData &io) override {
    while (io()) {
      // Write the signal to the output
      float waveValue = wave() * 0.5f;
      io.out(0) = waveValue;
      io.out(1) = -waveValue; // Phase inverted
    }
    // write the output buffers to the soundfile
    soundFile.write({io.outBuffer(0), io.outBuffer(1)}, io.framesPerBuffer());
  }
};

int main(int argc, char *argv[]) {
  MyApp app;
  app.start();
  return 0;
}
