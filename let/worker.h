//
// Created by yahuichen on 2019/1/23.
//

#ifndef LET_WORKER_H
#define LET_WORKER_H
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace let
{

using Task = std::function<void()>;

class Worker
{
public:
  Worker();

  ~Worker();

  void postTask(const Task &);

  void stop();

private:
  void threadFunc();

private:
  boost::lockfree::queue<Task> task_queue_;
  std::thread work_thread_;

  std::mutex mutex_;
  std::condition_variable cond_;
  bool finish_ = false;
};

class WorkerPool
{
public:
  WorkerPool(size_t num)
      : workers_(num)
  {
  }

  ~WorkerPool();

  void stop();

  void postTask(const Task &);

private:
  std::vector<Worker> workers_;

  size_t next_ = 0;
};

} // namespace let
#endif //LET_WORKER_H
