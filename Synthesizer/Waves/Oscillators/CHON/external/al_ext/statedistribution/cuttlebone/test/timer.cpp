#define LOG_SIMPLE
#include "Cuttlebone/Timer.hpp"
#include "Cuttlebone/Log.hpp"
#include <cstdio>
#include <cstdlib>

using namespace cuttlebone;

unsigned char* buffer;

struct App : public Timer {
  void onTimer() { printf("this happened\n"); }
};

int main(int argc, char* argv[]) {
  float timerPeriod = 0.2f;
  if (argc > 1) timerPeriod = atof(argv[1]);
  printf("usage: %s timerPeriod\n", argv[0]);
  App app;
  app.rate(timerPeriod);
  app.start();
  getchar();
}
