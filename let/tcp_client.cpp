//
// Created by yahuichen on 2019/1/23.
//

#include "tcp_client.h"
#include "tcp_conn.h"
#include "util.h"

namespace let
{
TcpClient::TcpClient(EventLoop *event_loop, const IpAddress &remote_addr)
    : event_loop_(event_loop),
      remote_addr_(remote_addr),
      connector_(event_loop, remote_addr)
{
}

void TcpClient::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpClient::setConnectionCallback(const ConnectionCallback &cb)
{
    connection_cb_ = cb;
}

void TcpClient::setCloseCallback(const CloseCallback &cb)
{
    close_cb_ = cb;
}

void TcpClient::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

void TcpClient::connect()
{
    connector_.connect();
}

void TcpClient::newConnection(evutil_socket_t fd)
{
    auto local_addr = IpAddress(get_local_addr(fd));
    auto tcp_conn = std::make_shared<TcpConnection>(fd, local_addr, remote_addr_);

    // set callbacks
    tcp_conn->setMessageCallback(message_cb_);
    tcp_conn->setCloseCallback(close_cb_);
    tcp_conn->setErrorCallback(error_cb_);

    auto buf_ev = connector_.getBufferEvent();
    tcp_conn->setBufferEvent(buf_ev);
    tcp_conn->bindEventLoop(event_loop_);

    // call callback
    connection_cb_(tcp_conn);
}

} // namespace let