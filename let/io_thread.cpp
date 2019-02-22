//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "io_thread.h"

namespace let
{
std::shared_ptr<TcpConnection> IoThread::newConnection(evutil_socket_t fd)
{
    auto buf_ev = bufferevent_socket_new(ev_base_, fd, 0);
    return std::make_shared<TcpConnection>(buf_ev);
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
} // namespace let