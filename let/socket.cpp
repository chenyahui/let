//
// Created by yahuichen(陈亚辉) on 2019/1/25.
//

#include <arpa/inet.h>

#include "socket.h"

namespace let
{
std::string sockaddr_to_ip_port(const struct sockaddr *addr)
{
    char buf[64] = "";

    if (addr->sa_family == AF_INET)
    {
        const struct sockaddr_in *addr4 = (sockaddr_in *)(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(sizeof(buf)));
    }
    else if (addr->sa_family == AF_INET6)
    {
        const struct sockaddr_in6 *addr6 = (sockaddr_in6 *)(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(sizeof(buf)));
    }

    return {buf};
}
} // namespace let