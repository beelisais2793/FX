#ifndef CUTTLEBONE_APPLOOP
#define CUTTLEBONE_APPLOOP

#include "Cuttlebone/Log.hpp"

namespace cuttlebone {

// the point of this class is to provide a Processing-like alternative to the
// standard c++ entry point, main. instead, we present an app where you can
// fill in 3 functions (i.e. setup, loop, and cleanup) that are guaranteed to
// be executed from the same thread that called main. why? stuff like GLUT
// requires it.
//
// TODO
// - handle quit/exit messages like control-c
//

class AppLoop {
  bool running; // = false;

 public:
  virtual void setup() = 0;
  virtual void loop() = 0;
  virtual void cleanup() = 0;

  AppLoop() : running(false) {}
  virtual ~AppLoop() {}

  void start() {
    // XXX - make an error or warning if this is not the main thread
    setup();
    running = true;
    while (running) loop();
    cleanup();
  }

  void stop(const char* message = "") {
    LOG("%s", message);
    running = false;
  }
};

}  // cuttlebone
#endif
