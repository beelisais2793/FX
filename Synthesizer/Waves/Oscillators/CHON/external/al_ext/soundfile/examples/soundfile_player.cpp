#include <memory>

#include "al/app/al_App.hpp"
#include "al_ext/soundfile/al_SoundfileBuffered.hpp"

using namespace al;

// This example shows usage of the SoundFileBuffered class
// This class should be used when you need to stream a soundfile
// from disk in a flexible manner (with seek)

struct MyApp : public App {
  // SoundFileBuffered object
  SoundFileBuffered soundFile;
  // Buffer to read samples into
  std::unique_ptr<float[]> buffer;

  void onInit() {
    // Open sound file. Point this to an available soundfile in your system
    if (!soundFile.open("data/count.wav")) {
      std::cout << "**** ERROR: Soundfile not found. Quitting." << std::endl;
      quit();
      return;
    }

    // Instantiate buffer according to number of channels in file and frames per
    // buffer for sound card
    buffer = std::make_unique<float[]>(audioIO().framesPerBuffer() *
                                       soundFile.channels());
  }

  void onSound(AudioIOData &io) override {
    // Read from sound file into buffer
    soundFile.read(buffer.get(), io.framesPerBuffer());
    float *sfBuffer = buffer.get();
    while (io()) {
      io.out(0) = *sfBuffer;
      // If soundfile had two channels, you could read it with:
      //            io.out(1) = *(sfBuffer + 1);
      sfBuffer += soundFile.channels();  // advance pointer to next audio frame
    }

    // Alternatively to this, the sound file object can copy directly to the
    // output buffer if it is mono. This will overwrite anything already in the
    // buffer, though...
    //      soundFile.read(io.outBuffer(0), io.framesPerBuffer());
  }
};

int main(int argc, char *argv[]) {
  MyApp app;
  app.start();
  return 0;
}
