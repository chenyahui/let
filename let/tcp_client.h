#ifndef LET_TCP_CLIENT_H
#define LET_TCP_CLIENT_H

#include "ip_addr.h"
#include "event_loop.h"
#include "callback.h"
#include "tcp_handler.h"
#include "threaded_executor.h"
#include "event_loop_thread_pool.h"
#include "connector.h"

namespace let
{
class ClientOptions
{
};

class TcpClient
{
public:
  TcpClient();

  bool connect(const IpAddress &remote_addr);

  void disConnect();

  void stop();

  TcpClient &setEventLoop(EventLoop *event_loop);

  TcpClient &setHandler(TcpHandler* handler);

  TcpClient &setExecutor(ThreadedExecutorPool *task_pool);

private:
  void newConnection(evutil_socket_t fd);

  void onMessage(TcpConnectionPtr);

  void onDisconnected(TcpConnectionPtr);

  void onError(TcpConnectionPtr, int error);

  void onWriteCompleted(TcpConnectionPtr);

private:
  ThreadedExecutorPool *task_pool_ = nullptr;
  EventLoop *event_loop_ = nullptr;

  std::unique_ptr<Connector> connector_;

  IpAddress remote_addr_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;
  DisconnectionCallback disconnection_cb_;
  ErrorCallback error_cb_;

  TcpHandler* handler_ = nullptr;

  TcpConnectionPtr conn_;
};
} // namespace let
#endif //LET_TCP_CLIENT_H
