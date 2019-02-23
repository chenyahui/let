//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "io_thread.h"

namespace let
{
void IoThread::addConnection(TcpConnectionPtr tcp_conn)
{
    auto buf_ev = bufferevent_socket_new(ev_base_, tcp_conn->getFd(), 0);
}

void IoThread::start()
{
    thread_ = std::thread([=]() {
        event_base_loop(ev_base_, EVLOOP_NO_EXIT_ON_EMPTY);
    });
}

void IoThread::stop()
{
    event_base_loopbreak(ev_base_);
    thread_.join();
}

IoThread::~IoThread()
{
    stop();
}

void IoThreadPool::addConnnection(TcpConnectionPtr ptr)
{
    auto tcp_conn = io_threads_[next_]->newConnection(fd);
    
}
} // namespace let