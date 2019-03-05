#include "pool.h"

namespace let
{
FreeConnection::FreeConnection(ConnectionPool *pool, std::shared_ptr<Connection> conn)
{
}
FreeConnection::~FreeConnection()
{
    pool_->giveBack(conn_);
}

ConnectionPool::ConnectionPool(size_t max_idle_conns,
                               size_t max_open_conns,
                               const ConnectionFactory &factory)
    : max_idle_conns_(max_idle_conns),
      max_open_conns_(max_open_conns),
      factory_(factory)
{
}

FreeConnection ConnectionPool::borrow()
{
    std::unique_lock<std::mutex> locker(mutex_);
    ConnectionPtr conn;
    if (free_conns_.empty())
    {
        // 如果已经打开的数目大于最大的数目，则阻塞等待
        if (open_num_ >= max_open_conns_)
        {
            std::unique_lock<std::mutex> locker(mutex_);
            cond_.wait(locker, [&] { return !free_conns_.empty(); });
            auto free_conn = free_conns_.back();
            free_conns_.pop_back();
        }
        else
        {
            // 否则就新建一个新的链接
            conn = factory_();
            open_num_++;
        }
    }
    else
    {
        conn = free_conns_.back();
        free_conns_.pop_back();
    }

    return FreeConnection(this, conn);
}

void ConnectionPool::giveBack(ConnectionPtr conn)
{
    std::unique_lock<std::mutex> locker(mutex_);
    if (free_conns_.size() < max_idle_conns_)
    {
        free_conns_.push_back(conn);
        cond_.notify_one();
    }
    else
    {
        open_num_--;
    }
}
} // namespace let