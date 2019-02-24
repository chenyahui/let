//
// Created by yahuichen on 2019/1/23.
//

#include "tcp_client.h"

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
    
}
} // namespace let