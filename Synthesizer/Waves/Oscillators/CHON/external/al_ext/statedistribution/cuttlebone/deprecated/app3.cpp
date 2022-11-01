//#define LOG_FILE ("/tmp/log.txt")
#include "Cuttlebone/App.hpp"
#include <cstring>  // memset
#include <string>
#include <sstream>
#include <cmath>

//ConfigurationData configurationData[] = {
//    {"00", "192.168.7.255", true, false, false, false},
//    {"image", "", false, false, true, false},
//    {"quux.zzz", "192.168.2.255", true, false, false, false},
//    {"quux.local", "192.168.2.255", true, false, false, false},
//    {"quux.cf", "192.168.7.255", true, false, false, false}, };

struct State {
  int data[100];
};

struct MyApp : App<State> {
  MyApp() {
    App<State>::shouldLog = true;
//    configuration = Configuration(configurationData);
    configuration.log();
    LOG("State is %d bytes", sizeof(State));
  }

  virtual void onSimulatorInit(State& state) {
    memset(&state, 0, sizeof(state));
  }

  virtual void onSimulate(float dt, State& state) { LOG("onSimulate"); }

  virtual void onRendererLocal(float dt, State& state, int popCount) {
    LOG("onRendererLocal(%f)", dt);
  }

  virtual void onDraw(Graphics& g, const State& state) {}
  virtual void onSound(AudioIOData& io, const State& state) {}
};

int main() {
  LOG("main()");
  MyApp app;
  app.start();
}
