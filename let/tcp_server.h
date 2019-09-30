#ifndef LET_TCP_SERVER_H
#define LET_TCP_SERVER_H

#include <vector>
#include <memory>
#include <map>
#include <atomic>
#include <event.h>

#include "ip_addr.h"
#include "noncopyable.h"
#include "acceptor.h"
#include "event_executor_group.h"
#include "threaded_executor.h"
#include "tcp_handler.h"

namespace let
{
struct ServerOptions
{
    int idle_timeout_sec = -1; // Default: -1(disabled)

    int32_t max_connection_nums = -1;
};

class TcpServer : NonCopyAble
{
public:
    TcpServer() = default;
    
    ~TcpServer();
    
    TcpServer &setGroup(EventExecutorGroup *event_loop_group);

    TcpServer &setGroup(EventExecutorGroup *parent_group, EventExecutorGroup *child_group);

    TcpServer &setHandlerFactory(TcpHandlerFactory handler_factory);

    TcpServer &setExecutor(ThreadedExecutorPool *task_pool);

    void listen(const IpAddress &inet_addr, const ServerOptions &options = ServerOptions());

    void gracefullyStop();

    bool isRunning() const;

private:
    struct IoContext;

    struct ConnectionContext;

    void newConnection(evutil_socket_t sock_fd, const IpAddress &remote_addr);

    void checkIdleConnections(IoContext *ioc);

    void onMessage(TcpConnectionPtr);

    void onDisconnected(TcpConnectionPtr);

    void onError(TcpConnectionPtr, int error);

    void onWriteCompleted(TcpConnectionPtr);

    ConnectionContext &getConnectionContext(TcpConnectionPtr);

    void removeTcpConnection(TcpConnectionPtr);

    void cleanUpLoop(EventLoop* event_loop);

private:
    EventExecutorGroup *parent_group_ = nullptr;
    EventExecutorGroup *child_group_ = nullptr;

    TcpHandlerFactory handler_factory_;

    ThreadedExecutorPool *task_pool_ = nullptr;

    std::map<EventLoop *, IoContext*> io_context_map_;

    std::unique_ptr<Acceptor> acceptor_ = nullptr;

    std::atomic_uint connection_nums_;

    bool is_running_ = false;

    ServerOptions options_;

    IpAddress listen_addr_;
};
} // namespace let

#endif