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

Acceptor::Acceptor(EventLoop *loop, const IpAddress &ip_addr)
    : loop_(loop)
{
    auto listen_on_addr = ip_addr.getSockAddr();

    int sock_len = ip_addr.isIpv6() ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);

    // 设置 LEV_OPT_DISABLED， 暂时不把这个事件加入到event_loop中
    auto flags = LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE | LEV_OPT_DISABLED;

    // create socket fd
    // bind
    // new listener obj
    ev_listener_ = evconnlistener_new_bind(loop_->lowLevelEvBase(),
                                           handleAccept,
                                           this,
                                           flags,
                                           -1,
                                           listen_on_addr,
                                           sock_len);
    if (ev_listener_ == nullptr)
    {
        LOG_FATAL << "couldn't open listener! errno[" << errno << "]: " << strerror(errno);
        return;
    }

    evconnlistener_set_error_cb(ev_listener_, handleAcceptError);
}

Acceptor::~Acceptor()
{
    // 因为我们定义了LEV_OPT_CLOSE_ON_FREE标志，所以free的时候会自动close该fd
    loop_->execute([=]() {
        evconnlistener_free(ev_listener_);
    });
}

// listen，实际上最终调的不是系统的listen函数
// 而是将这个事件加入到event_loop中
void Acceptor::listen()
{
    loop_->execute([=]() {
        evconnlistener_enable(ev_listener_);
        LOG_VERBOSE << "start listen ";
    });
}

// 将事件从事件循环中拿出来
void Acceptor::stopListen()
{
    // todo 将acceptor的标志置为已关闭
    loop_->execute([=] {
        evconnlistener_disable(ev_listener_);
    });
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

    LOG_VERBOSE << "accept new connection: " << ip_addr.format() << ", fd is " << fd;

    if (self->new_connect_cb_)
    {
        self->new_connect_cb_(fd, ip_addr);
    }
}

void Acceptor::handleAcceptError(struct evconnlistener *, void *ctx)
{
    auto self = (Acceptor *)ctx;

    LOG_ERROR << "accept error";
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback callback)
{
    new_connect_cb_ = std::move(callback);
}
} // namespace let
