//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_TCP_CLIENT_H
#define LET_TCP_CLIENT_H

#include "ip_addr.h"
#include "io_thread.h"

namespace let
{
class TcpClient
{
public:
  TcpClient(const IpAddress &remote_ip_addr, IoThread *io_thread);

  void connect();

  void setMessageCallback(const MessageCallback &);

  void setConnectionCallback(const ConnectionCallback &);

  void setCloseCallback(const CloseCallback &);

  void setErrorCallback(const ErrorCallback &);

private:
  IoThread *io_thread_;
  IpAddress remote_ip_addr_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  ErrorCallback error_cb_;
};
} // namespace let
#endif //LET_TCP_CLIENT_H
