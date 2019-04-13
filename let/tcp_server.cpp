//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include <functional>

#include "tcp_server.h"
#include "utility.h"
#include "logger.h"

namespace let
{
TcpServer::TcpServer(EventLoop *loop, const IpAddress &ip_addr, const ServerOptions &options)
    : loop_(loop),
      options_(options),
      acceptor_(loop, ip_addr)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection,
                                                 this,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2));

    if(options.io_thread_num > 0){
        event_loop_thread_pool_ = std::move(std::unique_ptr<EventLoopThreadPool>(new EventLoopThreadPool(options_.io_thread_num)));
    }
}

void TcpServer::run()
{
    event_loop_thread_pool_->start();
    acceptor_.listen();

    // 如果开启了闲置连接检查，则注册timer
    if (options_.idle_timeout_sec > 0)
    {
        loop_->runEvery(1000, std::bind(&TcpServer::checkIdleConnections, this));
    }
}

void TcpServer::stop()
{
    acceptor_.stop();
    event_loop_thread_pool_->stop();

    // todo stop timer
}

void TcpServer::setMessageCallback(const MessageCallback &cb)
{
    message_cb_ = cb;
}

void TcpServer::setConnectionCallback(const ConnectionCallback &cb)
{
    connection_cb_ = cb;
}

void TcpServer::setDisconnectionCallback(const DisconnectionCallback &cb)
{
    disconnection_cb_ = cb;
}

void TcpServer::setErrorCallback(const ErrorCallback &cb)
{
    error_cb_ = cb;
}

void TcpServer::newConnection(evutil_socket_t sock_fd, const IpAddress &remote_addr)
{
    LOG_INFO << "tcp server get new connection, " << remote_addr.format();

    auto remote_addr_str = remote_addr.format();
    if (options_.max_connections > 0 && connections_.size() >= options_.max_connections)
    {
        LOG_ERROR << "tcp server has max connections: " << options_.max_connections
                  << ", and reject the connection: " << remote_addr_str;

        evutil_closesocket(sock_fd);
        return;
    }

    auto local_addr = IpAddress(get_local_addr(sock_fd));

    auto tcp_conn = std::make_shared<TcpConnection>(sock_fd, local_addr, remote_addr);

    connections_[remote_addr_str] = tcp_conn;

    tcp_conn->setNoDelay(options_.tcp_no_delay);
    tcp_conn->setKeepAlive(options_.so_keep_alive);

    // set callback
    tcp_conn->setMessageCallback(message_cb_);

    tcp_conn->setDisconnectionCallback([&](TcpConnectionPtr conn) {
      LOG_DEBUG << "disconnection callback called";

      if (disconnection_cb_)
      {
          disconnection_cb_(conn);
      }
      removeConnection(conn);
    });

    tcp_conn->setErrorCallback([&](TcpConnectionPtr conn, int err_code) {
      LOG_DEBUG << "error callback called, err_code is " << err_code;
      if (error_cb_)
      {
          error_cb_(conn, err_code);
      }
      removeConnection(conn);
    });

    if (connection_cb_)
    {
        LOG_INFO << "connection callback called~";
        connection_cb_(tcp_conn);
    }

    EventLoop *loop = nullptr;

    if (options_.io_thread_num > 0)
    {
        // select a sub eventloop
        auto ev_loop_thread = event_loop_thread_pool_->getNextEventLoopThread();
        auto &ev_loop = ev_loop_thread->getEventLoop();
        loop = const_cast<EventLoop *>(&ev_loop);
    }
    else
    {
        loop = loop_;
    }

    // create bufferevent
    auto buf_ev = bufferevent_socket_new(loop->getEvBase(), sock_fd, BEV_OPT_CLOSE_ON_FREE);

    if (!buf_ev)
    {
        LOG_ERROR << "bufferevent_socket_new error";
        evutil_closesocket(sock_fd);
        return;
    }

    // 设置高低水位
    bufferevent_setwatermark(buf_ev,
                             EV_READ,
                             options_.read_low_water,
                             options_.read_high_water);

    tcp_conn->bindBufferEvent(buf_ev);
}

void TcpServer::removeConnection(TcpConnectionPtr conn)
{
    LOG_DEBUG << "remove connection: " << conn->getRemoteAddr().format();
    connections_.erase(conn->getRemoteAddr().format());
}

// 定时检查闲置连接
void TcpServer::checkIdleConnections()
{
    auto expire = (get_monotonic_timestamp() / 1000) - (options_.idle_timeout_sec * 1000);
    for (auto &c : connections_)
    {
        if (c.second->getLastActiveTime() < expire)
        {
            connections_.erase(c.first);
        }
    }
}
} // namespace let