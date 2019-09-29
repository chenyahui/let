#ifndef LET_EVENT_LOOP_THREAD_POOL_H
#define LET_EVENT_LOOP_THREAD_POOL_H

#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

#include "event_loop.h"

namespace let
{
class EventLoopThread
{
public:
    explicit EventLoopThread();

    ~EventLoopThread();

    void start();

    void stop();

    bool isRunning();

    EventLoop* getEventLoop() ;

private:
    void threadFunc();

private:
    bool is_running_ = false;

    std::mutex mutex_;
    std::condition_variable cond_;
    std::unique_ptr<EventLoop> event_loop_;

    std::mutex event_loop_init_mutex_;
    std::condition_variable event_loop_init_cond_;
    std::thread thread_;
};

class EventLoopThreadPool
{
public:
    explicit EventLoopThreadPool(size_t num);

    EventLoopThread *next();

    EventLoopThread *at(size_t index);

    size_t size() const;

    void stop();

    void start();

private:
    std::vector<std::unique_ptr<EventLoopThread>> event_loop_thread_pool_;
    std::atomic_uint32_t counter_;
};

} // namespace let
#endif