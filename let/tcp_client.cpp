//
// Created by yahuichen on 2019/1/23.
//

#include "tcp_client.h"
#include "tcp_conn.h"

namespace let
{
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
    auto tcp_conn = std::make_shared<TcpConnection>(fd, remote_addr_);
    
    // set callbacks
    tcp_conn->setMessageCallback(message_cb_);
    tcp_conn->setCloseCallback(close_cb_);
    tcp_conn->setErrorCallback(error_cb_);
}

} // namespace let