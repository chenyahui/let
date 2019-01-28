//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#include "tcp_server.h"

namespace let
{
void TcpServer::run(ServerOptions option)
{
    acceptor_->start();
}

void TcpServer::run()
{
}
} // namespace let