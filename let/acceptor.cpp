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

Acceptor::Acceptor(const IpAddress &ip_addr)
    : ev_base_(event_base_new())
{

    auto listen_on_addr = ip_addr.getSockAddr();

    int sock_len = ip_addr.isIpv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);

    listener_ = evconnlistener_new_bind(ev_base_,
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
    stop();
    event_base_free(ev_base_);
    evconnlistener_free(listener_);
}

void Acceptor::listen()
{
    event_base_loop(ev_base_, 0);
}

void Acceptor::stop()
{
    event_base_loopbreak(ev_base_);
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
    LOG_INFO << "accept new connection, fd is " << fd;
    IpAddress ip_addr(address);

    LOG_INFO << "address has parsed, ip is: " << ip_addr.ip()
             << ", port is " << ip_addr.port();
             
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