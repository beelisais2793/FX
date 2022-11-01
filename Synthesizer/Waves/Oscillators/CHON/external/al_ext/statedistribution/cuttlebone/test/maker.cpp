//#define LOG_FILE ("/tmp/log.txt")
#include "Cuttlebone/Cuttlebone.hpp"

using namespace cuttlebone;

#include <unistd.h>  // usleep

struct State {
  int frame;
  char _[8 * 1024 * 1024 - 4];
};

int main() {
  Maker<State, 64336> maker("192.168.10.255");
  //Maker<State, 9184> maker("192.168.10.255");
  maker.shouldLog = true;
  State* state = new State;
  state->frame = 1;
  maker.start();
printf("%d bytes\n", sizeof(State));
  while (true) {
    maker.set(*state);
    state->frame++;
    usleep(50000);
  }
}
