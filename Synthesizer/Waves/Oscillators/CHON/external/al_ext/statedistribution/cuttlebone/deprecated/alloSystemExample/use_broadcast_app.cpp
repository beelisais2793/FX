#include "Cuttlebone/al_BroadcastOmniApp.hpp"
using namespace al;

#include "Gamma/Oscillator.h"

#include <iostream>
using namespace std;

// Make a struct that encapsulates *all* your dynamic data. This needs to be
// POD (no pointers, etc).
//
#define N (60)
struct State {
  unsigned n;
  Pose pose;
  Vec3f position[N];
  float radius[N];
  float frequency[N];
};

// Inherit from BroadcastApp, handing in your State type as a template
// parameter. BroadcastApp has a few pure virtual methods that you must
// implement: onSimulatorInit, onSimulate, onRendererLocal, and onDraw.
//
struct MyBroadcastedStateApp : BroadcastApp<State> {

  // Member variables declared here are machine-local copies. They will not be
  // broadcast to rendering machines. If you want to send data to rendering
  // machines, put that data in your state struct. Only use these variables for
  // machine-local stuff.
  //
  Mesh m;
  gam::Sine<> sine[N];
  bool shouldReset;

  // This is a machine-local constructor. It will only initialize and do things
  // on the local machine.
  //
  MyBroadcastedStateApp() {
    shouldReset = false;
    cout << "my State is " << sizeof(State) << " bytes\n";

    addSphere(m);
    m.primitive(Graphics::TRIANGLES);
    m.generateNormals();
  }

  // This method will be called once on the simulation machine after the
  // constructor but before "things get going". It will not be called on any
  // rendering machines.
  //
  void onSimulatorInit(State& state) {

    nav().pos(0, 0, 10);
    state.pose = nav();

    state.n = 0;

    for (int i = 0; i < N; ++i) {
      state.position[i] = Vec3f(sin(i), i, 5 + -i * i / 7);
      state.frequency[i] = 220.0 + i * i;
      state.radius[i] = rnd::uniform(.3, 1.0);
    }
  }

  // This method will be called on the simulation machine, be it your laptop or
  // one of the sphere machines that is not responsible for graphics rendering.
  // In this method, you will do some sort of simulation (possibly reading from
  // your state), then *write* to the state variable. To be clear, when state
  // is passed into this method, it is identical to the last state you wrote.
  //
  void onSimulate(double dt, State& state) {
    LOG("simulate!");

    state.pose = nav();

    /*
    // you can check the keyboard state like this...
        if (keyboard().key('4') && keyboard().down()) {
          cout << "4 is down\n";
        } else {
          cout << "4 is up\n";
        }
      */

    // cout << state.n << endl;
    state.n++;

    if (shouldReset) {
      shouldReset = false;
      for (int i = 0; i < N; ++i) {
        state.position[i] = Vec3f(sin(i), i, 5 + -i * i / 7);
        state.frequency[i] = 220.0 + i * i;
        state.radius[i] = rnd::uniform(.3, 1.0);
      }
    }
    else
      for (int i = 0; i < N; ++i) {
        state.position[i] +=
            Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * 0.1;
        state.frequency[i] = 220.0 + state.position[i].mag() * 5;
      }
  }

  // This method will be called on a rendering machine, be it your laptop or
  // any/all of the AlloSphere rendering machines. Use this method to upsample,
  // decorate, filter, decompress or otherwise interpret your state data before
  // it gets drawn in onDraw.
  //
  void onRendererLocal(double dt, State& state, int popCount) {
    LOG("render!");
    // may want some measure of how behind we are?
    // -- db
    if (! popCount) {
      // make a guess at next state given some knowledge of the simulation
    }
    nav().set(state.pose);
  }

  // This method will be called on a rendering machine, be it your laptop or
  // any/all of the AlloSphere rendering machines. Use this method to draw your
  // state. Only do drawing because this method may be called *many* times.
  //
  void onDraw(Graphics& g, const State& state) {

    // Here we do not use our passed-in state directly, because we want to take
    // the opportunity to translate our state data into something drawable. If
    // we did that step here, we'd be doing it 12 times instead of just once.
    //
    for (int i = 0; i < N; ++i) {
      g.pushMatrix();
      g.color(HSV((float)i / N, 1, 1));
      g.translate(state.position[i]);
      g.scale(state.radius[i]);
      g.draw(m);
      g.popMatrix();
    }
  }

  virtual void onSound(AudioIOData& io, const State& state) {
    gam::Sync::master().spu(audioIO().fps());
    for (int i = 0; i < N; ++i) sine[i].freq(state.frequency[i]);

LOG("got here!");

    while (io()) {
      float s = 0;
      for (int i = 0; i < N; ++i)
        s += sine[i]() / (state.pose.pos() - state.position[i]).mag();
      io.out(0) = io.out(1) = s / N;
    }
  }

  virtual bool onKeyDown(const Keyboard& k) {
    shouldReset = (k.key() == ' ');
    return true;
  }

  virtual void myStart() {
    // this let's some internal threads run free. at one point, i was doing
    // this at the end of BroadcastApp's constructor but this lead to crashes
    // when those threads would access unallocated memory.
    //
    waitingToStart = false;
    start();
  }
};

int main() {
  MyBroadcastedStateApp* app;
  app = new MyBroadcastedStateApp;
  app->myStart();
}
