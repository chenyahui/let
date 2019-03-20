#ifndef LET_WORKER_POOL_H
#define LET_WORKER_POOL_H

#include <queue>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace let
{
using Job = std::function<void()>;
class Worker
{
  public:
    Worker()
        : thread_(std::thread(std::bind(&Worker::threadFunc, this)))
    {
    }

    void start();
    void stop();

    void postJob(const Job &job)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(job);
        cond_.notify_one();
    }

  private:
    void threadFunc()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (queue_.empty())
            {
                cond_.wait(lock, [=]() { return !queue_.empty(); });
            }

            while (!queue_.empty())
            {
                Job job = queue_.front();
                queue_.pop();
                job();
            }
        }
    }

    std::queue<Job> queue_;

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class WorkerPool
{
  public:
    WorkerPool(size_t worker_num)
    {
        for (size_t i = 0; i < worker_num; i++)
        {
            workers_.emplace_back(new Worker());
        }
    }

    void start()
    {
    }

    void stop()
    {
    }

    void post(const Job &job)
    {
        workers_[current_worker_]->postJob(job);
    }

    void post(Job &&job)
    {
        workers_[current_worker_]->postJob(job);
    }

  public:
    std::vector<std::unique_ptr<Worker>> workers_;
    std::atomic<size_t> current_worker_ = 0;
};
} // namespace let

#endif