//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include <event2/util.h>
#include <cstring>

#include "acceptor.h"
#include "logger.h"
#include "ip_addr.h"

namespace let
{

Acceptor::Acceptor(EventLoop* loop, const IpAddress &ip_addr)
    : loop_(loop)
{

    auto listen_on_addr = ip_addr.getSockAddr();

    LOG_DEBUG << "ip addr is ipv6: " << ip_addr.isIpv6() << ", " << ip_addr.format();

    int sock_len = ip_addr.isIpv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);

    listener_ = evconnlistener_new_bind(loop_->getEvBase(),
                                        handleAccept,
                                        this,
                                        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE,
                                        -1,
                                        listen_on_addr,
                                        sock_len);
    if (listener_ == nullptr)
    {
        LOG_FATAL << "couldn't open listener! errno[" << errno << "]: " << strerror(errno);
    }
}

Acceptor::~Acceptor()
{
    evconnlistener_free(listener_);
}

void Acceptor::handleAccept(struct evconnlistener *listener,
                            evutil_socket_t fd,
                            struct sockaddr *address,
                            int socklen,
                            void *ctx)
{
    auto self = (Acceptor *)ctx;
    if (self == nullptr)
    {
        LOG_ERROR << "not an acceptor, fd : " << fd;
        return;
    }
    
    IpAddress ip_addr(address);

    LOG_INFO << "accept new connection: " << ip_addr.format() << ", fd is " << fd;

    if (self->new_connect_cb_)
    {
        self->new_connect_cb_(fd, ip_addr);
    }
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &callback)
{
    new_connect_cb_ = callback;
}
} // namespace let