#include "Cuttlebone/Timer.hpp"
#include <stdlib.h>
#include <dispatch/dispatch.h>

namespace cuttlebone {

Timer* that;
static void func(void*);

struct TimerImpl {
  double period;
  dispatch_source_t timer;

  TimerImpl(Timer* timer) : period(1.0 / 60) { that = timer; }

  void stop() { dispatch_source_cancel(timer); }

  void start() {
    uint64_t interval = period * 1000000000;
    uint64_t leyway = 1000ULL;
    dispatch_queue_t queue =
        dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
    // this only works on Mavericks (OSX 10.9)
    // timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0,
    // DISPATCH_TIMER_STRICT, queue);
    if (!timer) exit(-1);
    dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, interval, leyway);
    dispatch_source_set_event_handler_f(timer, func);
    dispatch_resume(timer);
  }
};

Timer::Timer() : impl(new TimerImpl(this)) {}
Timer::~Timer() { delete impl; }
void Timer::rate(double period) { impl->period = period; }
void Timer::start() { impl->start(); }
void Timer::stop() { impl->stop(); }

static void func(void*) { that->onTimer(); }

}  // cuttlebone
