//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_CONN_HUB_H
#define LET_CONN_HUB_H

#include <event2/event.h>
#include <map>
#include <thread>

#include "tcp_conn.h"

namespace let {
    /**
     * A ConnectionHub is responsible of managing connections.
     * A ConnectionHub contains an event_loop, that all connections register on it.
     */
    class ConnectionHub {
    public:
        explicit ConnectionHub()
                : ev_base_(event_base_new()) {
        }

        void start() {
            thread_ = std::thread([=]() {
                event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
            });
        }

        void stop() {
            event_base_loopbreak(ev_base_);
            thread_.join();
        }

        void addConnection(evutil_socket_t fd, const std::string &ip_port);

    private:
        static void connectionReadCallback(struct bufferevent *bev, void *ctx);

        static void connectionWriteCallback(struct bufferevent *bev, void *ctx);

        static void connectionEventCallback(struct bufferevent *bev, short what, void *ctx);

    private:
        event_base *ev_base_;
        std::map<std::string, TcpConnectionPtr> connections_;

        std::thread thread_;
    };
}
#endif //LET_CONN_HUB_H
