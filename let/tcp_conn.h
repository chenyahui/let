//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_TCP_CONN_H
#define LET_TCP_CONN_H

#include <event2/bufferevent.h>
#include <memory>
#include <string>
#include <atomic>

#include "callback.h"
#include "ip_addr.h"
#include "event_loop.h"
#include "buffer.h"

namespace let
{

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
  TcpConnection(int fd,
                const IpAddress &local_addr,
                const IpAddress &remote_addr);

  ~TcpConnection();

  void send(const void *message, size_t len);
  void send(const std::string &message);

  evutil_socket_t getFd();

  Buffer *inBuffer();

  Buffer *outBuffer();

  void setContext(void* context);

  void* getContext();

  void setMessageCallback(const MessageCallback &);

  void setDisconnectionCallback(const DisconnectionCallback &);

  void setErrorCallback(const ErrorCallback &);

  void bindBufferEvent(bufferevent *);

  const IpAddress &getLocalAddr() const;

  const IpAddress &getRemoteAddr() const;

  int64_t getLastActiveTime() const;

  void setNoDelay(bool no_delay);

  void setKeepAlive(bool keep_alive);

  void closeRead();

  void closeWrite();

  void close();

private:
  static void readCallback(struct bufferevent *bev, void *ctx);

  static void writeCallback(struct bufferevent *bev, void *ctx);

  static void eventCallback(struct bufferevent *bev, short events, void *ctx);

  void changeEvent(short event);

private:
  IpAddress remote_addr_;
  IpAddress local_addr_;

  evutil_socket_t fd_;

  bufferevent *buf_ev_;

  std::unique_ptr<Buffer> in_buf_;
  std::unique_ptr<Buffer> out_buf_;

  MessageCallback message_cb_;
  DisconnectionCallback disconnection_cb_;
  ErrorCallback error_cb_;

  void* context_ = nullptr;

  std::atomic<int64_t> last_readtime_ms_; // ms
  std::atomic<int64_t> last_writetime_ms_; // ms
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
} // namespace let
#endif //LET_TCP_CONN_H
