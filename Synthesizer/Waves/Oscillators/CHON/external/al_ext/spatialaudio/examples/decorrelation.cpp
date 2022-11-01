/*
  Decorrelation example
  by: Andres Cabrera
*/

#include <iostream>

#include "al/app/al_AudioDomain.hpp"
#include "al/app/al_ConsoleDomain.hpp"

#include "al_ext/spatialaudio/al_Decorrelation.hpp"

#include "Gamma/Envelope.h"
#include "Gamma/Noise.h"

/* This example will generate an exponentially decaying noise burst and
 * then produce 8 decorrelated copies of it using random phase all-pass
 * filters. It uses the Kendall method to produce the IRs, that consists
 * of generating a random phase spectrum and then doing an inverse FFT
 * to obtain the IR for that random phase filter.
 * In this example, every 3 bursts of noise, a new set of decorrelation IRs
 * is produced changing the maxjump parameter. As the maxjump parameter
 * decreases, the correlation between the IRs increases as there will be less
 * variation in the randomness of the phase, and the sound will become
 * "narrower".
 */

using namespace al;

class MyApp {
 public:
  // The app will run an "AudioDomain"
  AudioDomain audioDomain;
  ConsoleDomain consoleDomain;

  float mMaxjump{M_PI};
  int mCounter;
  int mCounterTarget;

  // The decorrelation IR size is set to 1024
  Decorrelation decorrelation{1024};

  gam::NoisePink<> noise;
  gam::Decay<> env{48000};

  // The decorrelation object is initialized in the constructor

  // The input channel is set to -1 to indicate parallel or "many to many" mode
  // The number of channels is set to 8
  // And the last parameter "inputsAreBuses" is set to true to indicate
  // that inputs are buses instead of hardware input channels
  MyApp() {
    // 8 audio outputs
    audioDomain.configure(44100, 512, 8, 0);
    // Define routing. Key to map are inputs, map value are the channel
    // indeces where decorrelated copies of the input are sent.
    map<uint32_t, vector<uint32_t>> routingMap = {
        {0, {0, 1, 2, 3, 4, 5, 6, 7}}};

    decorrelation.configure(audioDomain.audioIO().framesPerBuffer(), routingMap,
                            true, 1000, mMaxjump);

    // Counter to force a generation of new decorrelation IRs every three
    // seconds
    mCounterTarget = 3 * audioDomain.audioIO().framesPerSecond() /
                     audioDomain.audioIO().framesPerBuffer();
    mCounter = mCounterTarget;
  }

  // This start function starts the audio domain, waits for 3 seconds and
  // then exits
  void start() {
    audioDomain.init();
    consoleDomain.init();
    // Set audio callback through a lambda
    audioDomain.onSound =
        std::bind(&MyApp::onSound, this, std::placeholders::_1);

    consoleDomain.onLine = [&](std::string line) { return false; };

    // Set sample rate of Gamma from rate configured in audio domain
    gam::sampleRate(audioDomain.audioIO().framesPerSecond());
    audioDomain.audioIO().print();
    // start audio domain. This domain is non blocking, so we will keep the
    // application alive by starting the console domain
    audioDomain.start();

    std::cout << "Press enter to quit." << std::endl;
    consoleDomain.start();  // Console Domain is a blocking domain

    // stop and cleanup domains
    audioDomain.stop();
    consoleDomain.stop();
    audioDomain.cleanup();
    consoleDomain.cleanup();
  }

  // Audio callback
  void onSound(AudioIOData& io) {
    if (++mCounter >=
        mCounterTarget) {  // Check if it's time to generate new IRs.
      map<uint32_t, vector<uint32_t>> routingMap = {
          {0, {0, 1, 2, 3, 4, 5, 6, 7}}};
      decorrelation.configure(audioDomain.audioIO().framesPerBuffer(),
                              routingMap, true, 1000, mMaxjump);
      std::cout << "Max jump set to: " << mMaxjump << std::endl;
      mMaxjump *= 0.5;
      if (mMaxjump < 0.05) {
        mMaxjump = M_PI;
      }  // Wrap max jump
      mCounter = 0;
    }
    auto inBuffer = decorrelation.getInputBuffer(0);
    while (io()) {
      *inBuffer++ = noise() * env();  // Generate noise bursts into input buffer
      if (env.done()) {
        env.reset();
      }  // Retrigger envelope when done
    }
    decorrelation.processBuffer();
    for (int i = 0; i < 8; i++) {
      auto outputBuffer = decorrelation.getOutputBuffer(i);
      memcpy(io.outBuffer(i), outputBuffer, io.framesPerBuffer());
    }
    // Decorrelation is performed without the need to add anything here
    // as the decorrelation object was "appended" to the AudioIO object.
  }
};

int main() {
  MyApp app;
  app.start();
  //	std::cout << "Press enter to quit" << std::endl;
  //	app.beginAudio();
  //	getchar();
  //	app.endAudio();
  return 0;
}
