#include <sys/socket.h>
#include <cstring>

#include "connector.h"
#include "logger.h"
#include "util.h"

namespace let
{
Connector::Connector(EventLoop *event_loop, const IpAddress &remote_addr)
    : event_loop_(event_loop), remote_addr_(remote_addr)
{
}

bool Connector::connect()
{
    auto sock = create_noblocking_socket();
    
    if (sock < 0)
    {
        LOG_ERROR << "create socket error";
        return false;
    }

    buf_ev_ = bufferevent_socket_new(event_loop_->getEvBase(), sock, BEV_OPT_CLOSE_ON_FREE);

    if (!buf_ev_)
    {
        evutil_closesocket(sock);
        LOG_ERROR << "create bufferevent error";
        return false;
    }

    bufferevent_setcb(buf_ev_, nullptr, nullptr, handleEvent, this);

    auto sock_addr = remote_addr_.getSockAddr();

    int sock_len = remote_addr_.isIpv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
    
    if (bufferevent_socket_connect(buf_ev_,
                                   (struct sockaddr *)sock_addr,
                                   sock_len) < 0)
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
        if (self->new_connect_cb_)
        {
            self->new_connect_cb_(bufferevent_getfd(bev));
        }
    }
    else if (events & BEV_EVENT_ERROR)
    {
        LOG_ERROR << "connector connect error, erron[" << errno << "]: " << strerror(errno);
    }
}

const bufferevent *Connector::getBufferEvent() const
{
    return buf_ev_;
}

void Connector::setNewConnectionCallback(const NewConnectionCallback &cb)
{
    new_connect_cb_ = cb;
}
} // namespace let