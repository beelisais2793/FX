#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // IPPROTO_UDP
#include <string.h> // memset
#include <cstdlib>
#include <cstdio>

#define LOG_FILE ("recv.txt")
#include "Cuttlebone/Log.hpp"

using namespace cuttlebone;

#define N (2048)

int main() {
  int s = 0;
  
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) <= 0) {
    fprintf(stderr, "socket()\n");
    exit(1);
  }

  // magic
  //
  int window = 16777216;
  if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &window, sizeof(int)) == -1) {
    fprintf(stderr, "Error setting socket opts: %s\n", strerror(errno));
  }
  printf("%d byte receive buffer (aka \"window\")\n", window);

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi("10101"));
  server.sin_addr.s_addr = INADDR_ANY;

  if (::bind(s, (struct sockaddr *)&server , sizeof(server)) < 0) {
    fprintf(stderr, "bind()\n");
    exit(1);
  }

  int buffer[N];
  memset(buffer, 0, sizeof(buffer));

  struct iovec iov;
  iov.iov_base = buffer;
  iov.iov_len = N * 4;

  struct msghdr header;
  memset(&header, 0, sizeof(header));
  header.msg_namelen = sizeof(struct sockaddr);
  header.msg_iov = &iov;
  header.msg_iovlen = 1;

  LOG("started...");
  ssize_t bytes = 0;
  int last = -1;
  while (true) {
    if ((bytes = recvmsg(s, &header, 0)) < 0) {
    //if ((bytes = recvmsg(s, &header, MSG_WAITALL)) < 0) {
      fprintf(stderr, "recvmsg()\n");
      exit(1);
    }
    else {
      if (bytes != (N * 4)) {
        LOG("truncation (got %d bytes)", bytes);
      }
      else {
        LOG("got message %d (missed %d)", buffer[0], buffer[0] - last - 1);
        last = buffer[0];
      }
    }
  }
}
