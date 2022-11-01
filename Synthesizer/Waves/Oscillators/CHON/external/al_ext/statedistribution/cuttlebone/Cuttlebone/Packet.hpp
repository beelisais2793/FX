#ifndef __PACKET__
#define __PACKET__

#include <string.h>  // for memcpy
#include <string>
#include <iostream>

#include "Cuttlebone/Log.hpp"

namespace cuttlebone {

template <unsigned SIZE>
struct Packet {
  struct Header {
    unsigned frameNumber, partNumber, partSize, totalPartCount;
  };
  enum {
    HEADER_SIZE = sizeof(Header),
    DATA_SIZE = SIZE - sizeof(Header),
  };
  Header header;
  char byte[DATA_SIZE];
};

template <typename STATE, typename PACKET>
struct PacketMaker {
  enum {
    // XXX what if sizeof STATE is an integer multiple of PACKET::DATA_SIZE
    TOTAL_PART_COUNT = 1 + sizeof(STATE) / PACKET::DATA_SIZE,
    LAST_DATA_SIZE = sizeof(STATE) % PACKET::DATA_SIZE
  };

  const STATE& state;
  unsigned frameNumber, partNumber;

  PacketMaker(const STATE& state, unsigned frameNumber)
      : state(state), frameNumber(frameNumber), partNumber(0) {}

  bool fill(PACKET& packet) {
    if (partNumber >= TOTAL_PART_COUNT) return false;

    packet.header.totalPartCount = TOTAL_PART_COUNT;
    packet.header.frameNumber = frameNumber;
    packet.header.partNumber = partNumber;
    if (partNumber == (TOTAL_PART_COUNT - 1))
      packet.header.partSize = LAST_DATA_SIZE;
    else
      packet.header.partSize = PACKET::DATA_SIZE;

    // XXX what if this fails? when would it fail? how?
    memcpy(packet.byte,
           (void*)(((unsigned long)&state) + (partNumber * PACKET::DATA_SIZE)),
           packet.header.partSize);

    partNumber++;
    return true;
  }
};

template <typename STATE, typename PACKET>
struct PacketTaker {
  enum {
    // XXX what if sizeof STATE is an integer multiple of PACKET::DATA_SIZE
    TOTAL_PART_COUNT = 1 + sizeof(STATE) / PACKET::DATA_SIZE,
    LAST_DATA_SIZE = sizeof(STATE) % PACKET::DATA_SIZE
  };

  STATE& state;
  unsigned char part[TOTAL_PART_COUNT];
  unsigned frameNumber;

  // XXX add a log for missing parts

  void summary() {
    unsigned missing = 0;
    for (unsigned i = 0; i < TOTAL_PART_COUNT; ++i)
      if (part[i] == 0) missing++;
    string report;
    report += to_string(missing);
    report += " missing from frame ";
    report += to_string(frameNumber);
    LOG("%s", report.c_str());
  }

  PacketTaker(STATE& state, unsigned frameNumber)
      : state(state), frameNumber(frameNumber) {
    for (unsigned i = 0; i < TOTAL_PART_COUNT; ++i) part[i] = 0;
  }

  bool take(PACKET& packet) {
    if (packet.header.frameNumber != frameNumber) return false;

    // XXX handle duplicate parts?

    memcpy((void*)(((unsigned long)&state) +
                   (packet.header.partNumber * PACKET::DATA_SIZE)),
           packet.byte, packet.header.partSize);

    part[packet.header.partNumber] = 1;
    return true;
  }

  // XXX find a better way to do this. faster, cleaner.
  bool isComplete() {
    for (unsigned i = 0; i < TOTAL_PART_COUNT; ++i)
      if (part[i] != 1) return false;
    return true;
  }
};

}  // cuttlebone

#endif
