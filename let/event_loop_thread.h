
//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//
#ifndef LET_IO_THREAD_H
#define LET_IO_THREAD_H

#include <event2/event.h>
#include <map>
#include <memory>
#include <thread>

#include "tcp_conn.h"
#include "event_loop.h"

namespace let
{
/**
  * A IoThread is responsible of managing connections.
  * A IoThread contains an event_loop, that all connections register on it.
*/
class EventLoopThread
{
public:
  explicit EventLoopThread()
  {
  }

  ~EventLoopThread();

  void start();

  void stop();

  event_base *getEvBase() const;

private:
  EventLoop event_loop_;

  std::thread thread_;
};

class EventLoopPool
{
public:
  explicit EventLoopPool(size_t thread_num)
      : thread_num_(thread_num)
  {
  }

  void addConnnection(TcpConnectionPtr ptr);

  EventLoopThread *getNextIoThread();

private:
  std::vector<EventLoopThread *> io_threads_;
  std::size_t next_ = 0;

  size_t thread_num_ = 0;
};

} // namespace let
#endif //LET_IO_THREAD_H
