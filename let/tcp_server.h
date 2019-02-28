#ifndef LET_TCP_SERVER_H
#define LET_TCP_SERVER_H

#include <event2/listener.h>
#include <memory>
#include <map>

#include "acceptor.h"
#include "callback.h"
#include "ip_addr.h"
#include "event_loop_thread.h"

namespace let
{

struct ServerOptions
{
  size_t io_thread_num = 1;     // io线程的个数
  int read_high_water = -1; // 读的高水位
  int read_low_water = 0;   // 读的低水位
};

class TcpServer
{
public:
  TcpServer(const ServerOptions &options, const IpAddress &ip_addr);

  void run();

  void stop();

  void setMessageCallback(const MessageCallback &);

  void setConnectionCallback(const ConnectionCallback &);

  void setCloseCallback(const CloseCallback &);

  void setErrorCallback(const ErrorCallback &);

private:
  void newConnection(int sockfd, const IpAddress &);

private:
  Acceptor acceptor_;
  EventLoopThreadPool event_loop_thread_pool_;
  ServerOptions options_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  ErrorCallback error_cb_;

  std::map<int, TcpConnectionPtr> connections_;
};
} // namespace let

#endif