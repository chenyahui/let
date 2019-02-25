#ifndef LET_IP_ADDR_H
#define LET_IP_ADDR_H

#include <string>
#include <sys/socket.h>

namespace let
{
class IpAddress
{
public:
  IpAddress(const std::string &ip, int port)
      : ip_(ip),
        port_(port)
  {
  }

  IpAddress(struct sockaddr *address, int socklen);

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
    return ip_ + std::to_string(port_);
  }

  const struct sockaddr* getSockAddr(){
    return (struct sockaddr*)&addr_;
  }
private:
  std::string ip_;
  int port_;

  union
  {
    struct sockaddr_in addr_;
    struct sockaddr_in6 addr6_;
  };
};
} // namespace let
#endif /* LET_IP_ADDR_H */
