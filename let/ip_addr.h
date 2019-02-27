#ifndef LET_IP_ADDR_H
#define LET_IP_ADDR_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

namespace let
{
class IpAddress
{
public:
  IpAddress(int port);
  IpAddress(const std::string &ip, int port);
  IpAddress(const struct sockaddr_in &addr);

  const std::string &ip() const
  {
    return ip_;
  }

  int port() const
  {
    return port_;
  }

  std::string format() const
  {
    return ip_ + ":" + std::to_string(port_);
  }

  struct sockaddr_in *getSockAddrIn() const
  {
    return (struct sockaddr_in *)&addr_;
  }

  struct sockaddr *getSockAddr() const
  {
    return (struct sockaddr *)&addr_;
  }

private:
  std::string ip_;
  int port_;

  struct sockaddr addr_;
  int socket_len_;
};
} // namespace let
#endif /* LET_IP_ADDR_H */
