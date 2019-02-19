#include <event2/listener.h>

#include "acceptor.h"
#include "callback.h"

namespace let
{

struct ServerOptions
{
    size_t hub_num;
    std::string ip_port;
};

class TcpServer
{
  public:
    void run(ServerOptions options);

    void run();

    void stop();

    void setMessageCallback(const MessageCallback &messageCallback);

    void setConnectionCallback();

    void setCloseCallback();

    void setErrorCallback();

  private:
    Acceptor *acceptor_;
};
} // namespace let