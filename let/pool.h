#ifndef LET_POOL_H
#define LET_POOL_H
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace let
{
class ConnectionPool;
/*
通用的连接池的实现，可以用它实现数据库连接池、rpc连接池等
*/
class Connection
{
public:
  virtual ~Connection()
  {
    close();
  }

  virtual void release() = 0;
  virtual void close() = 0;
  virtual std::string getConnectionId() = 0;
};

class FreeConnection
{
public:
  FreeConnection(ConnectionPool *pool, std::shared_ptr<Connection> conn);

  ~FreeConnection();

private:
  ConnectionPool *pool_;
  std::shared_ptr<Connection> conn_;
};

class ConnectionPool
{
public:
  using ConnectionPtr = std::shared_ptr<Connection>;
  using ConnectionFactory = std::function<ConnectionPtr()>;

  ConnectionPool(size_t max_idle_conns,
                 size_t max_open_conns,
                 const ConnectionFactory &factory);

  // 这块可能有问题，因为指针在默认拷贝构造函数里面不会拷贝，所以pool_会失效
  FreeConnection borrow();

  void giveBack(ConnectionPtr conn);

private:
  size_t max_idle_conns_;
  size_t max_open_conns_;
  ConnectionFactory factory_;

  std::vector<ConnectionPtr> free_conns_;
  size_t open_num_;

  std::mutex mutex_;
  std::condition_variable cond_;
};

} // namespace let
#endif