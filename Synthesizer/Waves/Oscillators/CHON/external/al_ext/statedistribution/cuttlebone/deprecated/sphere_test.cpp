#include "Cuttlebone/Configuration.hpp"
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
/*

BROKEN
#define PACKET_SIZE (1400)

// must be a POD type
//
struct State {
  unsigned n;
  char data[100000];
  void zero() { memset(this, 0, sizeof(State)); }
};

struct Simulator {

  void onSimulate(State& state) {
    //
    // XXX - do something interesting
    //
    usleep(16666);
  }

  void run() {
    Queue<State> simulateBroadcast;
    bool waitingToStart = true, done = false;

    thread simulate([&]() {
      Timestamp<> t;
      //Checksum c;
      State state;
      state.zero();
      unsigned frame = 0;
      while (waitingToStart)
        usleep(100);

      while (!done) {
        onSimulate(state);

        sprintf(state.data, "%lf | %u", t.stamp(), frame);
        printf("%s | ", state.data);
        //c.checksum((unsigned char*)&state, sizeof(State));
        //c.print();
        printf("\n");

        simulateBroadcast.push(state);

        frame++;
      }
    });

    thread broadcast([&]() {
      Broadcaster broadcaster;
      broadcaster.init(PACKET_SIZE, "192.168.0.255", 8888);
      Packet<PACKET_SIZE> p;
      State state;
      int frame = 0;
      while (waitingToStart)
        usleep(100);

      while (!done) {
        bool hadAny = false;
        //while (simulateBroadcast.pop(state))
        if (simulateBroadcast.pop(state))
          hadAny = true;

        if (hadAny) {
          PacketMaker<State, Packet<PACKET_SIZE> > packetMaker(state, frame);
          while (packetMaker.fill(p))
            broadcaster.send((unsigned char*)&p);
          frame++;
        } else
          usleep(100);
      }
    });

    waitingToStart = false;
    getchar();
    done = true;
    simulate.join();
    broadcast.join();
  }
};

struct Renderer {
  void onRender(State& state) {
    //
    // XXX - do something interesting
    //
    usleep(16666);
  }

  void run() {
    Queue<State> receiveRender;
    bool waitingToStart = true, done = false;

    thread receive([&]() {
      Receiver receiver;
      receiver.init(8888);
      Packet<PACKET_SIZE> p;
      State state;
      while (waitingToStart)
        usleep(100);

      while (!done) {

        if (!receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f)) {
          usleep(100);
          continue;
        }

        // wait until we're at the begining of a frame
        if (p.header.partNumber != 0)
          continue;

        PacketTaker<State, Packet<PACKET_SIZE> > packetTaker(
            state, p.header.frameNumber);

        packetTaker.take(p);

        while (!packetTaker.isComplete()) {
          if (receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f)) {
            if (!packetTaker.take(p)) {
              // got a part from an unexpected frame before we finished this
              // frame
              printf("lost frame\n");
              goto ABORT_FRAME;
            }
          } else
            usleep(100);
        }

        // we're all done, try to push
        receiveRender.push(state);
      ABORT_FRAME:
        ;
      }
    });

    thread render([&]() {
      Timestamp<> t;
      //Checksum c;
      State state;
      while (waitingToStart)
        usleep(100);

      while (!done) {
        bool hadAny = false;
        //while (receiveRender.pop(state))
        if (receiveRender.pop(state))
          hadAny = true;

        if (hadAny) {
          double delta = t.stamp() - atof(state.data);
          printf("%s | %lf | ", state.data, delta);
          //c.checksum((unsigned char*)&state, sizeof(State));
          //c.print();
          printf("\n");

          onRender(state);
        } else
          usleep(100);
      }
    });

    waitingToStart = false;
    getchar();
    done = true;
    receive.join();
    render.join();
  }
};

const char* configuration[] = {"nonce", "gr01", "gr02", "gr03", "gr04", "gr05", "gr06", "gr07", "gr08", "gr09", "gr10", "gr12", "gr13", "gr14", };

int main(int argc, char* argv[]) {
  HostRole hostRole;
  hostRole.init(configuration);
  if (hostRole.isBroadcaster) {
    Simulator s;
    s.run();
  } else {
    Renderer r;
    r.run();
  }
}
*/
int main() {}
