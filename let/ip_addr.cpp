#include <strings.h> // bzero
#include <arpa/inet.h>
#include <event2/util.h>

#include "ip_addr.h"
#include "logger.h"

namespace let
{
IpAddress::IpAddress(int port)
    : port_(port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = INADDR_ANY;
    addr_.sin_port = htons(port);
}

IpAddress::IpAddress(const std::string &ip, int port)
    : ip_(ip),
      port_(port)
{
    int socket_len = sizeof(addr6_);

    auto format_ip_port = format();

    if (evutil_parse_sockaddr_port(format_ip_port.c_str(), (struct sockaddr *)&addr6_, &socket_len) < 0)
    {
        LOG_ERROR << "address parse error: " << format();
    }

    is_ipv6_ = addr6_.sin6_family == AF_INET6;
}

IpAddress::IpAddress(const struct sockaddr_in6 &addr)
    : IpAddress((const struct sockaddr *)&addr)
{
}

IpAddress::IpAddress(const struct sockaddr *sa)
{
    char b[128];
    const char *res = NULL;

    is_ipv6_ = sa->sa_family == AF_INET6;
    
    if (sa->sa_family == AF_INET)
    {
        const struct sockaddr_in *sin = (const struct sockaddr_in *)sa;
        addr_ = *sin;

        res = evutil_inet_ntop(AF_INET, &sin->sin_addr, b, sizeof(b));
        port_ = ntohs(sin->sin_port);
    }

    if (sa->sa_family == AF_INET6)
    {
        const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)sa;
        addr6_ = *sin6;

        res = evutil_inet_ntop(AF_INET6, &sin6->sin6_addr, b, sizeof(b));
        port_ = ntohs(sin6->sin6_port);
    }

    if (!res)
    {
        LOG_ERROR << "evutil_inet_ntop ipv4 error";
    }
    else
    {
        ip_ = std::string(b);
    }
}

std::string IpAddress::format() const
{
    // todo ipv6
    return ip_ + ":" + std::to_string(port_);
}
} // namespace let