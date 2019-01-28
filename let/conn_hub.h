//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_CONN_HUB_H
#define LET_CONN_HUB_H

#include <event2/event.h>
#include <map>
#include <thread>

#include "tcp_conn.h"

namespace let
{
/**
     * A ConnectionHub is responsible of managing connections.
     * A ConnectionHub contains an event_loop, that all connections register on it.
     */
class ConnectionHub
{
  public:
    explicit ConnectionHub()
        : ev_base_(event_base_new())
    {
    }

    ~ConnectionHub();

    void start();

    void stop();

    // todo use std::move
    void addConnection(evutil_socket_t fd, const std::string &ip_port);

  private:
    event_base *ev_base_;
    std::map<std::string, TcpConnectionPtr> connections_;

    std::thread thread_;
};
} // namespace let
#endif //LET_CONN_HUB_H
