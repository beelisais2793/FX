#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lo/lo.h"
#include "common.hpp"

int main(int argc, char *argv[]) {
  char verstr[200];
  int  verstr_size;
  int  major;
  int  minor;
  char extra[200];
  int  extra_size;
  int  lt_major;
  int  lt_minor;
  int  lt_bug;

  lo_version(verstr, 200, &major, &minor, extra, 200, &lt_major, &lt_minor, &lt_bug);
  printf("verion: %s\n", verstr);

  Foo foo;
  foo.n = 0;
  lo_address t = lo_address_new_with_proto(LO_UDP, "224.0.0.1", "9999");
  while (true) {
    sprintf(foo.name, "%d", foo.n);
    lo_blob b = lo_blob_new(sizeof(Foo), &foo);
    if (lo_send(t, "/state", "b", b) == -1)
      printf("OSC error %d: %s\n", lo_address_errno(t), lo_address_errstr(t));
    lo_blob_free(b);
    foo.n++;
    usleep(1000000);
  }
  return 0;
}
