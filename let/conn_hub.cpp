//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "conn_hub.h"

namespace let {
    void ConnectionHub::addConnection(evutil_socket_t fd) {
        auto buf_ev = bufferevent_socket_new(ev_base_, fd, 0);

        auto conn = std::make_shared<TcpConnection>(buf_ev);


        bufferevent_setcb(buf_ev, connectionReadCallback, connectionWriteCallback, connectionEventCallback, conn.get());

    }

    void ConnectionHub::connectionReadCallback(struct bufferevent *bev, void *ctx) {

    }
}