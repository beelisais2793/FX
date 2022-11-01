#ifndef INCLUDE_APP_HPP
#define INCLUDE_APP_HPP

#include "Cuttlebone/AppLoop.hpp"
#include "Cuttlebone/Broadcaster.hpp"
#include "Cuttlebone/Stats.hpp"
#include "Cuttlebone/Log.hpp"
#include "Cuttlebone/Packet.hpp"
#include "Cuttlebone/Queue.hpp"
#include "Cuttlebone/Receiver.hpp"
#include "Cuttlebone/Time.hpp"
#include "Cuttlebone/Timer.hpp"

#include <thread>
using namespace std;

namespace cuttlebone {

template <typename STATE, unsigned PACKET_SIZE = 1400, unsigned PORT = 63059>
class Maker {
  const char* broadcastIp;
  bool done;
  bool waitingToStart;
  thread broadcast;
  Queue<STATE>* queue;

 public:
  bool shouldLog;
  virtual void set(STATE& state) { queue->push(state); }

  Maker(const char* broadcastIp = "127.0.0.1")
      : broadcastIp(broadcastIp),
        done(false),
        waitingToStart(true),
        queue(new Queue<STATE>),
        shouldLog(false) {}

  virtual void start() {
    broadcast = thread([&]() {
      Broadcaster broadcaster;
      broadcaster.init(PACKET_SIZE, broadcastIp, PORT, false);
      Packet<PACKET_SIZE> p;
      STATE* state = new STATE;
      int frame = 0;

      while (!done) {
        int popCount = 0;
        while (queue->pop(*state))
          popCount++;

        if (popCount) {
          if (shouldLog)
            LOG("sent packet %d", frame);
          PacketMaker<STATE, Packet<PACKET_SIZE> > packetMaker(*state, frame);

          // XXX consider making this multithreaded. the timing should be measured, at least.
          while (packetMaker.fill(p))
            broadcaster.send((unsigned char*)&p);
          frame++;
        }
      }

      delete state;
    });
  }

  virtual void stop() {
    done = true;
    broadcast.join();
  }
};

template <typename STATE, unsigned PACKET_SIZE = 1400, unsigned PORT = 63059>
class Taker {
  bool done;
  bool waitingToStart;
  thread receive;
  Queue<STATE>* queue;

 public:
  bool shouldLog;

  virtual int get(STATE& state) {
    int popCount = 0;
    while (queue->pop(state)) popCount++;
    return popCount;
  }

  Taker() : done(false), waitingToStart(true), queue(new Queue<STATE>), shouldLog(false) {}

  virtual void start() {
    receive = thread([&]() {
      Receiver receiver;
      receiver.init(PORT, false);
      Packet<PACKET_SIZE> p;
      STATE* localState = new STATE;

      while (!done) {

        // XXX make this timeout a parameter
        if (!receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f))
          continue;

      ABORT_FRAME:
        ;
        // wait until we're at the begining of a frame
        // XXX is this really necessary? reconsider.
        if (p.header.partNumber != 0)
          continue;

        PacketTaker<STATE, Packet<PACKET_SIZE> > packetTaker(
            *localState, p.header.frameNumber);

        packetTaker.take(p);

        while (!packetTaker.isComplete()) {
          // XXX make this timeout a parameter
          if (receiver.receive((unsigned char*)&p, PACKET_SIZE, 0.2f)) {

            // XXX the semantics of this method are not clear. make clear.
            if (!packetTaker.take(p)) {
              // got a part from an unexpected frame before we finished this
              // frame
              LOG("ABORT FRAME");
              packetTaker.summary();
              goto ABORT_FRAME;
            }
          }
        }

        if (shouldLog)
          LOG("got packet %d", p.header.frameNumber);

        queue->push(*localState);
      }

      delete localState;
    });

    waitingToStart = false;
  }

  virtual void stop() {
    done = true;
    receive.join();
  }
};

template <typename STATE, unsigned PACKET_SIZE = 1400, unsigned PORT = 63059,
          unsigned WAIT_TIME_US = 1000>
class MakerApp : public AppLoop {
  Maker<STATE, PACKET_SIZE, PORT> maker;
  STATE* state;
  // XXX might want this later
  // STATE* previousState;

  virtual void setup() {
    // XXX should this go in the constructor?
    //
    state = new STATE;

    // XXX need to think about the order of events here
    //
    init(*state);

    // XXX should we push/send/set the initial state right now?
    //
    maker.start();
  }

  virtual void loop() {
    update(*state);
    maker.set(*state);
    usleep(WAIT_TIME_US);
  }

  virtual void cleanup() { maker.stop(); }

 public:
  virtual void init(STATE& state) = 0;
  virtual void update(STATE& state) = 0;
};

template <typename STATE, unsigned PACKET_SIZE = 1400, unsigned PORT = 63059,
          unsigned WAIT_TIME_US = 1000>
class TakerApp : public AppLoop {
  Taker<STATE, PACKET_SIZE, PORT> taker;
  STATE* state;

  virtual void setup() {
    state = new STATE;
    // XXX ordering?
    started();
    taker.start();
  }

  virtual void loop() {
    int popCount = taker.get(*state);
    // XXX maybe only call got if popCount > 0
    got(*state, 1.0, popCount);
    usleep(WAIT_TIME_US);
  }

  virtual void cleanup() { taker.stop(); }

 public:
  virtual void started() = 0;
  virtual void got(STATE& state, double dt, int popCount) = 0;
};

}  // cuttlebone

#endif
