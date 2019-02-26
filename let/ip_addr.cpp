#include <strings.h> // bzero
#include <arpa/inet.h>

#include "ip_addr.h"
#include "logger.h"

namespace let
{
IpAddress::IpAddress(int port)
    : IpAddress("127.0.0.1", port)
{
}

IpAddress::IpAddress(const std::string &ip, int port)
    : ip_(ip),
      port_(port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_port = htobe16(port);

    if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)
    {
        LOG_ERROR << "inet_pton error: " << ip << ":" << port;
    }
}

IpAddress::IpAddress(const struct sockaddr_in &addr)
    : addr_(addr)
{
}

} // namespace let