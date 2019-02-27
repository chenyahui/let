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
  IpAddress(const struct sockaddr_in6 &addr);
  IpAddress(const struct sockaddr *sa);


  const std::string &ip() const
  {
    return ip_;
  }

  int port() const
  {
    return port_;
  }

  std::string format() const;

  const struct sockaddr *getSockAddr() const
  {
    return (const struct sockaddr *)&addr_;
  }

  bool isIpv6() const{
    return is_ipv6_;
  }

private:
  std::string ip_;
  int port_;
  bool is_ipv6_ = false;

  union {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};
} // namespace let
#endif /* LET_IP_ADDR_H */
