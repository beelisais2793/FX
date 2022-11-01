//#define LOG_FILE ("/tmp/log.txt")
#include "Cuttlebone/App.hpp"
#include <cstring>  // memset
#include <string>
#include <sstream>
#include <cmath>

#define SK (0.01f)
#define NK (0.1f)
#define D (0.97)
//.01, .1, .765

ConfigurationData configurationData[] = {
    {"00", "192.168.7.255", true, false, false, false},
    {"image", "", false, false, true, false},
    {"quux.zzz", "192.168.2.255", true, false, false, false},
    {"quux.local", "192.168.2.255", true, false, false, false},
    {"quux.cf", "192.168.7.255", true, false, false, false}, };

#include "BlobState.hpp"

float r() { return 2.0f * rand() / RAND_MAX - 1.0f; }

struct MyApp : App<State> {
  MyApp() {
    // App<State>::shouldLog = true;
    configuration = Configuration(configurationData);
    configuration.log();
    LOG("State is %d bytes", sizeof(State));
  }

  float d, sk, nk;

  State original;
  Vertex velocity[N];

  vector<vector<unsigned short> > neighbor;

  virtual void onSimulatorInit(State& state) {
    memset(&state, 0, sizeof(state));

    vector<unsigned short> tri, lin;
    load(ICOSPHERE_FILE, state, tri, lin, neighbor);
    memcpy(&original, &state, sizeof(State));
    memset(&velocity, 0, sizeof(Vertex) * N);
  }

  virtual void onSimulate(float dt, State& state) {
    static float period = 0;
    static int simCount = 0;
    if (period > 1.0) {
      period -= 1.0;
      LOG("Sim: %d", simCount);
      simCount = 0;
    }
    period += dt;
    simCount++;

    static int n = 0;

    if ((n % 300) == 0) {
      Vertex v{r(), r(), r()};
      v *= 4.0f;
      int randomVertex = rand() % N;
      state.position[randomVertex] += v;
      v *= 0.7f;
      for (auto n : neighbor[randomVertex]) state.position[n] += v;
      LOG("poke!");
    }
    n++;

    static float time = 0;
    state.x = cos(time / 5) * 1.3;
    time += dt;

    for (int i = 0; i < N; i++) {
      Vertex& v = state.position[i];
      Vertex force = (v - original.position[i]) * -SK;

      for (int k = 0; k < neighbor[i].size(); k++) {
        Vertex& n = state.position[neighbor[i][k]];
        force += (v - n) * -NK;
      }

      velocity[i] += force;
      velocity[i] *= D;
    }

    for (int i = 0; i < N; i++) state.position[i] += velocity[i];
  }

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
