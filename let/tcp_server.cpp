#include <memory>
#include <mutex>
#include <map>
#include <set>
#include <assert.h>

#include "tcp_server.h"
#include "event_loop_thread_pool.h"
#include "tcp_connection.h"
#include "logger.h"
#include "tcp_handler.h"
#include "utility.h"

using namespace let;

namespace let
{
struct TcpServer::ConnectionContext
{
    std::shared_ptr<TcpHandler> handler;
};

struct TcpServer::IoContext
{
    std::map<TcpConnectionPtr, TcpServer::ConnectionContext> connections;
    EventLoop *loop = nullptr;
};
} // namespace let

TcpServer::~TcpServer()
{
    gracefullyStop();
}

TcpServer &TcpServer::setGroup(EventLoopThreadPool *event_loop_group)
{
    setGroup(event_loop_group, event_loop_group);
    return *this;
}

TcpServer &TcpServer::setGroup(EventLoopThreadPool *parent, EventLoopThreadPool *child)
{
    parent_group_ = parent;
    child_group_ = child;
    return *this;
}

TcpServer &TcpServer::setExecutor(ThreadedExecutorPool *task_pool)
{
    task_pool_ = task_pool;
    return *this;
}

TcpServer &TcpServer::setHandlerFactory(TcpHandlerFactory handler_factory)
{
    handler_factory_ = std::move(handler_factory);
    return *this;
}

void TcpServer::listen(const IpAddress &inet_addr, const ServerOptions &options)
{
    if (is_running_)
    {
        return;
    }

    // check valid
    assert(parent_group_ != nullptr);
    assert(child_group_ != nullptr);
    assert(handler_factory_);

    // todo check port is valid

    listen_addr_ = inet_addr;

    // select a event_loop thread as acceptor thread
    acceptor_ = std::move(std::make_unique<Acceptor>(parent_group_->next()->getEventLoop(), inet_addr));

    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection,
                                                  this,
                                                  std::placeholders::_1,
                                                  std::placeholders::_2));

    // register iocontext and timer
    for (size_t i = 0; i < child_group_->size(); i++)
    {
        EventLoop *loop = child_group_->at(i)->getEventLoop();

        // create io_context of this loop thread
        auto io_context = new IoContext();
        io_context->loop = loop;

        io_context_map_[loop] = io_context;

        loop->runEvery(1000, std::bind(&TcpServer::checkIdleConnections,
                                       this,
                                       io_context));
    }

    acceptor_->listen();

    is_running_ = true;
}

// work in parent loop thread
void TcpServer::newConnection(evutil_socket_t sock_fd, const IpAddress &remote_addr)
{
    if (options_.max_connection_nums > 0 && connection_nums_ >= options_.max_connection_nums)
    {
        LOG_ERROR << "tcp server has max connections: " << options_.max_connection_nums
                  << ", and reject the connection: " << remote_addr.format();

        evutil_closesocket(sock_fd);
        return;
    }

    // select a child event loop thread
    EventLoop *loop = child_group_->next()->getEventLoop();

    auto tcp_conn = std::make_shared<TcpConnection>(loop,
                                                    sock_fd,
                                                    listen_addr_,
                                                    remote_addr);

    if (!tcp_conn->isValid())
    {
        LOG_ERROR << "create tcp_connection failed";
        evutil_closesocket(sock_fd);
        return;
    }

    connection_nums_++;

    tcp_conn->setMessageCallback(std::bind(&TcpServer::onMessage,
                                           this,
                                           std::placeholders::_1));

    tcp_conn->setDisconnectionCallback(std::bind(&TcpServer::onDisconnected,
                                                 this,
                                                 std::placeholders::_1));

    tcp_conn->setErrorCallback(std::bind(&TcpServer::onError,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));

    tcp_conn->setWriteCompletedCallback(std::bind(&TcpServer::onWriteCompleted,
                                                  this,
                                                  std::placeholders::_1));

    // save the tcp connection to io context
    ConnectionContext conn_context;
    conn_context.handler = handler_factory_();
    auto handler = conn_context.handler;

    io_context_map_[loop]->connections[tcp_conn] = conn_context;

    // 开启读事件, 正式将tcp_connection加入到event loop中
    loop->execute([tcp_conn, handler]() {
        handler->onConnected(tcp_conn);
        tcp_conn->enableRead();
        LOG_VERBOSE << "tcp conn enable read";
    });
}

void TcpServer::onMessage(TcpConnectionPtr tcp_conn)
{
    // get connection context
    auto &ctx = getConnectionContext(tcp_conn);
    auto handler = ctx.handler;

    // split message
    auto &read_buf = tcp_conn->readBuffer();

    // make buffer contiguous
    auto data = read_buf.pullUp();

    int ret = handler->splitMessage(tcp_conn, data, read_buf.size());

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

    std::string msg = read_buf.retrieveAsString(ret);

    // if task pool is setuped, handle this message at task thread pool
    if (task_pool_ != nullptr)
    {
        task_pool_->submit([tcp_conn, msg, handler]() {
            handler->onMessage(tcp_conn, msg.data(), msg.size());
        });
    }
    else
    {
        // just handle message at io thread
        handler->onMessage(tcp_conn, msg.data(), msg.size());
    }
}
void TcpServer::onDisconnected(TcpConnectionPtr tcp_conn)
{
    auto &ctx = getConnectionContext(tcp_conn);

    ctx.handler->onDisconnected(tcp_conn);

    removeTcpConnection(tcp_conn);
}

void TcpServer::onError(TcpConnectionPtr tcp_conn, int error)
{
    auto &ctx = getConnectionContext(tcp_conn);

    ctx.handler->onError(tcp_conn, error);

    removeTcpConnection(tcp_conn);
}

void TcpServer::onWriteCompleted(TcpConnectionPtr tcp_conn)
{
    // get connection context
    auto &ctx = getConnectionContext(tcp_conn);
}

TcpServer::ConnectionContext &TcpServer::getConnectionContext(TcpConnectionPtr conn)
{
    return io_context_map_[conn->getLoop()]->connections[conn];
}

void TcpServer::removeTcpConnection(TcpConnectionPtr conn)
{
    // remove之后，onError或者onDisconnected还会被调用吗？
    io_context_map_[conn->getLoop()]->connections.erase(conn);
    connection_nums_--;
    LOG_VERBOSE << "remove connections:" << conn->remoteAddr().format();
}

void TcpServer::checkIdleConnections(IoContext *ioc)
{
    for (auto &conn_pair : ioc->connections)
    {
        auto tcp_conn = conn_pair.first;

        auto &ctx = getConnectionContext(tcp_conn);
        auto lasest_access_time = std::max(tcp_conn->lastReadTime(), tcp_conn->lastWriteTime());
        auto now_time = get_monotonic_milliseconds();

        if (options_.idle_timeout_sec > 0 &&
            lasest_access_time > 0 &&
            options_.idle_timeout_sec * 1000 < (now_time - lasest_access_time))
        {
            removeTcpConnection(tcp_conn);
        }
    }
}

void TcpServer::cleanUpLoop(EventLoop *event_loop)
{
    auto ioc = io_context_map_[event_loop];
    // stop timer

    // remove connections
    for (auto &conn_pair : ioc->connections)
    {
        removeTcpConnection(conn_pair.first);
    }
}

void TcpServer::gracefullyStop()
{
    acceptor_->stopListen();

    for (auto &ioc : io_context_map_)
    {
        cleanUpLoop(ioc.first);

        io_context_map_.erase(ioc.first);

        delete ioc.second;
    }

    is_running_ = false;
}

bool TcpServer::isRunning() const
{
    return is_running_;
}