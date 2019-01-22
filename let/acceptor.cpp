//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include "acceptor.h"

namespace let {
    void Acceptor::newConnectionCallback(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address,
                                         int socklen, void *ctx) {

    }
}