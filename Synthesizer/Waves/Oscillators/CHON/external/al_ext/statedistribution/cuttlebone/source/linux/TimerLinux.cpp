#include "Cuttlebone/Timer.hpp"
#include "Cuttlebone/Log.hpp"
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

namespace cuttlebone {

void callbackFunction(union sigval argument);

struct TimerImpl {
  Timer* timer;
  double period;
  struct sigevent linuxSigEvent;
  struct itimerspec linuxTimerSpec;
  timer_t linuxTimer;

  TimerImpl(Timer* timer) : timer(timer), period(1.0 / 60) {
    memset(&linuxSigEvent, 0, sizeof(struct sigevent));
    memset(&linuxTimerSpec, 0, sizeof(struct itimerspec));
  }

  void stop() { timer_delete(linuxTimer); }

  void start() {
    linuxSigEvent.sigev_notify = SIGEV_THREAD;
    linuxSigEvent.sigev_notify_function = callbackFunction;
    linuxSigEvent.sigev_value.sival_ptr = (void*)timer;

    timer_create(CLOCK_REALTIME, &linuxSigEvent, &linuxTimer);
    // timer_create(CLOCK_MONOTONIC, &linuxSigEvent, &linuxTimer);

    int seconds = (int)period;
    int nanoseconds = (period - (int)period) * 1000000000;
    if (nanoseconds > 999999999) nanoseconds = 999999999;

    linuxTimerSpec.it_value.tv_sec = seconds;
    linuxTimerSpec.it_value.tv_nsec = nanoseconds;
    linuxTimerSpec.it_interval.tv_sec = linuxTimerSpec.it_value.tv_sec;
    linuxTimerSpec.it_interval.tv_nsec = linuxTimerSpec.it_value.tv_nsec;

    timer_settime(linuxTimer, 0, &linuxTimerSpec, NULL);
  }
};

void callbackFunction(union sigval argument) {
  ((Timer*)(argument.sival_ptr))->onTimer();
}

Timer::Timer() : impl(new TimerImpl(this)) {}
Timer::~Timer() { delete impl; }
void Timer::rate(double period) { impl->period = period; }
void Timer::start() { impl->start(); }
void Timer::stop() { impl->stop(); }

}  // cuttlebone
