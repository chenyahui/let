//
// Created by yahuichen on 2019/1/23.
//

#include <event2/event.h>

#include "tcp_conn.h"
#include "event_loop.h"
#include "buffer.h"
#include "logger.h"
#include "utility.h"

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
    LOG_DEBUG << "tcp connection release";
    bufferevent_free(buf_ev_);
}

evutil_socket_t TcpConnection::getFd()
{
    return bufferevent_getfd(buf_ev_);
}

void TcpConnection::send(const std::string &message)
{
    send(message.c_str(), message.size());
}

void TcpConnection::send(const void *message, size_t len)
{
    if (!out_buf_->add(message, len))
    {
        LOG_ERROR << "outbuffer add message error";
        return;
    }
    changeEvent(EV_WRITE);
}

void TcpConnection::readCallback(struct bufferevent *bev, void *ctx)
{
    LOG_INFO << "tcp connnection read callback called";
    auto self = (TcpConnection *)ctx;

    self->last_readtime_ms_ = get_monotonic_timestamp() / 1000;

    if (self->message_cb_ && !self->in_buf_->empty())
    {
        std::string str;
        str.resize(self->in_buf_->length());
        self->in_buf_->copyOut((void *)str.c_str(), str.size());
        LOG_DEBUG << "长度是: " << str.size() << "; 内容是:" << str << "#";
        LOG_DEBUG << "begin call message callback";
        self->message_cb_(self->shared_from_this());
        LOG_DEBUG << "end call message callback";
    }
}

void TcpConnection::writeCallback(struct bufferevent *bev, void *ctx)
{
    LOG_INFO << "tcp connnection write callback called";
    auto self = (TcpConnection *)ctx;

    self->last_writetime_ms_ = get_monotonic_timestamp() / 1000;

    self->changeEvent(EV_READ | EV_WRITE);

    // 如果buffer中被发送完了，则禁用write事件
    if (self->out_buf_->empty())
    {
        LOG_INFO << "outbuf is empty, disable EV_WRITE";
        bufferevent_disable(self->buf_ev_, EV_WRITE);
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
    else
    {
        LOG_INFO << "tcp connnection event callback called";
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

const IpAddress &TcpConnection::getLocalAddr() const
{
    return local_addr_;
}

const IpAddress &TcpConnection::getRemoteAddr() const
{
    return remote_addr_;
}

void TcpConnection::changeEvent(short event)
{
    auto read_cb = !(event & EV_READ) ? nullptr : readCallback;

    bufferevent_setcb(buf_ev_,
                      read_cb,
                      writeCallback,
                      eventCallback,
                      this);

    bufferevent_enable(buf_ev_, EV_READ | event);

    if (!(event & EV_WRITE))
    {
        bufferevent_disable(buf_ev_, EV_WRITE);
    }
}

void TcpConnection::bindBufferEvent(bufferevent *buf_ev)
{
    buf_ev_ = buf_ev;

    in_buf_ = std::move(std::unique_ptr<Buffer>(new Buffer(bufferevent_get_input(buf_ev_))));
    out_buf_ = std::move(std::unique_ptr<Buffer>(new Buffer(bufferevent_get_output(buf_ev_))));

    changeEvent(EV_READ);

    readCallback(buf_ev, this);
}

void TcpConnection::setContext(void* context)
{
    context_ = context;
}

void* TcpConnection::getContext()
{
    return context_;
}

int64_t TcpConnection::getLastActiveTime() const
{
    return std::max(last_readtime_ms_, last_writetime_ms_);
}

void TcpConnection::setKeepAlive(bool keep_alive)
{
    set_socket_option(fd_, SO_KEEPALIVE, keep_alive);
}

void TcpConnection::setNoDelay(bool no_delay)
{
    set_tcp_option(fd_, TCP_NODELAY, no_delay);
}

void TcpConnection::closeRead()
{
    changeEvent(EV_READ);
}

void TcpConnection::closeWrite()
{
    changeEvent(EV_WRITE);
}

void TcpConnection::close()
{
    evutil_closesocket(fd_);
}

} // namespace let