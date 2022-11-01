#include "timer.h"

Timer::Timer() {
    Update();
}

Timer::~Timer() {}

size_t Timer::Elapsed() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds> (now - then).count();
}

void Timer::Update() {
    then = std::chrono::system_clock::now();
}
