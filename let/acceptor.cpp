//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include <event2/util.h>
#include <cstring>

#include "acceptor.h"
#include "logger.h"
#include "socket.h"

namespace let
{

/* recognized formats are:
	 * [ipv6]:port
	 * ipv6
	 * [ipv6]
	 * ipv4:port
	 * ipv4
	 */
Acceptor::Acceptor(const IpAddress &ipaddr)
    : ev_base_(event_base_new())
{
    struct sockaddr_in listen_on_addr;

    int socklen = sizeof(listen_on_addr);

    auto ip_addr_str = ipaddr.format();

    // todo 优化
    if (evutil_parse_sockaddr_port(ip_addr_str.c_str(),
                                   (struct sockaddr *)&listen_on_addr,
                                   &socklen))
    {
        LOG_FATAL << "ip format is wrong: " << ip_addr_str;
    }

    listener_ = evconnlistener_new_bind(ev_base_,
                                        handleAccept,
                                        this,
                                        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE,
                                        -1,
                                        (struct sockaddr *)&listen_on_addr,
                                        socklen);
    if (!listener_)
    {
        LOG_FATAL << "couldn't open listener";
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
    IpAddress ip_addr(address, socklen);
    self->new_connect_cb_(fd, ip_addr);
}

void Acceptor::setNewConnectionCallback(const NewConnectionCallback &callback)
{
    new_connect_cb_ = callback;
}
} // namespace let