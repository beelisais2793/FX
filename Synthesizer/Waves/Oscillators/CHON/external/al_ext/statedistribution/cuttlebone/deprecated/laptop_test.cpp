#include "Cuttlebone/Packet.hpp"
#include "Cuttlebone/Checksum.hpp"
#include "Cuttlebone/Time.hpp"
#include "Cuttlebone/Receiver.hpp"
#include "Cuttlebone/Broadcaster.hpp"
#include "Cuttlebone/Queue.hpp"

#include <unistd.h>  // usleep
#include <iostream>  // cout
#include <thread>
#include <string.h>
using namespace std;

#define PACKET_SIZE (1400)

// must be a POD type
//
struct State {
  unsigned n;
  char data[190000];
  void zero() { memset(this, 0, sizeof(State)); }
};

int main(int argc, char* argv[]) {
  thread simulate, broadcast, receive, render;

  Queue<State> simulateBroadcast, receiveRender;

  unsigned simulateBroadcastWasFull = 0;
  unsigned simulateBroadcastWasEmpty = 0;
  unsigned receiveRenderWasFull = 0;
  unsigned receiveRenderWasEmpty = 0;

  bool waitingToStart = true, done = false;

  simulate = thread([&]() {
    Timestamp<> t;
    // Checksum c;
    State state;
    state.zero();
    while (waitingToStart)
      usleep(1000);

    while (!done) {
      sprintf(state.data, "%lf | %u", t.stamp(), state.n);
      // c.checksum((unsigned char*)&state, sizeof(State));
      // c.print();
      // printf("\n");
      if (simulateBroadcast.push(state)) {
        state.n++;
      } else
        simulateBroadcastWasFull++;
      usleep(16666);
    }
  });

  broadcast = thread([&]() {
    // Stopwatch<> stopwatch;
    Broadcaster broadcaster;
    broadcaster.init(PACKET_SIZE, "192.168.1.255", 8888);
    // broadcaster.init(PACKET_SIZE, "127.0.0.1", 8888);
    Packet<PACKET_SIZE> p;
    State state;
    int frame = 0;
    while (waitingToStart)
      usleep(1000);

    while (!done) {
      if (simulateBroadcast.pop(state)) {  // XXX while() for greed

        // stopwatch.tic();
        PacketMaker<State, Packet<PACKET_SIZE> > packetMaker(state, frame);
        while (packetMaker.fill(p))
          broadcaster.send((unsigned char*)&p);
        // printf("frame %u took %f seconds to broadcast\n", frame,
        // stopwatch.toc());
        frame++;

      } else {
        simulateBroadcastWasEmpty++;
        usleep(1000);
      }
    }
  });

  receive = thread([&]() {
    // Stopwatch<> stopwatch;
    Receiver receiver;
    receiver.init(8888);
    Packet<PACKET_SIZE> p;
    State state;
    while (waitingToStart)
      usleep(1000);

    while (!done) {
      /*
            if (p.header.frameNumber > 0)
              printf("frame %u took %f seconds to receive\n",
         p.header.frameNumber,
                     stopwatch.toc());
              */
      if (!receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f)) {
        usleep(1000);
        continue;
      }

      // wait until we're at the begining of a frame
      if (p.header.partNumber != 0)
        continue;

      // stopwatch.tic();

      PacketTaker<State, Packet<PACKET_SIZE> > packetTaker(
          state, p.header.frameNumber);

      packetTaker.take(p);

      while (!packetTaker.isComplete()) {
        if (receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f)) {
          if (!packetTaker.take(p)) {
            // got a part from an unexpected frame before we finished this frame
            printf("lost frame\n");
            goto ABORT_FRAME;
          }
        } else {
          usleep(1000);
        }
      }

      // we're all done, try to push
      if (!receiveRender.push(state))
        receiveRenderWasFull++;
    ABORT_FRAME:
      ;
    }
  });

  render = thread([&]() {
    Timestamp<> t;
    // Checksum c;
    State state;
    while (waitingToStart)
      usleep(1000);

    while (!done) {
      while (receiveRender.pop(state))
        ;
      // if (receiveRender.pop(state)) {

      double delta = t.stamp() - atof(state.data);
      printf("%s | R | %lf\n", state.data, delta);

      // printf("%s | R | %lf\n", state.data, t.stamp());

      // c.checksum((unsigned char*)&state, sizeof(State));
      // c.print();
      // printf("\n");

      //} else
      //  receiveRenderWasEmpty++;
      usleep(16665);
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
