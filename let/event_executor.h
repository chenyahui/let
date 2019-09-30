//
// Created by yahuichen on 2019/9/30.
//

#ifndef LET_LET_EVENT_EXECUTOR_H_
#define LET_LET_EVENT_EXECUTOR_H_
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>
#include <condition_variable>

#include "event_loop.h"

namespace let {
class EventExecutor {
public:
    explicit EventExecutor();

    ~EventExecutor();

    void start();

    void stop();

    bool isRunning();

    EventLoop* getEventLoop();

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
}  // namespace let
#endif  //LET_LET_EVENT_EXECUTOR_H_
