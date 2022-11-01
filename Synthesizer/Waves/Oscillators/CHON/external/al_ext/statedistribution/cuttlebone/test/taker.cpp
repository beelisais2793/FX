//#define LOG_FILE ("/tmp/log.txt")
#include "Cuttlebone/Cuttlebone.hpp"

using namespace cuttlebone;

struct State {
  int frame;
  char _[8 * 1024 * 1024 - 4];
};

int main() {
  Taker<State, 64336> taker;
  //Taker<State, 9184> taker;
  taker.shouldLog = true;
  State* state = new State;
  taker.start();
printf("%d bytes\n", sizeof(State));
  while (true) {
    int popCount = taker.get(*state);
    if (popCount) LOG("got %d", state->frame);
  }
}
