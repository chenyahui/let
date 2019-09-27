#include "event_loop_thread_pool.h"

#include <thread>
#include <functional>

#include "event_loop_thread_pool.h"

using namespace let;

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
    std::lock_guard<std::mutex> lock_guard(mutex_);
    is_running_ = false;
    event_loop_->stop();
    thread_.join();
}

void EventLoopThread::threadFunc()
{
    // event loop must construct at thread
    {
        std::unique_lock<std::mutex> lock(event_loop_init_mutex_);
        event_loop_ = std::move(std::unique_ptr<EventLoop>(new EventLoop()));
        event_loop_init_cond_.notify_one();
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&]() { return is_running_; });
    }

    event_loop_->startLoop();
}

EventLoop *EventLoopThread::getEventLoop()
{
    if (!event_loop_)
    {
        std::unique_lock<std::mutex> lock(event_loop_init_mutex_);
        event_loop_init_cond_.wait(lock, [this]() { return event_loop_ != nullptr; });
    }

    return event_loop_.get();
}

void EventLoopThread::start()
{
    std::unique_lock<std::mutex> lock(mutex_);
    is_running_ = true;
    lock.unlock();
    cond_.notify_one();
}

bool EventLoopThread::isRunning()
{
    return is_running_;
}

EventLoopThreadPool::EventLoopThreadPool(size_t num)
{
    for (size_t i = 0; i < num; i++)
    {
        event_loop_thread_pool_.emplace_back(std::move(std::unique_ptr<EventLoopThread>(new EventLoopThread())));
    }
}

EventLoopThread *EventLoopThreadPool::next()
{
    return event_loop_thread_pool_[counter_++ % event_loop_thread_pool_.size()].get();
}

EventLoopThread *const EventLoopThreadPool::at(size_t index)
{
    return event_loop_thread_pool_[index].get();
}

size_t EventLoopThreadPool::size() const
{
    return event_loop_thread_pool_.size();
}

void EventLoopThreadPool::stop()
{
    for (auto &i : event_loop_thread_pool_)
    {
        i->stop();
    }
}

void EventLoopThreadPool::start()
{
    for (auto &i : event_loop_thread_pool_)
    {
        i->start();
    }
}