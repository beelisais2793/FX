#include "StdPosix.h"

using namespace Std;

// C
#include <stdio.h>
#include <string.h>

// POSIX
#include <netdb.h>

// STL
#include <sstream>


class protect_addrinfo
{
    public:
        protect_addrinfo(addrinfo* ai) : m_ai(ai) {}
        ~protect_addrinfo() throw () { if (m_ai) { freeaddrinfo(m_ai); } }

    private:
        addrinfo* m_ai;
        protect_addrinfo(const protect_addrinfo&);
        protect_addrinfo& operator = (const protect_addrinfo&);
};

hostname :: hostname()
    : address()
    , port()
{
}

hostname :: hostname(const char* _hostname, in_port_t _port)
    : address(_hostname)
    , port(_port)
{
}

hostname :: hostname(const location& loc)
    : address()
    , port(loc.port)
{
    std::ostringstream ostr;
    ostr << loc.address;
    address = ostr.str();
}

hostname :: hostname(const hostname& other)
    : address(other.address)
    , port(other.port)
{
}

hostname :: ~hostname() throw ()
{
}

location hostname :: connect(int domain, int type, int protocol, socket* sock) const
{
    // Convert the port to a C string
    char port_cstr[6];
    snprintf(port_cstr, 6, "%u", port);

    // Setup the hints
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
    hints.ai_family = domain;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    // Make the call
    addrinfo* res = NULL;
    int gai_err = getaddrinfo(address.c_str(), port_cstr, &hints, &res);

    if (gai_err)
    {
        return location();
    }

    protect_addrinfo pai(res);
    location loc;

    for (addrinfo* ai = res; ai; ai = ai->ai_next)
    {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
        {
            errno = ECONNREFUSED;
            continue;
        }

        if (!sock->reset(ai->ai_family, ai->ai_socktype, ai->ai_protocol))
        {
            continue;
        }

        assert(sock->get() >= 0);

        if (::connect(sock->get(), ai->ai_addr, ai->ai_addrlen) < 0)
        {
            sock->close();
            continue;
        }

        if (loc.set(ai->ai_addr, ai->ai_addrlen))
        {
            break;
        }

        loc = location();
    }

    if (sock->get() < 0)
    {
        return location();
    }

    return loc;
}

location hostname :: lookup(int type, int protocol) const
{
    // Convert the port to a C string
    char port_cstr[6];
    snprintf(port_cstr, 6, "%u", port);

    // Setup the hints
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_ADDRCONFIG | AI_NUMERICSERV;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_protocol = protocol;

    // Make the call
    addrinfo* res = NULL;
    int gai_err = getaddrinfo(address.c_str(), port_cstr, &hints, &res);

    if (gai_err)
    {
        return location();
    }

    protect_addrinfo pai(res);
    location loc;

    for (addrinfo* ai = res; ai; ai = ai->ai_next)
    {
        if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
        {
            errno = ECONNREFUSED;
            continue;
        }

        if (loc.set(ai->ai_addr, ai->ai_addrlen))
        {
            break;
        }

        loc = location();
    }

    return loc;
}

bool
hostname :: operator < (const hostname& rhs) const
{
    return compare(rhs) < 0;
}

bool
hostname :: operator <= (const hostname& rhs) const
{
    return compare(rhs) <= 0;
}

bool
hostname :: operator == (const hostname& rhs) const
{
    return compare(rhs) == 0;
}

bool
hostname :: operator != (const hostname& rhs) const
{
    return compare(rhs) != 0;
}

bool
hostname :: operator >= (const hostname& rhs) const
{
    return compare(rhs) >= 0;
}

bool
hostname :: operator > (const hostname& rhs) const
{
    return compare(rhs) > 0;
}

int
hostname :: compare(const hostname& rhs) const
{
    if (address < rhs.address)
    {
        return -1;
    }

    if (address > rhs.address)
    {
        return 1;
    }

    if (port < rhs.port)
    {
        return -1;
    }

    if (port > rhs.port)
    {
        return -1;
    }

    return 0;
}

std::ostream&
operator << (std::ostream& lhs, const hostname& rhs)
{
    if (rhs.address.find(':') != std::string::npos)
    {
        lhs << "[" << rhs.address << "]:" << rhs.port;
    }
    else
    {
        lhs << rhs.address << ":" << rhs.port;
    }

    return lhs;
}