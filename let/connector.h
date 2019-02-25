#ifndef LET_CONNECTOR_H
#define LET_CONNECTOR_H

#include <functional>

#include "ip_addr.h"
#include "io_thread.h"

namespace let
{
class Connector
{
  public:
    typedef std::function<void(evutil_socket_t)> NewConnectionCallback;

    Connector(const IpAddress &remote_addr, IoThread *io_thread);

    void connect();

    void setNewConnectionCallback(const NewConnectionCallback &callback);

  private:
    static void handleEvent(struct bufferevent *bev, short events, void *ctx);

    IpAddress remote_addr_;
    IoThread *io_thread_;

    NewConnectionCallback new_connect_cb_;
};
} // namespace let
#endif