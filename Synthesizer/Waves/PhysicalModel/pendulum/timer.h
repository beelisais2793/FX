#ifndef TIMER_H
#define TIMER_H
#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::system_clock> then;
public:
    Timer();
    void Update();
    size_t Elapsed();
    ~Timer();
};
#endif // TIMER_H
