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

namespace let
{
class TcpConnection
{
public:
  explicit TcpConnection(bufferevent *buf_ev);

  ~TcpConnection();

  void send(const void *message, size_t len);

  evutil_socket_t getFd();

  Buffer *inputBuffer();

  Buffer *outBuffer();

  void setUserData(void *user_data);

  void *getUserData() const;

private:
  static void readCallback(struct bufferevent *bev, void *ctx);

  static void writeCallback(struct bufferevent *bev, void *ctx);

  static void errorCallback(struct bufferevent *bev, short what, void *ctx);

private:
  std::string ip_addr_;
  bufferevent *buf_ev_;

  Buffer in_buf_;
  Buffer out_buf_;

  MessageCallback message_callback_;

  void *user_data_ = nullptr;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
} // namespace let
#endif //LET_TCP_CONN_H
