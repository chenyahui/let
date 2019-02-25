
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
  * A EventLoopThread is responsible of managing connections.
  * A EventLoopThread contains an event_loop, that all connections register on it.
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

  const EventLoop & getEventLoop() const;

private:
  EventLoop event_loop_;

  std::thread thread_;
};

class EventLoopThreadPool
{
public:
  explicit EventLoopThreadPool(size_t thread_num)
      : thread_num_(thread_num)
  {
  }

  EventLoopThread *getNextEventLoopThread();

private:
  std::vector<EventLoopThread *> event_loop_threads_;
  std::size_t next_ = 0;

  size_t thread_num_ = 0;
};

} // namespace let
#endif //LET_IO_THREAD_H
