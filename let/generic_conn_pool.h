#ifndef LET_GENERIC_CONNECTION_POOL_H
#define LET_GENERIC_CONNECTION_POOL_H
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace let
{
// 一个通用的连接池。可以用它实现mysql连接池、rpc连接池
template <class T>
class GenericConnectionPool
{
public:
  using ConnectionPtr = std::shared_ptr<T>;
  using NewConnectionCallback = std::function<ConnectionPtr()>;
  using CloseConnectionCallback = std::function<void(ConnectionPtr)>;

  GenericConnectionPool(size_t max_idle_conns,
                        size_t max_open_conns,
                        const NewConnectionCallback &new_conn_cb,
                        const CloseConnectionCallback &close_conn_cb)
      : max_idle_conns_(max_idle_conns),
        max_open_conns_(max_open_conns),
        new_conn_cb_(new_conn_cb),
        close_conn_cb_(close_conn_cb)
  {
  }

  ~GenericConnectionPool()
  {
    // todo stop connections
  }

  ConnectionPtr get()
  {
    std::unique_lock<std::mutex> locker(mutex_);
    ConnectionPtr conn;
    if (!free_conns_.empty())
    {
      conn = free_conns_.back();
      free_conns_.pop_back();
    }
    else if (open_num_ >= max_open_conns_)
    {
      // 如果已经打开的数目大于最大的数目，则阻塞等待
      std::unique_lock<std::mutex> locker(mutex_);
      cond_.wait(locker, [&] { return !free_conns_.empty(); });
      auto free_conn = free_conns_.back();
      free_conns_.pop_back();
    }
    else
    {
      // 否则就新建一个新的链接
      conn = new_conn_cb_();
      open_num_++;
    }

    return conn;
  }

  void put(ConnectionPtr conn)
  {
    std::unique_lock<std::mutex> locker(mutex_);
    if (free_conns_.size() < max_idle_conns_)
    {
      free_conns_.push_back(conn);
      cond_.notify_one();
    }
    else
    {
      close_conn_cb_(conn);
      open_num_--;
    }
  }

private:
  size_t max_idle_conns_;
  size_t max_open_conns_;

  NewConnectionCallback new_conn_cb_;
  CloseConnectionCallback close_conn_cb_;

  std::vector<ConnectionPtr> free_conns_;
  size_t open_num_;

  std::mutex mutex_;
  std::condition_variable cond_;
};

} // namespace let
#endif