#include "StdPosix.h"

// C
#include <stdlib.h>

// STL
#include <stdexcept>


using namespace Std;

cond :: cond(mutex* mtx)
    : m_mtx(mtx)
    , m_cond()
{
    int ret = pthread_cond_init(&m_cond, NULL);

    if (ret != 0)
    {
        abort();
    }
}

cond :: ~cond() throw ()
{
    int ret = pthread_cond_destroy(&m_cond);

    if (ret != 0)
    {
        abort();
    }
}

void
cond :: lock()
{
    m_mtx->lock();
}

void
cond :: unlock()
{
    m_mtx->unlock();
}

void
cond :: wait()
{
    int ret = pthread_cond_wait(&m_cond, &m_mtx->m_mutex);

    if (ret != 0)
    {
        abort();
    }
}

void
cond :: signal()
{
    int ret = pthread_cond_signal(&m_cond);

    if (ret != 0)
    {
        abort();
    }
}

void
cond :: broadcast()
{
    int ret = pthread_cond_broadcast(&m_cond);

    if (ret != 0)
    {
        abort();
    }
}