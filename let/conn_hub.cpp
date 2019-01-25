//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "conn_hub.h"

namespace let {
    void ConnectionHub::addConnection(evutil_socket_t fd, const std::string &ip_port) {
        auto buf_ev = bufferevent_socket_new(ev_base_, fd, 0);

        auto conn = std::make_shared<TcpConnection>(buf_ev);
        connections_.insert(std::make_pair(ip_port, conn));

    }

    void ConnectionHub::start() {
        thread_ = std::thread([=]() {
            event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
        });
    }

    void ConnectionHub::stop() {
        event_base_loopbreak(ev_base_);
        thread_.join();
    }

    ConnectionHub::~ConnectionHub() {
        stop();
    }
}