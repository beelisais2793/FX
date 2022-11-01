#include "Cuttlebone/Time.hpp"
#include <iostream>  // cout, endl
#include <unistd.h>  // usleep
using namespace std;

using namespace cuttlebone;

int main() {
  Timestamp<> t;
  cout << t.stamp() << endl;
  Stopwatch<> s;
  for (int i = 0; i < 100; ++i) {
    s.tic();
    usleep(1000);
    cout << s.toc() << endl;
  }
}
