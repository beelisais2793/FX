#include "StdPosix.h"


using namespace Std;

uint64_t
location :: hash(const location& loc)
{
    uint64_t ip_hash = ipaddr::hash(loc.address);
    return ip_hash ^ loc.port;
}

location :: location()
    : address()
    , port()
{
}

location :: location(const ipaddr& _address)
    : address(_address)
    , port()
{
}

location :: location(const ipaddr& _address, in_port_t _port)
    : address(_address)
    , port(_port)
{
}

location :: location(const sockaddr_in* sa)
    : address()
    , port()
{
    set(sa);
}

location :: location(const sockaddr_in6* sa)
    : address()
    , port()
{
    set(sa);
}

location :: location(const location& other)
    : address(other.address)
    , port(other.port)
{
}

location :: ~location() throw ()
{
}

void
location :: pack(struct sockaddr* addr, socklen_t* addrlen) const
{
    address.pack(addr, addrlen, port);
}

bool
location :: set(const char* addr)
{
    return address.set(addr);
}

bool
location :: set(const char* addr, in_port_t p)
{
    if (address.set(addr))
    {
        port = p;
        return true;
    }

    return false;
}

bool
location :: set(const sockaddr* sa, socklen_t salen)
{
    if (sa->sa_family == AF_INET && salen >= sizeof(sockaddr_in))
    {
        set(reinterpret_cast<const sockaddr_in*>(sa));
        return true;
    }
    else if (sa->sa_family == AF_INET6 && salen >= sizeof(sockaddr_in6))
    {
        set(reinterpret_cast<const sockaddr_in6*>(sa));
        return true;
    }
    else
    {
        return false;
    }
}

void
location :: set(const sockaddr_in* sa)
{
    address = ipaddr(sa->sin_addr);
    port = ntohs(sa->sin_port);
}

void
location :: set(const sockaddr_in6* sa)
{
    address = ipaddr(sa->sin6_addr);
    port = ntohs(sa->sin6_port);
}

location&
location :: operator = (const location& rhs)
{
    if (this != &rhs)
    {
        address = rhs.address;
        port = rhs.port;
    }

    return *this;
}

bool
location :: operator < (const location& rhs) const
{
    return compare(rhs) < 0;
}

bool
location :: operator <= (const location& rhs) const
{
    return compare(rhs) <= 0;
}

bool
location :: operator == (const location& rhs) const
{
    return compare(rhs) == 0;
}

bool
location :: operator != (const location& rhs) const
{
    return compare(rhs) != 0;
}

bool
location :: operator >= (const location& rhs) const
{
    return compare(rhs) >= 0;
}

bool
location :: operator > (const location& rhs) const
{
    return compare(rhs) > 0;
}

int
location :: compare(const location& rhs) const
{
    const location& lhs(*this);

    if (lhs.address < rhs.address)
    {
        return -1;
    }
    else if (lhs.address > rhs.address)
    {
        return 1;
    }

    return lhs.port - rhs.port;
}

std::ostream& operator << (std::ostream& lhs, const location& rhs)
{
    if (rhs.address.family() == AF_INET6)
    {
        lhs << "[" << rhs.address << "]:" << rhs.port;
    }
    else
    {
        lhs << rhs.address << ":" << rhs.port;
    }

    return lhs;
}