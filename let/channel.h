#ifndef LET_CHANNEL_H
#define LET_CHANNEL_H
#include "ip_addr.h"
namespace let
{

// channel的定位是什么：
// 1. 负责管理连接，这些连接可能是单连接、可能是集群、也有可能是连接池

class ChannelBase
{
  public:
    virtual const IpAddress &getAddr() = 0;
};


} // namespace let
#endif