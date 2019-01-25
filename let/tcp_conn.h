//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_TCP_CONN_H
#define LET_TCP_CONN_H

#include <event2/bufferevent.h>
#include <memory>
#include <string>

namespace let {
    class TcpConnection {
    public:
        TcpConnection(bufferevent *buf_ev);

        void send();

    private:
        std::string ip_addr_;
        bufferevent *buf_ev_;
    };

    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
}
#endif //LET_TCP_CONN_H
