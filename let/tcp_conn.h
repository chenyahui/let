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
class TcpConnection
{
public:
  explicit TcpConnection(int fd, const IpAddress& ip_addr_);

  explicit TcpConnection(bufferevent *buf_ev);

  ~TcpConnection();

  void send(const void *message, size_t len);

  evutil_socket_t getFd();

  Buffer *inputBuffer();

  Buffer *outBuffer();

  void setUserData(void *user_data);

  void *getUserData() const;

  void setMessageCallback(const MessageCallback&);

private:
  static void readCallback(struct bufferevent *bev, void *ctx);

  static void writeCallback(struct bufferevent *bev, void *ctx);

  static void errorCallback(struct bufferevent *bev, short what, void *ctx);

private:
  IpAddress ip_addr_;
  int fd_;

  bufferevent *buf_ev_;

  Buffer in_buf_;
  Buffer out_buf_;

  MessageCallback message_callback_;

  void *user_data_ = nullptr;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
} // namespace let
#endif //LET_TCP_CONN_H
