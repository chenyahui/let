#ifndef LET_IP_ADDR_H
#define LET_IP_ADDR_H

#include <string>

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

private:
  std::string ip_;
  int port_;
};
} // namespace let
#endif /* LET_IP_ADDR_H */
