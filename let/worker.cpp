#include "worker.h"
#include "logger.h"

namespace let
{
WorkerPool::WorkerPool(size_t worker_num)
    : current_worker_(0)
{
    for (size_t i = 0; i < worker_num; i++)
    {
        workers_.emplace_back(new Worker());
    }
}

void WorkerPool::post(const Job &job)
{
    dispatch(current_worker_, job);
    current_worker_ = (current_worker_ + 1) % workers_.size();
}

void WorkerPool::post(Job &&job)
{
    dispatch(current_worker_, job);
    current_worker_ = (current_worker_ + 1) % workers_.size();

}

void WorkerPool::dispatch(size_t hash, const Job &job)
{
    workers_[hash % workers_.size()]->postJob(job);
}

void WorkerPool::dispatch(size_t hash, Job &&job)
{
    workers_[hash % workers_.size()]->postJob(job);
}

void Worker::threadFunc()
{
    while (!finish_)
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

void Worker::postJob(const Job &job)
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(job);
    cond_.notify_one();
}

void Worker::stop()
{
    LOG_DEBUG << "stop worker";
    finish_ = true;
}

Worker::Worker()
    : thread_(std::thread(std::bind(&Worker::threadFunc, this)))
{
}

void WorkerPool::stop()
{
    for (auto &worker: workers_)
    {
        worker->stop();
    }
}

WorkerPool::~WorkerPool()
{
    stop();
}

}