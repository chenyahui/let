//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>

#include <thread>
#include "event_loop_thread.h"

namespace let
{
void EventLoopThread::start()
{
    thread_ = std::thread([&]() {
        event_loop_.loop();
    });
}

void EventLoopThread::stop()
{
    event_loop_.stop();
    thread_.join();
}

EventLoopThread::~EventLoopThread()
{
    stop();
}


IoThread *EventLoopThread::getNextIoThread()
{
    auto io_thread = io_threads_[next_];
    next_ = (next_ + 1) % io_threads_.size();
}
} // namespace let