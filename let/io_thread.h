
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

namespace let
{
/**
  * A IoThread is responsible of managing connections.
  * A IoThread contains an event_loop, that all connections register on it.
*/
class IoThread
{
public:
  explicit IoThread()
      : ev_base_(event_base_new())
  {
  }

  ~IoThread();

  void start();

  void stop();

  void addConnection(TcpConnectionPtr);

private:
  event_base *ev_base_;
  std::map<std::string, TcpConnectionPtr> connections_;

  std::thread thread_;
};

class IoThreadPool
{
public:
  explicit IoThreadPool(size_t thread_num)
      : thread_num_(thread_num)
  {
  }

  void addConnnection(TcpConnectionPtr ptr);

private:
  std::vector<IoThread *> io_threads_;
  std::size_t next_ = 0;

  size_t thread_num_ = 0;
};

} // namespace let
#endif //LET_IO_THREAD_H
