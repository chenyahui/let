#ifndef LET_CONNECTOR_H
#define LET_CONNECTOR_H

#include <functional>

#include "ip_addr.h"
#include "event_loop.h"

namespace let
{
class Connector
{
public:
  typedef std::function<void(evutil_socket_t)> NewConnectionCallback;

  Connector(const IpAddress &remote_addr, EventLoop *event_loop);

  void connect();

  void setNewConnectionCallback(const NewConnectionCallback &callback);

  bufferevent *getBufferEvent();

private:
  static void handleEvent(struct bufferevent *bev, short events, void *ctx);

  IpAddress remote_addr_;
  EventLoop *event_loop_ = nullptr;

  NewConnectionCallback new_connect_cb_;

  bufferevent *buf_ev_ = nullptr;
};
} // namespace let
#endif