#include "threaded_executor.h"

using namespace let;

ThreadedExecutor::ThreadedExecutor()
    : thread_(std::bind(&ThreadedExecutor::threadFunc, this))
{

}


ThreadedExecutor::~ThreadedExecutor()
{
    stop();
}

void ThreadedExecutor::threadFunc()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&]() { return is_running_; });
    }

    while (is_running_)
    {
        {
            std::vector<Task> prepared_tasks;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cond_.wait(lock, [&] { return !task_pool_.empty(); });

                prepared_tasks.swap(task_pool_);
            }

            for (auto &task : prepared_tasks)
            {
                task();
            }
        }
    }
}

void ThreadedExecutor::submit(Task func)
{
    std::unique_lock<std::mutex> lock(mutex_);
    task_pool_.emplace_back(std::move(func));
    lock.unlock();
    cond_.notify_one();
}

void ThreadedExecutor::start()
{
    std::unique_lock<std::mutex> lock(mutex_);
    is_running_ = true;
    lock.unlock();
    cond_.notify_one();
}

void ThreadedExecutor::stop()
{
    {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        is_running_ = false;
    }

    thread_.join();
}

ThreadedExecutorPool::ThreadedExecutorPool(size_t num)
{
    for (size_t i = 0; i < num; i++)
    {
        thread_pool_.emplace_back(std::unique_ptr<ThreadedExecutor>(new ThreadedExecutor()));
    }
}

ThreadedExecutorPool::~ThreadedExecutorPool()
{
    stop();
}

void ThreadedExecutorPool::stop()
{
    for (size_t i = 0; i < thread_pool_.size(); i++)
    {
        thread_pool_[i]->stop();
    }
}

void ThreadedExecutorPool::start()
{
    for (size_t i = 0; i < thread_pool_.size(); i++)
    {
        thread_pool_[i]->start();
    }
}

void ThreadedExecutorPool::submit(Task task)
{
    thread_pool_[counter_++ % thread_pool_.size()]->submit(std::move(task));
}