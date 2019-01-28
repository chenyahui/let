//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_ACCEPTOR_H
#define LET_ACCEPTOR_H

#include <event2/listener.h>
#include <event2/event.h>
#include <vector>

#include "conn_hub.h"

namespace let
{
// Acceptor仅负责接收新连接
class Acceptor
{
  public:
    explicit Acceptor(const std::string &ipaddr);

    ~Acceptor();

    void start();

  private:
    void schedule(evutil_socket_t fd, struct sockaddr *address,
                  int socklen);

    static void newConnectionCallback(struct evconnlistener *listener,
                                      evutil_socket_t fd,
                                      struct sockaddr *address,
                                      int socklen,
                                      void *ctx);

  private:
    event_base *ev_base_;
    evconnlistener *listener_;

    std::vector<ConnectionHub *> conn_hubs_;
    std::size_t next_hub_ = 0;
};
} // namespace let
#endif //LET_ACCEPTOR_H
