#include "StdPosix.h"

// C
#include <assert.h>
#include <stdlib.h>

using namespace Std;

rwlock :: rwlock()
    : m_rwlock()
{
    int ret = pthread_rwlock_init(&m_rwlock, NULL);

    if (ret != 0)
    {
        abort();
    }
}

rwlock :: ~rwlock() throw ()
{
    int ret = pthread_rwlock_destroy(&m_rwlock);

    if (ret != 0)
    {
        abort();
    }
}

void
rwlock :: rdlock()
{
    int ret = pthread_rwlock_rdlock(&m_rwlock);

    if (ret != 0)
    {
        abort();
    }
}

void
rwlock :: wrlock()
{
    int ret = pthread_rwlock_wrlock(&m_rwlock);

    if (ret != 0)
    {
        abort();
    }
}

void
rwlock :: unlock()
{
    int ret = pthread_rwlock_unlock(&m_rwlock);

    if (ret != 0)
    {
        abort();
    }
}

rdhold:: rdhold(rwlock* rwl)
    : m_held(false)
    , m_rwl(rwl)
{
    m_rwl->rdlock();
    m_held = true;
}

void
rdhold:: release()
{
    assert(m_held);
    m_held = false;
    m_rwl->unlock();
}

rdhold:: ~rdhold() throw ()
{
    if (m_held)
    {
        release();
    }
}

wrhold:: wrhold(rwlock* rwl)
    : m_held(false)
    , m_rwl(rwl)
{
    m_rwl->wrlock();
    m_held = true;
}

void
wrhold:: release()
{
    assert(m_held);
    m_held = false;
    m_rwl->unlock();
}

wrhold:: ~wrhold() throw ()
{
    if (m_held)
    {
        release();
    }
}