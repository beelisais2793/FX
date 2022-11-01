#ifndef CUTTLEBONE_TIMER__
#define CUTTLEBONE_TIMER__

namespace cuttlebone {

struct TimerImpl;
class Timer {
  TimerImpl* impl;

 public:
  Timer();
  virtual ~Timer();
  void start();
  void stop();
  void rate(double period);
  virtual void onTimer() = 0;
};

}  // cuttlebone
#endif  // __TIMER__
