//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include <event2/util.h>

#include "acceptor.h"
#include "logger.h"

namespace let {
    void Acceptor::newConnectionCallback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address,
                                         int socklen, void *ctx) {

        auto acceptor = (Acceptor *) ctx;
        acceptor->conn_hubs_[0]->addConnection(fd);
    }

    evutil_socket_t Acceptor::makeListenSocket(const let::IpAddress &ip_addr) {
        evutil_socket_t socket_fd = ::socket(ip_addr.toIpV4(), SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                                             IPPROTO_TCP);;
        if (socket_fd < 0) {
            LOG_FATAL << "create socket failed";
        }

        if (evutil_make_socket_nonblocking(socket_fd) < 0) {
            LOG_FATAL << "make socket nonblocking failed";
        }

        return 0;
    }
}