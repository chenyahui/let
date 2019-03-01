//
// Created by yahuichen on 2019/1/23.
//

#include <event2/event.h>
#include "tcp_conn.h"
#include "event_loop.h"
#include "buffer.h"
#include "logger.h"

namespace let
{
TcpConnection::TcpConnection(int fd,
                             const IpAddress &local_addr,
                             const IpAddress &remote_addr)
    : fd_(fd),
      remote_addr_(remote_addr),
      local_addr_(local_addr)
{
}

TcpConnection::~TcpConnection()
{
    bufferevent_free(buf_ev_);
}

evutil_socket_t TcpConnection::getFd()
{
    return bufferevent_getfd(buf_ev_);
}

// void TcpConnection::send(const std::string &message)
// {
//     send(message.c_str(), message.size());
// }

void TcpConnection::send(std::string_view message)
{
    send(message.data(), message.size());
}

void TcpConnection::send(const void *message, size_t len)
{
    out_buf_->add(message, len);

    bufferevent_setcb(buf_ev_,
                      nullptr, // 设为null是因为禁止读，但是还想监听到close
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ | EV_WRITE);
}

void TcpConnection::readCallback(struct bufferevent *bev, void *ctx)
{
    LOG_INFO << "tcp connnection read callback called";
    auto self = (TcpConnection *)ctx;

    if (self->message_cb_ && !self->inBuffer()->empty())
    {
        LOG_DEBUG << "begin call message callback";
        self->message_cb_(self->shared_from_this());
        LOG_DEBUG << "end call message callback";
    }
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *ctx)
{
    auto conn = (TcpConnection *)ctx;

    bufferevent_setcb(conn->buf_ev_,
                      readCallback, // 设为null是因为禁止读，但是还想监听到close
                      writeCallback,
                      eventCallback,
                      ctx);

    bufferevent_enable(conn->buf_ev_, EV_READ);

    // 如果buffer中被发送完了，则禁用write事件
    if (conn->out_buf_->empty())
    {
        bufferevent_disable(conn->buf_ev_, EV_WRITE);
    }
}

void TcpConnection::eventCallback(struct bufferevent *bev, short events, void *ctx)
{
    auto self = (TcpConnection *)ctx;

    bool finished = false;
    if (events & BEV_EVENT_EOF)
    {
        LOG_INFO << "tcp connnection close callback called";

        if (self->disconnection_cb_)
        {
            self->disconnection_cb_(self->shared_from_this());
        }
    }
    else if (events & BEV_EVENT_ERROR)
    {
        LOG_INFO << "tcp connnection error callback called";

        if (self->error_cb_)
        {
            self->error_cb_(self->shared_from_this(), EVUTIL_SOCKET_ERROR());
        }
    }
}

void TcpConnection::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpConnection::setDisconnectionCallback(const DisconnectionCallback &cb)
{
    disconnection_cb_ = cb;
}

void TcpConnection::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

Buffer *TcpConnection::inBuffer()
{
    return in_buf_.get();
}

Buffer *TcpConnection::outBuffer()
{
    return out_buf_.get();
}

void TcpConnection::setContext(std::any context)
{
    context_ = context;
}

const IpAddress &TcpConnection::getLocalAddr() const
{
    return local_addr_;
}

const IpAddress &TcpConnection::getRemoteAddr() const
{
    return remote_addr_;
}

void TcpConnection::setBufferEvent(bufferevent *buf_ev)
{
    buf_ev_ = buf_ev;

    in_buf_ = std::move(std::make_unique<Buffer>(bufferevent_get_input(buf_ev_)));
    out_buf_ = std::move(std::make_unique<Buffer>(bufferevent_get_input(buf_ev_)));

    bufferevent_setcb(buf_ev_,
                      readCallback,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ);
    bufferevent_disable(buf_ev_, EV_WRITE);

    readCallback(buf_ev_, this);
}

} // namespace let