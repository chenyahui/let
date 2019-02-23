#include <event2/listener.h>
#include <memory>

#include "acceptor.h"
#include "callback.h"
#include "ip_addr.h"
#include "io_thread.h"

namespace let
{

struct ServerOptions
{
  size_t io_thread;
  std::string ip_port;
};

class TcpServer
{
public:
  TcpServer(const ServerOptions &options, const IpAddress &ip_addr);

  void run();

  void stop();

  void setMessageCallback(const MessageCallback &messageCallback);

  void setConnectionCallback(const ConnectionCallback &connectionCallback);

  void setCloseCallback();

  void setErrorCallback();

private:
  void connectionCallbackWrappe(TcpConnectionPtr);
  void messageCallbackWrapper(TcpConnectionPtr);

  void newConnection(int sockfd, const IpAddress &);

private:
  Acceptor acceptor_;
  ServerOptions options_;

  MessageCallback message_cb_;
  ConnectionCallback connection_cb_;

  IoThreadPool io_thread_pool_;
  
};
} // namespace let