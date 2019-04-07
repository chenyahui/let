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
  Worker();

  void stop();

  void postJob(const Job &job);

 private:
  void threadFunc();

  std::queue<Job> queue_;

  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;
  volatile bool finish_ = false;
};

class WorkerPool
{
 public:
  explicit WorkerPool(size_t worker_num);

  ~WorkerPool();

  void stop();

  void post(const Job &job);

  void post(Job &&job);

  void dispatch(size_t hash, const Job &job);

  void dispatch(size_t hash, Job &&job);

 public:
  std::vector<std::unique_ptr<Worker>> workers_;
  std::atomic<size_t> current_worker_;
};
} // namespace let

#endif