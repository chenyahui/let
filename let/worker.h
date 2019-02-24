//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_WORKER_H
#define LET_WORKER_H
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <thread>

namespace let
{

class Worker
{
  public:
    using Task = std::function<void()>;

    Worker();
    
    void postTask(const Task &);

  private:
    void threadFunc();

  private:
    boost::lockfree::queue<Task> task_queue_;
    std::thread work_thread_;
};

class WorkerPool
{
};

} // namespace let
#endif //LET_WORKER_H
