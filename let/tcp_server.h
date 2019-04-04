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

  size_t read_high_water = 0; // 读的高水位
  size_t read_low_water = 0;   // 读的低水位

  size_t max_connections = 0; // 最大连接数, 0代表不限制连接数

  size_t idle_timeout_sec = 0; // 单位 s，如果在设定时间内该连接没有读写，则关闭该连接。为0则不开启

  bool tcp_no_delay = true; // 开启tcp_no_delay, 默认开启
  
  bool socket_keep_alive = false; // 开始socket的keep alive选项
};

class TcpServer
{
public:
  TcpServer(EventLoop*, const IpAddress &ip_addr, const ServerOptions &options = ServerOptions());

  void run();

  void stop();

  void gracefulStop();

  void setMessageCallback(const MessageCallback &);

  void setConnectionCallback(const ConnectionCallback &);

  void setDisconnectionCallback(const DisconnectionCallback &);

  void setErrorCallback(const ErrorCallback &);

private:
  void newConnection(int sockfd, const IpAddress &);

  void removeConnection(TcpConnectionPtr);

  void checkIdleConnections();

private:
  EventLoop* loop_;
  Acceptor acceptor_;
  EventLoopThreadPool event_loop_thread_pool_;
  ServerOptions options_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  DisconnectionCallback disconnection_cb_;
  ErrorCallback error_cb_;

  std::map<int, TcpConnectionPtr> connections_;
};
} // namespace let

#endif