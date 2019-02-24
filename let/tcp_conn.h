//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_TCP_CONN_H
#define LET_TCP_CONN_H

#include <event2/bufferevent.h>
#include <memory>
#include <string>
#include <boost/any.hpp>

#include "callback.h"
#include "buffer.h"
#include "ip_addr.h"

namespace let
{
class IoThread;

class TcpConnection : std::enable_shared_from_this<TcpConnection>
{
public:
  TcpConnection(int fd, const IpAddress &ip_addr_);

  ~TcpConnection();

  void send(const void *message, size_t len);

  evutil_socket_t getFd();

  Buffer *inputBuffer();

  Buffer *outBuffer();

  void setContext(boost::any context);

  boost::any *getContext();

  void setMessageCallback(const MessageCallback &);

  void setCloseCallback(const CloseCallback &);

  void setErrorCallback(const ErrorCallback &);

  void bindIoThread(IoThread *);

private:
  static void readCallback(struct bufferevent *bev, void *ctx);

  static void writeCallback(struct bufferevent *bev, void *ctx);

  static void eventCallback(struct bufferevent *bev, short events, void *ctx);

private:
  IpAddress ip_addr_;
  evutil_socket_t fd_;

  bufferevent *buf_ev_;

  Buffer *in_buf_;
  Buffer *out_buf_;

  MessageCallback message_cb_;
  CloseCallback close_cb_;
  ErrorCallback error_cb_;

  boost::any context_;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
} // namespace let
#endif //LET_TCP_CONN_H
