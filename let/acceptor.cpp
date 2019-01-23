//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include "acceptor.h"

namespace let {
    void Acceptor::newConnectionCallback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address,
                                         int socklen, void *ctx) {

        auto acceptor = (Acceptor *) ctx;
        acceptor->conn_hubs_[0]->addConnection(fd);
    }

    evutil_socket_t Acceptor::makeListenSocket(const let::IpAddress &ip_addr) {
        return 0;
    }
}