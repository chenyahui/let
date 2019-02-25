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

const EventLoop &EventLoopThread::getEventLoop() const
{
    return event_loop_;
}

EventLoopThread *EventLoopThreadPool::getNextEventLoopThread()
{
    auto io_thread = event_loop_threads_[next_];
    next_ = (next_ + 1) % event_loop_threads_.size();
}
} // namespace let