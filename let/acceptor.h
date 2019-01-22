//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_ACCEPTOR_H
#define LET_ACCEPTOR_H

#include <event2/listener.h>
#include <event2/event.h>
#include <vector>

#include "addr.h"
#include "conn_hub.h"

namespace let {
    class Acceptor {
    public:
        Acceptor(const IpAddress &ip_addr)
                : ev_base_(event_base_new()),
                  listener_(evconnlistener_new(
                          ev_base_,
                          NULL,
                          this,
                          LEV_OPT_CLOSE_ON_FREE,
                          -1,
                          0
                  )) {

        }

        static void newConnectionCallback(struct evconnlistener *listener,
                                          evutil_socket_t fd,
                                          struct sockaddr *address,
                                          int socklen,
                                          void *ctx);

    private:
        event_base *ev_base_;
        evconnlistener *listener_;
    };
}
#endif //LET_ACCEPTOR_H
