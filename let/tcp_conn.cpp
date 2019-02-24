//
// Created by yahuichen on 2019/1/23.
//

#include <event2/event.h>

#include "tcp_conn.h"

namespace let
{
TcpConnection::TcpConnection(evutil_socket_t fd, const IpAddress &ip_addr_)
    : fd_(fd), ip_addr_(ip_addr_)
{
}

void TcpConnection::bindBufferEvent(bufferevent *buf_ev)
{
    buf_ev_ = buf_ev,
    in_buf_ = new Buffer(bufferevent_get_input(buf_ev));
    out_buf_ = new Buffer(bufferevent_get_output(buf_ev));

    bufferevent_setcb(buf_ev,
                      readCallback,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev, EV_READ);
    bufferevent_disable(buf_ev, EV_WRITE);

    readCallback(buf_ev, this);
}

TcpConnection::~TcpConnection()
{
    bufferevent_free(buf_ev_);
    delete in_buf_;
    delete out_buf_;
}

evutil_socket_t TcpConnection::getFd()
{
    return bufferevent_getfd(buf_ev_);
}

void TcpConnection::send(const void *message, size_t len)
{
    out_buf_->add(message, len);

    bufferevent_setcb(buf_ev_,
                      nullptr,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ | EV_WRITE);
}

void TcpConnection::readCallback(struct bufferevent *bev, void *ctx)
{
    auto self = (TcpConnection *)ctx;

    self->message_cb_(shared_from_this());
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *ctx)
{
    auto conn = (TcpConnection *)ctx;
}

void TcpConnection::eventCallback(struct bufferevent *bev, short events, void *ctx)
{
    auto self = (TcpConnection *)ctx;

    bool finished = false;
    if (events & BEV_EVENT_EOF)
    {
        self->close_cb_(shared_from_this());
    }
    else if (events & BEV_EVENT_ERROR)
    {
        self->error_cb_(shared_from_this(), EVUTIL_SOCKET_ERROR());
    }
}

void TcpConnection::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpConnection::setCloseCallback(const CloseCallback &cb)
{
    close_cb_ = cb;
}

void TcpConnection::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

Buffer *TcpConnection::inputBuffer()
{
    return in_buf_;
}

Buffer *TcpConnection::outBuffer()
{
    return out_buf_;
}

void TcpConnection::setUserData(void *user_data)
{
    user_data_ = user_data;
}

void *TcpConnection::getUserData() const
{
    return user_data_;
}

} // namespace let