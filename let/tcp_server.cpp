//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include <functional>
#include "tcp_server.h"

namespace let
{

TcpServer::TcpServer(const ServerOptions &options, const IpAddress &ip_addr)
    : options_(options),
      acceptor_(ip_addr),
      io_thread_pool_(options.io_thread_num)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection,
                                                 this,
                                                 std::placeholders::_1));
}

void TcpServer::run()
{
}

void TcpServer::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connection_cb_ = cb;
}

void TcpServer::setCloseCallback(const CloseCallback &cb)
{
    close_cb_ = cb;
}

void TcpServer::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

void TcpServer::newConnection(evutil_socket_t sockfd, const IpAddress &ip_addr)
{
    auto tcp_conn = std::make_shared<TcpConnection>(sockfd, ip_addr);

    connections_[sockfd] = tcp_conn;

    // set callback
    tcp_conn->setMessageCallback(message_cb_);
    tcp_conn->setCloseCallback(close_cb_);
    tcp_conn->setErrorCallback(error_cb_);
    
    //
    io_thread_pool_.addConnnection(tcp_conn);
}

} // namespace let