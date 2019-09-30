#ifndef LET_TCP_HANLDER_H
#define LET_TCP_HANLDER_H

#include <memory>

#include "tcp_connection.h"
#include "event_executor_group.h"
#include "tcp_connection.h"

namespace let {
class TcpHandler {
public:
    virtual void onConnected(TcpConnectionPtr)
    {
    }

    virtual int splitMessage(TcpConnectionPtr, const char* data, size_t length) = 0;

    virtual void onMessage(TcpConnectionPtr, const char* data, size_t length) = 0;

    virtual void onDisconnected(TcpConnectionPtr)
    {
    }

    virtual void onError(TcpConnectionPtr, int error)
    {
    }
};

using TcpHandlerPtr = std::shared_ptr<TcpHandler>;
using TcpHandlerFactory = std::function<TcpHandlerPtr()>;

class ChannelPipeLine {
private:
    void addFirst();
    void addLast();

    void fireOnActive();
};

class ChannelHandlerContext {
public:
    EventExecutor* executor()
    {
        return executor_;
    }

    TcpConnectionPtr connection();

    void fireOnConnected();

    void fireOnDisConnected();

    void fireOnRead();

private:
    EventExecutor* executor_;
    TcpConnectionPtr conn_;
};

class ITcpHandler {
public:
    enum class Type {
        IN,
        OUT,
        NONE
    };

    explicit ITcpHandler(Type bound_type)
        : bound_type_(bound_type)
    {
    }

private:
    Type bound_type_ = Type::NONE;
};

class TcpInBoundHandler : public ITcpHandler {
public:
    TcpInBoundHandler()
        : ITcpHandler(Type::IN)
    {
    }

    virtual void onConnected(ChannelHandlerContext& ctx)
    {
        ctx.fireOnConnected();
    };

    virtual void onDisconnected(ChannelHandlerContext& ctx)
    {
        ctx.fireOnConnected();
    };

    virtual void onRead(ChannelHandlerContext& ctx, void* buffer, size_t any)
    {
    }

    virtual void onError(ChannelHandlerContext& ctx)
    {
    }
};

class TcpOutBoundHandler : public ITcpHandler {
public:
    TcpOutBoundHandler()
        : ITcpHandler(Type::OUT)
    {
    }

    virtual void onWrite()
    {
    }
};

}  // namespace let

#endif