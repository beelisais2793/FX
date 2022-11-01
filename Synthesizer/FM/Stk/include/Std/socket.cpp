#include "StdPosix.h"

using namespace Std;

socket :: socket()
    : fd(-1)
{
}

socket :: ~socket() throw ()
{
}

bool
socket :: reset(int domain, int type, int protocol)
{
    *this = ::socket(domain, type, protocol);
    return get() >= 0;
}

// On all platforms I checked, a sockaddr_in6 is larger than a
// sockaddr, which in turn is larger than a sockaddr_in.  As a
// result, we allocate the largest of the three and call it a
// sockaddr.

bool
socket :: bind(const ipaddr& addr, in_port_t port)
{
    sockaddr_in6 sa6;
    socklen_t salen = sizeof(sa6);
    sockaddr* sa = reinterpret_cast<sockaddr*>(&sa6);
    addr.pack(sa, &salen, port);
    return ::bind(get(), sa, salen) == 0;
}

bool
socket :: bind(const ipaddr& addr)
{
    return bind(addr, 0);
}

bool
socket :: bind(const location& loc)
{
    return bind(loc.address, loc.port);
}

bool
socket :: connect(const ipaddr& addr, in_port_t port)
{
    sockaddr_in6 sa6;
    socklen_t salen = sizeof(sa6);
    sockaddr* sa = reinterpret_cast<sockaddr*>(&sa6);
    addr.pack(sa, &salen, port);
    return ::connect(get(), sa, salen) == 0;
}

bool
socket :: connect(const location& loc)
{
    return connect(loc.address, loc.port);
}

bool
socket :: listen(int backlog)
{
    return ::listen(get(), backlog) == 0;
}

bool
socket :: accept(socket* newsock)
{
    newsock->close();
    int ret;

    if ((ret = ::accept(get(), NULL, NULL)) < 0)
    {
        return false;
    }

    *newsock = ret;
    return true;
}

bool
socket :: shutdown(int how)
{
    return ::shutdown(get(), how) == 0;
}

bool
socket :: getpeername(location* loc)
{
    sockaddr_in6 sa6;
    socklen_t salen = sizeof(sa6);
    sockaddr* sa = reinterpret_cast<sockaddr*>(&sa6);

    if (::getpeername(get(), sa, &salen) < 0)
    {
        return false;
    }

    return loc->set(sa, salen);
}

bool
socket :: getsockname(location* loc)
{
    sockaddr_in6 sa6;
    socklen_t salen = sizeof(sa6);
    sockaddr* sa = reinterpret_cast<sockaddr*>(&sa6);

    if (::getsockname(get(), sa, &salen) < 0)
    {
        return false;
    }

    return loc->set(sa, salen);
}

bool
socket :: set_sockopt(int level, int optname,
                                    const void *optval, socklen_t optlen)
{
    return setsockopt(get(), level, optname, optval, optlen) == 0;
}

bool
socket :: set_reuseaddr()
{
    int yes = 1;
    return set_sockopt(SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
}

bool
socket :: set_tcp_nodelay()
{
    int yes = 1;
    return set_sockopt(IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
}

bool
socket :: sndbuf(size_t size)
{
    return set_sockopt(SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

bool
socket :: rcvbuf(size_t size)
{
    return set_sockopt(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

bool
socket :: sndlowat(size_t size)
{
    return set_sockopt(SOL_SOCKET, SO_SNDLOWAT, &size, sizeof(size));
}

bool
socket :: rcvlowat(size_t size)
{
    return set_sockopt(SOL_SOCKET, SO_RCVLOWAT, &size, sizeof(size));
}

ssize_t
socket :: recv(void *buf, size_t len, int flags)
{
    return ::recv(get(), buf, len, flags);
}

ssize_t
socket :: xrecv(void *buf, size_t len, int flags)
{
    size_t rem = len;
    ssize_t amt = 0;

    while (rem > 0)
    {
        if ((amt = recv(buf, rem, flags)) < 0)
        {
            if (rem == len)
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

    return len - rem;
}

ssize_t
socket :: send(const void *buf, size_t len, int flags)
{
    return ::send(get(), buf, len, flags);
}

ssize_t
socket :: xsend(const void *buf, size_t len, int flags)
{
    size_t rem = len;
    ssize_t amt = 0;

    while (rem > 0)
    {
        if ((amt = send(buf, rem, flags)) < 0)
        {
            if (rem == len)
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

    return len - rem;
}

Std::socket& socket::operator = (int f)
{
    *dynamic_cast<fd*>(this) = f;
    return *this;
}