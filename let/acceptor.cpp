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
Acceptor::Acceptor(const std::string &ip_addr)
    : ev_base_(event_base_new())
{
    sockaddr_in listen_on_addr{};

    int socklen = sizeof(listen_on_addr);

    if (evutil_parse_sockaddr_port(ip_addr.c_str(),
                                   (struct sockaddr *)&listen_on_addr,
                                   &socklen))
    {
        LOG_FATAL << "ip format is wrong: " << ip_addr;
    }

    listener_ = evconnlistener_new_bind(ev_base_,
                                        newConnectionCallback,
                                        this,
                                        LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE,
                                        -1,
                                        (struct sockaddr *)&listen_on_addr,
                                        socklen);
    if (!listener_)
    {
        LOG_FATAL << "Couldn't open listener";
    }
}

Acceptor::~Acceptor()
{
    event_base_free(ev_base_);
    evconnlistener_free(listener_);
}

void Acceptor::start()
{
    for (int i = 0; i < conn_hubs_.size(); ++i)
    {
        conn_hubs_[0]->start();
    }
}

void Acceptor::newConnectionCallback(struct evconnlistener *listener,
                                     evutil_socket_t fd,
                                     struct sockaddr *address,
                                     int socklen,
                                     void *ctx)
{

    auto acceptor = (Acceptor *)ctx;
    acceptor->schedule(fd, address, socklen);
}

void Acceptor::schedule(evutil_socket_t fd, struct sockaddr *address, int socklen)
{
    std::string ip_port = sockaddr_to_ip_port(address);

    // 此处得到的fd，libevent会帮助我们设置为noblock的

    conn_hubs_[next_hub_]->addConnection(fd, ip_port);

    next_hub_ = (next_hub_ + 1) % conn_hubs_.size();
}
} // namespace let