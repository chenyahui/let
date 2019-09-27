#include <assert.h>

#include "tcp_connection.h"
#include "event_loop.h"
#include "logger.h"
#include "buffer.h"
#include "utility.h"

using namespace let;

TcpConnection::TcpConnection(EventLoop *loop,
                             evutil_socket_t fd,
                             const IpAddress &local_addr,
                             const IpAddress &remote_addr)
    : event_loop_(loop),
      socket_fd_(fd),
      local_addr_(local_addr),
      remote_addr_(remote_addr)
{
    buffer_ev_ = bufferevent_socket_new(loop->lowLevelEvBase(),
                                        fd,
                                        BEV_OPT_CLOSE_ON_FREE);

    if (!buffer_ev_)
    {
        LOG_ERROR << " create bufferevent error";
        return;
    }

    read_buffer_.setBuffer(bufferevent_get_input(buffer_ev_));
    write_buffer_.setBuffer(bufferevent_get_output(buffer_ev_));
}

TcpConnection::TcpConnection(
    EventLoop *loop,
    bufferevent *buffer_ev,
    int fd,
    const IpAddress &local_addr,
    const IpAddress &remote_addr) : event_loop_(loop),
                                    buffer_ev_(buffer_ev),
                                    socket_fd_(fd),
                                    local_addr_(local_addr),
                                    remote_addr_(remote_addr)
{
    assert(buffer_ev_);

    read_buffer_.setBuffer(bufferevent_get_input(buffer_ev_));
    write_buffer_.setBuffer(bufferevent_get_output(buffer_ev_));
}

TcpConnection::~TcpConnection()
{
    event_loop_->execute([=]() {
      // 因为设置了BEV_OPT_CLOSE_ON_FREE, 所以不用手动close对应的fd了
      bufferevent_free(buffer_ev_);
    });
}

void TcpConnection::write(const char *content, size_t length)
{
    // 避免一次拷贝
    if (event_loop_->isInEventLoop())
    {
        if (evbuffer_add(buffer_ev_->output, content, length) != 0)
        {
            // todo write error
            LOG_ERROR << "add buffer error";
        }
    }
    else
    {
        std::string data(content, length);
        event_loop_->execute([=]() {
          // write in eventloop
          // 该函数在bufferevent中，会同时enable写事件，所以无需手动enable了
          if (evbuffer_add(buffer_ev_->output, data.c_str(), data.size()) != 0)
          {
              // todo write error
              LOG_ERROR << "add buffer error";
          }
        });
    }
}

void TcpConnection::enableRead()
{
    readable_ = true;
    event_loop_->execute([this]() {
      bufferevent_enable(buffer_ev_, EV_READ);
      bufferevent_setcb(buffer_ev_,
                        readAbleCallback,
                        writeCompleteCallback,
                        eventCallback,
                        this);
    });
}

void TcpConnection::disableRead()
{
    readable_ = false;
    event_loop_->execute([this]() {
      bufferevent_setcb(buffer_ev_,
                        nullptr,
                        writeCompleteCallback,
                        eventCallback,
                        this);
    });
}

bool TcpConnection::readable() const
{
    return readable_;
}

void TcpConnection::close()
{
    evutil_closesocket(socket_fd_);
}

void TcpConnection::readAbleCallback(struct bufferevent *bev, void *ctx)
{
    auto self = (TcpConnection *) ctx;

    self->last_read_time_ = get_monotonic_milliseconds();

    if (self->message_cb_)
    {
        self->message_cb_(self->shared_from_this());
    }
}

void TcpConnection::writeCompleteCallback(struct bufferevent *bev, void *ctx)
{
    auto self = (TcpConnection *) ctx;

    self->last_write_time_ = get_monotonic_milliseconds();

    // 由于bufferevent也会自动的判断，当outbuf为空的时候，帮我们禁用掉写事件
    // 因此这里我们不需要手动禁用了
    if (self->write_completed_cb_)
    {
        self->write_completed_cb_(self->shared_from_this());
    }
}

void TcpConnection::eventCallback(struct bufferevent *bev, short events, void *ctx)
{
    auto self = (TcpConnection *) ctx;

    bool finished = false;
    if (events & BEV_EVENT_EOF)
    {
        LOG_VERBOSE << "tcp connection close callback called";

        if (self->disconnection_cb_)
        {
            self->disconnection_cb_(self->shared_from_this());
        }
    }
    else if (events & BEV_EVENT_ERROR)
    {
        LOG_INFO << "tcp connection error callback called";

        if (self->error_cb_)
        {
            self->error_cb_(self->shared_from_this(), EVUTIL_SOCKET_ERROR());
        }
    }
    else
    {
        LOG_INFO << "tcp connection event callback called";
    }
}

bool TcpConnection::isValid() const
{
    return buffer_ev_ != nullptr;
}

void TcpConnection::setMessageCallback(MessageCallback cb)
{
    message_cb_ = std::move(cb);
}

void TcpConnection::setDisconnectionCallback(DisconnectionCallback cb)
{
    disconnection_cb_ = std::move(cb);
}

void TcpConnection::setErrorCallback(ErrorCallback cb)
{
    error_cb_ = std::move(cb);
}

void TcpConnection::setWriteCompletedCallback(WriteCompletedCallback cb)
{
    write_completed_cb_ = std::move(cb);
}

Buffer &TcpConnection::readBuffer()
{
    return read_buffer_;
}

Buffer &TcpConnection::writeBuffer()
{
    return write_buffer_;
}

const IpAddress &TcpConnection::localAddr() const
{
    return local_addr_;
}

const IpAddress &TcpConnection::remoteAddr() const
{
    return remote_addr_;
}

EventLoop *TcpConnection::getLoop() const
{
    return event_loop_;
}

uint64_t TcpConnection::lastReadTime() const
{
    return last_read_time_;
}

uint64_t TcpConnection::lastWriteTime() const
{
    return last_write_time_;
}