//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_ACCEPTOR_H
#define LET_ACCEPTOR_H

#include <event2/listener.h>
#include <event2/event.h>
#include <vector>

#include "io_thread.h"
#include "ip_addr.h"
#include "callback.h"

namespace let
{
// Acceptor仅负责接收新连接
class Acceptor
{
public:
  explicit Acceptor(const IpAddress &ipaddr);

  ~Acceptor();

  void start();

  void setConnectionCallback(const ConnectionCallback& callback);
private:

  static void newConnectionCallback(struct evconnlistener *listener,
                                    evutil_socket_t fd,
                                    struct sockaddr *address,
                                    int socklen,
                                    void *ctx);

private:
  event_base *ev_base_;
  evconnlistener *listener_;

  std::vector<IoThread *> io_threads_;
  std::size_t next_ = 0;
  ConnectionCallback connect_cb_;
};
} // namespace let
#endif //LET_ACCEPTOR_H
