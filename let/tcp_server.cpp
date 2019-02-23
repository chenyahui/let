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
      io_thread_pool_(options.io_thread)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection,
                                                 this,
                                                 std::placeholders::_1));
}

void TcpServer::run()
{
}

void TcpServer::connectionCallbackWrapper(TcpConnectionPtr tcp_conn)
{
    if (connection_cb_)
    {
        connection_cb_(tcp_conn);
    }
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connection_cb_ = cb;
}

void TcpServer::newConnection(int sockfd, const IpAddress &ip_addr)
{
    auto tcp_conn = std::make_shared<TcpConnection>(sockfd, ip_addr);

    // set callback
    tcp_conn->setMessageCallback(std::bind(&TcpServer::messageCallbackWrapper,
                                           this,
                                           std::placeholders::_1));
    //
    io_thread_pool_.addConnnection(tcp_conn);
}

} // namespace let