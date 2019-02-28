//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_TCP_CLIENT_H
#define LET_TCP_CLIENT_H

#include "ip_addr.h"
#include "event_loop.h"
#include "connector.h"
#include "callback.h"

namespace let
{
class TcpClient
{
public:
  TcpClient(EventLoop *event_loop, const IpAddress &remote_addr);

  void connect();

  void start();

  void stop();

  void setMessageCallback(const MessageCallback &);

  void setConnectionCallback(const ConnectionCallback &);

  void setDisconnectionCallback(const DisconnectionCallback &);

  void setErrorCallback(const ErrorCallback &);

private:
  static void handleEvent(struct bufferevent *bev, short events, void *ctx);

  void newConnection(evutil_socket_t fd);

private:
  EventLoop *event_loop_;
  IpAddress remote_addr_;
  Connector connector_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  DisconnectionCallback disconnection_cb_;
  ErrorCallback error_cb_;

};
} // namespace let
#endif //LET_TCP_CLIENT_H
