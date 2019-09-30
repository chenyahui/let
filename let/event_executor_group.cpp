#include "event_executor_group.h"

#include <memory>
#include <thread>
#include <functional>

#include "event_executor_group.h"

using namespace let;

EventExecutor::EventExecutor()
    : thread_(std::bind(&EventExecutor::threadFunc, this))
{
}

EventExecutor::~EventExecutor()
{
    stop();
}

void EventExecutor::stop()
{
    std::lock_guard<std::mutex> lock_guard(mutex_);
    is_running_ = false;
    event_loop_->stop();
    thread_.join();
}

void EventExecutor::threadFunc()
{
    // event loop must construct at thread
    {
        std::unique_lock<std::mutex> lock(event_loop_init_mutex_);
        event_loop_ = std::move(std::make_unique<EventLoop>());
        event_loop_init_cond_.notify_one();
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&]() { return is_running_; });
    }

    event_loop_->startLoop();
}

EventLoop *EventExecutor::getEventLoop()
{
    if (!event_loop_) {
        std::unique_lock<std::mutex> lock(event_loop_init_mutex_);
        event_loop_init_cond_.wait(lock, [this]() { return event_loop_ != nullptr; });
    }

    return event_loop_.get();
}

void EventExecutor::start()
{
    std::unique_lock<std::mutex> lock(mutex_);
    is_running_ = true;
    lock.unlock();
    cond_.notify_one();
}

bool EventExecutor::isRunning()
{
    return is_running_;
}

EventExecutorGroup::EventExecutorGroup(size_t num)
{
    for (size_t i = 0; i < num; i++) {
        event_loop_thread_pool_.emplace_back(std::move(std::make_unique<EventExecutor>()));
    }
}

EventExecutor *EventExecutorGroup::next()
{
    return event_loop_thread_pool_[counter_++ % event_loop_thread_pool_.size()].get();
}

EventExecutor *EventExecutorGroup::at(size_t index)
{
    return event_loop_thread_pool_[index].get();
}

size_t EventExecutorGroup::size() const
{
    return event_loop_thread_pool_.size();
}

void EventExecutorGroup::stop()
{
    for (auto &i : event_loop_thread_pool_) {
        i->stop();
    }
}

void EventExecutorGroup::start()
{
    for (auto &i : event_loop_thread_pool_) {
        i->start();
    }
}