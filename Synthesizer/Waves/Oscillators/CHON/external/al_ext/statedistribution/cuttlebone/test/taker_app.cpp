// run this along program along side test/maker_app.cpp
//

#include "Cuttlebone/Cuttlebone.hpp"
using namespace cuttlebone;

#include <unistd.h>  // usleep

// this definition must agree with the definition in the Maker's program.
//
struct State {
  int data[100];
};

struct App : TakerApp<State> {

  virtual void started() { LOG("started()"); }

  virtual void got(State& state, double dt, int popCount) {

    // do nothing if we didn't get a new state
    //
    if (!popCount) return;

    // do something with this new state
    //
    LOG("got() state.data[0] = %d popCount = %d", state.data[0], popCount);
  }
};

int main() { App().start(); }
