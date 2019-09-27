#ifndef LET_TCP_HANLDER_H
#define LET_TCP_HANLDER_H

#include <memory>

#include "tcp_connection.h"

namespace let
{
class TcpHandler
{
public:
    virtual void onConnected(TcpConnectionPtr) {}

    virtual int splitMessage(TcpConnectionPtr, const char *data, size_t length) = 0;

    virtual int onMessage(TcpConnectionPtr, const char *data, size_t length) = 0;

    virtual int onDisconnected(TcpConnectionPtr) {}

    virtual int onError(TcpConnectionPtr, int error) {}
};

using TcpHandlerPtr = std::shared_ptr<TcpHandler>;
using TcpHandlerFactory = std::function<TcpHandlerPtr()>;

} // namespace let

#endif