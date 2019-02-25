//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_ACCEPTOR_H
#define LET_ACCEPTOR_H

#include <event2/listener.h>
#include <event2/event.h>

#include <functional>
#include <vector>

#include "ip_addr.h"

namespace let
{
// Acceptor仅负责接收新连接
// todo accept失败了怎么办
class Acceptor
{
public:
  using NewConnectionCallback = std::function<void(evutil_socket_t sockfd, const IpAddress &)>;

  explicit Acceptor(const IpAddress &ipaddr);

  ~Acceptor();

  void listen();

  void setNewConnectionCallback(const NewConnectionCallback &callback);

private:
  static void handleAccept(struct evconnlistener *listener,
                                    evutil_socket_t fd,
                                    struct sockaddr *address,
                                    int socklen,
                                    void *ctx);

private:
  event_base *ev_base_;
  evconnlistener *listener_;

  NewConnectionCallback new_connect_cb_;
};
} // namespace let
#endif //LET_ACCEPTOR_H
