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
    LOG_DEBUG << "io thread stop";

    event_loop_.stop();
    thread_.join();
}

void EventLoopThread::threadFunc()
{
    LOG_DEBUG << "io thread start loop";
    event_loop_.loop();
}

const EventLoop &EventLoopThread::getEventLoop() const
{
    return event_loop_;
}

void EventLoopThreadPool::stop()
{
    for (auto &loop : event_loop_threads_)
    {
        loop->stop();
    }
}

void EventLoopThreadPool::start()
{
    stop();

    event_loop_threads_.clear();

    for (size_t i = 0; i < thread_num_; i++)
    {
        event_loop_threads_.push_back(std::move(std::make_unique<EventLoopThread>()));
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    stop();
}

EventLoopThread *EventLoopThreadPool::getNextEventLoopThread()
{
    if(event_loop_threads_.empty() || next_ > event_loop_threads_.size()){
        LOG_ERROR << "event_loop_threads is empty"; 
        return nullptr;
    }

    auto event_loop_thread = event_loop_threads_[next_].get();
    next_ = (next_ + 1) % event_loop_threads_.size();
    return event_loop_thread;
}
} // namespace let