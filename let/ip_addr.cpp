#include <strings.h> // bzero
#include <arpa/inet.h>
#include <event2/util.h>

#include "ip_addr.h"
#include "logger.h"

namespace let
{
IpAddress::IpAddress(int port)
    : IpAddress("0.0.0.0", port)
{
}

IpAddress::IpAddress(const std::string &ip, int port)
    : ip_(ip),
      port_(port)
{
    int socket_len = 0;
    std::string format_ip = format();
    LOG_DEBUG << "format ip" << format_ip;
    if (evutil_parse_sockaddr_port(format_ip.c_str(), (struct sockaddr *)&addr_, &socket_len) < 0)
    {
        LOG_ERROR << "address parse error: " << format_ip;
    }
}

IpAddress::IpAddress(const struct sockaddr_in &addr)
    : addr_(addr)
{
}
} // namespace let