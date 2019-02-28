//
// Created by yahuichen on 2019/1/22.
//

#include <event2/bufferevent.h>

#include <thread>
#include <functional>

#include "event_loop_thread.h"
#include "logger.h"

namespace let
{
EventLoopThread::EventLoopThread()
    : thread_(std::bind(&EventLoopThread::threadFunc, this))
{
}

EventLoopThread::~EventLoopThread()
{
    stop();
}

void EventLoopThread::stop()
{
    event_loop_.stop();

    thread_.join();
}

void EventLoopThread::threadFunc()
{
    event_loop_.loop();
}

const EventLoop &EventLoopThread::getEventLoop() const
{
    return event_loop_;
}

void EventLoopThreadPool::stop(bool is_clear)
{
    for (auto &loop : event_loop_threads_)
    {
        loop->stop();
        if (is_clear)
        {
            delete loop;
        }
    }
}

void EventLoopThreadPool::start()
{
    stop(true);

    for (size_t i = 0; i < thread_num_; i++)
    {
        event_loop_threads_.push_back(new EventLoopThread());
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    stop(true);
}

EventLoopThread *EventLoopThreadPool::getNextEventLoopThread()
{
    if(event_loop_threads_.empty() || next_ > event_loop_threads_.size()){
        LOG_ERROR << "event_loop_threads is empty"; 
        return nullptr;
    }

    auto event_loop_thread = event_loop_threads_[next_];
    next_ = (next_ + 1) % event_loop_threads_.size();
    return event_loop_thread;
}
} // namespace let