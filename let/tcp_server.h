#include <event2/listener.h>
#include <memory>

#include "acceptor.h"
#include "callback.h"
#include "ip_addr.h"

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
  void connectionCallbackWrapper(TcpConnection *);
  void messageCallbackWrapper(TcpConnection *);

private:
  std::unique_ptr<Acceptor> acceptor_;
  ServerOptions options_;
};
} // namespace let