#include "Cuttlebone/Cuttlebone.hpp"
using namespace cuttlebone;

struct App : AppLoop {
  virtual void setup() { LOG("setup()"); }
  virtual void loop() {
    LOG("loop()");
    stop("stopped from loop()");
  }
  virtual void cleanup() { LOG("cleanup()"); }
};

int main() { App().start(); }
