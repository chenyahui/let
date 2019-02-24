//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "io_thread.h"

namespace let
{
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

void IoThreadPool::addConnnection(TcpConnectionPtr tcp_conn)
{
    tcp_conn->bindIoThread(getNextIoThread());
}

IoThread *IoThreadPool::getNextIoThread()
{
    auto io_thread = io_threads_[next_];
    next_ = (next_ + 1) % io_threads_.size();
}
} // namespace let