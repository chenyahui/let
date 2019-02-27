#include <sys/socket.h>
#include <cstring>

#include "connector.h"
#include "logger.h"

namespace let
{
bool Connector::connect()
{
    buf_ev_ = bufferevent_socket_new(event_loop_->getEvBase(), -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(buf_ev_, nullptr, nullptr, handleEvent, nullptr);

    auto sock_addr = remote_addr_.getSockAddr();
    if (bufferevent_socket_connect(buf_ev_,
                                   (struct sockaddr *)sock_addr,
                                   sizeof(struct sockaddr_in)) < 0)
    {
        bufferevent_free(buf_ev_);
        return false;
    }
    return true;
}

void Connector::handleEvent(struct bufferevent *bev, short events, void *ctx)
{
    auto self = (Connector *)ctx;
    if (events & BEV_EVENT_CONNECTED)
    {
        /* We're connected to 127.0.0.1:8080.   Ordinarily we'd do
            something here, like start reading or writing. */
        // 1. new tcp conn
        // 2. bind to io threads
        self->new_connect_cb_(bufferevent_getfd(bev));
    }
    else if (events & BEV_EVENT_ERROR)
    {
        LOG_ERROR << "connector accept error, erron[" << errno << "]: " << strerror(errno);
    }
}
} // namespace let