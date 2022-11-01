#include "StdPosix.h"

// C
#include <assert.h>
#include <stdlib.h>


using namespace Std;

thread :: thread(function func)
    : m_started(false)
    , m_joined(false)
    , m_func(func)
    , m_thread()
{
}

thread :: ~thread() throw ()
{
    if (m_started && !m_joined)
    {
        abort();
    }
}

void
thread :: start()
{
    assert(!m_started);
    int ret = pthread_create(&m_thread, NULL, thread::start_routine, &m_func);

    if (ret != 0)
    {
        abort();
    }

    m_started = true;
}

void
thread :: join()
{
    assert(m_started && !m_joined);
    int ret = pthread_join(m_thread, NULL);

    if (ret != 0)
    {
        abort();
    }

    m_joined = true;
}

void*
thread :: start_routine(void * arg)
{
    function* f;
    f = static_cast<function*>(arg);
    (*f)();
    return NULL;
}