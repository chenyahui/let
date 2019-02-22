//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>
#include "io_thread.h"

namespace let
{
void IoThread::addConnection(evutil_socket_t fd, const std::string &ip_port)
{
    auto buf_ev = bufferevent_socket_new(ev_base_, fd, 0);

    auto conn = std::make_shared<TcpConnection>(buf_ev);
    connections_.insert(std::make_pair(ip_port, conn));
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