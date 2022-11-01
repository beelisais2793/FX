#include <memory>

#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"

using namespace al;

// OutputRecorder is a convenience class that records the output buffers
// You should use OutputRecorder unless you need to record specific channels
// Or you want to record audio in some point of the processing other than
// at the outputs (e.g. save audio without reverb)

struct MyApp : public App {
  OutputRecorder recorder;

  void onInit() override {
    if (audioIO().channelsIn() == 0) {
      std::cout << "**** ERROR: Could not open audio input. Quitting."
                << std::endl;
      quit();
      return;
    }
    // Open the sound file for writing. This will be a two channel soundfile
    // The default is to use 16-bit int WAV files.
    recorder.start("output.wav", audioIO().framesPerSecond(), 2);
    // Append recorder to audio IO object. This will run the recorder
    // after running the onSound() function below
    audioIO().append(recorder);
  }

  void onSound(AudioIOData &io) override {
    while (io()) {
      // Copy inputs to outputs
      io.out(0) = io.in(0) * 0.3;
      io.out(1) = io.in(1) * 0.3;
    }
    // Output will be recorded to the file after this
  }

  void onExit() override { recorder.close(); }
};

int main(int argc, char *argv[]) {
  MyApp app;
  app.start();
  return 0;
}
