#ifndef __SELECTOR__
#define __SELECTOR__

#include <arpa/inet.h>  // struct sockaddr_in
#include <unistd.h>     // sleep()
#include <cstring>      // linux
#include <cstdlib>      // linux
#include <cstdio>       // linux
#include <string>
#include <thread>
#include <cassert>

namespace cuttlebone {

template <typename APP>
struct Selector {
  std::thread t;
  int packetSize, port;
  float timeOut;
  bool done;

  void start(unsigned packetSize, float timeOut, unsigned port) {
    done = false;
    this->packetSize = packetSize;
    this->timeOut = timeOut;
    this->port = port;
    t = std::thread(&Selector::init, this);
  }

  void init() {

    int fileDescriptor;
    if ((fileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      perror("socket");
      exit(-1);
    }

    // int broadcast = 1;
    // if (setsockopt(fileDescriptor, SOL_SOCKET, SO_BROADCAST, &broadcast,
    //               sizeof(broadcast)) == -1) {
    //  perror("setsockopt");
    //  exit(-1);
    //}

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    // we need the :: to disambiguate between std::bind (c++11) socket.h bind
    //
    if (::bind(fileDescriptor, (sockaddr*)&address, sizeof(sockaddr)) == -1) {
      perror("bind");
      exit(-1);
    }

    while (!done) {
      fd_set fileDescriptorSet;
      FD_ZERO(&fileDescriptorSet);
      FD_SET(fileDescriptor, &fileDescriptorSet);

      int seconds = (int)timeOut;
      int microseconds = (timeOut - (int)timeOut) * 1000000;
      if (microseconds > 999999) microseconds = 999999;

      struct timeval tv;  // = {0, timeOut};  // sec, usec
      tv.tv_sec = seconds;
      tv.tv_usec = microseconds;
      // printf("BEFORE: %ld, %ld\n", tv.tv_sec, tv.tv_usec);
      int rv = select(fileDescriptor + 1, &fileDescriptorSet, 0, 0, &tv);
      // printf("AFTER: %ld, %ld\n", tv.tv_sec, tv.tv_usec);

      // XXX so is tv now the actual time that select waited?

      if (rv == -1) {
        perror("select");
      } else if (rv == 0) {
        printf("Timeout! Who cares?\n");
      } else {
        int bytesReceived =
            recvfrom(fileDescriptor, static_cast<APP*>(this)->buffer,
                     packetSize, 0, 0, 0);
        if (bytesReceived == -1) {
          perror("recvfrom");
        } else if (bytesReceived != packetSize) {
          printf("Received less than expected\n");
        } else {
          static_cast<APP*>(this)->onNewBuffer();
        }
      }
    }
  }

  void stop() {
    done = true;
    t.join();
  }
};

}  // cuttlebone

#endif
