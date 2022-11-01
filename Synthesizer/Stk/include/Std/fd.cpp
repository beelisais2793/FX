#include "StdPosix.h"

using namespace Std;

fd :: fd()
    : m_fd(-1)
{
}

fd :: fd(int f)
    : m_fd(f)
{
}

fd :: ~fd() throw ()
{
    close();
}

void
fd :: close()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
    }

    m_fd = -1;
}

ssize_t
fd :: read(void *buf, size_t nbytes)
{
    return ::read(m_fd, buf, nbytes);
}

ssize_t
fd :: xread(void* buf, size_t nbytes)
{
    size_t rem = nbytes;
    ssize_t amt = 0;

    while (rem > 0)
    {
        if ((amt = read(buf, rem)) < 0)
        {
            if (rem == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        }
        else if (amt == 0)
        {
            break;
        }

        rem -= amt;
        buf = static_cast<char*>(buf) + amt;
    }

    return nbytes - rem;
}

ssize_t
fd :: write(const void *buf, size_t nbytes)
{
    return ::write(m_fd, buf, nbytes);
}

ssize_t
fd :: xwrite(const void *buf, size_t nbytes)
{
    size_t rem = nbytes;
    ssize_t amt = 0;

    while (rem > 0)
    {
        if ((amt = write(buf, rem)) < 0)
        {
            if (rem == nbytes)
            {
                return -1;
            }
            else
            {
                break;
            }
        }
        else if (amt == 0)
        {
            break;
        }

        rem -= amt;
        buf = static_cast<const char*>(buf) + amt;
    }

    return nbytes - rem;
}

bool
fd :: set_nonblocking()
{
    long flags = O_NONBLOCK;
    return fcntl(get(), F_SETFL, flags) >= 0;
}

void
fd :: swap(fd* other) throw ()
{
    int tmp = this->m_fd;
    this->m_fd = other->m_fd;
    other->m_fd = tmp;
}

fd&
fd :: operator = (int f)
{
    close();
    m_fd = f;
    return *this;
}