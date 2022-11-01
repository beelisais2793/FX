#include "StdPosix.h"
#include <time.h>

using namespace Std;

uint64_t
Std::time()
{
    return wallclock_time();
}

uint64_t
Std::monotonic_time()
{
    timespec ts;
    clockid_t ci;
#ifdef CLOCK_UPTIME
    ci = CLOCK_UPTIME;
#elif defined CLOCK_MONOTONIC_RAW
    ci = CLOCK_MONOTONIC_RAW;
#elif defined CLOCK_MONOTONIC
    ci = CLOCK_MONOTONIC;
#else
#error no known-working monotonic clock
#endif

    if (clock_gettime(ci, &ts) < 0)
    {
        return 0;
    }

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

uint64_t
Std::wallclock_time()
{
    timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
    {
        return 0;
    }

    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

void
Std::sleep(uint64_t x)
{
    struct timespec ts;
    ts.tv_sec = x / PO6_SECONDS;
    ts.tv_nsec = x % PO6_SECONDS;
    nanosleep(&ts, NULL);
}

