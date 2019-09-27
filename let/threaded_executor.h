#ifndef LET_THREADED_EXECUTOR_H
#define LET_THREADED_EXECUTOR_H
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <atomic>
#include <condition_variable>
#include "noncopyable.h"

namespace let
{
using Task = std::function<void()>;

class ThreadedExecutor : NonCopyAble
{
public:
    ThreadedExecutor();
    ~ThreadedExecutor();

    void submit(Task);

    void start();

    void stop();

private:
    void threadFunc();

private:
    bool is_running_ = false;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;

    std::vector<Task> task_pool_;
};

class ThreadedExecutorPool
{
public:
    ThreadedExecutorPool(size_t num);
    ~ThreadedExecutorPool();

    void submit(Task);

    void stop();

    void start();

private:
    std::vector<std::unique_ptr<ThreadedExecutor>> thread_pool_;
    std::atomic_uint32_t counter_;
};
} // namespace let
#endif