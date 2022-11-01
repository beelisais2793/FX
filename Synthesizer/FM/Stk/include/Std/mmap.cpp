#include "StdPosix.h"

#include <errno.h>
#include <sys/mman.h>

using namespace Std;

mmap :: mmap(void* addr, size_t length,
                          int prot, int flags,
                          int fd, off_t offset)
    : m_base(NULL)
    , m_length(length)
    , m_error(0)
{
    m_base = ::mmap(addr, length, prot, flags, fd, offset);

    if (m_base == MAP_FAILED)
    {
        m_base = NULL;
        m_error = errno;
    }
}

mmap :: ~mmap() throw ()
{
    close();
}


void
mmap :: close()
{
    if (m_base)
    {
        munmap(m_base, m_length);
    }
}

