#ifndef LET_TCP_CONNECTION_H
#define LET_TCP_CONNECTION_H

#include <memory>
#include <event.h>

#include "ip_addr.h"
#include "event_loop.h"
#include "callback.h"
#include "buffer.h"

namespace let {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop *loop,
                  evutil_socket_t fd,
                  const IpAddress &local_addr,
                  const IpAddress &remote_addr);

    TcpConnection(EventLoop *loop,
                  bufferevent *buffer_ev,
                  int fd,
                  const IpAddress &local_addr,
                  const IpAddress &remote_addr);

    ~TcpConnection();

    void write(const std::string& content);

    void write(const char *content, size_t length);

    Buffer &readBuffer();

    Buffer &writeBuffer();

    bool readable() const;

    void enableRead();

    void disableRead();

    void close();

    bool isValid() const;

    EventLoop *getLoop() const;

    const IpAddress &localAddr() const;

    const IpAddress &remoteAddr() const;

    int64_t lastReadTime() const;

    int64_t lastWriteTime() const;

    // set callbacks
    void setMessageCallback(MessageCallback cb);

    void setDisconnectionCallback(DisconnectionCallback cb);

    void setErrorCallback(ErrorCallback cb);

    void setWriteCompletedCallback(WriteCompletedCallback cb);

private:
    static void readAbleCallback(struct bufferevent *bev, void *ctx);

    // 当消息低于低水位的时候，会触发
    // 所以可以认为此时已经不能再写了
    static void writeCompleteCallback(struct bufferevent *bev, void *ctx);

    static void eventCallback(struct bufferevent *bev, short events, void *ctx);

private:
    EventLoop *event_loop_;
    evutil_socket_t socket_fd_ = -1;

    IpAddress local_addr_;
    IpAddress remote_addr_;

    bufferevent *buffer_ev_ = nullptr;
    uint64_t connection_id_ = 0;

    MessageCallback message_cb_;
    DisconnectionCallback disconnection_cb_;
    ErrorCallback error_cb_;
    WriteCompletedCallback write_completed_cb_;

    bool readable_ = false;

    Buffer read_buffer_;
    Buffer write_buffer_;

    int64_t last_read_time_ = 0;
    int64_t last_write_time_ = 0;
};

}  // namespace let
#endif