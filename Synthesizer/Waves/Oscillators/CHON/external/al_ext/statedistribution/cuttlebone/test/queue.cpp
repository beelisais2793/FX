#include "Cuttlebone/Queue.hpp"

#include <unistd.h>  // usleep
#include <iostream>  // cout
#include <thread>
#include <string.h>
using namespace std;

using namespace cuttlebone;

//#include <cstddef>
//#include <string>
//#include <stdlib.h>

// must be a POD type
//
struct State {
  unsigned frame;
  char data[256];
  void zero() { memset(this, 0, sizeof(State)); }
};

int main(int argc, char* argv[]) {
  Queue<State> simulateBroadcast, receiveRender;
  thread simulate, broadcast, receive, render;

  unsigned simulateBroadcastWasFull = 0;
  unsigned simulateBroadcastWasEmpty = 0;
  unsigned receiveRenderWasFull = 0;
  unsigned receiveRenderWasEmpty = 0;

  bool waitingToStart = true, done = false;

  simulate = thread([&]() {
    State s;
    s.zero();
    while (waitingToStart)
      usleep(1000);
    while (!done) {
      sprintf(s.data, "%u", s.frame);
      if (simulateBroadcast.push(s))
        s.frame++;
      else
        simulateBroadcastWasFull++;
      usleep(16666);
    }
  });

  broadcast = thread([&]() {
    State s;
    while (waitingToStart)
      usleep(1000);
    while (!done) {
      if (simulateBroadcast.pop(s)) {
        // cut s into pieces and broadcast
      } else
        simulateBroadcastWasEmpty++;
      usleep(16666);
    }
  });

  receive = thread([&]() {
    State s;
    while (waitingToStart)
      usleep(1000);
    while (!done) {
      // select with a small timeout
      // receive lots of little pieces of s and assemble
      // indicate when we lost a piece
      // when s is ready, then push
      if (!receiveRender.push(s))
        receiveRenderWasFull++;
      usleep(1000);
    }
  });

  render = thread([&]() {
    State s;
    while (waitingToStart)
      usleep(1000);
    while (!done) {
      if (receiveRender.pop(s)) {
        // make a pretty picture with s
      } else
        receiveRenderWasEmpty++;
      usleep(16666);
    }
  });

  waitingToStart = false;
  getchar();
  done = true;
  simulate.join();
  broadcast.join();
  receive.join();
  render.join();

  cout << "simulateBroadcastWasFull:" << simulateBroadcastWasFull << endl;
  cout << "simulateBroadcastWasEmpty:" << simulateBroadcastWasEmpty << endl;
  cout << "receiveRenderWasFull:" << receiveRenderWasFull << endl;
  cout << "receiveRenderWasEmpty:" << receiveRenderWasEmpty << endl;
}
