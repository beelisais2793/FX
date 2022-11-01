#ifndef __SELECTOR__
#define __SELECTOR__

#include <sys/select.h>  // select
#include <arpa/inet.h>   // struct sockaddr_in
#include <cstring>       // linux
#include <cstdlib>       // linux
#include <cstdio>        // linux
#include <string>
#include "Cuttlebone/Log.hpp"

namespace cuttlebone {

struct Receiver {
  int fileDescriptor;

  void init(unsigned port, bool useLargeWindow = true) {

    if ((fileDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      perror("socket");
      exit(-1);
    }

    if (useLargeWindow) {
      // magic
      //
      int window = 16777216;
      if (setsockopt(fileDescriptor, SOL_SOCKET, SO_RCVBUF, &window,
                     sizeof(int)) == -1) {
        fprintf(stderr, "Error setting socket opts: %s\n", strerror(errno));
      }
      printf("%d byte receive buffer (aka \"window\")\n", window);
    }

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

    LOG("Receiver listening on port %d", port);
  }

  bool receive(void* buffer, unsigned packetSize, float timeOut) {
    fd_set fileDescriptorSet;
    FD_ZERO(&fileDescriptorSet);
    FD_SET(fileDescriptor, &fileDescriptorSet);

    int seconds = (int)timeOut;
    int microseconds = (timeOut - (int)timeOut) * 1000000;
    if (microseconds > 999999) microseconds = 999999;

    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;

    int rv = select(fileDescriptor + 1, &fileDescriptorSet, 0, 0, &tv);

    if (rv == -1) {
      LOG("select error %d", errno);
      return false;
    } else if (rv == 0) {
      return false;
    } else {
      int bytesReceived = recvfrom(fileDescriptor, buffer, packetSize, 0, 0, 0);
      if (bytesReceived == -1) {
        perror("recvfrom");
        return false;
      } else if (bytesReceived != (int)packetSize) {
        printf("Received less than expected\n");
        return false;
      } else {
        return true;
      }
    }
  }
};

}  // cuttlebone

#endif
