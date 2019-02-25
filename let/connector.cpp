#include <sys/socket.h>

#include "connector.h"

namespace let
{
void Connector::connect()
{
    auto bev = bufferevent_socket_new(io_thread_->getEvBase(), -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, nullptr, nullptr, handleEvent, nullptr);

    auto sock_addr = remote_addr_.getSockAddr();
    if (bufferevent_socket_connect(bev,
            sock_addr, sizeof(struct sockaddr_in)) < 0) {
            bufferevent_free(bev);
    }
}

void Connector::handleEvent(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_CONNECTED) {
         /* We're connected to 127.0.0.1:8080.   Ordinarily we'd do
            something here, like start reading or writing. */
        // 1. new tcp conn
        // 2. bind to io threads
        new_connect_cb_(bufferevent_getfd(bev));
    } else if (events & BEV_EVENT_ERROR) {
        
    }
}
} // namespace let