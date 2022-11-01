#include "StdPosix.h"

using namespace Std;

barrier :: barrier(uint64_t count)
    : m_lock()
    , m_cv(&m_lock)
    , m_height(count)
    , m_level(0)
    , m_generation(0)
{
}

barrier :: ~barrier() throw ()
{
}

bool
barrier :: wait()
{
    int cancelstate;
    uint64_t gen;
    hold hold(&m_lock);
    gen = m_generation;
    ++m_level;

    if (m_level == m_height)
    {
        m_level = 0;
        ++m_generation;
        m_cv.broadcast();
        return true;
    }
    else
    {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancelstate);

        while (m_generation == gen)
        {
            m_cv.wait();
        }

        pthread_setcancelstate(cancelstate, NULL);
        return false;
    }
}