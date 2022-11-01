#include "StdPosix.h"

// C
#include <assert.h>
#include <stdlib.h>

using namespace Std;

mutex :: mutex()
    : m_mutex()
{
    int ret = pthread_mutex_init(&m_mutex, NULL);

    if (ret != 0)
    {
        abort();
    }
}

mutex :: ~mutex() throw ()
{
    int ret = pthread_mutex_destroy(&m_mutex);

    if (ret != 0)
    {
        abort();
    }
}

void
mutex :: lock()
{
    int ret = pthread_mutex_lock(&m_mutex);

    if (ret != 0)
    {
        abort();
    }
}

void
mutex :: unlock()
{
    int ret = pthread_mutex_unlock(&m_mutex);

    if (ret != 0)
    {
        abort();
    }
}

hold::hold(mutex* mtx)
    : m_held(false)
    , m_mtx(mtx)
{
    m_mtx->lock();
    m_held = true;
}

void
hold::release()
{
    assert(m_held);
    m_held = false;
    m_mtx->unlock();
}

hold::~hold() throw ()
{
    if (m_held)
    {
        release();
    }
}