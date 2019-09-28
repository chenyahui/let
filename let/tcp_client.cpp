#include <assert.h>

#include "tcp_client.h"
#include "socket.h"
#include "logger.h"

using namespace let;

TcpClient::TcpClient()
{

}

TcpClient &TcpClient::setEventLoop(EventLoop *event_loop)
{
    event_loop_ = event_loop;
    return *this;
}

TcpClient &TcpClient::setExecutor(ThreadedExecutorPool *task_pool)
{
    task_pool_ = task_pool;
    return *this;
}

TcpClient &TcpClient::setHandler(TcpHandler* handler)
{
    handler_ = handler;
    return *this;
}

bool TcpClient::connect(const IpAddress &remote_addr)
{
    // check valid
    assert(event_loop_ != nullptr);
    assert(handler_);

    remote_addr_ = remote_addr;

    connector_ = std::move(std::unique_ptr<Connector>(new Connector(event_loop_,
                                                                    remote_addr)));

    connector_->setNewConnectionCallback(std::bind(&TcpClient::newConnection,
                                                   this,
                                                   std::placeholders::_1));

    return connector_->connect();
}

void TcpClient::newConnection(evutil_socket_t sock_fd)
{
    // select a child event loop thread
    EventLoop *loop = event_loop_;

    auto local_addr = IpAddress(get_local_addr(sock_fd));

    auto tcp_conn = std::make_shared<TcpConnection>(loop,
                                                    connector_->getBufferEvent(),
                                                    sock_fd,
                                                    remote_addr_,
                                                    local_addr);
    
    tcp_conn->enableRead();

    handler_->onConnected(tcp_conn);
}

void TcpClient::onMessage(TcpConnectionPtr tcp_conn)
{
    auto &read_buf = tcp_conn->readBuffer();

     // make buffer contiguous
    auto data = read_buf.pullUp();

     int ret = handler_->splitMessage(tcp_conn, data, read_buf.size());
      // the message is imcompleted, need more buffer
    if (ret == 0)
    {
        return;
    }
    else if (ret < 0)
    {
        LOG_ERROR << "msg is error";
        return;
    }

     // if task pool is setuped, handle this message at task thread pool
    if (task_pool_ != nullptr)
    {
        std::string msg = read_buf.retrieveAsString(ret);

        task_pool_->submit([tcp_conn, msg, this]() {
            handler_->onMessage(tcp_conn, msg.data(), msg.size());
        });
    }
    else
    {
        // just handle message at io thread
        handler_->onMessage(tcp_conn, data, ret);
        read_buf.consume(ret);
    }
}

void TcpClient::onDisconnected(TcpConnectionPtr tcp_conn)
{
    handler_->onDisconnected(tcp_conn);
}

void TcpClient::onError(TcpConnectionPtr tcp_conn, int error)
{
    handler_->onError(tcp_conn, error);
}

void TcpClient::onWriteCompleted(TcpConnectionPtr)
{
}

void TcpClient::disConnect()
{}

void TcpClient::stop()
{}
