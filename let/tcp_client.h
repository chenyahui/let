//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_TCP_CLIENT_H
#define LET_TCP_CLIENT_H

#include "ip_addr.h"
#include "io_thread.h"
#include "connector.h"

namespace let
{
class TcpClient
{
public:
  TcpClient(const IpAddress &remote_ip_addr, IoThread *io_thread);

  void connect();

  void start();

  void stop();

  void setMessageCallback(const MessageCallback &);

  void setConnectionCallback(const ConnectionCallback &);

  void setCloseCallback(const CloseCallback &);

  void setErrorCallback(const ErrorCallback &);

private:
  static void handleEvent(struct bufferevent *bev, short events, void *ctx);

  void newConnection(evutil_socket_t fd);

private:
  IoThread *io_thread_;
  IpAddress remote_addr_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  ErrorCallback error_cb_;

  Connector connector_;
};
} // namespace let
#endif //LET_TCP_CLIENT_H
