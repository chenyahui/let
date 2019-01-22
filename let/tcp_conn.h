//
// Created by yahuichen(陈亚辉) on 2019/1/19.
//

#ifndef LET_TCP_CONN_H
#define LET_TCP_CONN_H

#include <event2/bufferevent.h>

namespace let {
    class TcpConnection {
    public:

    private:
        bufferevent *buf_ev_;
    };
}
#endif //LET_TCP_CONN_H
