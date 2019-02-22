//
// Created by yahuichen on 2019/1/23.
//

#include <event2/event.h>

#include "tcp_conn.h"

namespace let
{
TcpConnection::TcpConnection(bufferevent *buf_ev)
    : buf_ev_(buf_ev),
      in_buf_(bufferevent_get_input(buf_ev)),
      out_buf_(bufferevent_get_output(buf_ev))
{

    bufferevent_setcb(buf_ev, readCallback, writeCallback, errorCallback, this);

    bufferevent_enable(buf_ev, EV_READ);
    bufferevent_disable(buf_ev, EV_WRITE);

    readCallback(buf_ev, this);
}

TcpConnection::~TcpConnection()
{
    bufferevent_free(buf_ev_);
}

evutil_socket_t TcpConnection::getFd()
{
    return bufferevent_getfd(buf_ev_);
}

void TcpConnection::send(const void *message, size_t len)
{

    out_buf_.add(message, len);

    bufferevent_setcb(buf_ev_,
                      nullptr,
                      writeCallback,
                      errorCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ | EV_WRITE);
}

void TcpConnection::readCallback(struct bufferevent *bev, void *ctx)
{
    auto conn = (TcpConnection *)ctx;

    conn->message_callback_(conn);
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *ctx)
{
    auto conn = (TcpConnection *)ctx;
}

void TcpConnection::errorCallback(struct bufferevent *bev, short what, void *ctx)
{
    auto conn = (TcpConnection *)ctx;
}

Buffer *TcpConnection::inputBuffer()
{
    return &in_buf_;
}

Buffer *TcpConnection::outBuffer()
{
    return &out_buf_;
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