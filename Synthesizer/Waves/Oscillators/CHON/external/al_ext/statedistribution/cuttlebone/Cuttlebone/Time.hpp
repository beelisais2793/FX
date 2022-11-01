#ifndef __MY_TIME__
#define __MY_TIME__

#include <chrono>
using namespace std;
using namespace std::chrono;

namespace cuttlebone {

template <typename CLOCK = high_resolution_clock>
struct Timestamp {
  static time_point<CLOCK>& born() {
    static time_point<CLOCK> born = CLOCK::now();
    return born;
  }
  inline double stamp() {
    return (duration_cast<duration<double>>(CLOCK::now() - born())).count();
  }
  inline double operator()() { return stamp(); }
};

template <typename CLOCK = high_resolution_clock>
struct Stopwatch {
  time_point<CLOCK> start, end;
  void tic() { start = CLOCK::now(); }
  inline double toc() {
    end = CLOCK::now();
    duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
  }
};

} // cuttlebone
#endif
