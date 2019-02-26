#include "worker.h"

namespace let
{
void Worker::threadFunc()
{
    while (finish_)
    {
        if (task_queue_.empty())
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [=]() { return !task_queue_.empty() || finish_; });
        }

        task_queue_.consume_all([=](Task task) {
            task();
        });
    }
}

Worker::~Worker()
{
    stop();
}

void Worker::stop()
{
    finish_ = true;
    work_thread_.join();
}

WorkerPool::~WorkerPool()
{
    stop();
}


void WorkerPool::stop()
{
    for (auto &worker : workers_)
    {
        worker.stop();
    }
}

void WorkerPool::postTask(const Task &task)
{
    workers_[next_].postTask(task);
    next_ = (next_ + 1) % workers_.size();
}
} // namespace let